

#include <stdint.h>
#include "bireme.h"
#include "nrf_gzll.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "nrf_drv_clock.h"
#include "nrf_drv_rtc.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define RTC_SCAN_PRESCALER  32
#define RTC_SCAN_INTERVAL   5
#define RTC_SLEEP_PRESCALER   32
#define RTC_SLEEP_INTERVAL    500
#define RTC_OFF_PRESCALER   32
#define RTC_OFF_INTERVAL    30000

#define KEEP_ALIVE_INTERVAL 25
#define INACTIVITY_SLEEP_TIMEOUT 100 /* 500ms */
#define INACTIVITY_OFF_TIMEOUT   

/*****************************************************************************/
/** Configuration */
/*****************************************************************************/
const nrf_drv_rtc_t rtc_scan = NRF_DRV_RTC_INSTANCE(0); /**< Declaring an instance of nrf_drv_rtc for RTC0. */
const nrf_drv_rtc_t rtc_sleep = NRF_DRV_RTC_INSTANCE(1); /**< Declaring an instance of nrf_drv_rtc for RTC1. */
const nrf_drv_rtc_t rtc_off = NRF_DRV_RTC_INSTANCE(2); /**< Declaring an instance of nrf_drv_rtc for RTC2. */


// Debounce time (dependent on tick frequency)
#define DEBOUNCE 5
#define ACTIVITY 100

typedef struct key_def_s
{
	uint8_t port:1;
	uint8_t pin_index:6;
} key_def_s;


// Key buffers
static key_def_s keys[] = { 
    S00, S01, S02, S03, S04, S05, S06, S07, S08, S09, 
	S10, S11, S12, S13, S14, S15, S16, S17, S18, S19, 
	S20, S21, S22, S23, S24, S25, S26, S27, S28, S29, 
	S30, S31, S32, S33, S34};

#define NUM_KEYS sizeof(keys)

// Define payload length
#define ACK_PAYLOAD_LENGTH 1
#define DATA_PAYLOAD_LENGTH (NUM_KEYS/8 + 1)

// Data and acknowledgement payloads
static uint8_t data_payload[DATA_PAYLOAD_LENGTH]; 
static uint8_t ack_payload[NRF_GZLL_CONST_MAX_PAYLOAD_LENGTH];


static uint32_t port_debounced[2];
static uint32_t debounce_last[2];
static uint32_t port_mask[2] = {0};

static volatile bool key_down = false;

// Debug helper variables
static volatile bool init_ok, enable_ok, push_ok, pop_ok, tx_success;  

// Setup switch pins with pullups
static void gpio_config(void)
{
    for (int s = 0; s < NUM_KEYS; s++)
    {   
        port_mask[keys[s].port] |= (1 << keys[s].pin_index);

        uint32_t pin_number = NRF_GPIO_PIN_MAP(keys[s].port, keys[s].pin_index);
        nrf_gpio_cfg_sense_input(pin_number, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    }
    debounce_last[0] = port_debounced[0] = port_mask[0];
    debounce_last[1] = port_debounced[1] = port_mask[1];
}

// Assemble packet and send to receiver
static void send_data(void)
{
    for (int i = 0; i < DATA_PAYLOAD_LENGTH; i++)
        data_payload[i] = 0;

    uint8_t bit = 0;
    uint8_t byte = 0;
    for (int i = 0; i < NUM_KEYS; i++)
    {
        if ((port_debounced[keys[i].port] & (1 << keys[i].pin_index)) == 0 &&
            (port_mask[keys[i].port] & (1 << keys[i].pin_index)) )
            data_payload[byte] |= (1 << bit);
        bit++;
        if (bit == 8)
        {
            bit = 0;
            byte++;
        }        
    }

    nrf_gzll_add_packet_to_tx_fifo(PIPE_NUMBER, data_payload, DATA_PAYLOAD_LENGTH);
}


static void rtc_reset(const nrf_drv_rtc_t* rtc, uint32_t interval)
{
    nrf_drv_rtc_cc_disable(rtc, 0);
    nrf_drv_rtc_counter_clear(rtc);
    nrf_drv_rtc_cc_set(rtc, 0, interval, true);
}

static void rtc_restart(const nrf_drv_rtc_t* rtc, uint32_t interval)
{
    nrf_drv_rtc_counter_clear(rtc);
    nrf_drv_rtc_cc_set(rtc, 0, interval, true);
}

static void rtc_start(const nrf_drv_rtc_t* rtc, uint32_t interval)
{
    nrf_drv_rtc_cc_set(rtc, 0, interval, true);
    nrf_drv_rtc_enable(rtc);
}

static void rtc_stop(const nrf_drv_rtc_t* rtc)
{
    nrf_drv_rtc_cc_disable(rtc, 0);
    nrf_drv_rtc_disable(rtc);
}


static void handler_sleep(nrf_drv_rtc_int_type_t int_type)
{
    rtc_stop(&rtc_scan);    
    rtc_stop(&rtc_sleep);
    rtc_start(&rtc_off, RTC_OFF_INTERVAL - RTC_SLEEP_INTERVAL);

    // Set the GPIOTE PORT event as interrupt source, and enable interrupts for GPIOTE
    NRF_GPIOTE->INTENSET = GPIOTE_INTENSET_PORT_Msk;
    NRF_GPIOTE->EVENTS_PORT = 0;
    NVIC_EnableIRQ(GPIOTE_IRQn);
}

static void handler_off(nrf_drv_rtc_int_type_t int_type)
{
    rtc_stop(&rtc_scan);
    rtc_stop(&rtc_sleep);
    rtc_stop(&rtc_off);
    NRF_POWER->SYSTEMOFF = 1;
}
    

// 200Hz debounce sampling
static void handler_scan(nrf_drv_rtc_int_type_t int_type)
{
    static uint32_t keep_alive_count = 0;

    rtc_restart(&rtc_scan, RTC_SCAN_INTERVAL);

    bool key_event_detected = false;
    uint32_t port[2];
    uint32_t current[2];
    uint32_t changed[2];
    uint32_t debounced[2];

    port[0] = nrf_gpio_port_in_read(NRF_P0);
    port[1] = nrf_gpio_port_in_read(NRF_P1);


    for (int i = 0; i < 2; i++)
    {
        current[i] = port[i] & port_mask[i];
        changed[i] = debounce_last[i] ^ current[i];
        debounce_last[i] = current[i];

        debounced[i] = (port_debounced[i] & changed[i]) | 
                       (current[i] & ~changed[i]);
    }

    key_down = (debounced[0] != port_mask[0]) ||
               (debounced[1] != port_mask[1]);

    if (port_debounced[0] != debounced[0] || port_debounced[1] != debounced[1])
    {
        port_debounced[0] = debounced[0];
        port_debounced[1] = debounced[1];
        send_data();

        NRF_LOG_INFO("Key detected 0x%08X:0x%08X", port_debounced[0], port_debounced[1]);

        key_event_detected = true;
    }

    if (key_event_detected || key_down)
    {
       rtc_reset(&rtc_sleep, RTC_SLEEP_INTERVAL);
    }

    keep_alive_count++;

    // 8Hz held key maintenance, keeping the reciever keystates valid
    if (keep_alive_count >= KEEP_ALIVE_INTERVAL)
    {
        send_data();
        keep_alive_count = 0;
    }
}


// Low frequency clock configuration
static void lfclk_config(void)
{
    nrf_drv_clock_init();

    nrf_drv_clock_lfclk_request(NULL);
}

// RTC peripheral configuration
static void rtc_config(void)
{
    nrf_drv_rtc_config_t scan_config = NRF_DRV_RTC_DEFAULT_CONFIG;
    scan_config.prescaler = RTC_SCAN_PRESCALER;

    nrf_drv_rtc_init(&rtc_scan, &scan_config, handler_scan);

    nrf_drv_rtc_config_t sleep_config = NRF_DRV_RTC_DEFAULT_CONFIG;
    sleep_config.prescaler = RTC_SLEEP_PRESCALER;

    nrf_drv_rtc_init(&rtc_sleep, &sleep_config, handler_sleep);


    nrf_drv_rtc_config_t off_config = NRF_DRV_RTC_DEFAULT_CONFIG;
    off_config.prescaler = RTC_OFF_PRESCALER;

    nrf_drv_rtc_init(&rtc_off, &off_config, handler_off);
}

void log_init(void)
{
    // Set up logger
    NRF_LOG_INIT(NULL);

    NRF_LOG_DEFAULT_BACKENDS_INIT();

    NRF_LOG_INFO("Bireme started.");
    NRF_LOG_FLUSH();    
}

int main()
{
    log_init();

    // Initialize Gazellnrf_gzll_add_packet_to_tx_fifo
    nrf_gzll_init(NRF_GZLL_MODE_DEVICE);
    
    // Attempt sending every packet up to 100 times    
    nrf_gzll_set_max_tx_attempts(100);

    // Addressing
    nrf_gzll_set_base_address_0(0x01020304);
    nrf_gzll_set_base_address_1(0x05060708);

    // Enable Gazell to start sending over the air
    nrf_gzll_enable();

    // Configure 32kHz xtal oscillator
    lfclk_config(); 

    // Configure RTC peripherals with ticks
    rtc_config();

    // Configure all keys as inputs with pullups
    gpio_config();

    // Start rtc tasks
    rtc_start(&rtc_scan, RTC_SCAN_INTERVAL);
    rtc_start(&rtc_sleep, RTC_SLEEP_INTERVAL);


    // Main loop, constantly sleep, waiting for RTC and gpio IRQs
    while(1)
    {
        NRF_LOG_FLUSH();        
        __SEV();
        __WFE();
        __WFE(); 
    }
}

// This handler will be run after wakeup from system ON (GPIO wakeup)
void GPIOTE_IRQHandler(void)
{
    if(NRF_GPIOTE->EVENTS_PORT)
    {
        //clear wakeup event
        NRF_GPIOTE->EVENTS_PORT = 0;
        rtc_start(&rtc_scan, RTC_SCAN_INTERVAL);
        rtc_start(&rtc_sleep, RTC_SLEEP_INTERVAL);
        rtc_stop(&rtc_off);        
        NVIC_DisableIRQ(GPIOTE_IRQn);
        NRF_LOG_INFO("Bireme wakeup.");        
    }
}



/*****************************************************************************/
/** Gazell callback function definitions  */
/*****************************************************************************/

void  nrf_gzll_device_tx_success(uint32_t pipe, nrf_gzll_device_tx_info_t tx_info)
{
    uint32_t ack_payload_length = NRF_GZLL_CONST_MAX_PAYLOAD_LENGTH;    

    if (tx_info.payload_received_in_ack)
    {
        // Pop packet and write first byte of the payload to the GPIO port.
        nrf_gzll_fetch_packet_from_rx_fifo(pipe, ack_payload, &ack_payload_length);
    }
}

// no action is taken when a packet fails to send, this might need to change
void nrf_gzll_device_tx_failed(uint32_t pipe, nrf_gzll_device_tx_info_t tx_info)
{
    
}

// Callbacks not needed
void nrf_gzll_host_rx_data_ready(uint32_t pipe, nrf_gzll_host_rx_info_t rx_info)
{}
void nrf_gzll_disabled()
{}

