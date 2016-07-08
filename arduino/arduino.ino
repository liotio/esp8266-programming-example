#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include "wifi_config.h"

uint8 led_pin = 2;
uint8 led_active = 0;

ESP8266WebServer server ( 80 );

void setup() {
    Serial.begin(115200);
    pinMode(led_pin, OUTPUT);
    digitalWrite(led_pin, HIGH); 

    WiFi.begin (SSID, PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }

    Serial.println(WiFi.localIP());

    server.on("/", []() {
        if (led_active == 0) {
            digitalWrite(led_pin, LOW);
            led_active = 1;
            server.send(200, "text/plain", "LED is now ON");
        } else {
            digitalWrite(led_pin, HIGH);
            led_active = 0;
            server.send(200, "text/plain", "LED is now OFF");
        }
    });

    server.begin();
}

void loop() {
    server.handleClient();
}

