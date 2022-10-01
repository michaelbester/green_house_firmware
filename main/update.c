/******************************************************************************/
/* Project:     Green House                                       			  */
/*                                                                            */
/* FileName:    update.c                                              		  */
/*                                                                            */
/* Author:      Michael Bester                                                */
/*                                                                            */
/* Overview:    This module contains the update functions.                    */
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
#include "sntp.h"
#include "green_house.h"
#include "mqtt.h"
#include "request.h"
#include "control.h"

/******************************************************************************/
/*************************** Global Variables *********************************/
/******************************************************************************/
int8_t temp = 0;
int8_t humidity = 0;
char tempHistory[TEMP_HISTORY_DEPTH] = {0};
char humidityHistory[HUMIDITY_HISTORY_DEPTH] = {0};
char dummyHistory[TEMP_HISTORY_DEPTH];

/******************************************************************************/
/**************************** Local Variables *********************************/
/******************************************************************************/
static const char *tag = "update";

/******************************************************************************/
/**************************** Local Functions *********************************/
/******************************************************************************/


/******************************************************************************/
/* Function:    void update_fan(void)                                         */
/*                                                                            */
/* Inputs:      None.                                                         */
/*                                                                            */
/* Outputs:     None.                                                         */
/*                                                                            */
/* Description: This function updates the fan control.                        */
/*                                                                            */
/* Author:      Michael Bester                                                */
/*                                                                            */
/* Notes:                                                                     */
/*                                                                            */
/******************************************************************************/
void update_fan(void)
{
    static bool fanStatus = OFF;

    if((greenHouseInfo.temp >= greenHouseInfo.fanTempOn) && (fanStatus == OFF))
    {
        fanStatus = ON;
        gpio_set_level(FAN1_CTRL_PIN, ON);			          /* Turn on fan. */
        gpio_set_level(FAN2_CTRL_PIN, ON);			          /* Turn on fan. */
        gpio_set_level(FAN1_LED_PIN, 0);                      /* Turn on led. */
        gpio_set_level(FAN2_LED_PIN, 0);                      /* Turn on led. */
    }
    else if((greenHouseInfo.temp <= 
            (greenHouseInfo.fanTempOn - FAN_OFF_TEMP_HYST) && (fanStatus == true)))
    {
        fanStatus = OFF;
        gpio_set_level(FAN1_CTRL_PIN, OFF);			         /* Turn off fan. */     
        gpio_set_level(FAN2_CTRL_PIN, OFF);			         /* Turn off fan. */    
        gpio_set_level(FAN1_LED_PIN, 1);                     /* Turn off led. */
        gpio_set_level(FAN2_LED_PIN, 1);                     /* Turn off led. */
    }
}

/******************************************************************************/
/* Function:    void update_drip(uint8_t num)                                 */
/*                                                                            */
/* Inputs:      num:  Drip system number to update.                           */
/*                                                                            */
/* Outputs:     None.                                                         */
/*                                                                            */
/* Description: This function updates the given drip system.                  */
/*                                                                            */
/* Author:      Michael Bester                                                */
/*                                                                            */
/* Notes:                                                                     */
/*                                                                            */
/******************************************************************************/
void update_drip(uint8_t num)
{
    static bool dripStatus[DRIP_SYS] = {OFF};
    int currTimeInSec;
    int offTimeInSec;
    static int prevTimeInSec = 0;
    static bool timeRollOver = false;
    
    currTimeInSec = greenHouseInfo.sec + (greenHouseInfo.min * 60) +
                    (greenHouseInfo.hour * 3600);
    
    offTimeInSec = greenHouseInfo.dripInfo[num].on_time + greenHouseInfo.dripInfo[num].duration;
    offTimeInSec = offTimeInSec % (60*60*24);

    if(currTimeInSec < prevTimeInSec)
    {
        timeRollOver = true;
    }

    if(dripStatus[num] == OFF)
    {
        if((currTimeInSec >= greenHouseInfo.dripInfo[num].on_time) && 
           (currTimeInSec < greenHouseInfo.dripInfo[num].on_time + 
            greenHouseInfo.dripInfo[num].duration))
        {
            dripStatus[num] = ON;
            Control_Drip(num+1,"ON");                        /* Turn on drip. */    
            timeRollOver = false;
            greenHouseInfo.dripInfo[num].state = ON;
        }
    }
    else if(offTimeInSec < greenHouseInfo.dripInfo[num].on_time)
    {
        if((currTimeInSec >= offTimeInSec) && (timeRollOver == true))
        {
            dripStatus[num] = OFF;
            Control_Drip(num+1,"Off");                      /* Turn off drip. */
            greenHouseInfo.dripInfo[num].state = OFF;
        }
    }
    else
    {
        if(currTimeInSec >= offTimeInSec)
        {
            dripStatus[num] = OFF;
            Control_Drip(num+1,"Off");                      /* Turn off drip. */
            greenHouseInfo.dripInfo[num].state = OFF;
        }
    }

    prevTimeInSec = currTimeInSec;
}

/******************************************************************************/
/* Function:    void update_time(void)                                        */
/*                                                                            */
/* Inputs:      None.                                                         */
/*                                                                            */
/* Outputs:     None.                                                         */
/*                                                                            */
/* Description: This function updates the current time.                       */
/*                                                                            */
/* Author:      Michael Bester                                                */
/*                                                                            */
/* Notes:       SNTP server will update time periodically to keep rtc from    */
/*              drifting off to much.                                         */
/*                                                                            */
/******************************************************************************/
void update_time(void)
{
    int mSecCount;
    int secCount;
    static int mSecLastSNTP;
    static bool initSNTPTime = false;
    bool result;

    if(initSNTPTime == false)           /* Get internet time when first run. */
    {
        result = sntp_get_time(); 
        if(result == PASSED)
        {
            initSNTPTime = true;
            mSecLastSNTP = (xTaskGetTickCount() * portTICK_PERIOD_MS);
        }   
        else
        {
            initSNTPTime = false;
        }
    }

    if((xTaskGetTickCount() * portTICK_PERIOD_MS) > (mSecLastSNTP + SNTP_UPDATE_INTERVAL_MSEC))
    {
        sntp_get_time();    
        mSecLastSNTP = (xTaskGetTickCount() * portTICK_PERIOD_MS);     
    }

    mSecCount = (xTaskGetTickCount() * portTICK_PERIOD_MS);
    mSecCount = mSecCount - mSecLastSNTP;
    secCount  = mSecCount / 1000;

    greenHouseInfo.sec = greenHouseInfo.sntpSec + secCount;
    greenHouseInfo.sec = greenHouseInfo.sec % 60;
    
    greenHouseInfo.min = (greenHouseInfo.sntpMin + ((greenHouseInfo.sntpSec + secCount) / 60));
    greenHouseInfo.min = (greenHouseInfo.min % 60);

    greenHouseInfo.hour = (greenHouseInfo.sntpHour + ((greenHouseInfo.sntpSec + secCount) / 3600));
    greenHouseInfo.hour = (greenHouseInfo.hour % 24);

    ESP_LOGI(tag, "Time ->  %02d:%02d:%02d", greenHouseInfo.hour, greenHouseInfo.min, greenHouseInfo.sec);

}

/******************************************************************************/
/* Function:    void Update_Temperature(void)                                 */
/*                                                                            */
/* Inputs:      None.                                                         */
/*                                                                            */
/* Outputs:     None.                                                         */
/*                                                                            */
/* Description: This function gets a new temperature measurement.             */
/*                                                                            */
/* Author:      Michael Bester                                                */
/*                                                                            */
/* Notes:                                                                     */
/*                                                                            */
/******************************************************************************/
void Update_Temperature(void)  
{
    memcpy(&dummyHistory, tempHistory, TEMP_HISTORY_DEPTH);
    temp = Request_Temperature();
    tempHistory[0] = (uint8_t)temp;
    memcpy(&tempHistory[1], dummyHistory, TEMP_HISTORY_DEPTH-1);
} 

/******************************************************************************/
/* Function:    void MQTT_Update_Humidity(void)                               */
/*                                                                            */
/* Inputs:      None.                                                         */
/*                                                                            */
/* Outputs:     None.                                                         */
/*                                                                            */
/* Description: This function gets a new humidity measurement.                */
/*                                                                            */
/* Author:      Michael Bester                                                */
/*                                                                            */
/* Notes:                                                                     */
/*                                                                            */
/******************************************************************************/
void Update_Humidity(void)  
{    
    memcpy(&dummyHistory, humidityHistory, HUMIDITY_HISTORY_DEPTH);
    humidity = Request_Humidity();
    humidityHistory[0] = humidity;
    memcpy(&humidityHistory[1], dummyHistory, HUMIDITY_HISTORY_DEPTH-1);
} 