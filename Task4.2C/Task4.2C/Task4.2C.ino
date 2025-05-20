#define USING_TIMER_TC3 true
#define TIMER_INTERRUPT_DEBUG         0
#define _TIMERINTERRUPT_LOGLEVEL_     1

#include <HCSR04.h>
#include "SAMDTimerInterrupt.h"

#define TIMER_INTERVAL_MS 100
#define SELECTED_TIMER TIMER_TC3

SAMDTimer MotionTimer(SELECTED_TIMER);

byte usdTrig = 20;
byte usdEcho = 21;
byte btnPin = 2;
byte ledPin = 8;
byte ledState = LOW;
double distance;
bool prevMotion = false;

void setup() {
  Serial.begin(9600);
  while(!Serial);

  HCSR04.begin(usdTrig, usdEcho);
  pinMode(btnPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(btnPin), button, CHANGE);
  MotionTimer.attachInterruptInterval_MS(TIMER_INTERVAL_MS, motion);
}

void loop() {
  Serial.println("hello");
  delay(500);
}

void button() {
  ledState = !ledState;
  digitalWrite(ledPin, ledState);
  Serial.println("Button was pressed!");
}

void motion() {
  double* distancePtr = HCSR04.measureDistanceCm();
  if(distancePtr != nullptr){
    distance = *distancePtr;
  }
  if(distance > 0 && distance <= 20 && !prevMotion)
  {
    digitalWrite(ledPin, HIGH);
    Serial.println("Motion detected!");
    prevMotion = !prevMotion;
  }
  else if (distance > 20){
    digitalWrite(ledPin, ledState);
    prevMotion = false;
  }
}
