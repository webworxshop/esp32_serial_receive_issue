#include <stdint.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/uart.h"

#define MY_UART UART_NUM_2
#define MY_BAUD 115200
#define MY_TX   4
#define MY_RX   16 

#define BUFSIZE 265
#define REQSIZE 11
#define RESPSIZE REQSIZE*3

#define STOP() while(1) { vTaskDelay(1000/portTICK_RATE_MS); }

static const char *TAG = "UartTask";

static void uart_task(void *pvParameters)
{
    const int uart_num = MY_UART;
    uart_config_t uart_config = {
        .baud_rate = MY_BAUD,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122,
    };
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(uart_num, MY_TX, MY_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(uart_num, BUFSIZE*2, BUFSIZE*2, 0, NULL, ESP_INTR_FLAG_LOWMED));

    uint8_t *data = (uint8_t*)malloc(BUFSIZE);
    uint32_t next_count = 0;
    int len = 0;
    int index = 0;
    uint8_t *dout = (uint8_t*)malloc(RESPSIZE);
    while(1)
    {
        len = uart_read_bytes(uart_num, data, BUFSIZE, 10/portTICK_RATE_MS);
        if(len < 0)
        {
            ESP_LOGE(TAG, "UART Error!");
        }
        else if(len > 0)
        {
            ESP_LOGI(TAG, "RX bytes = %d", len);
        }
        while(len >= REQSIZE)
        {
            ESP_LOGI(TAG, "Data:");
            ESP_LOG_BUFFER_HEX(TAG, data+index, REQSIZE);

            uint32_t count = data[index+0];
            count |= data[index+1] << 8;
            count |= data[index+2] << 16;
            count |= data[index+3] << 24;

            if(count != next_count)
            {
                ESP_LOGE(TAG, "Count incorrect! %d != %d", count, next_count);
                STOP();
            }
            next_count = count + 1;

            for(int i = 4; i < REQSIZE; i++)
            {
                if(data[index+i] != i-4)
                {
                    ESP_LOGE(TAG, "Incorrect msg byte @%d = %d (should be %d)", i, data[index+i], i-4);
                    STOP();
                }
            }

            for(int i = 0; i < RESPSIZE; i+= REQSIZE)
            {
                memcpy(dout+i, data+index, REQSIZE);
            }
            uart_write_bytes(uart_num, (const char*)dout, RESPSIZE);

            len -= REQSIZE;
            index += REQSIZE;
        }
        index = 0;
        memset(data, 0, BUFSIZE);
        
        //vTaskDelay(5/portTICK_RATE_MS);
    }
}

void app_main()
{
    xTaskCreate(uart_task, "uart_task", 4096, NULL, 10, NULL);
}

