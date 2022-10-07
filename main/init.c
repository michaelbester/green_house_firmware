/******************************************************************************/
/* Project:     Green House                                       			  */
/*                                                                            */
/* FileName:    init.c                                                 		  */
/*                                                                            */
/* Author:      Michael Bester                                                */
/*                                                                            */
/* Overview:    This module contains the init functions.                      */
/*                                                                            */
/* Notes:                                                                     */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "dht11.h"
#include "green_house.h"
#include "nvs.h"
#include "init.h"

/******************************************************************************/
/*************************** Global Variables *********************************/
/******************************************************************************/

/******************************************************************************/
/**************************** Local Variables *********************************/
/******************************************************************************/

/******************************************************************************/
/**************************** Local Functions *********************************/
/******************************************************************************/
static void Init_GPIO(void);
static void Init_Settings(void);

/******************************************************************************/
/* Function:    void Init(void)                                               */
/*                                                                            */
/* Inputs:      None.                                                         */
/*                                                                            */
/* Outputs:     None.                                                         */
/*                                                                            */
/* Description: This function initializes the ESP32 peripherials.             */
/*                                                                            */
/* Author:      Michael Bester                                                */
/*                                                                            */
/* Notes:                                                                     */
/*                                                                            */
/******************************************************************************/
void Init(void)
{
	uint8_t i;
    esp_err_t err;

    err = nvs_flash_init();

    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) 
	{
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

	ESP_ERROR_CHECK( err );

    Init_GPIO();                                          /* Initialize GPIO. */
	DHT11_init(DHT11_CTRL_PIN);

	greenHouseInfo.fanState[0] = OFF;
	greenHouseInfo.fanState[1] = OFF;
	greenHouseInfo.ventState = OFF;

	for(i=0;i<DRIP_SYS;i++)
	{
		greenHouseInfo.dripInfo[i].state = OFF;
	}

	greenHouseInfo.timeIsValid = false;

	Init_Settings();							 /* Read settings from Flash. */
}

/******************************************************************************/
/* Function:    static void Init_GPIO(void)					                  */
/*                                                                            */
/* Inputs:      None.                                                         */
/*                                                                            */
/* Outputs:     None.		                                                  */
/*                                                                            */
/* Description: This function initializes ESP32 GPIO pins.                    */
/*                                                                            */
/* Author:      Michael Bester                                                */
/*                                                                            */
/* Notes:                                                                     */
/*                                                                            */
/******************************************************************************/
static void Init_GPIO(void)
{
    gpio_config_t gpioConfig;			/* GPIO pad configuration parameters. */

	/**************************************************************************/
	/*							CONFIGURE GPIO INPUTS						  */
	/**************************************************************************/
    #if 0
	gpioConfig.intr_type = GPIO_INTR_NEGEDGE;
	gpioConfig.mode = GPIO_MODE_INPUT;
	gpioConfig.pull_up_en = GPIO_PULLUP_ENABLE;
	gpioConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
	gpioConfig.pin_bit_mask = (1ULL << PWR_BTN_PIN);

	gpio_config(&gpioConfig);
#endif

	/**************************************************************************/
	/*							CONFIGURE GPIO OUTPUTS						  */
	/**************************************************************************/
	
	gpioConfig.intr_type = GPIO_INTR_DISABLE;
    gpioConfig.mode = GPIO_MODE_OUTPUT;
    gpioConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpioConfig.pull_up_en = GPIO_PULLUP_DISABLE;
	gpioConfig.pin_bit_mask = (1ULL << MISC_LED_PIN | 1ULL << SOL1_LED_PIN |
							   1ULL << SOL2_LED_PIN | 1ULL << SOL3_LED_PIN |
							   1ULL << FAN1_LED_PIN | 1ULL << FAN2_LED_PIN |
							   1ULL << STATUS_LED_PIN | 1ULL << VENT1_LED_PIN |
							   1ULL << FAN1_CTRL_PIN | 1ULL << FAN2_CTRL_PIN |
							   1ULL << VENT1_CTRL_PIN | 1ULL << DRIP1_OPEN_CTRL_PIN |
							   1ULL << DRIP1_CLOSE_CTRL_PIN | 1ULL << DRIP2_OPEN_CTRL_PIN |
							   1ULL << DRIP2_CLOSE_CTRL_PIN | 1ULL << DRIP3_OPEN_CTRL_PIN |
							   1ULL << DRIP3_CLOSE_CTRL_PIN | 1ULL << MISC1_CTRL_PIN |
							   1ULL << EN_24V_PIN);

    gpio_config(&gpioConfig);

	gpioConfig.pull_up_en = GPIO_PULLUP_ENABLE;
	gpioConfig.pin_bit_mask = 1ULL << EN_24V_PIN;
	gpio_config(&gpioConfig);
	gpio_set_level(EN_24V_PIN, 1);						/* Enable 24V output. */

    gpio_set_level(MISC_LED_PIN, 1);						 /* Turn off LED. */
	gpio_set_level(SOL1_LED_PIN, 1);	
	gpio_set_level(SOL2_LED_PIN, 1);	
	gpio_set_level(SOL3_LED_PIN, 1);	
	gpio_set_level(FAN1_LED_PIN, 1);	
	gpio_set_level(FAN2_LED_PIN, 1);	
	gpio_set_level(STATUS_LED_PIN, 1);	
	gpio_set_level(VENT1_LED_PIN, 1);	

	gpio_set_level(FAN1_CTRL_PIN, 0);						/* Turn off fans. */
	gpio_set_level(FAN2_CTRL_PIN, 0);	

	gpio_set_level(VENT1_CTRL_PIN, 0);						/* Turn off vent. */

	gpio_set_level(MISC1_CTRL_PIN, 0);						/* Turn off misc. */

	gpio_set_level(DRIP1_CLOSE_CTRL_PIN, 0);	/* De-energize drip controls. */
	gpio_set_level(DRIP1_OPEN_CTRL_PIN, 0);
	gpio_set_level(DRIP2_CLOSE_CTRL_PIN, 0);
	gpio_set_level(DRIP2_OPEN_CTRL_PIN, 0);
	gpio_set_level(DRIP3_CLOSE_CTRL_PIN, 0);
	gpio_set_level(DRIP3_OPEN_CTRL_PIN, 0);
}

/******************************************************************************/
/* Function:    static void Init_Settings(void)				                  */
/*                                                                            */
/* Inputs:      None.                                                         */
/*                                                                            */
/* Outputs:     None.		                                                  */
/*                                                                            */
/* Description: This function initializes the application settings.           */
/*                                                                            */
/* Author:      Michael Bester                                                */
/*                                                                            */
/* Notes:                                                                     */
/*                                                                            */
/******************************************************************************/
static void Init_Settings(void)	
{
	esp_err_t err;

	err = NVS_Read("FAN_TEMP_ON", &greenHouseInfo.fanTempOn);

	if(err != ESP_OK)
	{
		NVS_Write("FAN_TEMP_ON", 95);
		greenHouseInfo.fanTempOn = 95;
	}

	err = NVS_Read("FAN_TEMP_OFF", &greenHouseInfo.fanTempOn);

	if(err != ESP_OK)
	{
		NVS_Write("FAN_TEMP_OFF", 90);
		greenHouseInfo.fanTempOff = 90;
	}

	err = NVS_Read("DRIP_1_START", &greenHouseInfo.dripInfo[0].start_time);

	if(err != ESP_OK)
	{
		NVS_Write("DRIP_1_START", (1*60*60));
		greenHouseInfo.dripInfo[0].start_time = (1*60*60);
	}

	err = NVS_Read("DRIP_2_START", &greenHouseInfo.dripInfo[1].start_time);

	if(err != ESP_OK)
	{
		NVS_Write("DRIP_2_START", (2*60*60));
		greenHouseInfo.dripInfo[1].start_time = (2*60*60);
	}

	err = NVS_Read("DRIP_3_START", &greenHouseInfo.dripInfo[2].start_time);

	if(err != ESP_OK)
	{
		NVS_Write("DRIP_3_START", (3*60*60));
		greenHouseInfo.dripInfo[0].start_time = (3*60*60);
	}

	err = NVS_Read("DRIP_1_DUR", &greenHouseInfo.dripInfo[0].duration);

	if(err != ESP_OK)
	{
		NVS_Write("DRIP_1_DUR", (5 * 60));
		greenHouseInfo.dripInfo[0].start_time = (5 * 60);
	}

	err = NVS_Read("DRIP_2_DUR", &greenHouseInfo.dripInfo[1].duration);

	if(err != ESP_OK)
	{
		NVS_Write("DRIP_2_DUR", (1*60*60));
		greenHouseInfo.dripInfo[1].start_time = (5 * 60);
	}

	err = NVS_Read("DRIP_3_DUR", &greenHouseInfo.dripInfo[2].duration);

	if(err != ESP_OK)
	{
		NVS_Write("DRIP_3_DUR", (3*60*60));
		greenHouseInfo.dripInfo[2].start_time = (5 * 60);
	}

	ESP_LOGI("init", "DRIP 1 START TIME:  %d", greenHouseInfo.dripInfo[0].start_time);
	ESP_LOGI("init", "DRIP 2 START TIME:  %d", greenHouseInfo.dripInfo[1].start_time);
	ESP_LOGI("init", "DRIP 3 START TIME:  %d", greenHouseInfo.dripInfo[2].start_time);

	ESP_LOGI("init", "DRIP 1 DURATION:  %d", greenHouseInfo.dripInfo[0].duration);
	ESP_LOGI("init", "DRIP 2 DURATION:  %d", greenHouseInfo.dripInfo[1].duration);
	ESP_LOGI("init", "DRIP 3 DURATION:  %d", greenHouseInfo.dripInfo[2].duration);

}