/******************************************************************************/
/* Project:     Green House                                       			  */
/*                                                                            */
/* FileName:    control.c                                             		  */
/*                                                                            */
/* Author:      Michael Bester                                                */
/*                                                                            */
/* Overview:    This module contains the mqtt command functions.              */
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
#include "green_house.h"
#include "control.h"

/******************************************************************************/
/*************************** Global Variables *********************************/
/******************************************************************************/

/******************************************************************************/
/**************************** Local Variables *********************************/
/******************************************************************************/
static char *tag = "control";

/******************************************************************************/
/**************************** Local Functions *********************************/
/******************************************************************************/


/******************************************************************************/
/* Function:    void Control_Fan(uint8+t id, char *cmd)                       */
/*                                                                            */
/* Inputs:      id: Fan ID.                                                   */
/*              cmd: pointer to command string.                               */
/*                                                                            */
/* Outputs:     None.                                                         */
/*                                                                            */
/* Description: This function controls the fan.                               */
/*                                                                            */
/* Author:      Michael Bester                                                */
/*                                                                            */
/* Notes:       command comes from mqtt message.                              */
/*                                                                            */
/******************************************************************************/
void Control_Fan(uint8_t id, char* cmd) 
{
    gpio_num_t fanID;
    gpio_num_t ledID;

    switch(id)
    {
        case(1):
            fanID = FAN1_CTRL_PIN;
            ledID = FAN1_LED_PIN;
        break;

        case(2):
            fanID = FAN2_CTRL_PIN;
            ledID = FAN2_LED_PIN;
        break;

        default:
            fanID = FAN1_CTRL_PIN;
            ledID = FAN1_LED_PIN;
        break;
    }

    if(strstr(cmd, "ON"))
    {
        gpio_set_level(fanID, ON);			                  /* Turn on fan. */
        ESP_LOGI(tag, "Fan %d turned on.", id);
        greenHouseInfo.fanState[id-1] = ON;
        gpio_set_level(ledID, 0);                             /* Turn on LED. */
    }
    else if(strstr(cmd, "OFF"))
    {
        gpio_set_level(fanID, OFF);			                 /* Turn off fan. */
        ESP_LOGI(tag, "Fan %d turned off.", id);
        greenHouseInfo.fanState[id-1] = OFF;
        gpio_set_level(ledID, 1);                            /* Turn off LED. */
    }
    else
    {
        ESP_LOGI(tag, "Unknown fan command!");            /* Unknown command. */
        ESP_LOGI(tag, "Command = %s", cmd);
    }
}

/******************************************************************************/
/* Function:    void Control_Fan_Set_Temp_On(char *cmd)                       */
/*                                                                            */
/* Inputs:      cmd: pointer to command string.                               */
/*                                                                            */
/* Outputs:     None.                                                         */
/*                                                                            */
/* Description: This function sets the fan temperature on trip point.         */
/*                                                                            */
/* Author:      Michael Bester                                                */
/*                                                                            */
/* Notes:       command comes from mqtt message.                              */
/*                                                                            */
/******************************************************************************/
void Control_Fan_Set_Temp_On(char *cmd)
{
    int8_t tempVal;

    tempVal = atoi(cmd);

    if(tempVal)
    {
        greenHouseInfo.fanTempOn = tempVal;
    }
}

/******************************************************************************/
/* Function:    void Control_Drip(uint8_t id, char *cmd)                      */
/*                                                                            */
/* Inputs:      id: Fan ID.                                                   */
/*              cmd: pointer to command string.                               */
/*                                                                            */
/* Outputs:     None.                                                         */
/*                                                                            */
/* Description: This function controls the fan.                               */
/*                                                                            */
/* Author:      Michael Bester                                                */
/*                                                                            */
/* Notes:       command comes from mqtt message.                              */
/*                                                                            */
/******************************************************************************/
void Control_Drip(uint8_t id, char* cmd) 
{
    switch(id)
    {
        case(1):
            if(strstr(cmd, "ON"))
            {
                gpio_set_level(DRIP1_OPEN_CTRL_PIN, 0);
                vTaskDelay(100 / portTICK_PERIOD_MS);
                gpio_set_level(DRIP1_CLOSE_CTRL_PIN, 1);
                vTaskDelay(50 / portTICK_PERIOD_MS);
                gpio_set_level(DRIP1_CLOSE_CTRL_PIN, 0);
                gpio_set_level(SOL1_LED_PIN, 0);
                ESP_LOGI(tag, "DRIP1 ON");
                greenHouseInfo.dripInfo[0].state = ON;
            }
            else
            {
                gpio_set_level(DRIP1_CLOSE_CTRL_PIN, 0);
                vTaskDelay(100 / portTICK_PERIOD_MS);
                gpio_set_level(DRIP1_OPEN_CTRL_PIN, 1);
                vTaskDelay(50 / portTICK_PERIOD_MS);
                gpio_set_level(DRIP1_OPEN_CTRL_PIN, 0);
                ESP_LOGI(tag, "DRIP1 OFF");
                greenHouseInfo.dripInfo[0].state = OFF;
                gpio_set_level(SOL1_LED_PIN, 1);
            }
        break;

        case(2):
            if(strstr(cmd, "ON"))
            {
                gpio_set_level(DRIP2_OPEN_CTRL_PIN, 0);
                vTaskDelay(100 / portTICK_PERIOD_MS);
                gpio_set_level(DRIP2_CLOSE_CTRL_PIN, 1);
                vTaskDelay(50 / portTICK_PERIOD_MS);
                gpio_set_level(DRIP2_CLOSE_CTRL_PIN, 0);
                gpio_set_level(SOL2_LED_PIN, 0);
                ESP_LOGI(tag, "DRIP2 ON");
                greenHouseInfo.dripInfo[1].state = ON;
            }
            else
            {
                gpio_set_level(DRIP2_CLOSE_CTRL_PIN, 0);
                vTaskDelay(100 / portTICK_PERIOD_MS);
                gpio_set_level(DRIP2_OPEN_CTRL_PIN, 1);
                vTaskDelay(50 / portTICK_PERIOD_MS);
                gpio_set_level(DRIP2_OPEN_CTRL_PIN, 0);
                gpio_set_level(SOL2_LED_PIN, 1);
                ESP_LOGI(tag, "DRIP2 OFF");
                greenHouseInfo.dripInfo[1].state = OFF;
            }
        break;

        case(3):
            if(strstr(cmd, "ON"))
            {
                gpio_set_level(DRIP3_OPEN_CTRL_PIN, 0);
                vTaskDelay(100 / portTICK_PERIOD_MS);
                gpio_set_level(DRIP3_CLOSE_CTRL_PIN, 1);
                vTaskDelay(50 / portTICK_PERIOD_MS);
                gpio_set_level(DRIP3_CLOSE_CTRL_PIN, 0);
                gpio_set_level(SOL3_LED_PIN, 0);
                ESP_LOGI(tag, "DRIP3 ON");
                greenHouseInfo.dripInfo[2].state = ON;
            }
            else
            {
                gpio_set_level(DRIP3_CLOSE_CTRL_PIN, 0);
                vTaskDelay(100 / portTICK_PERIOD_MS);
                gpio_set_level(DRIP3_OPEN_CTRL_PIN, 1);
                vTaskDelay(50 / portTICK_PERIOD_MS);
                gpio_set_level(DRIP3_OPEN_CTRL_PIN, 0);
                gpio_set_level(SOL3_LED_PIN, 1);
                ESP_LOGI(tag, "DRIP3 OFF");
                greenHouseInfo.dripInfo[2].state = OFF;
            }
        break;

        default:
                gpio_set_level(DRIP1_CLOSE_CTRL_PIN, 0);
                gpio_set_level(DRIP2_CLOSE_CTRL_PIN, 0);
                gpio_set_level(DRIP3_CLOSE_CTRL_PIN, 0);
                gpio_set_level(DRIP1_OPEN_CTRL_PIN, 0);
                gpio_set_level(DRIP2_OPEN_CTRL_PIN, 0);
                gpio_set_level(DRIP3_OPEN_CTRL_PIN, 0);

                gpio_set_level(SOL1_LED_PIN, 1);
                gpio_set_level(SOL2_LED_PIN, 1);
                gpio_set_level(SOL3_LED_PIN, 1);

                ESP_LOGI(tag, "DRIP1 OFF");
                ESP_LOGI(tag, "DRIP2 OFF");
                ESP_LOGI(tag, "DRIP3 OFF");

                greenHouseInfo.dripInfo[0].state = OFF;
                greenHouseInfo.dripInfo[1].state = OFF;
                greenHouseInfo.dripInfo[2].state = OFF;
        break;
    }
}

/******************************************************************************/
/* Function:    void Control_Drip_On_Time(uint8_t id, char *on_time)          */
/*                                                                            */
/* Inputs:      id: Fan ID.                                                   */
/*              on_time: pointer to time string.                              */
/*                                                                            */
/* Outputs:     None.                                                         */
/*                                                                            */
/* Description: This function controls the drip on time.                      */
/*                                                                            */
/* Author:      Michael Bester                                                */
/*                                                                            */
/* Notes:       command comes from mqtt message.                              */
/*                                                                            */
/******************************************************************************/
void Control_Drip_On_Time(uint8_t id, char *on_time)  
{
    char *colon_loc;
    int hour, min;
    
    colon_loc = strchr(on_time, ':');
    hour = atoi(on_time);
    min = atoi(++colon_loc);

    ESP_LOGI(tag, "Drip #%d On Time = %d:%d", id, hour, min);

    hour *= (60 * 60);                      /* Convert to secs from midnight. */
    min *= 60;

    if(strstr(on_time, "PM"))
    {
        hour += (12 * 60 * 60);
    }

    greenHouseInfo.dripInfo[id-1].on_time = (hour + min);

    ESP_LOGI(tag, "on time stored in seconds:  %d", greenHouseInfo.dripInfo[id-1].on_time);
}