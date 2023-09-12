#include "../main.h"
#include "queue_example.h"
#include "freertos/queue.h"

#define TAG                 "UART_QUEUE"

#define PATTERN_LEN         (3)

QueueHandle_t uart_queue;

void uart_event_task(void *args)
{
    size_t data_len;
    uart_event_t uart_event;
    uint8_t *received_buffer = (uint8_t *)malloc(RX_BUFFER_LEN*sizeof(uint8_t));

    while (true) {
        if (xQueueReceive(uart_queue, &uart_event, portMAX_DELAY)) 
        {
            switch (uart_event.type)
            {
            case UART_DATA:
                ESP_LOGI(TAG, "UART_DATA");
                memset(received_buffer, 0, RX_BUFFER_LEN);
                uart_read_bytes(UART_NUM_1, received_buffer, uart_event.size, portMAX_DELAY);
                ESP_LOGI(TAG, "Received %.*s", uart_event.size, received_buffer);
                break;

            case UART_BREAK:
                ESP_LOGI(TAG, "UART_BREAK");
                break;

            case UART_BUFFER_FULL:
                ESP_LOGI(TAG, "UART_BUFFER_FULL");
                break;

            case UART_FIFO_OVF:
                ESP_LOGI(TAG, "UART_FIFO_OVF");
                uart_flush_input(UART_NUM_1);
                xQueueReset(uart_queue);
                break;

            case UART_FRAME_ERR:
                ESP_LOGI(TAG, "UART_FRAME_ERR");
                break;

            case UART_PARITY_ERR:
                ESP_LOGI(TAG, "UART_PARITY_ERR");
                break;

            case UART_DATA_BREAK:
                ESP_LOGI(TAG, "UART_DATA_BREAK");
                break;

            case UART_PATTERN_DET:
                ESP_LOGI(TAG, "UART_PATTERN_DET");
                memset(received_buffer, 0, RX_BUFFER_LEN);
                uart_get_buffered_data_len(UART_NUM_1, &data_len);
                int position = uart_pattern_pop_pos(UART_NUM_1);
                ESP_LOGI(TAG, "Detected %d data, pos %d", data_len, position);
                uart_read_bytes(UART_NUM_1, received_buffer, data_len, pdMS_TO_TICKS(100));
                ESP_LOGI(TAG, "Command is %s", received_buffer);
                break;
            
            default:
                break;
            }
        }
    }
}

void run_queue_example(void)
{
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_1, RX_BUFFER_LEN, TX_BUFFER_LEN, 20, &uart_queue, 0);

    /** This 2 routines below ar enot necessary, jsut use them to detect a pattern */
    uart_enable_pattern_det_intr(UART_NUM_1, '+', PATTERN_LEN, 10000, 10, 10);
    uart_pattern_queue_reset(UART_NUM_1, 20);
    xTaskCreate(uart_event_task, "uart_event_task", 2048, NULL, 10, NULL);

}