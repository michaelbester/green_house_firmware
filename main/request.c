/******************************************************************************/
/* Project:     Green House                                       			  */
/*                                                                            */
/* FileName:    request.c                                             		  */
/*                                                                            */
/* Author:      Michael Bester                                                */
/*                                                                            */
/* Overview:    This module contains the mqtt reply functions.                */
/*                                                                            */
/* Notes:                                                                     */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "string.h"
#include "stdio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "nvs_flash.h"
#include "dht11.h"
#include "green_house.h"
#include "request.h"

/******************************************************************************/
/*************************** Global Variables *********************************/
/******************************************************************************/

/******************************************************************************/
/**************************** Local Variables *********************************/
/******************************************************************************/
static char *tag = "request";

/******************************************************************************/
/**************************** Local Functions *********************************/
/******************************************************************************/


/******************************************************************************/
/* Function:    int Request_Temperature(void)                                 */
/*                                                                            */
/* Inputs:      None.                                                         */
/*                                                                            */
/* Outputs:     Temperature in degrees F.                                     */
/*                                                                            */
/* Description: This function reads the temperature sensor.                   */
/*                                                                            */
/* Author:      Michael Bester                                                */
/*                                                                            */
/* Notes:       request comes from mqtt message.                              */
/*                                                                            */
/******************************************************************************/
int Request_Temperature(void)  
{
    int result;
    ESP_LOGI(tag, "Getting Temperature Reading...");

    do
    {
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        result = DHT11_read().temperature;
    } while ((result == DHT11_CRC_ERROR) || (result == -1));
    
    result = ((9.0/5.0)*result) + 32.0;
    ESP_LOGI(tag, "Temperature Read = %d", result);
    return(result);
}

/******************************************************************************/
/* Function:    int Request_Humidity(void)                                    */
/*                                                                            */
/* Inputs:      None.                                                         */
/*                                                                            */
/* Outputs:     Temperature in degrees F.                                     */
/*                                                                            */
/* Description: This function reads the humidity sensor.                      */
/*                                                                            */
/* Author:      Michael Bester                                                */
/*                                                                            */
/* Notes:       request comes from mqtt message.                              */
/*                                                                            */
/******************************************************************************/
int Request_Humidity(void)  
{
    int result;

    do
    {
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        result = DHT11_read().humidity;
    } while ((result == DHT11_CRC_ERROR) || (result == -1));
    
    ESP_LOGI(tag, "Humidity Read = %d", result);
    return(result);
}