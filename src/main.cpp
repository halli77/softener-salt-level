#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "config.h"


const char *wifi_network_name = WIFI_NETWORK_NAME;
const char *wifi_password = WIFI_PASSWORD;

const char *mqtt_server = MQTT_SERVER;
const int  mqtt_port = MQTT_PORT;
const char *mqtt_client_name = MQTT_CLIENT_NAME;
const char *mqtt_user = MQTT_USER;
const char *mqtt_password = MQTT_PASSWORD;
const char *mqtt_topic_distance =  MQTT_TOPIC_DISTANCE;

const int  app_deep_sleep_sec = APP_DEEP_SLEEP_SEC;

WiFiClient client;
PubSubClient mqttclient(client);

void setup() {
    // trigger pin
    pinMode(D6, OUTPUT);
    // echo pin
    pinMode(D5, INPUT);
    // builtin led
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(9600);
    Serial.println("Starting up softener_salt_level");

    /* connect to Wifi*/
    Serial.print("Connecting to ");
    Serial.println(wifi_network_name);
    WiFi.begin(wifi_network_name, wifi_password);

    Serial.print("Connecting");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();

    Serial.print("Connected, IP address: ");
    Serial.println(WiFi.localIP());

}

int getDistance() {
    long duration;
    int distance;
    
    Serial.println("##### start new measure cycle");
    // digitalWrite(LED_BUILTIN, LOW);

    digitalWrite(D6, LOW); //set trigger signal low for 2us
    delayMicroseconds(2);

    /*send 10 microsecond pulse to trigger pin of HC-SR04 */
    digitalWrite(D6, HIGH); 
    delayMicroseconds(10); 
    digitalWrite(D6, LOW);  

    /*Measure the Echo output signal duration or puls width */
    duration = pulseIn(D5, HIGH); 
    distance = duration * 0.034 / 2; 


    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    // delay(1000);
    // digitalWrite(LED_BUILTIN, HIGH);

    return distance;

}

void sendMessage(int dist) {
    mqttclient.setServer(mqtt_server, mqtt_port);
    while (!client.connected()) {
        Serial.print("Connect to MQTT on ");
        Serial.println(mqtt_server);
        if (mqttclient.connect(mqtt_client_name, mqtt_user, mqtt_password )) {
            Serial.println("connected");
        } else {
            Serial.print("Could not connect to MQTT. Status: ");
            Serial.println(mqttclient.state());
        }
    }

    char mqtt_payload [33];
    itoa(dist, mqtt_payload, 10);
    mqttclient.publish(mqtt_topic_distance, mqtt_payload);

}

void loop() {

    int d = 0;
    d =  getDistance();
    sendMessage(d);

    uint64_t maxDeepSleepTime = ESP.deepSleepMax();
    uint64_t deepSleepTime = app_deep_sleep_sec * 10e5; 
    if (deepSleepTime > maxDeepSleepTime or deepSleepTime == 0) {
        Serial.print("Given deepsleep-time is to long or zero, using max deepsleep time [microseconds] of  ");
        Serial.println(maxDeepSleepTime);
        deepSleepTime = maxDeepSleepTime;
    }

    Serial.println("Going to deepsleep...");
    delay(500);
    
    ESP.deepSleep(deepSleepTime);
 
}

 
 