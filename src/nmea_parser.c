// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nmea_parser.h"

/**
 * @brief NMEA Parser runtime buffer size
 *
 */
#define NMEA_PARSER_RUNTIME_BUFFER_SIZE (CONFIG_NMEA_PARSER_RING_BUFFER_SIZE / 2)
#define NMEA_MAX_STATEMENT_ITEM_LENGTH (16)
#define NMEA_EVENT_LOOP_QUEUE_SIZE (16)
#define YEAR_BASE (2000) //date in GPS starts from 2000
#define TIME_ZONE (-8)   //Canada Time


/**
 * @brief Define of NMEA Parser Event base
 *
 */
ESP_EVENT_DEFINE_BASE(ESP_NMEA_EVENT);

static const char *GPS_TAG = "nmea_parser";

//Typedef for GPS Object decoding statements, and other GPS parameters
/**
 * @brief GPS parser library runtime structure
 *
 */
typedef struct {
    uint8_t item_pos;                              /*!< Current position in item */
    uint8_t item_num;                              /*!< Current item number */
    uint8_t asterisk;                              /*!< Asterisk detected flag */
    uint8_t crc;                                   /*!< Calculated CRC value */
    uint8_t parsed_statement;                      /*!< OR'd of statements that have been parsed */
    uint8_t sat_num;                               /*!< Satellite number */
    uint8_t sat_count;                             /*!< Satellite count */
    uint8_t cur_statement;                         /*!< Current statement ID */
    uint32_t all_statements;                       /*!< All statements mask */
    char item_str[NMEA_MAX_STATEMENT_ITEM_LENGTH]; /*!< Current item */
    gps_t parent;                                  /*!< Parent class */
    uart_port_t uart_port;                         /*!< Uart port number */
    uint8_t *buffer;                               /*!< Runtime buffer */
    esp_event_loop_handle_t event_loop_hdl;        /*!< Event loop handle */
    TaskHandle_t tsk_hdl;                          /*!< NMEA Parser task handle */
    QueueHandle_t event_queue;                     /*!< UART event queue handle */
} esp_gps_t;

//Function that parses the latitude and longitude of the item in the statement
/**
 * @brief parse latitude or longitude
 *              format of latitude in NMEA is ddmm.sss and longitude is dddmm.sss
 * @param esp_gps esp_gps_t type object
 * @return float Latitude or Longitude value (unit: degree)
 */
static float parse_lat_long(esp_gps_t *esp_gps)
{
    float ll = strtof(esp_gps->item_str, NULL);
    int deg = ((int)ll) / 100;
    float min = ll - (deg * 100);
    ll = deg + min / 60.0f;
    return ll;
}

//Function that converts two individual digits to a number (used for utc time)
/**
 * @brief Converter two continuous numeric character into a uint8_t number
 *
 * @param digit_char numeric character
 * @return uint8_t result of converting
 */
static inline uint8_t convert_two_digit2number(const char *digit_char)
{
    return 10 * (digit_char[0] - '0') + (digit_char[1] - '0');
}

//Use the above function to parse the time from the line, and store
/**
 * @brief Parse UTC time in GPS statements
 *
 * @param esp_gps esp_gps_t type object
 */
static void parse_utc_time(esp_gps_t *esp_gps)
{
    esp_gps->parent.tim.hour = convert_two_digit2number(esp_gps->item_str + 0);
    esp_gps->parent.tim.minute = convert_two_digit2number(esp_gps->item_str + 2);
    esp_gps->parent.tim.second = convert_two_digit2number(esp_gps->item_str + 4);
    if (esp_gps->item_str[6] == '.') {
        uint16_t tmp = 0;
        uint8_t i = 7;
        while (esp_gps->item_str[i]) {
            tmp = 10 * tmp + esp_gps->item_str[i] - '0';
            i++;
        }
        esp_gps->parent.tim.thousand = tmp;
    }
    
}

//Function that defines what each "item" in the $GGA NMEA string is, and the appropriate parsing function to update values (if statement because this can be configured on/off in menuconfig)
#if CONFIG_NMEA_STATEMENT_GGA
/**
 * @brief Parse GGA statements
 *
 * @param esp_gps esp_gps_t type object
 */
static void parse_gga(esp_gps_t *esp_gps)
{
    /* Process GGA statement */
    switch (esp_gps->item_num) {
    case 1: /* Process UTC time */
        parse_utc_time(esp_gps);
        break;
    case 2: /* Latitude */
        esp_gps->parent.latitude = parse_lat_long(esp_gps);
        break;
    case 3: /* Latitude north(1)/south(-1) information */
        if (esp_gps->item_str[0] == 'S' || esp_gps->item_str[0] == 's') {
            esp_gps->parent.latitude *= -1;
        }
        break;
    case 4: /* Longitude */
        esp_gps->parent.longitude = parse_lat_long(esp_gps);
        break;
    case 5: /* Longitude east(1)/west(-1) information */
        if (esp_gps->item_str[0] == 'W' || esp_gps->item_str[0] == 'w') {
            esp_gps->parent.longitude *= -1;
        }
        break;
    case 6: /* Fix status */
        esp_gps->parent.fix = (gps_fix_t)strtol(esp_gps->item_str, NULL, 10);
        break;
    case 7: /* Satellites in use */
        esp_gps->parent.sats_in_use = (uint8_t)strtol(esp_gps->item_str, NULL, 10);
        break;
    case 8: /* HDOP */
        esp_gps->parent.dop_h = strtof(esp_gps->item_str, NULL);
        break;
    case 9: /* Altitude */
        esp_gps->parent.altitude = strtof(esp_gps->item_str, NULL);
        break;
    case 11: /* Altitude above ellipsoid */
        esp_gps->parent.altitude += strtof(esp_gps->item_str, NULL);
        break;
    default:
        break;
    }
}
#endif

//Function that defines what each "item" in the $GLL NMEA string is, and the appropriate parsing function to update values (if statement because this can be configured on/off in menuconfig)
#if CONFIG_NMEA_STATEMENT_GLL
/**
 * @brief Parse GLL statements
 *
 * @param esp_gps esp_gps_t type object
 */
static void parse_gll(esp_gps_t *esp_gps)
{
    /* Process GPGLL statement */
    switch (esp_gps->item_num) {
    case 1:/* Latitude */
        esp_gps->parent.latitude = parse_lat_long(esp_gps);
        break;
    case 2: /* Latitude north(1)/south(-1) information */
        if (esp_gps->item_str[0] == 'S' || esp_gps->item_str[0] == 's') {
            esp_gps->parent.latitude *= -1;
        }
        break;
    case 3: /* Longitude */
        esp_gps->parent.longitude = parse_lat_long(esp_gps);
        break;
    case 4: /* Longitude east(1)/west(-1) information */
        if (esp_gps->item_str[0] == 'W' || esp_gps->item_str[0] == 'w') {
            esp_gps->parent.longitude *= -1;
        }
        break;
    case 5:/* Process UTC time */
        parse_utc_time(esp_gps);
        break;
    case 6: /* Process valid status */
        esp_gps->parent.valid = (esp_gps->item_str[0] == 'A');
        break;
    default:
        break;
    }
}
#endif

//Function that parses each "item" in the statement. "Items" are the information contained inbetween two commas in statement
/**
 * @brief Parse received item
 *
 * @param esp_gps esp_gps_t type object
 * @return esp_err_t ESP_OK on success, ESP_FAIL on error
 */
static esp_err_t parse_item(esp_gps_t *esp_gps)
{
    esp_err_t err = ESP_OK;
    /* Check if start of a statement */
    if (esp_gps->item_num == 0 && esp_gps->item_str[0] == '$') {
        if (0) {
        }
#if CONFIG_NMEA_STATEMENT_GGA //Check if $GGA statements are enabled
        else if (strstr(esp_gps->item_str, "GGA")) { //Check if the current statement was detected to be $GGA via the first item of statement
            esp_gps->cur_statement = STATEMENT_GGA; //Declare the whole string to be a $GGA statement
        }
#endif
#if CONFIG_NMEA_STATEMENT_GLL
        else if (strstr(esp_gps->item_str, "GLL")) { //Check if the current statement was detected to be $GLL via the first item of statement
            esp_gps->cur_statement = STATEMENT_GLL; //Declare the whole string to be a $GLL statement
        }
#endif

        else {
            esp_gps->cur_statement = STATEMENT_UNKNOWN; //If not $GGA or $GLL, declare statement to be unknown and exit
        }
        goto out;
    }
    /* Parse each item, depend on the type of the statement */
    if (esp_gps->cur_statement == STATEMENT_UNKNOWN) {
        goto out;
    }
#if CONFIG_NMEA_STATEMENT_GGA
    else if (esp_gps->cur_statement == STATEMENT_GGA) {
        parse_gga(esp_gps); //Call the function that parses $GGA, if the current statement is $GGA from above
    }
#endif
#if CONFIG_NMEA_STATEMENT_GLL
    else if (esp_gps->cur_statement == STATEMENT_GLL) {
        parse_gll(esp_gps); //Call the function that parses $GLL, if the current statement is $GLL from above
    }
#endif

    else {
        err =  ESP_FAIL; //Otherwise, declare an ESP_Fail and return an error
    }
out:
    return err;
}

//This function decodes the inbound statements, it tracks position in statement, detects special characters ($ * , and end of line character), and calls the parse item function on each "item" in the statement. Also handles unknown characters in statement
/**
 * @brief Parse NMEA statements from GPS receiver
 *
 * @param esp_gps esp_gps_t type object
 * @param len number of bytes to decode
 * @return esp_err_t ESP_OK on success, ESP_FAIL on error
 */
static esp_err_t gps_decode(esp_gps_t *esp_gps, size_t len)
{
    const uint8_t *d = esp_gps->buffer;
    while (*d) {
        /* Start of a statement */
        if (*d == '$') {
            /* Reset runtime information */
            esp_gps->asterisk = 0;
            esp_gps->item_num = 0;
            esp_gps->item_pos = 0;
            esp_gps->cur_statement = 0;
            esp_gps->crc = 0;
            esp_gps->sat_count = 0;
            esp_gps->sat_num = 0;
            /* Add character to item */
            esp_gps->item_str[esp_gps->item_pos++] = *d;
            esp_gps->item_str[esp_gps->item_pos] = '\0';
        }
        /* Detect item separator character */
        else if (*d == ',') {
            /* Parse current item */
            parse_item(esp_gps);
            /* Add character to CRC computation */
            esp_gps->crc ^= (uint8_t)(*d);
            /* Start with next item */
            esp_gps->item_pos = 0;
            esp_gps->item_str[0] = '\0';
            esp_gps->item_num++;
        }
        /* End of CRC computation */
        else if (*d == '*') {
            /* Parse current item */
            parse_item(esp_gps);
            /* Asterisk detected */
            esp_gps->asterisk = 1;
            /* Start with next item */
            esp_gps->item_pos = 0;
            esp_gps->item_str[0] = '\0';
            esp_gps->item_num++;
        }
        /* End of statement */
        else if (*d == '\r') {
            /* Convert received CRC from string (hex) to number */
            uint8_t crc = (uint8_t)strtol(esp_gps->item_str, NULL, 16);
            /* CRC passed */
            if (esp_gps->crc == crc) {
                switch (esp_gps->cur_statement) {
#if CONFIG_NMEA_STATEMENT_GGA
                case STATEMENT_GGA:
                    esp_gps->parsed_statement |= 1 << STATEMENT_GGA;
                    break;
#endif
#if CONFIG_NMEA_STATEMENT_GLL
                case STATEMENT_GLL:
                    esp_gps->parsed_statement |= 1 << STATEMENT_GLL;
                    break;
#endif

                default:
                    break;
                }
                /* Check if all statements have been parsed */
                if (((esp_gps->parsed_statement) & esp_gps->all_statements) == esp_gps->all_statements) {
                    esp_gps->parsed_statement = 0;
                    /* Send signal to notify that GPS information has been updated */
                    esp_event_post_to(esp_gps->event_loop_hdl, ESP_NMEA_EVENT, GPS_UPDATE,
                                      &(esp_gps->parent), sizeof(gps_t), 100 / portTICK_PERIOD_MS);
                }
            } else {
                ESP_LOGD(GPS_TAG, "CRC Error for statement:%s", esp_gps->buffer);
            }
            if (esp_gps->cur_statement == STATEMENT_UNKNOWN) {
                /* Send signal to notify that one unknown statement has been met */
                esp_event_post_to(esp_gps->event_loop_hdl, ESP_NMEA_EVENT, GPS_UNKNOWN,
                                  esp_gps->buffer, len, 100 / portTICK_PERIOD_MS);
            }
        }
        /* Other non-space character */
        else {
            if (!(esp_gps->asterisk)) {
                /* Add to CRC */
                esp_gps->crc ^= (uint8_t)(*d);
            }
            /* Add character to item */
            esp_gps->item_str[esp_gps->item_pos++] = *d;
            esp_gps->item_str[esp_gps->item_pos] = '\0';
        }
        /* Process next character */
        d++;
    }
    return ESP_OK;
}

//This function handles inbound transmission over UART from the GPS. It ensures that the inbound line can be properly decoded, and flushes the buffer when data has been used
/**
 * @brief Handle when a pattern has been detected by uart
 *
 * @param esp_gps esp_gps_t type object
 */
static void esp_handle_uart_pattern(esp_gps_t *esp_gps)
{
    int pos = uart_pattern_pop_pos(esp_gps->uart_port);
    if (pos != -1) {
        /* read one line(include '\n') */
        int read_len = uart_read_bytes(esp_gps->uart_port, esp_gps->buffer, pos + 1, 100 / portTICK_PERIOD_MS);
        /* make sure the line is a standard string */
        esp_gps->buffer[read_len] = '\0';
        /* Send new line to handle */
        if (gps_decode(esp_gps, read_len + 1) != ESP_OK) {
            ESP_LOGW(GPS_TAG, "GPS decode line failed");
        }
    } else {
        ESP_LOGW(GPS_TAG, "Pattern Queue Size too small");
        uart_flush_input(esp_gps->uart_port);
    }
}

//Function that checks for specific errors in the buffer from UART transmission. If errors do not occur (break statement), run the defined event loop to decode statements
/**
 * @brief NMEA Parser Task Entry
 *
 * @param arg argument
 */
static void nmea_parser_task_entry(void *arg)
{
    esp_gps_t *esp_gps = (esp_gps_t *)arg;
    uart_event_t event;
    while (1) {
        if (xQueueReceive(esp_gps->event_queue, &event, pdMS_TO_TICKS(200))) {
            switch (event.type) {
            case UART_DATA:
                break;
            case UART_FIFO_OVF:
                ESP_LOGW(GPS_TAG, "HW FIFO Overflow");
                uart_flush(esp_gps->uart_port);
                xQueueReset(esp_gps->event_queue);
                break;
            case UART_BUFFER_FULL:
                ESP_LOGW(GPS_TAG, "Ring Buffer Full");
                uart_flush(esp_gps->uart_port);
                xQueueReset(esp_gps->event_queue);
                break;
            case UART_BREAK:
                ESP_LOGW(GPS_TAG, "Rx Break");
                break;
            case UART_PARITY_ERR:
                ESP_LOGE(GPS_TAG, "Parity Error");
                break;
            case UART_FRAME_ERR:
                ESP_LOGE(GPS_TAG, "Frame Error");
                break;
            case UART_PATTERN_DET:
                esp_handle_uart_pattern(esp_gps);
                break;
            default:
                ESP_LOGW(GPS_TAG, "unknown uart event type: %d", event.type);
                break;
            }
        }
        /* Drive the event loop */
        esp_event_loop_run(esp_gps->event_loop_hdl, pdMS_TO_TICKS(50));
    }
    vTaskDelete(NULL);
}

//Function that initializes the parsing program. Includes memory allocation, configuring UART and installing its drivers, creates the event loop, and creates the task. Much of this is error checking with appropriate fixes when detected
/**
 * @brief Init NMEA Parser
 *
 * @param config Configuration of NMEA Parser
 * @return nmea_parser_handle_t handle of nmea_parser
 */
nmea_parser_handle_t nmea_parser_init(const nmea_parser_config_t *config)
{
    esp_gps_t *esp_gps = calloc(1, sizeof(esp_gps_t));
    if (!esp_gps) {
        ESP_LOGE(GPS_TAG, "calloc memory for esp_fps failed");
        goto err_gps;
    }
    esp_gps->buffer = calloc(1, NMEA_PARSER_RUNTIME_BUFFER_SIZE);
    if (!esp_gps->buffer) {
        ESP_LOGE(GPS_TAG, "calloc memory for runtime buffer failed");
        goto err_buffer;
    }

#if CONFIG_NMEA_STATEMENT_GGA
    esp_gps->all_statements |= (1 << STATEMENT_GGA);
#endif
#if CONFIG_NMEA_STATEMENT_GLL
    esp_gps->all_statements |= (1 << STATEMENT_GLL);
#endif
    /* Set attributes */
    esp_gps->uart_port = config->uart.uart_port;
    esp_gps->all_statements &= 0xFE;
    /* Install UART friver */
    uart_config_t uart_config = {
        .baud_rate = config->uart.baud_rate,
        .data_bits = config->uart.data_bits,
        .parity = config->uart.parity,
        .stop_bits = config->uart.stop_bits,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    if (uart_driver_install(esp_gps->uart_port, CONFIG_NMEA_PARSER_RING_BUFFER_SIZE, 0,
                            config->uart.event_queue_size, &esp_gps->event_queue, 0) != ESP_OK) {
        ESP_LOGE(GPS_TAG, "install uart driver failed");
        goto err_uart_install;
    }
    if (uart_param_config(esp_gps->uart_port, &uart_config) != ESP_OK) {
        ESP_LOGE(GPS_TAG, "config uart parameter failed");
        goto err_uart_config;
    }
    if (uart_set_pin(esp_gps->uart_port, UART_PIN_NO_CHANGE, config->uart.rx_pin,
                     UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE) != ESP_OK) {
        ESP_LOGE(GPS_TAG, "config uart gpio failed");
        goto err_uart_config;
    }
    /* Set pattern interrupt, used to detect the end of a line */
    uart_enable_pattern_det_baud_intr(esp_gps->uart_port, '\n', 1, 9, 0, 0);
    /* Set pattern queue size */
    uart_pattern_queue_reset(esp_gps->uart_port, config->uart.event_queue_size);
    uart_flush(esp_gps->uart_port);
    /* Create Event loop */
    esp_event_loop_args_t loop_args = {
        .queue_size = NMEA_EVENT_LOOP_QUEUE_SIZE,
        .task_name = NULL
    };
    if (esp_event_loop_create(&loop_args, &esp_gps->event_loop_hdl) != ESP_OK) {
        ESP_LOGE(GPS_TAG, "create event loop faild");
        goto err_eloop;
    }
    /* Create NMEA Parser task */
    BaseType_t err = xTaskCreatePinnedToCore(
                         nmea_parser_task_entry,
                         "nmea_parser",
                         CONFIG_NMEA_PARSER_TASK_STACK_SIZE,
                         esp_gps,
                         CONFIG_NMEA_PARSER_TASK_PRIORITY,
                         &esp_gps->tsk_hdl,0);
    if (err != pdTRUE) {
        ESP_LOGE(GPS_TAG, "create NMEA Parser task failed");
        goto err_task_create;
    }
    ESP_LOGI(GPS_TAG, "NMEA Parser init OK");
    return esp_gps;
    /*Error Handling*/
err_task_create:
    esp_event_loop_delete(esp_gps->event_loop_hdl);
err_eloop:
err_uart_install:
    uart_driver_delete(esp_gps->uart_port);
err_uart_config:
err_buffer:
    free(esp_gps->buffer);
err_gps:
    free(esp_gps);
    return NULL;
}

//Custom function that takes the parser and returns an event handler. It allows us to pass the defined task in the sensors.c file
/**
 * @brief Add user defined handler for NMEA parser
 *
 * @param nmea_hdl handle of NMEA parser
 * @param event_handler user defined event handler
 * @param handler_args handler specific arguments
 * @return esp_err_t
 *  - ESP_OK: Success
 *  - ESP_ERR_NO_MEM: Cannot allocate memory for the handler
 *  - ESP_ERR_INVALIG_ARG: Invalid combination of event base and event id
 *  - Others: Fail
 */
esp_err_t nmea_parser_add_handler(nmea_parser_handle_t nmea_hdl, esp_event_handler_t event_handler, void *handler_args)
{
    esp_gps_t *esp_gps = (esp_gps_t *)nmea_hdl;
    return esp_event_handler_register_with(esp_gps->event_loop_hdl, ESP_NMEA_EVENT, ESP_EVENT_ANY_ID,
                                           event_handler, handler_args);
}
