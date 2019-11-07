#include "nordic_common.h"
#include "nrf.h"
#include "nrf_soc.h"
#include "nrf_drv_saadc.h"


#define ADC_REF_VOLTAGE_IN_MILLIVOLTS   600  /**< Reference voltage (in milli volts) used by ADC while doing conversion. */
#define ADC_PRE_SCALING_COMPENSATION    6    /**< The ADC is configured to use VDD with 1/3 prescaling as input. And hence the result of conversion is to be multiplied by 3 to get the actual value of the battery voltage.*/
#define ADC_RES_10BIT                   1024 /**< Maximum digital value for 10-bit ADC conversion. */
#define ADC_NUM_SAMPLES                 1

#define ADC_RESULT_IN_MILLI_VOLTS(ADC_VALUE)\
        ((((ADC_VALUE) * ADC_REF_VOLTAGE_IN_MILLIVOLTS) / ADC_RES_10BIT) * ADC_PRE_SCALING_COMPENSATION)

static nrf_saadc_value_t adc_buf;

static volatile uint8_t battery_level;

void saadc_event_handler(nrf_drv_saadc_evt_t const * p_event)
{
    if (p_event->type == NRF_DRV_SAADC_EVT_DONE)
    {
        nrf_saadc_value_t adc_value;
        uint16_t          batt_lvl_in_milli_volts;

        adc_value = p_event->data.done.p_buffer[0];

        batt_lvl_in_milli_volts = ADC_RESULT_IN_MILLI_VOLTS(adc_value);
        battery_level = battery_level_in_percent(batt_lvl_in_milli_volts);
    }
}

static void adc_configure(void)
{

    nrf_drv_saadc_config_t saadc_config;

    saadc_config.low_power_mode = true;
    saadc_config.resolution = NRF_SAADC_RESOLUTION_12BIT;
    saadc_config.oversample = NRF_SAADC_OVERSAMPLE_4X;
    saadc_config.interrupt_priority = APP_IRQ_PRIORITY_LOW;

    ret_code_t err_code = nrf_drv_saadc_init(&saadc_config, saadc_event_handler);
    APP_ERROR_CHECK(err_code);

    nrf_saadc_channel_config_t config =
        NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_VDD);

    err_code = nrf_drv_saadc_channel_init(0, &config);
    APP_ERROR_CHECK(err_code);

    /* Enable burst mode */
    NRF_SAADC->CH[0].CONFIG |= 0x01000000;		
}



void bat_level_init(void)
{
    adc_configure();
}

void bat_level_update(void)
{
    ret_code_t err_code;

    err_code = nrf_drv_saadc_buffer_convert(&adc_buf, ADC_NUM_SAMPLES);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_saadc_sample();
    APP_ERROR_CHECK(err_code); 
}

void bat_level_uninit(void)
{
    nrf_drv_saadc_uninit();
}


uint8_t bat_level_read(void)
{
    return battery_level;
}

