#include <Arduino.h>
#include <WS2812B.h>
#define NUM_LEDS 16

#define LED_BUILTIN PC13

#define 	IR_SENSOR PA9
#define 	LED_1 PA0
#define 	LED_2 PA1

void setup() 
{
      Serial.begin(9600);
	  delay(500);
	
	pinMode(LED_BUILTIN, OUTPUT);
}

void loop() { 
	Serial.println("loop");
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
   digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(100);   
}