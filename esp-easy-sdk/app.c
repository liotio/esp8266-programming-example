#include "osapi.h"
#include "user_interface.h"
#include "espconn.h"

#include "lib/httplib.h"
#include "driver/uart.h"

#include "user_config.h"
#include "wifi_config.h"

static const uint8 led_pin = 2;
static uint8 led_active = 0;

static os_timer_t led_timer;
static os_timer_t wifi_timer;

// listening connection data
static struct espconn httpdConn;
static esp_tcp httpdTcp;

void ICACHE_FLASH_ATTR
user_esp_platform_check_ip(void) {
    struct ip_info ipconfig;

    //disarm timer first
    os_timer_disarm(&wifi_timer);

    //get ip info of ESP8266 station
    wifi_get_ip_info(STATION_IF, &ipconfig);

    if (wifi_station_get_connect_status() == STATION_GOT_IP
            && ipconfig.ip.addr != 0) {
        os_printf("got ip\n");
    } else if (wifi_station_get_connect_status() == STATION_WRONG_PASSWORD
            || wifi_station_get_connect_status() == STATION_NO_AP_FOUND
            || wifi_station_get_connect_status() == STATION_CONNECT_FAIL) {
        os_printf("Unable to connect\n");
    } else {
        //re-arm timer to check ip
        os_timer_setfn(&wifi_timer,
                (os_timer_func_t *) user_esp_platform_check_ip, NULL);
        os_timer_arm(&wifi_timer, 100, 0);
    }
}

//Callback called when there's data available on a socket.
void ICACHE_FLASH_ATTR
httpd_recv_cb(void *arg, char *data, unsigned short len) {
    struct espconn *conn = arg;

    char method[5];
    char location[256];

    http_request_method(data, method, 5);
    http_request_location(data, location, 256);

    if (strcmp(method,"GET") == 0) {

        if (strcmp(location,"/") == 0) {
            if (led_active == 0) {
                gpio_output_set(0, (1 << led_pin), 0, 0);
                led_active = 1;
                espconn_sent(conn, "LED is now ON", 13);
            } else {
                gpio_output_set((1 << led_pin), 0, 0, 0);
                led_active = 0;
                espconn_sent(conn, "LED is now OFF", 14);
            }
        }
    }

    espconn_disconnect(conn);
}


void ICACHE_FLASH_ATTR
httpd_sent_cb(void *arg) {
    //data sent successfully
}


void ICACHE_FLASH_ATTR
httpd_conn_cb(void *arg) {
    struct espconn *conn = arg;
    // only set recv callback, skip others
    espconn_regist_recvcb(conn, httpd_recv_cb);
    espconn_set_opt(conn, ESPCONN_NODELAY);
}

void ICACHE_FLASH_ATTR
wifi_init() {
    struct station_config station_conf;
    // set struct data to zero
    os_memset(station_conf.ssid, 0, 32);
    os_memset(station_conf.password, 0, 32);
    // don't need mac address
    station_conf.bssid_set = 0;

    os_memcpy(&station_conf.ssid, SSID, 32);
    os_memcpy(&station_conf.password, PASS, 64);
    wifi_station_set_config(&station_conf);

    //set a timer to check whether got ip from router succeed or not.
    os_timer_disarm(&wifi_timer);
    os_timer_setfn(&wifi_timer, (os_timer_func_t *)user_esp_platform_check_ip, NULL);
    os_timer_arm(&wifi_timer, 100, 0);
}

//
void ICACHE_FLASH_ATTR
httpd_init() {
    httpdConn.type = ESPCONN_TCP;
    httpdConn.state = ESPCONN_NONE;
    httpdTcp.local_port = 80;
    httpdConn.proto.tcp = &httpdTcp;

    os_printf("Httpd init, conn=%p\n", &httpdConn);
    espconn_regist_connectcb(&httpdConn, httpd_conn_cb);
    espconn_accept(&httpdConn);
}

void ICACHE_FLASH_ATTR
sys_init_done_cb() {
    wifi_init();
    httpd_init();
}

void ICACHE_FLASH_ATTR
user_init()
{
    gpio_init();
    gpio_output_set(0, 0, (1 << led_pin), 0);
    // uart initialisation is done implicitly, only set baudrate
    // TODO: with gpio_init???
    UART_SetBaudrate(UART0, BIT_RATE_115200);
    //Set softAP + station mode
    wifi_set_opmode(STATION_MODE);

    // wifi_station_scan has to be called after system init done and station enable.
    system_init_done_cb(sys_init_done_cb);
}
