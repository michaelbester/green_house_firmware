/******************************************************************************/
/* Project:     Green House		                                     	      */
/*                                                                            */
/* FileName:    main.c                                                        */
/*                                                                            */
/* Author:      Michael Bester                                                */
/*                                                                            */
/* Overview:    This module contains the top-level main function.             */
/*                                                                            */
/* Notes:                                                                     */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "time.h"
#include "nvs_flash.h"
#include "wifi_connect.h"
#include "mqtt.h"
#include "init.h"
#include "update.h"
#include "green_house.h"
#include "driver/gpio.h"
#include "control.h"

/******************************************************************************/
/*************************** Global Variables *********************************/
/******************************************************************************/
struct GREEN_HOUSE_INFO greenHouseInfo;

/******************************************************************************/
/**************************** Local Variables *********************************/
/******************************************************************************/

/******************************************************************************/
/**************************** Local Functions *********************************/
/******************************************************************************/
static void app_loop(void);

/******************************************************************************/
/* Function:    void app_main(void)                                           */
/*                                                                            */
/* Inputs:      None.                                                         */
/*                                                                            */
/* Outputs:     None.                                                         */
/*                                                                            */
/* Description: This is the top-level function.                               */
/*                                                                            */
/* Author:      Michael Bester                                                */
/*                                                                            */
/* Notes:       To run J-TAG debug, first run ESP-IDF command prompt short    */
/*				cut and then run at the prompt: 							  */
/*				openocd -f board/esp32-wrover-kit-3.3v.cfg					  */
/*                                                                            */
/******************************************************************************/
void app_main(void)
{
    nvs_flash_init();

    Init();

    esp_netif_init();

    esp_event_loop_create_default();

    wifi_connect();

    mqtt_start();
    
    app_loop();
}

/******************************************************************************/
/* Function:    static void app_loop(void)                                    */
/*                                                                            */
/* Inputs:      None.                                                         */
/*                                                                            */
/* Outputs:     None.                                                         */
/*                                                                            */
/* Description: This function runs forever in a loop.                         */
/*                                                                            */
/* Author:      Michael Bester                                                */
/*                                                                            */
/* Notes:                                                                     */
/*                                                                            */
/******************************************************************************/
static void app_loop(void)
{
    ESP_LOGI("main", "Starting app_loop %d", portTICK_PERIOD_MS);

    while(1)
    {
        Update_Temperature();              /* Update temperature measurement. */
        Update_Humidity();                    /* Update humidity measurement. */
        update_time();                              /* Update the local time. */
        update_fan();                                     /* Control the fan. */ 
        update_drip(0);                              /* Control drip systems. */
        update_drip(1); 
        update_drip(2); 
    }
}

