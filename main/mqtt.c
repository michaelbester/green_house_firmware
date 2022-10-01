/******************************************************************************/
/* Project:     Green House		                                     	      */
/*                                                                            */
/* FileName:    mqtt.c                                                        */
/*                                                                            */
/* Author:      Michael Bester                                                */
/*                                                                            */
/* Overview:    This module contains the functions releated to the mqtt       */
/*              connection.                                                   */
/*                                                                            */
/* Notes:                                                                     */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "esp_log.h"
#include "esp_event.h"
#include "mqtt_client.h"
#include "control.h"
#include "request.h"
#include "mqtt.h"
#include "update.h"
#include "green_house.h"

/******************************************************************************/
/**************************** Local Variables *********************************/
/******************************************************************************/
static const char *TAG = "mqtt";
static esp_mqtt_client_handle_t client;

/******************************************************************************/
/**************************** Local Functions *********************************/
/******************************************************************************/
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, 
                               int32_t event_id, void *event_data);
static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event);
static void log_error_if_nonzero(const char * message, int error_code);

/******************************************************************************/
/* Function:    void mqtt_start(void)                                         */
/*                                                                            */
/* Inputs:      None.                                                         */
/*                                                                            */
/* Outputs:     None.                                                         */
/*                                                                            */
/* Description: This is the                                                   */
/*                                                                            */
/* Author:      Michael Bester                                                */
/*                                                                            */
/* Notes:                                                                     */
/*                                                                            */
/******************************************************************************/
void mqtt_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = CONFIG_BROKER_URL,
        };

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, 
                                   client);
    esp_mqtt_client_start(client);
}

/******************************************************************************/
/* Function:    static void mqtt_event_handler(void *handler_args,            */
/*                                             esp_event_base_t base,         */
/*                                             int32_t event_id,              */
/*                                             void *event_data)              */
/*                                                                            */
/* Inputs:      handler_args:                                                 */
/*              base:                                                         */
/*              event_id:                                                     */
/*              event_data:                                                   */
/*                                                                            */
/* Outputs:     None.                                                         */
/*                                                                            */
/* Description: This is the                                                   */
/*                                                                            */
/* Author:      Michael Bester                                                */
/*                                                                            */
/* Notes:                                                                     */
/*                                                                            */
/******************************************************************************/
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, 
                               int32_t event_id, void *event_data) 
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", 
             base, event_id);

    mqtt_event_handler_cb(event_data);
}

/******************************************************************************/
/* Function:    static esp_err_t mqtt_event_handler_cb                        */
/*                                          (esp_mqtt_event_handle_t event)   */
/*                                                                            */
/* Inputs:      event:                                                        */
/*                                                                            */
/* Outputs:     None.                                                         */
/*                                                                            */
/* Description: This is the                                                   */
/*                                                                            */
/* Author:      Michael Bester                                                */
/*                                                                            */
/* Notes:                                                                     */
/*                                                                            */
/******************************************************************************/
static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    client = event->client;
    int msg_id;
    int8_t result;
    char result_string[16] = {0};

    switch (event->event_id) 
    {
        case MQTT_EVENT_CONNECTED:
            msg_id = esp_mqtt_client_subscribe(client, GREEN_HOUSE_TEMP_REQUEST_TOPIC, 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, GREEN_HOUSE_TEMP_HISTORY_REQUEST_TOPIC, 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, GREEN_HOUSE_HUMIDITY_REQUEST_TOPIC, 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);


            msg_id = esp_mqtt_client_subscribe(client, GREEN_HOUSE_HUMIDITY_HISTORY_REQUEST_TOPIC, 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, GREEN_HOUSE_TIME_TOPIC, 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            /*** FAN ***/
            msg_id = esp_mqtt_client_subscribe(client, GREEN_HOUSE_FAN_SET_STATE_SUB_TOPIC, 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, GREEN_HOUSE_FAN_GET_STATE_SUB_TOPIC, 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, GREEN_HOUSE_FAN_SET_TEMP_ON_SUB_TOPIC, 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
            
            msg_id = esp_mqtt_client_subscribe(client, GREEN_HOUSE_FAN_GET_TEMP_ON_SUB_TOPIC, 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            /*** DRIP #1 ***/
            msg_id = esp_mqtt_client_subscribe(client, GREEN_HOUSE_DRIP1_REQUEST_STATUS_TOPIC, 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, GREEN_HOUSE_DRIP1_CTRL_TOPIC, 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, GREEN_HOUSE_DRIP1_ON_TIME_TOPIC, 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, GREEN_HOUSE_DRIP1_DURATION_TOPIC, 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            /*** DRIP #2 ***/
            msg_id = esp_mqtt_client_subscribe(client, GREEN_HOUSE_DRIP2_REQUEST_STATUS_TOPIC, 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, GREEN_HOUSE_DRIP2_CTRL_TOPIC, 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, GREEN_HOUSE_DRIP2_ON_TIME_TOPIC, 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, GREEN_HOUSE_DRIP2_DURATION_TOPIC, 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

             /*** DRIP #3 ***/
            msg_id = esp_mqtt_client_subscribe(client, GREEN_HOUSE_DRIP3_REQUEST_STATUS_TOPIC, 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, GREEN_HOUSE_DRIP3_CTRL_TOPIC, 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, GREEN_HOUSE_DRIP3_ON_TIME_TOPIC, 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, GREEN_HOUSE_DRIP3_DURATION_TOPIC, 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
#if 0

            /*** GROW LIGHT ***/
            msg_id = esp_mqtt_client_subscribe(client, GREEN_HOUSE_GROWLIGHT_STATUS_TOPIC, 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, GREEN_HOUSE_GROWLIGHT_CTRL_TOPIC, 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, GREEN_HOUSE_GROWLIGHT_ON_TIME_TOPIC, 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, GREEN_HOUSE_GROWLIGHT_OFF_TIME_TOPIC, 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
#endif
        break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            //msg_id = esp_mqtt_client_publish(client, "/greenhouse/temp", "87", 2, 1, 0);
            //ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        break;

        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;

        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;

        case MQTT_EVENT_DATA:           
            if(strstr(event->topic, GREEN_HOUSE_TEMP_REQUEST_TOPIC) != NULL)
            {
                ESP_LOGI(TAG, "Received temperature request command!");
                memset(result_string, 0x00, sizeof(temp));
                itoa(temp, result_string, 10);
                esp_mqtt_client_publish(client, GREEN_HOUSE_TEMP_STATUS_TOPIC, 
                                        result_string, 2, 0, 0);
            }

            if(strstr(event->topic, GREEN_HOUSE_TEMP_HISTORY_REQUEST_TOPIC) != NULL)
            {
                ESP_LOGI(TAG, "Received temperature history request command!");
                esp_mqtt_client_publish(client, GREEN_HOUSE_TEMP_HISTORY_TOPIC, 
                                        tempHistory, TEMP_HISTORY_DEPTH, 0, 0);
            }

            if(strstr(event->topic, GREEN_HOUSE_HUMIDITY_REQUEST_TOPIC) != NULL)
            {
                ESP_LOGI(TAG, "Received humidity request command!");
                memset(result_string, 0x00, sizeof(humidity));
                itoa(humidity, result_string, 10);
                esp_mqtt_client_publish(client, GREEN_HOUSE_HUMIDITY_STATUS_TOPIC, 
                                        result_string, 2, 0, 0);
            }

            if(strstr(event->topic, GREEN_HOUSE_HUMIDITY_HISTORY_REQUEST_TOPIC) != NULL)
            {
                ESP_LOGI(TAG, "Received humidity history request command!");
                esp_mqtt_client_publish(client, GREEN_HOUSE_HUMIDITY_HISTORY_TOPIC, 
                                        humidityHistory, TEMP_HISTORY_DEPTH, 0, 0);
            }

            /*** FAN CONTROL & SETTINGS ***/
            if(strstr(event->topic, GREEN_HOUSE_FAN_SET_STATE_SUB_TOPIC) != NULL)
            {
                ESP_LOGI(TAG, "Fan control request command!");
                Control_Fan(1, event->data);
                Control_Fan(2, event->data);
            }

            if(strstr(event->topic, GREEN_HOUSE_FAN_GET_STATE_SUB_TOPIC) != NULL)
            {
                ESP_LOGI(TAG, "Fan status request command!");
                if(greenHouseInfo.fanState[0] == ON)
                {
                    sprintf(result_string, "ON");
                }
                else
                {
                    sprintf(result_string, "OFF");
                }

                esp_mqtt_client_publish(client, GREEN_HOUSE_FAN_GET_STATE_PUB_TOPIC, 
                                        result_string, strlen(result_string), 0, 0);
            }

            if(strstr(event->topic, GREEN_HOUSE_FAN_SET_TEMP_ON_SUB_TOPIC) != NULL)
            {
                ESP_LOGI(TAG, "Fan set temperature on setting command!");
                Control_Fan_Set_Temp_On(event->data);
            }

            if(strstr(event->topic, GREEN_HOUSE_FAN_GET_TEMP_ON_SUB_TOPIC) != NULL)
            {
                ESP_LOGI(TAG, "Fan get temp setpoint command!");
                itoa(greenHouseInfo.fanTempOn, result_string, 10);
                esp_mqtt_client_publish(client, GREEN_HOUSE_FAN_GET_TEMP_ON_PUB_TOPIC, 
                                        result_string, strlen(result_string), 0, 0);
            }
            
            /*** DRIP CONTROL & SETTINGS ***/
            if(strstr(event->topic, GREEN_HOUSE_DRIP1_CTRL_TOPIC) != NULL)
            {
                Control_Drip(1, event->data);
            }

            if(strstr(event->topic, GREEN_HOUSE_DRIP2_CTRL_TOPIC) != NULL)
            {
                Control_Drip(2, event->data);
            }

            if(strstr(event->topic, GREEN_HOUSE_DRIP3_CTRL_TOPIC) != NULL)
            {
                Control_Drip(3, event->data);
            }

            if(strstr(event->topic, GREEN_HOUSE_DRIP1_REQUEST_STATUS_TOPIC) != NULL)
            {
                ESP_LOGI(TAG, "Drip #1 Status Requested.");
                if(greenHouseInfo.dripInfo[1].state)
                {
                    sprintf(result_string, "%s", "ON");
                }
                else
                {
                    sprintf(result_string, "%s", "OFF");
                }

                esp_mqtt_client_publish(client, GREEN_HOUSE_DRIP1_STATUS_TOPIC, 
                                        result_string, strlen(result_string), 0, 0);
            }

            if(strstr(event->topic, GREEN_HOUSE_DRIP2_REQUEST_STATUS_TOPIC) != NULL)
            {
                ESP_LOGI(TAG, "Drip #2 Status Requested.");
                if(greenHouseInfo.dripInfo[1].state)
                {
                    sprintf(result_string, "%s", "ON");
                }
                else
                {
                    sprintf(result_string, "%s", "OFF");
                }

                esp_mqtt_client_publish(client, GREEN_HOUSE_DRIP2_STATUS_TOPIC, 
                                        result_string, strlen(result_string), 0, 0);        
            }

            if(strstr(event->topic, GREEN_HOUSE_DRIP3_REQUEST_STATUS_TOPIC) != NULL)
            {
                ESP_LOGI(TAG, "Drip #3 Status Requested.");
                if(greenHouseInfo.dripInfo[0].state)
                {
                    sprintf(result_string, "%s", "ON");
                }
                else
                {
                    sprintf(result_string, "%s", "OFF");
                }

                esp_mqtt_client_publish(client, GREEN_HOUSE_DRIP3_STATUS_TOPIC, 
                                        result_string, strlen(result_string), 0, 0);
            }

            if(strstr(event->topic, GREEN_HOUSE_DRIP1_ON_TIME_TOPIC) != NULL)
            {
                event->data[event->data_len] = 0x00;
                Control_Drip_On_Time(1, event->data);
            }

            if(strstr(event->topic, GREEN_HOUSE_DRIP2_ON_TIME_TOPIC) != NULL)
            {
                event->data[event->data_len] = 0x00;
                Control_Drip_On_Time(2, event->data);
            }

            if(strstr(event->topic, GREEN_HOUSE_DRIP3_ON_TIME_TOPIC) != NULL)
            {
                event->data[event->data_len] = 0x00;
                Control_Drip_On_Time(3, event->data);
            }

        break;

        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) 
            {
                log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
                log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
                log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
                ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
            }
        break;

        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }

    return ESP_OK;
}

/******************************************************************************/
/* Function:    static void log_error_if_nonzero(const char * message,        */
/*                                               int error_code)              */
/*                                                                            */
/* Inputs:      message:                                                      */
/*              error_code:                                                   */
/*                                                                            */
/* Outputs:     None.                                                         */
/*                                                                            */
/* Description: This is the                                                   */
/*                                                                            */
/* Author:      Michael Bester                                                */
/*                                                                            */
/* Notes:                                                                     */
/*                                                                            */
/******************************************************************************/
static void log_error_if_nonzero(const char * message, int error_code)
{
    if (error_code != 0) 
    {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

