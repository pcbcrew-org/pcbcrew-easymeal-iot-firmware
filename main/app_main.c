#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "esp_log.h"

#include "storage.c"
#include "button.c"
#include "router.c"
// #include "rtc.c"
#include "webserver.c"
#include "MDB_core.h"
#include "USB_com.h"

static const char *TAG_MAIN = "pcbcrew-router-main";

#define TXD_PIN (GPIO_NUM_16)
#define RXD_PIN (GPIO_NUM_15)

#define LED_PIN 9


void led_blink(void *pvParams) {
    gpio_pad_select_gpio(LED_PIN);
    gpio_set_direction (LED_PIN,GPIO_MODE_OUTPUT);
    while (1) {
        gpio_set_level(LED_PIN,0);
        vTaskDelay(500/portTICK_RATE_MS);
        gpio_set_level(LED_PIN,1);
        vTaskDelay(500/portTICK_RATE_MS);
    }
}

// void rx_task(void *arg)
// {
//     static const char *RX_TASK_TAG = "RX_TASK";
//     esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
//     char data[64];

//     int length = 0;

//     while (1) {
//         uart_get_buffered_data_len(UART_NUM_0, (size_t*)&length);

//         //ESP_LOGI(RX_TASK_TAG, "Cached bytes: %d", length);
//         if(length>1){

//             length = uart_read_bytes(UART_NUM_0, data, length, 10 / portTICK_PERIOD_MS);
//             data[length] = '\0'; //null terminate
//             if( memcmp(data, MDB_USB_BEGIN_SESSION_HEADER, strlen(MDB_USB_BEGIN_SESSION_HEADER))  == 0)
//             {
//                 //data+strlen(MDB_USB_BEGIN_SESSION_HEADER);
//                 //ESP_LOGI(RX_TASK_TAG, "Num bytes: %d", length);
//                 uint16_t fund;
//                 fund = atoi(data+strlen(MDB_USB_BEGIN_SESSION_HEADER));
//                 if(fund>0)
//                     MDB_send_fund(fund);
//                 //ESP_LOGI(RX_TASK_TAG, "%d", ( atoi(data+strlen(MDB_USB_BEGIN_SESSION_HEADER)) ) );
//             }



//             ESP_LOGI(RX_TASK_TAG, "%s", data);
//             length = 0;
//         }

//         vTaskDelay(100 / portTICK_PERIOD_MS);

//     }
//     free(data);
// }


void app_main(void)
{
    esp_log_level_set("*", ESP_LOG_INFO);

    setup_storage();
    // setup_button();
    // setup_router();
    // setup_webserver();

    // xTaskCreate(&rx_task, "uart_rx_task", 4096, NULL, 5, NULL);
    xTaskCreate(&MDB_core_task, "MDB_task", 4*1024,NULL, configMAX_PRIORITIES-1, NULL);
    xTaskCreate(&led_blink,"LED_BLINK",4*1024,NULL,4,NULL);
}
