/******************************************************************************/
/* Project:     Green House                                       			  */
/*                                                                            */
/* FileName:    sntp.c                                                		  */
/*                                                                            */
/* Author:      Michael Bester                                                */
/*                                                                            */
/* Overview:    This module contains the simple network time protocol         */
/*              functions.                                                    */
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
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_sntp.h"
#include "sntp.h"
#include "green_house.h"

/******************************************************************************/
/*************************** Global Variables *********************************/
/******************************************************************************/

/******************************************************************************/
/**************************** Local Variables *********************************/
/******************************************************************************/
static char *tag = "sntp";

/******************************************************************************/
/**************************** Local Functions *********************************/
/******************************************************************************/
static void initialize_sntp(void);
static void time_sync_notification_cb(struct timeval *tv);

/******************************************************************************/
/* Function:    bool sntp_get_time(void)                                      */
/*                                                                            */
/* Inputs:      None.                                                         */
/*                                                                            */
/* Outputs:     PASS or FAIL                                                  */
/*                                                                            */
/* Description: This function gets the current time via the internet.         */
/*                                                                            */
/* Author:      Michael Bester                                                */
/*                                                                            */
/* Notes:       current time is saved withing the greenhouse info struct.     */
/*                                                                            */
/******************************************************************************/
bool sntp_get_time(void)  
{
    time_t now = 0;
    struct tm timeinfo = {0};
    int retry = 0;
    const int retry_count = 15;
    char strftime_buf[64];
    bool result;

    ESP_LOGI(tag, "Getting internet time...");

    time(&now);
    localtime_r(&now, &timeinfo);
    
    sntp_servermode_dhcp(1);       // accept NTP offers from DHCP server, if any
    initialize_sntp();


    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) 
    {
        ESP_LOGI(tag, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }

    if(retry == retry_count)
    {
        result = FAILED;
    }
    else
    {
        result = PASSED;
    }

    time(&now);
    localtime_r(&now, &timeinfo);
    time(&now);
    setenv("TZ", "EST5EDT,M3.2.0/2,M11.1.0", 1);
    tzset();

    localtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    ESP_LOGI(tag, "The current date/time in New York is: %s", strftime_buf);

    greenHouseInfo.sntpSec = timeinfo.tm_sec;
    greenHouseInfo.sntpMin = timeinfo.tm_min;
    greenHouseInfo.sntpHour = timeinfo.tm_hour;

    sntp_stop();

    return(result);
}




static void initialize_sntp(void)
{
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_set_time_sync_notification_cb(time_sync_notification_cb);
    sntp_init();
    for (uint8_t i = 0; i < SNTP_MAX_SERVERS; ++i){
        if (sntp_getservername(i)){
            ESP_LOGI(tag, "server %d: %s", i, sntp_getservername(i));
        } else {
            // we have either IPv4 or IPv6 address, let's print it
            char buff[46];
            ip_addr_t const *ip = sntp_getserver(i);
            if (ipaddr_ntoa_r(ip, buff, 46) != NULL)
                ESP_LOGI(tag, "server %d: %s", i, buff);
        }
    }
}


static void time_sync_notification_cb(struct timeval *tv)
{
    ESP_LOGI(tag, "Notification of a time synchronization event");
}