#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "esp_log.h"
#include "driver/uart.h"
#include "driver/gpio.h"

static const char *TAG_MAIN = "EASYMEAL";

#define MDB1_TX 16
#define MDB1_RX 15
#define MDB2_TX 18
#define MDB2_RX 17
#define MDB1_UART UART_NUM_1
#define MDB2_UART UART_NUM_2
#define MDB_LOG_TICK 200

uart_parity_t parity_mode = {UART_PARITY_EVEN};


void init_mdb_uart(int uart_num, int tx_pin, int rx_pin) {
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_ODD,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    uart_set_parity(uart_num, parity_mode);
    uart_driver_install(uart_num, 1024, 0, 0, NULL, 0);
    uart_param_config(uart_num, &uart_config);
    uart_set_pin(uart_num, tx_pin, rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_set_line_inverse(uart_num, UART_SIGNAL_TXD_INV); //invert TX signal 
    uart_set_tx_idle_num(uart_num, 1);
    uart_set_rx_full_threshold(uart_num, 1);
}

int get_buffered_data_len(int mdb_uart){
    int length = 0;
    uart_get_buffered_data_len(mdb_uart, (size_t*)&length);
    return length;
}

void get_mdb_data(int mdb_uart, uint8_t *buffer, int len){
    int length = 0;
    length = uart_read_bytes(mdb_uart, buffer, len, 1 / portTICK_PERIOD_MS);
}

void log_mdb_data(int mdb_uart) {
    int buffered_data_len = 0;
    uint8_t mdb_data[64];

    buffered_data_len = get_buffered_data_len(mdb_uart);
    if (buffered_data_len > 0) {
        get_mdb_data(mdb_uart, mdb_data, buffered_data_len);
        for (int i = 0; i < buffered_data_len; i++) {
            printf("%02x ", mdb_data[i]);
            // Do something with data[i]
        }
        printf("\n");
    } else {
        printf("none\n");
    }
}

void mdb1_log_task(void *arg) {
    init_mdb_uart(MDB1_UART, MDB1_TX, MDB1_RX);
    while (1) {
        printf("(MDB1 RX) [VM] ---tx--> [CC]  :  ");
        log_mdb_data(MDB1_UART);
        vTaskDelay(pdMS_TO_TICKS(MDB_LOG_TICK));
    }
    vTaskDelete(NULL);
}

void mdb2_log_task(void *arg) {
    init_mdb_uart(MDB2_UART, MDB2_TX, MDB2_RX);
    while (1) {
        printf("(MDB2 RX) [VM] <--rx--- [CC]  :  ");
        log_mdb_data(MDB2_UART);

        printf("\n");
        vTaskDelay(pdMS_TO_TICKS(MDB_LOG_TICK));
    }
    vTaskDelete(NULL);
}
    
void app_main(void) {
    xTaskCreate(mdb1_log_task, "mdb1_log_task", 2048, NULL, 12, NULL);
    xTaskCreate(mdb2_log_task, "mdb2_log_task", 2048, NULL, 12, NULL);
}
