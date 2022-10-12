/******************************************************************************/
/* Project:     Green House                                      			  */
/*                                                                            */
/* FileName:    green_house.h       	                                      */
/*                                                                            */
/* Author:      Michael Bester                                                */
/*                                                                            */
/* Overview:    This module contains project wide definitions.	              */
/*                                                                            */
/* Notes:                                                                     */
/*                                                                            */
/******************************************************************************/
#ifndef _GREEN_HOUSE_
#define _GREEN_HOUSE_

/******************************************************************************/
/****************************** Enumerations **********************************/
/******************************************************************************/

/******************************************************************************/
/******************************* Definitions **********************************/
/******************************************************************************/
#define ESP_WIFI_SSID               "home_network"
#define ESP_WIFI_PASS               "chelsea101"
#define ESP_MAXIMUM_RETRY           5

#define OFF                         0
#define ON                          1

#define CLOSE                       0
#define OPEN                        1

#define WIFI_CONNECTED_BIT          BIT0
#define WIFI_FAIL_BIT               BIT1

#define FAN_OFF_TEMP_HYST           2
#define VENT_OFF_TEMP_HYST          5

#define SNTP_UPDATE_INTERVAL_MSEC   (1000*60*60*1)

#define DRIP_SYS                    3
#define FANS                        2

#define FAILED                      0
#define PASSED                      1

/*************************** MICROCONTROLLER PIN-OUT **************************/
#define MISC_LED_PIN                2
#define SOL1_LED_PIN                22
#define SOL2_LED_PIN                21
#define SOL3_LED_PIN                19
#define FAN1_LED_PIN                18
#define FAN2_LED_PIN                5
#define STATUS_LED_PIN              15
#define VENT1_LED_PIN               4

#define FAN1_CTRL_PIN               32
#define FAN2_CTRL_PIN               10 //35
#define VENT1_CTRL_PIN              9 //34
#define MISC1_CTRL_PIN              17 //39

#define DRIP1_OPEN_CTRL_PIN         14
#define DRIP1_CLOSE_CTRL_PIN        12
#define DRIP2_OPEN_CTRL_PIN         26
#define DRIP2_CLOSE_CTRL_PIN        27
#define DRIP3_OPEN_CTRL_PIN         33
#define DRIP3_CLOSE_CTRL_PIN        25

#define DHT11_CTRL_PIN              23

#define EN_24V_PIN                  13

/******************************************************************************/
/******************************** Structures **********************************/
/******************************************************************************/
struct DRIP_INFO
{
    bool state;                                                 /* ON or OFF. */
    int duration;                       /* Water on time duration in seconds. */
    int start_time;           /* Time drip turns on (seconds since midnight). */
    int days;        /* Days to run, (1=Sun,2=Mon,4=Tue,8=Wed,16=Thur, 32=Fri)*/
};

struct GREEN_HOUSE_INFO
{
    int8_t temp;
    uint8_t humidity;
    bool fanState[FANS];
    bool ventState;
    struct DRIP_INFO dripInfo[DRIP_SYS];
    int fanTempOn;
    int fanTempOff;
    int sntpSec;
    int sntpMin;
    int sntpHour;
    int sntpDay;
    int sec;
    int min;
    int hour;
    int day;
    bool timeIsValid;
};

/******************************************************************************/
/**************************** External Variables ******************************/
/******************************************************************************/
extern struct GREEN_HOUSE_INFO greenHouseInfo;



#endif