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
#include "mqtt_client.h"
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
/* Function:    bool update_drip(uint8_t num)                                 */
/*                                                                            */
/* Inputs:      num:  Drip system number to update.                           */
/*                                                                            */
/* Outputs:     Change flag.                                                  */
/*                                                                            */
/* Description: This function updates the given drip system.                  */
/*                                                                            */
/* Author:      Michael Bester                                                */
/*                                                                            */
/* Notes:                                                                     */
/*                                                                            */
/******************************************************************************/
bool update_drip(uint8_t num)
{
    static bool dripStatus[DRIP_SYS] = {OFF};
    int currTimeInMin;
    int offTimeInMin;
    static int prevTimeInMin = 0;
    static bool timeRollOver = false;
    bool result = false;
    
    currTimeInMin = (greenHouseInfo.min +(greenHouseInfo.hour * 60));
    
    offTimeInMin = greenHouseInfo.dripInfo[num].start_time + greenHouseInfo.dripInfo[num].duration;
    offTimeInMin = offTimeInMin % (60*24);

    if(currTimeInMin < prevTimeInMin)
    {
        timeRollOver = true;
    }

    if(dripStatus[num] == OFF)
    {
        if((currTimeInMin >= greenHouseInfo.dripInfo[num].start_time) && 
           (currTimeInMin < greenHouseInfo.dripInfo[num].start_time + 
            greenHouseInfo.dripInfo[num].duration) && (greenHouseInfo.dripInfo[num].days & (1 <<greenHouseInfo.day)))
        {
            dripStatus[num] = ON;
            Control_Drip(num+1,"ON");                        /* Turn on drip. */    
            timeRollOver = false;
            greenHouseInfo.dripInfo[num].state = ON;
            result = true;
        }
    }
    else if(offTimeInMin < greenHouseInfo.dripInfo[num].start_time)
    {
        if((currTimeInMin >= offTimeInMin) && (timeRollOver == true))
        {
            dripStatus[num] = OFF;
            Control_Drip(num+1,"Off");                      /* Turn off drip. */
            greenHouseInfo.dripInfo[num].state = OFF;
            result = true;
        }
    }
    else
    {
        if((currTimeInMin >= offTimeInMin))
        {
            dripStatus[num] = OFF;
            Control_Drip(num+1,"Off");                      /* Turn off drip. */
            greenHouseInfo.dripInfo[num].state = OFF;
            result = true;
        }
    }

    prevTimeInMin = currTimeInMin;

    return(result);
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
    bool result = FAILED;

    if(greenHouseInfo.timeIsValid  == false)      /* Get time when first run. */
    {
        result = sntp_get_time(); 

        if(result == PASSED)
        {
            mSecLastSNTP = (xTaskGetTickCount() * portTICK_PERIOD_MS);
            greenHouseInfo.timeIsValid = true;
        }   
        else
        {
            greenHouseInfo.timeIsValid = false;
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
    greenHouseInfo.day = greenHouseInfo.sntpDay;

    ESP_LOGI(tag, "Time ->  %02d:%02d:%02d", greenHouseInfo.hour, greenHouseInfo.min, greenHouseInfo.sec);
    ESP_LOGI(tag, "Day  ->  %d", greenHouseInfo.day);


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
    char result_string[16] = {0};

    memcpy(&dummyHistory, tempHistory, TEMP_HISTORY_DEPTH);
    temp = Request_Temperature();
    tempHistory[0] = (uint8_t)temp;
    memcpy(&tempHistory[1], dummyHistory, TEMP_HISTORY_DEPTH-1);

    if(tempHistory[0] != tempHistory[1])                /* Publish if change. */
    {
        memset(result_string, 0x00, sizeof(temp));
        itoa(temp, result_string, 10);
        esp_mqtt_client_publish(client, GREEN_HOUSE_TEMP_STATUS_TOPIC, 
                                result_string, 2, 0, 0);
    }
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
    char result_string[16] = {0};
    
    memcpy(&dummyHistory, humidityHistory, HUMIDITY_HISTORY_DEPTH);
    humidity = Request_Humidity();
    humidityHistory[0] = humidity;
    memcpy(&humidityHistory[1], dummyHistory, HUMIDITY_HISTORY_DEPTH-1);

    if(humidityHistory[0] != humidityHistory[1])       /* Publish if change. */
    {
        memset(result_string, 0x00, sizeof(temp));
        itoa(temp, result_string, 10);
        esp_mqtt_client_publish(client, GREEN_HOUSE_HUMIDITY_STATUS_TOPIC, 
                                result_string, 2, 0, 0);
    }
} 