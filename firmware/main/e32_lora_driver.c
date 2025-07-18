// LoRa Module (E32-433T30D by EByte) Driver Code
// File Name: e32_lora_driver.c
// Author: Fayyaz Shaikh
// Email ID: fayyaz.shaikh24@spit.ac.in
// LinkedIn: https://www.linkedin.com/in/fayyaz-shaikh-7646312a3/ 

// © 2025 Fayyaz Shaikh & Team Embedded Greens
// This code and associated idea are protected under Indian copyright law.
// Unauthorized reproduction or commercial use is strictly prohibited.
// Patent pending — do not use or file without written permission.


/*MIT License

Copyright (c) 2025 fayyaz-69

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#include <stdio.h>
#include <string.h>
#include "esp_system.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "driver/uart.h"
#include "esp_macros.h"
#include "driver/gpio.h"
#include <math.h>
#include "e32_lora_driver.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define tx 17
#define rx 16
#define m0 2
#define m1 4
// #define UART_PORT UART_NUM_1
static const char *TAG = "E32 LoRa";
const uart_port_t uart_num = UART_NUM_1;
// #define aux ?

esp_err_t e32_init(void){

    gpio_set_direction(m0, GPIO_MODE_OUTPUT);
    gpio_set_direction(m1, GPIO_MODE_OUTPUT);
    gpio_set_level(m0, 0);
    gpio_set_level(m1, 0);
    vTaskDelay(pdMS_TO_TICKS(50));

    const int buffer_size = (1024*2);
    // QueueHandle_t uart_queue;

    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        //.rx_flow_ctrl_thresh = 122,
    };
    // Give the config settings to parameter configuration function.
    uart_param_config(uart_num, &uart_config);
    // Set UART Pins (uart_num, TX, TX, RTS, CTS)
    // Set pin as UART_PIN_NO_CHANGE if not used.
    uart_set_pin(uart_num, tx, rx, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(uart_num, buffer_size, buffer_size, 0, NULL, 0);
    // uart_driver_install(uart_num, buffer_size, buffer_size, 10, &uart_queue, 0); // For event handling using RTOS.


    ESP_LOGI(TAG, "E32 LoRa Module Initialized");

    return ESP_OK;
}


esp_err_t e32_transmit(const char* data){

    int len = strlen(data);

    int sent_bytes = uart_write_bytes(uart_num, data, len);

    uart_wait_tx_done(uart_num, pdMS_TO_TICKS(100));

    if (sent_bytes != len){
        ESP_LOGE(TAG, "Failed to send full data!");
        return ESP_FAIL;
    }
    else{
    ESP_LOGI(TAG, "Sent: %s", data);
    return ESP_OK;
    }
}


esp_err_t e32_receive(uint8_t *rx_data, size_t max_len){
    
    size_t length = 0;

    uart_get_buffered_data_len(uart_num, &length);
    length = uart_read_bytes(uart_num, rx_data, MIN(length, max_len -1), pdMS_TO_TICKS(100));
    rx_data[length] = '\0';

    if (length > 0){
        ESP_LOGI(TAG, "Received %s", (char*)rx_data);
        return ESP_OK;
    }
    else {
        return ESP_FAIL;
    }
    
}

esp_err_t e32_sleep_mode(){
    gpio_set_direction(m0, GPIO_MODE_OUTPUT);
    gpio_set_direction(m1, GPIO_MODE_OUTPUT);
    gpio_set_level(m0, 1);
    gpio_set_level(m1, 1);

    return ESP_OK;
}
