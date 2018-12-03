
/* =====[ interpreter sample ]==============================================

   File Name:       main.c -- main file
   
   Description:     A fully functional command line for the ESP32
                    monitor terminal.

   Revisions:

      REV   DATE            BY              DESCRIPTION
      ----  -----------     ----------      ------------------------------
      0.00  dec.02.2018     Peter Glen      Release as ESP sample

   ======================================================================= */

#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

#include "interp.h"

esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}

void app_main(void)
{
    nvs_flash_init();
    tcpip_adapter_init();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    wifi_config_t sta_config = {
        .sta = {
            .ssid = "access_point_name",
            .password = "password",
            .bssid_set = false
        }
    };
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    ESP_ERROR_CHECK( esp_wifi_connect() );

    initparser();                             
    printf("Type help for list of commands. Dumb terminal, no arrows etc.\n");
             
    char  tmp[64];
    while (true) 
        {
        printf("ESP32>> ");  fflush(stdout);
        get_term_str(tmp, sizeof(tmp));
        printf("\n");
        
        interpret(tmp);
        vTaskDelay(50 / portTICK_RATE_MS);
        }
}


