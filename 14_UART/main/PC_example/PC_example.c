#include "../main.h"
#include "PC_example.h"

void run_pc_example(void)
{
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    /** UART0 is used for flashing and serial monitoring */
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_1, RX_BUFFER_LEN, TX_BUFFER_LEN, 0, NULL, 0);

    char tx_message[] = "I'm alive!!!";
    uart_write_bytes(UART_NUM_1, tx_message, sizeof(tx_message));
    printf("Sending: %s\n", tx_message);

    char rx_message[RX_BUFFER_LEN];

    while (true) {
        memset(rx_message, 0, sizeof(rx_message));
        
        if (uart_read_bytes(UART_NUM_1, rx_message, RX_BUFFER_LEN, pdMS_TO_TICKS(500)) > 0) {
            printf("Received: %s\n", rx_message);
        }
    }

}
