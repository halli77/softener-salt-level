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
    digitalWrite(LED_BUILTIN, LOW);

    digitalWrite(D6, LOW); //set trigger signal low for 2us
    delayMicroseconds(2);

    /*send 10 microsecond pulse to trigger pin of HC-SR04 */
    digitalWrite(D6, HIGH);  // make trigger pin active high
    delayMicroseconds(10);            // wait for 10 microseconds
    digitalWrite(D6, LOW);   // make trigger pin active low

    /*Measure the Echo output signal duration or pulss width */
    duration = pulseIn(D5, HIGH); // save time duration value in "duration variable
    distance = duration*0.034/2; //Convert pulse duration into distance


    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    delay(1000);
    digitalWrite(LED_BUILTIN, HIGH);

    return distance;

}

void loop() {

    int d = 0;
    d =  getDistance();

    mqttclient.setServer(mqtt_server, mqtt_port);
    while (!client.connected()) {
        Serial.print("Connect to MQTT on ");
        Serial.println(mqtt_server);
        if (mqttclient.connect(mqtt_client_name, mqtt_user, mqtt_password )) {
            Serial.println("connected");
        } else {
            Serial.print("Could not connect to MQTT. Status: ");
            Serial.println(mqttclient.state());
            delay(2000);
        }
    }

    //String mqtt_payload = String(d);
    // mqttclient.publish(mqtt_topic_distance, (char*) d);
    char mqtt_payload [33];
    itoa(d, mqtt_payload, 10);
    mqttclient.publish(mqtt_topic_distance, mqtt_payload);



    delay(5000);
 
}

 
 