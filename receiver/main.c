
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "nrf.h"
#include "app_uart.h"
#include "nrf_drv_uart.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "nrf_drv_clock.h"
#include "nrf_drv_rtc.h"
#include "nrf_gzll.h"
#include "nrf_gzll_error.h"

#include "bireme_gzll_config.h"

#define MAX_TEST_DATA_BYTES     (15U)                /**< max number of test bytes to be used for tx and rx. */
#define UART_TX_BUF_SIZE 256                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 1                           /**< UART RX buffer size. */


#define RX_PIN_NUMBER  25
#define TX_PIN_NUMBER  24
#define CTS_PIN_NUMBER 23
#define RTS_PIN_NUMBER 22
#define HWFC           false


#define RTC_INACTIVE_PRESCALER   32
#define RTC_INACTIVE_INTERVAL    BIREME_KEEP_ALIVE_TIMEOUT

// Define payload length
#define ACK_PAYLOAD_LENGTH BIREME_GZLL_ACK_PAYLOAD_LEN
#define KEY_PAYLOAD_LENGTH BIREME_GZLL_DATA_PAYLOAD_LEN

#define MATRIX_ROWS 6
#define QMK_UPDATE_LENGTH ((2 * (MATRIX_ROWS + 1)) + 1)



/* Inactivity timeout in units of 125ms. 
 * Keyboard(s) at a minimum send a keep alive update every 125ms. */
#define INACTIVITY_TIMEOUT 3

// Data and acknowledgement payloads
static uint8_t data_payload_left[NRF_GZLL_CONST_MAX_PAYLOAD_LENGTH];  ///< Placeholder for data payload received from host. 
static uint8_t data_payload_right[NRF_GZLL_CONST_MAX_PAYLOAD_LENGTH];  ///< Placeholder for data payload received from host. 
static uint8_t ack_payload[ACK_PAYLOAD_LENGTH];                   ///< Payload to attach to ACK sent to device.
static uint8_t data_buffer[QMK_UPDATE_LENGTH];

const nrf_drv_rtc_t rtc_inactive = NRF_DRV_RTC_INSTANCE(0); /**< Declaring an instance of nrf_drv_rtc for RTC0. */

// Debug helper variables
extern nrf_gzll_error_code_t nrf_gzll_error_code;   ///< Error code
static bool packet_received_left, packet_received_right;
static bool data_sent = false;
uint32_t left_inactive = 0;
uint32_t right_inactive = 0;
uint8_t c;

void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info)
{
    NVIC_SystemReset();
}


/*
static void rtc_reset(const nrf_drv_rtc_t* rtc, uint32_t interval)
{
    nrf_drv_rtc_cc_disable(rtc, 0);
    nrf_drv_rtc_counter_clear(rtc);
    nrf_drv_rtc_cc_set(rtc, 0, interval, true);
}
*/

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

static void handler_inactive(nrf_drv_rtc_int_type_t int_type)
{
    rtc_restart(&rtc_inactive, RTC_INACTIVE_INTERVAL);

    left_inactive++;
    right_inactive++;

    if (left_inactive == INACTIVITY_TIMEOUT)
    {
        data_buffer[0] = 0;
        data_buffer[2] = 0;
        data_buffer[4] = 0;
        data_buffer[6] = 0;
        data_buffer[8] = 0;
        data_buffer[10] = 0;
        data_buffer[12]  = 100;
    }
    if (right_inactive == INACTIVITY_TIMEOUT)
    {
        data_buffer[1] = 0;
        data_buffer[3] = 0;
        data_buffer[5] = 0;
        data_buffer[7] = 0;
        data_buffer[9] = 0;
        data_buffer[11] = 0;
        data_buffer[13]  = 100;
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
    nrf_drv_rtc_config_t inactive_config = NRF_DRV_RTC_DEFAULT_CONFIG;
    inactive_config.prescaler = RTC_INACTIVE_PRESCALER;

    nrf_drv_rtc_init(&rtc_inactive, &inactive_config, handler_inactive);

    rtc_start(&rtc_inactive, RTC_INACTIVE_INTERVAL);
}

void uart_event_handle(app_uart_evt_t * p_event)
{
    switch (p_event->evt_type)
    {
        case APP_UART_DATA_READY:
        break;
        case APP_UART_FIFO_ERROR:
            APP_ERROR_HANDLER(p_event->data.error_code);
            break;
        case APP_UART_COMMUNICATION_ERROR:
            APP_ERROR_HANDLER(p_event->data.error_communication);
            break;
        case APP_UART_TX_EMPTY:
            data_sent = true;
            break;
        case APP_UART_DATA:
        break;
        default:
        break;
    }

}

void uart_init(void)
{
    uint32_t err_code;
    const app_uart_comm_params_t comm_params =
      {
          RX_PIN_NUMBER,
          TX_PIN_NUMBER,
          RTS_PIN_NUMBER,
          CTS_PIN_NUMBER,
          APP_UART_FLOW_CONTROL_DISABLED,
          false,
          UART_BAUDRATE_BAUDRATE_Baud1M
      };

    APP_UART_FIFO_INIT(&comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       uart_event_handle,
                       APP_IRQ_PRIORITY_LOW,
                       err_code);

    APP_ERROR_CHECK(err_code);

}

void process_data_left(void)
{
    data_buffer[0] = ((data_payload_left[0] & 1<<0) ? 1:0) << 0 | /* L00 */
                     ((data_payload_left[0] & 1<<1) ? 1:0) << 1 | /* L01 */
                     ((data_payload_left[0] & 1<<2) ? 1:0) << 2 | /* L02 */
                     ((data_payload_left[0] & 1<<3) ? 1:0) << 3 | /* L03 */
                     ((data_payload_left[0] & 1<<4) ? 1:0) << 4 | /* L04 */
                     ((data_payload_left[0] & 1<<5) ? 1:0) << 5 | /* L05 */
                     ((data_payload_left[0] & 1<<6) ? 1:0) << 6;  /* L06 */

    data_buffer[2] = ((data_payload_left[0] & 1<<7) ? 1:0) << 0 | /* L07 */
                     ((data_payload_left[1] & 1<<0) ? 1:0) << 1 | /* L08 */
                     ((data_payload_left[1] & 1<<1) ? 1:0) << 2 | /* L09 */
                     ((data_payload_left[1] & 1<<2) ? 1:0) << 3 | /* L10 */
                     ((data_payload_left[1] & 1<<3) ? 1:0) << 4 | /* L11 */
                     ((data_payload_left[1] & 1<<4) ? 1:0) << 5 | /* L12 */
                     ((data_payload_left[1] & 1<<5) ? 1:0) << 6;  /* L13 */

    data_buffer[4] = ((data_payload_left[1] & 1<<6) ? 1:0) << 0 | /* L14 */
                     ((data_payload_left[1] & 1<<7) ? 1:0) << 1 | /* L15 */
                     ((data_payload_left[2] & 1<<0) ? 1:0) << 2 | /* L16 */
                     ((data_payload_left[2] & 1<<1) ? 1:0) << 3 | /* L17 */
                     ((data_payload_left[2] & 1<<2) ? 1:0) << 4 | /* L18 */
                     ((data_payload_left[2] & 1<<3) ? 1:0) << 5;  /* L19 */

    data_buffer[6] = ((data_payload_left[2] & 1<<4) ? 1:0) << 0 | /* L20 */
                     ((data_payload_left[2] & 1<<5) ? 1:0) << 1 | /* L21 */
                     ((data_payload_left[2] & 1<<6) ? 1:0) << 2 | /* L22 */
                     ((data_payload_left[2] & 1<<7) ? 1:0) << 3 | /* L23 */
                     ((data_payload_left[3] & 1<<0) ? 1:0) << 4 | /* L24 */
                     ((data_payload_left[3] & 1<<1) ? 1:0) << 5 | /* L25 */
                     ((data_payload_left[3] & 1<<2) ? 1:0) << 6;  /* L26 */

    data_buffer[8] = ((data_payload_left[3] & 1<<3) ? 1:0) << 0 | /* L27 */
                     ((data_payload_left[3] & 1<<4) ? 1:0) << 1 | /* L28 */
                     ((data_payload_left[3] & 1<<5) ? 1:0) << 2 | /* L29 */
                     ((data_payload_left[3] & 1<<6) ? 1:0) << 3 | /* L30 */
                     ((data_payload_left[3] & 1<<7) ? 1:0) << 4 | /* L31 */
                     ((data_payload_left[4] & 1<<0) ? 1:0) << 6;  /* L32 */

    data_buffer[10]= ((data_payload_left[4] & 1<<1) ? 1:0) << 5 | /* L33 */
                      ((data_payload_left[4] & 1<<2) ? 1:0) << 6;  /* L34 */

    data_buffer[12] = data_payload_left[5];
}

void process_data_right(void)
{
    data_buffer[1] = ((data_payload_right[0] & 1<<0) ? 1:0) << 6 | /* R00 */
                     ((data_payload_right[0] & 1<<1) ? 1:0) << 5 | /* R01 */
                     ((data_payload_right[0] & 1<<2) ? 1:0) << 4 | /* R02 */
                     ((data_payload_right[0] & 1<<3) ? 1:0) << 3 | /* R03 */
                     ((data_payload_right[0] & 1<<4) ? 1:0) << 2 | /* R04 */
                     ((data_payload_right[0] & 1<<5) ? 1:0) << 1 | /* R05 */
                     ((data_payload_right[0] & 1<<6) ? 1:0) << 0;  /* R06 */

    data_buffer[3] = ((data_payload_right[0] & 1<<7) ? 1:0) << 6 | /* R07 */
                     ((data_payload_right[1] & 1<<0) ? 1:0) << 5 | /* R08 */        
                     ((data_payload_right[1] & 1<<1) ? 1:0) << 4 | /* R09 */
                     ((data_payload_right[1] & 1<<2) ? 1:0) << 3 | /* R10 */
                     ((data_payload_right[1] & 1<<3) ? 1:0) << 2 | /* R11 */
                     ((data_payload_right[1] & 1<<4) ? 1:0) << 1 | /* R12 */
                     ((data_payload_right[1] & 1<<5) ? 1:0) << 0;  /* R13 */

    data_buffer[5] = ((data_payload_right[1] & 1<<6) ? 1:0) << 6 | /* R14 */
                     ((data_payload_right[1] & 1<<7) ? 1:0) << 5 | /* R15 */
                     ((data_payload_right[2] & 1<<0) ? 1:0) << 4 | /* R16 */
                     ((data_payload_right[2] & 1<<1) ? 1:0) << 3 | /* R17 */
                     ((data_payload_right[2] & 1<<2) ? 1:0) << 2 | /* R18 */
                     ((data_payload_right[2] & 1<<3) ? 1:0) << 1;  /* R19 */

    data_buffer[7] = ((data_payload_right[2] & 1<<4) ? 1:0) << 6 | /* R20 */
                     ((data_payload_right[2] & 1<<5) ? 1:0) << 5 | /* R21 */
                     ((data_payload_right[2] & 1<<6) ? 1:0) << 4 | /* R22 */
                     ((data_payload_right[2] & 1<<7) ? 1:0) << 3 | /* R23 */
                     ((data_payload_right[3] & 1<<0) ? 1:0) << 2 | /* R24 */
                     ((data_payload_right[3] & 1<<1) ? 1:0) << 1 | /* R25 */
                     ((data_payload_right[3] & 1<<2) ? 1:0) << 0;  /* R26 */

    data_buffer[9] = ((data_payload_right[3] & 1<<3) ? 1:0) << 6 | /* R27 */
                     ((data_payload_right[3] & 1<<4) ? 1:0) << 5 | /* R28 */
                     ((data_payload_right[3] & 1<<5) ? 1:0) << 4 | /* R29 */
                     ((data_payload_right[3] & 1<<6) ? 1:0) << 3 | /* R30 */
                     ((data_payload_right[3] & 1<<7) ? 1:0) << 1 | /* R31 */
                     ((data_payload_right[4] & 1<<0) ? 1:0) << 0;  /* R32 */

    data_buffer[11]= ((data_payload_right[4] & 1<<1) ? 1:0) << 1 | /* R33 */
                     ((data_payload_right[4] & 1<<2) ? 1:0) << 0;  /* R34 */

    data_buffer[13] = data_payload_right[5];
}

void gazell_init(void)
{
    /* Initialize Gazell */
    bool result_value = nrf_gzll_init(NRF_GZLL_MODE_HOST);
    GAZELLE_ERROR_CODE_CHECK(result_value);    

    /* Set address for pipes */
    nrf_gzll_set_base_address_0(0x01020304);
    nrf_gzll_set_base_address_1(0x05060708);
  
    /* Load ack payload data into TX queue */
    ack_payload[0] = 0x55;
    nrf_gzll_add_packet_to_tx_fifo(0, ack_payload, ACK_PAYLOAD_LENGTH);
    nrf_gzll_add_packet_to_tx_fifo(1, ack_payload, ACK_PAYLOAD_LENGTH);

    // Enable Gazell to start sending over the air
    nrf_gzll_enable();

}

int main(void)
{
    gazell_init();

    uart_init();

    /* Configure low frequency clock for rtc */
    lfclk_config(); 

    /* Configure RTC for inactivity timeout */
    rtc_config();

    // main loop
    while (true)
    {
        // detecting received packet from interupt, and unpacking
        if (packet_received_left)
        {
            packet_received_left = false;
            left_inactive =  0;

            process_data_left();
        }

        if (packet_received_right)
        {
            packet_received_right = false;
            right_inactive = 0;
            process_data_right();
        }

        uint32_t result = app_uart_get(&c);

        /* Check for a poll request from QMK */
        if (result == NRF_SUCCESS && c == 's')
        {
            data_sent = false;
            data_buffer[QMK_UPDATE_LENGTH - 1] = 0xE0;
            for (uint32_t i = 0; i < QMK_UPDATE_LENGTH; i++)
            {
                while (app_uart_put(data_buffer[i]) != NRF_SUCCESS);
            }
        }

        //while (!data_sent);
    }

    rtc_stop(&rtc_inactive);
}


/* Unused callbacks. */
void nrf_gzll_device_tx_success(uint32_t pipe, nrf_gzll_device_tx_info_t tx_info) {}
void nrf_gzll_device_tx_failed(uint32_t pipe, nrf_gzll_device_tx_info_t tx_info) {}
void nrf_gzll_disabled() {}

void nrf_gzll_host_rx_data_ready(uint32_t pipe, nrf_gzll_host_rx_info_t rx_info)
{   
    uint32_t data_payload_length = NRF_GZLL_CONST_MAX_PAYLOAD_LENGTH;
    
    if (pipe == BIREME_LEFT_HAND_PIPE)
    {
        packet_received_left = true;
        nrf_gzll_fetch_packet_from_rx_fifo(pipe, data_payload_left, &data_payload_length);
    }
    else if (pipe == BIREME_RIGHT_HAND_PIPE)
    {
        packet_received_right = true;
        nrf_gzll_fetch_packet_from_rx_fifo(pipe, data_payload_right, &data_payload_length);
    }
    
    nrf_gzll_flush_rx_fifo(pipe);

    /* Send acknowledgement */
    ack_payload[0] =  0x55;
    nrf_gzll_add_packet_to_tx_fifo(pipe, ack_payload, ACK_PAYLOAD_LENGTH);
}