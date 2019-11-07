

#include <stdint.h>
#include <assert.h> 
#include "bireme.h"
#include "bireme_gzll_config.h"
#include "bat_level.h"
#include "nrf_gzll.h"
#include "nrf_gpio.h"
#include "nrf_drv_clock.h"
#include "nrf_drv_rtc.h"

#define RTC_SCAN_PRESCALER  32      /* 1ms tick */
#define RTC_SCAN_INTERVAL   5       /* 5ms (200Hz)*/
#define RTC_OFF_PRESCALER   32      /* 1ms tick */
#define RTC_OFF_INTERVAL    30000   /* 30 seconds */

#define RTC_BAT_LEVEL_PRESCALER 32
#define RTC_BAT_LEVEL_INTERVAL  1000   /* 1000ms */

#define SLEEP_INTERVAL      (500/RTC_SCAN_INTERVAL)    /* 500ms */
#define KEEP_ALIVE_INTERVAL (BIREME_KEEP_ALIVE_TIMEOUT/RTC_SCAN_INTERVAL)

/* Configure RTC instances for tasks */
const nrf_drv_rtc_t rtc_scan = NRF_DRV_RTC_INSTANCE(0);
const nrf_drv_rtc_t rtc_bat_level = NRF_DRV_RTC_INSTANCE(1);
const nrf_drv_rtc_t rtc_off = NRF_DRV_RTC_INSTANCE(2);


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


#define ACK_PAYLOAD_LENGTH BIREME_GZLL_ACK_PAYLOAD_LEN
#define DATA_PAYLOAD_LENGTH BIREME_GZLL_DATA_PAYLOAD_LEN

static uint8_t data_payload[DATA_PAYLOAD_LENGTH]; 
static uint8_t ack_payload[NRF_GZLL_CONST_MAX_PAYLOAD_LENGTH];

static uint32_t port_debounced[2];
static uint32_t debounce_last[2];
static uint32_t port_mask[2] = {0};

static volatile bool key_down = false;

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

static void send_data(void)
{
    for (int i = 0; i < DATA_PAYLOAD_LENGTH; i++)
        data_payload[i] = 0;

    uint8_t bit = 0;
    uint8_t byte = 0;
    for (int i = 0; i < NUM_KEYS; i++)
    {
        if ((port_debounced[keys[i].port] & (1 << keys[i].pin_index)) == 0)
            data_payload[byte] |= (1 << bit);
        bit++;
        if (bit == 8)
        {
            bit = 0;
            byte++;
        }        
    }

    uint8_t bat_level = bat_level_read();
    data_payload [DATA_PAYLOAD_LENGTH - 1] = bat_level;

    nrf_gzll_add_packet_to_tx_fifo(PIPE_NUMBER, data_payload, DATA_PAYLOAD_LENGTH);
}

static void rtc_restart(const nrf_drv_rtc_t* rtc, uint32_t interval)
{
    nrf_drv_rtc_cc_set(rtc, 0, interval, true);
    nrf_drv_rtc_counter_clear(rtc);
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


static void system_on_sleep(void)
{
    /* Stop the scan and sleep tasks and start the system off task */
    rtc_stop(&rtc_scan);    
    rtc_start(&rtc_off, RTC_OFF_INTERVAL - SLEEP_INTERVAL);

    /* Set the GPIOTE PORT event as interrupt source, and enable interrupts for GPIOTE */
    NRF_GPIOTE->INTENSET = GPIOTE_INTENSET_PORT_Msk;
    NRF_GPIOTE->EVENTS_PORT = 0;
    NVIC_EnableIRQ(GPIOTE_IRQn);
}

static void system_on_wake(void)
{
    if(NRF_GPIOTE->EVENTS_PORT)
    {
        /* clear wakeup event */
        NRF_GPIOTE->EVENTS_PORT = 0;

        /* Start scan and battery tasks */
        rtc_start(&rtc_scan, RTC_SCAN_INTERVAL);

        /* Stop the system off task */
        rtc_stop(&rtc_off);       

        NVIC_DisableIRQ(GPIOTE_IRQn);
    }

}

static void handler_bat_level(nrf_drv_rtc_int_type_t int_type)
{
    bat_level_update();
}

static void handler_off(nrf_drv_rtc_int_type_t int_type)
{
    /* Stop all RTC based tasks */
    rtc_stop(&rtc_scan);
    rtc_stop(&rtc_bat_level);
    rtc_stop(&rtc_off);

    bat_level_uninit();

    /* Enter System Off mode. Waking from system off will generate a reset */
    NRF_POWER->SYSTEMOFF = 1;
}
    
static void handler_scan(nrf_drv_rtc_int_type_t int_type)
{
    static uint32_t keep_alive_count = 0;
    static uint32_t sleep_count = 0;

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

        key_event_detected = true;
    }


    sleep_count++;
    keep_alive_count++;

    if (key_event_detected)
    {
        send_data();
        sleep_count = 0;
        keep_alive_count = 0;
    }
    else if (key_down)
    {
        sleep_count = 0;
    }
    
    if (sleep_count >= SLEEP_INTERVAL)
    {
        sleep_count = 0;
        keep_alive_count = 0;
        system_on_sleep();
    }

    /* If no key state changes occur, no data will be sent to the receiver. 
     * To prevent the receiver from inferring that the keyboard has been 
     * disconnected, send a periodic keep alive update. 
     * Note: This is particularly important in the event a key is held down. */
    if (keep_alive_count >= KEEP_ALIVE_INTERVAL)
    {
        send_data();
        keep_alive_count = 0;
    }

    
}


static void lfclk_config(void)
{
    nrf_drv_clock_init();

    nrf_drv_clock_lfclk_request(NULL);
}

static void rtc_config(void)
{
    nrf_drv_rtc_config_t scan_config = NRF_DRV_RTC_DEFAULT_CONFIG;
    scan_config.prescaler = RTC_SCAN_PRESCALER;

    nrf_drv_rtc_init(&rtc_scan, &scan_config, handler_scan);

    nrf_drv_rtc_config_t bat_level_config = NRF_DRV_RTC_DEFAULT_CONFIG;
    bat_level_config.prescaler = RTC_BAT_LEVEL_PRESCALER;

    nrf_drv_rtc_init(&rtc_bat_level, &bat_level_config, handler_bat_level);


    nrf_drv_rtc_config_t off_config = NRF_DRV_RTC_DEFAULT_CONFIG;
    off_config.prescaler = RTC_OFF_PRESCALER;

    nrf_drv_rtc_init(&rtc_off, &off_config, handler_off);
}

void gazell_init(void)
{
    nrf_gzll_init(NRF_GZLL_MODE_DEVICE);
    
    nrf_gzll_set_max_tx_attempts(100);

    /* Set base address for Gazell pipes 0 and 1 */
    nrf_gzll_set_base_address_0(BIREME_GZLL_PIPE_0_ADDR);
    nrf_gzll_set_base_address_1(BIREME_GZLL_PIPE_1_ADDR);

    nrf_gzll_enable();
}

int main()
{
    /* Enable DC/DC converter */
    NRF_POWER->DCDCEN = 1;

    gazell_init();

    lfclk_config(); 

    rtc_config();

    gpio_config();

    bat_level_init();

    bat_level_update();

    rtc_start(&rtc_scan, RTC_SCAN_INTERVAL);
    rtc_start(&rtc_bat_level, RTC_BAT_LEVEL_INTERVAL);

    /* System On sleep. Will be woken by RTC interrupts */
    while(1)
    {
        __SEV();
        __WFE();
        __WFE(); 
    }
}

/* Executed when gpio sense event triggers a wakeup from System On sleep */
void GPIOTE_IRQHandler(void)
{
    system_on_wake();    
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

