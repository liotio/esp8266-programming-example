/* The classic "blink" example
 *
 * This sample code is in the public domain.
 */
#include "espressif/esp_common.h"
#include "esp/uart.h"

#include "FreeRTOS.h"
#include "task.h"
#include "esp8266.h"

#include "c_types.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "lwip/api.h"

#include "lwip/app/espconn.h"

#include "espressif/esp8266/eagle_soc.h"
#include "espressif/esp8266/gpio_register.h"
#include "espressif/esp8266/pin_mux_register.h"
#include "espressif/esp8266/ets_sys.h"
#include "espressif/esp_misc.h"
#include "esp/gpio.h"

#include "httplib.h"
#include "wifi_config.h"

#include <string.h>
#include <semphr.h>

#define WEB_SERVER "chainxor.org"
#define WEB_PORT 80
#define WEB_URL "http://chainxor.org/"

static const uint8 led_pin = 2;
static uint8 led_active = 0;

const uint8 UART0 = 0;

xSemaphoreHandle wifi_alive;

static struct espconn httpdConn;
static esp_tcp httpdTcp;

#define delay_ms(ms) vTaskDelay((ms) / portTICK_RATE_MS)

//Callback called when there's data available on a socket.
static void httpd_recv_cb(void *arg, char *data, unsigned short len) {
    struct espconn *conn = arg;

    char method[5];
    char location[256];

    http_request_method(data, method, 5);
    http_request_location(data, location, 256);

    if (strcmp(method,"GET") == 0) {

        if (strcmp(location,"/") == 0) {
            if (led_active == 0) {
                sdk_gpio_output_set(0, (1 << led_pin), 0, 0);
                led_active = 1;
                espconn_sent(conn, "LED is now ON", 13);
            } else {
                sdk_gpio_output_set((1 << led_pin), 0, 0, 0);
                led_active = 0;
                espconn_sent(conn, "LED is now OFF", 14);
            }
        }
    }

    espconn_disconnect(conn);
}

static void httpd_conn_cb(void *arg) {
    struct espconn *conn = arg;
    // only set recv callback, skip others
    espconn_regist_recvcb(conn, httpd_recv_cb);
    // espconn_set_opt(conn, ESPCONN_NODELAY);
}

static void httpd_init() {
    httpdConn.type = ESPCONN_TCP;
    httpdConn.state = ESPCONN_NONE;
    httpdTcp.local_port = 80;
    httpdConn.proto.tcp = &httpdTcp;

    printf("Httpd init, conn=%p\n", &httpdConn);
    espconn_regist_connectcb(&httpdConn, httpd_conn_cb);
    espconn_accept(&httpdConn);
}

static void wifi_init(void *pvParameters)
{
    uint8_t status  = 0;

    struct sdk_station_config config = {
        .ssid = "SSID",
        .password = "PASS",
    };

    sdk_wifi_station_set_config(&config);

    while(1)
    {
        while (status != STATION_GOT_IP){
            status = sdk_wifi_station_get_connect_status();
            printf("%s: status = %d\n\r", __func__, status );
            if( status == STATION_WRONG_PASSWORD ){
                printf("WiFi: wrong password\n\r");
                break;
            } else if( status == STATION_NO_AP_FOUND ) {
                printf("WiFi: AP not found\n\r");
                break;
            } else if( status == STATION_CONNECT_FAIL ) {
                printf("WiFi: connection failed\r\n");
                break;
            }
            vTaskDelay( 1000 / portTICK_RATE_MS );
        }
        if (status == STATION_GOT_IP) {
            printf("WiFi: Connected\n\r");
            xSemaphoreGive( wifi_alive );
            taskYIELD();
        }

        while ((status = sdk_wifi_station_get_connect_status()) == STATION_GOT_IP) {
            xSemaphoreGive( wifi_alive );
            taskYIELD();
        }
        printf("WiFi: disconnected\n\r");
        sdk_wifi_station_disconnect();
        vTaskDelay( 1000 / portTICK_RATE_MS );
    }
}

void user_init(void)
{
    // gpio_init();
    // gpio_output_set(0, 0, (1 << led_pin), 0);
    uart_set_baud(UART0, 115200);
    vSemaphoreCreateBinary(wifi_alive);

    //Set softAP + station mode
    sdk_wifi_set_opmode(STATION_MODE);

    xTaskCreate(&wifi_init, (signed char *)"wifi_init", 256, NULL, 2, NULL);
    xTaskCreate(&httpd_init, (signed char *)"wifi_init", 256, NULL, 2, NULL);

    // xTaskCreate(blinkenTask, (signed char *)"blinkenTask", 256, NULL, 2, NULL);
    // xTaskCreate(readTempAndHum, (signed char *)"readTempAndHum", 256, NULL, 2, NULL);
    // xTaskCreate(blinkenRegisterTask, (signed char *)"blinkenRegisterTask", 256, NULL, 2, NULL);
}
