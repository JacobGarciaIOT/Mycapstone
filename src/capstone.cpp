/* 
 * Project myProject
 * Author: Your Name
 * Date: 
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */
#include "Particle.h"
#include "Grove-Ultrasonic-Ranger.h"
#include <Adafruit_MQTT.h>
#include "Adafruit_MQTT/Adafruit_MQTT_SPARK.h"
#include "Adafruit_MQTT/Adafruit_MQTT.h"
#include "credentials.h"
#include <Wire.h>
#include "Stepper.h"

TCPClient TheClient;

Adafruit_MQTT_SPARK mqtt(&TheClient, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish pubFeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/random-number");

unsigned int lastTime;

SYSTEM_MODE(SEMI_AUTOMATIC);
SYSTEM_THREAD(ENABLED);

Ultrasonic ultrasonic(7);

// Stepper motor configuration
int SPR = 2048; // Steps per revolution
int IN1 = D6;
int IN3 = D4;
int IN2 = D5;
int IN4 = D3;
Stepper myStepper(SPR, IN1, IN3, IN2, IN4);

unsigned long previousMillis = 0; // Stores the last time a print occurred
const unsigned long interval = 250; // Time interval in milliseconds



void setup() {
    Serial.begin(9600);

    // WiFi connection
    WiFi.on();
    WiFi.connect();
    while (WiFi.connecting()) {
        Serial.printf(".");
    }
    Serial.printf("\n\n");

    // Initialize stepper motor speed
    myStepper.setSpeed(10);

    // Initialize MQTT
    mqtt.Update();
}

void loop() {


    // Ultrasonic sensor readings
    long RangeInInches = ultrasonic.MeasureInInches();
    long RangeInCentimeters = ultrasonic.MeasureInCentimeters();

    unsigned long currentMillis = millis();


    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis; // Update the time marker

        // Ultrasonic sensor readings
        Serial.println("The distance to obstacles in front is: ");
        Serial.print(RangeInInches);
        Serial.println(" inch");

        Serial.print(RangeInCentimeters);
        Serial.println(" cm");
    }


    // Publish to MQTT every 6 seconds
    if ((millis() - lastTime > 6000)) {
        if (mqtt.Update()) {
            pubFeed.publish(RangeInCentimeters);
            Serial.printf("Publishing %0.2f \n", RangeInCentimeters);
        }
        lastTime = millis();


    }
    myStepper.step(4000); 

    // Continuously rotate the stepper motor
}