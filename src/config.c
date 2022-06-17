#include "config.h"

void UARTInit()
{
const uart_port_t uart_num = UART_NUM_0;
    uart_config_t uart_config = {                                //uart 0 config
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    uart_param_config(uart_num, &uart_config);
    uart_driver_install(UART_NUM_0, 2048, 0, 0, NULL, 0);
}

