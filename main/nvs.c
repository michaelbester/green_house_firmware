/******************************************************************************/
/* Project:     Green House                                       			  */
/*                                                                            */
/* FileName:    nvs.c                                                 		  */
/*                                                                            */
/* Author:      Michael Bester                                                */
/*                                                                            */
/* Overview:    This module contains the nvs functions.                       */
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
#include "nvs_flash.h"
#include "green_house.h"
#include "nvs.h"

/******************************************************************************/
/*************************** Global Variables *********************************/
/******************************************************************************/

/******************************************************************************/
/**************************** Local Variables *********************************/
/******************************************************************************/
static const char *tag = "nvs";

/******************************************************************************/
/**************************** Local Functions *********************************/
/******************************************************************************/

/******************************************************************************/
/* Function:    void NVS_Write(char* key, int32_t val)                        */
/*                                                                            */
/* Inputs:      val:  Integer value to store.                                 */
/*              key:  Key name string pointer.                                */
/*                                                                            */
/* Outputs:     None.                                                         */
/*                                                                            */
/* Description: This function writes a value to flash.                        */
/*                                                                            */
/* Author:      Michael Bester                                                */
/*                                                                            */
/* Notes:                                                                     */
/*                                                                            */
/******************************************************************************/
void NVS_Write(char* key, int32_t val)    
{
    nvs_handle_t nvs_handle;
    esp_err_t err;

    err = nvs_open("storage", NVS_READWRITE, &nvs_handle);

    if(err != ESP_OK) 
    {
        ESP_LOGE(tag, "Error (%s) opening NVS handle!", esp_err_to_name(err));
    } 

    err = nvs_set_i32(nvs_handle, key, val);

    if(err != ESP_OK) 
    {
        ESP_LOGE(tag, "Error writing data to flash!");
    } 

    err = nvs_commit(nvs_handle);

    if(err != ESP_OK) 
    {
        ESP_LOGE(tag, "Error committing data to flash!");
    }
    else
    {
        ESP_LOGI(tag, "Wrote value %d to key name %s", val, key);
    }

    nvs_close(nvs_handle);
}

/******************************************************************************/
/* Function:    esp_err_t NVS_Read(char* key, int* val)                       */
/*                                                                            */
/* Inputs:      key:  Key to read from flash.                                 */
/*              val:  Value read from flash.                                  */
/*                                                                            */
/* Outputs:     Value read from flash.                                        */
/*                                                                            */
/* Description: This function reads a value from flash.                       */
/*                                                                            */
/* Author:      Michael Bester                                                */
/*                                                                            */
/* Notes:                                                                     */
/*                                                                            */
/******************************************************************************/
esp_err_t NVS_Read(char* key, int* val)   
{
    nvs_handle_t nvs_handle;
    esp_err_t err;

    err = nvs_open("storage", NVS_READWRITE, &nvs_handle);

    if(err != ESP_OK) 
    {
        ESP_LOGE(tag, "Error (%s) opening NVS handle!", esp_err_to_name(err));
        return(err);
    } 

    err = nvs_get_i32(nvs_handle, key, val);

    if(err != ESP_OK) 
    {
        ESP_LOGE(tag, "Error reading data from flash!");
        return(err);
    } 

    nvs_close(nvs_handle);

    return(err);
}

/******************************************************************************/
/* Function:    void NVS_Erase(char* key)                                     */
/*                                                                            */
/* Inputs:      key:  Key to erase from flash.                                */
/*                                                                            */
/* Outputs:     None.                                                         */
/*                                                                            */
/* Description: This function erases a key from flash.                        */
/*                                                                            */
/* Author:      Michael Bester                                                */
/*                                                                            */
/* Notes:                                                                     */
/*                                                                            */
/******************************************************************************/
void NVS_Erase(char* key)     
{
    nvs_handle_t nvs_handle;
    esp_err_t err;

    err = nvs_open("storage", NVS_READWRITE, &nvs_handle);

    if(err != ESP_OK) 
    {
        ESP_LOGE(tag, "Error (%s) opening NVS handle!", esp_err_to_name(err));
    } 


    err = nvs_erase_key(nvs_handle, key);

    if(err != ESP_OK)
    {
        ESP_LOGE(tag, "Error erasing key from flash!");
    }

    nvs_close(nvs_handle);
}