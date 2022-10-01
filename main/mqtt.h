/******************************************************************************/
/* Project:     Green House                                       			  */
/*                                                                            */
/* FileName:    mqtt.h              	                                      */
/*                                                                            */
/* Author:      Michael Bester                                                */
/*                                                                            */
/* Overview:    This module contains                                          */
/*                                                                            */
/* Notes:                                                                     */
/*                                                                            */
/******************************************************************************/
#ifndef _MQTT_
#define _MQTT_

/******************************************************************************/
/**************************** External Variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Enumerations **********************************/
/******************************************************************************/

/******************************************************************************/
/******************************* Definitions **********************************/
/******************************************************************************/
//#define CONFIG_BROKER_URL                         "mqtt://mqtt.iotwithus.com" 
#define CONFIG_BROKER_URL                           "mqtt://test.mosquitto.org"

#define GREEN_HOUSE_TEMP_REQUEST_TOPIC              "/greenhouse/temp/request"
#define GREEN_HOUSE_TEMP_STATUS_TOPIC               "/greenhouse/temp"
#define GREEN_HOUSE_TEMP_HISTORY_REQUEST_TOPIC      "/greenhouse/temp/history/request"
#define GREEN_HOUSE_TEMP_HISTORY_TOPIC              "/greenhouse/temp/history"

#define GREEN_HOUSE_HUMIDITY_REQUEST_TOPIC          "/greenhouse/humidity/request"
#define GREEN_HOUSE_HUMIDITY_STATUS_TOPIC           "/greenhouse/humidity"
#define GREEN_HOUSE_HUMIDITY_HISTORY_REQUEST_TOPIC  "/greenhouse/humidity/history/request"
#define GREEN_HOUSE_HUMIDITY_HISTORY_TOPIC          "/greenhouse/humidity/history"

#define GREEN_HOUSE_TIME_TOPIC                      "/greenhouse/time"

/*** FAN INTERFACE ***/
#define GREEN_HOUSE_FAN_GET_STATE_SUB_TOPIC         "/greenhouse/fan/get_state_sub"
#define GREEN_HOUSE_FAN_GET_STATE_PUB_TOPIC         "/greenhouse/fan/get_state_pub"

#define GREEN_HOUSE_FAN_SET_STATE_SUB_TOPIC         "/greenhouse/fan/set_state_sub"

#define GREEN_HOUSE_FAN_GET_TEMP_ON_SUB_TOPIC       "/greenhouse/fan/get_temp_on_sub"
#define GREEN_HOUSE_FAN_GET_TEMP_ON_PUB_TOPIC       "/greenhouse/fan/get_temp_on_pub"

#define GREEN_HOUSE_FAN_SET_TEMP_ON_SUB_TOPIC       "/greenhouse/fan/set_temp_on_sub"

/*** DRIP #1 INTERFACE ***/
#define GREEN_HOUSE_DRIP1_REQUEST_STATUS_TOPIC      "/greenhouse/drip1/status/request"
#define GREEN_HOUSE_DRIP1_STATUS_TOPIC              "/greenhouse/drip1/status"
#define GREEN_HOUSE_DRIP1_CTRL_TOPIC                "/greenhouse/drip1/ctrl"
#define GREEN_HOUSE_DRIP1_ON_TIME_TOPIC             "/greenhouse/drip1/ontime"
#define GREEN_HOUSE_DRIP1_DURATION_TOPIC            "/greenhouse/drip1/duration"

/*** DRIP #2 INTERFACE ***/
#define GREEN_HOUSE_DRIP2_REQUEST_STATUS_TOPIC      "/greenhouse/drip2/status/request"
#define GREEN_HOUSE_DRIP2_STATUS_TOPIC              "/greenhouse/drip2/status"
#define GREEN_HOUSE_DRIP2_CTRL_TOPIC                "/greenhouse/drip2/ctrl"
#define GREEN_HOUSE_DRIP2_ON_TIME_TOPIC             "/greenhouse/drip2/ontime"
#define GREEN_HOUSE_DRIP2_DURATION_TOPIC            "/greenhouse/drip2/duration"

/*** DRIP #3 INTERFACE ***/
#define GREEN_HOUSE_DRIP3_REQUEST_STATUS_TOPIC      "/greenhouse/drip3/status/request"
#define GREEN_HOUSE_DRIP3_STATUS_TOPIC              "/greenhouse/drip3/status"
#define GREEN_HOUSE_DRIP3_CTRL_TOPIC                "/greenhouse/drip3/ctrl"
#define GREEN_HOUSE_DRIP3_ON_TIME_TOPIC             "/greenhouse/drip3/ontime"
#define GREEN_HOUSE_DRIP3_DURATION_TOPIC            "/greenhouse/drip3/duration"

/*** GROW LIGHT INTERFACE ***/
#define GREEN_HOUSE_GROWLIGHT_STATUS_TOPIC          "/greenhouse/growlight/status"
#define GREEN_HOUSE_GROWLIGHT_CTRL_TOPIC            "/greenhouse/growlight/ctrl"
#define GREEN_HOUSE_GROWLIGHT_ON_TIME_TOPIC         "/greenhouse/growlight/ontime"
#define GREEN_HOUSE_GROWLIGHT_OFF_TIME_TOPIC        "/greenhouse/growlight/offtime"

/*** SOIL MOISTURE INTERFACE ***/
/* TBD */

#define TEMP_HISTORY_DEPTH          (12 * 24)  
#define HUMIDITY_HISTORY_DEPTH      (12 * 24)

/******************************************************************************/
/******************************** Prototypes **********************************/
/******************************************************************************/
void mqtt_start(void);
void MQTT_Send_Temp(void);
void MQTT_Send_Humidty(void);

#endif