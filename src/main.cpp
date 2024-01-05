#include <Arduino.h>

long duration;
int distance;

void setup() {
    // trigger pin
    pinMode(D6, OUTPUT);
    // echo pin
    pinMode(D5, INPUT);
    // builtin led
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(9600);
    Serial.println("Starting up softener_salt_level");
}

void getDistance() {
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

}

void loop() {
    getDistance();
    delay(5000);
 
}

 
 