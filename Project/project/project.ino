#include <Servo.h>
#include <BH1750.h>
#include <Wire.h>

//Component setup
Servo myServo;
BH1750 lightMeter;

//Pin allocation
const int soilPin = A7;
const int pumpPin = 2;
const int servoPin = A0;

//Configured soil sensor values
const float dry = 850.0;
const float wet = 339.0;

//Variables relevant to watering system
float rawMoisture = 0.0;
float percentMoisture = 0.0;
//% moisture to water until
float afterWater = 40.0;
//% moisture to start watering cycle
float waterThreshold = 20.0;
bool watering = false;

//Variables relevant to curtain system
float currentLight = 0.0;
float dangerLight = 10000;
bool curtainOpen = false;

//Values for timing
unsigned long lastLightCheck = 0;
unsigned long lastMoistureCheck = 0;
const unsigned long lightInterval = 600000;
const unsigned long defaultMoistureInterval = 900000;
unsigned long moistureInterval = defaultMoistureInterval;
unsigned long currentTime = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  myServo.attach(servoPin);
  Wire.begin();
  lightMeter.begin();
  pinMode(pumpPin, OUTPUT);
}

void curtain(String change){
  //If opening curtain
  if (change == "open"){
    //Turn servo counter-clockwise
    myServo.write(180);
    delay(5000);
    //Stop servo
    myServo.write(90);
    curtainOpen = true;
  }
  //If closing curtain
  if (change == "close"){
    //Turn servo clockwise
    myServo.write(0);
    delay(5000);
    //Stop servo
    myServo.write(90);
    curtainOpen = false;
  }
}

void water(){
  digitalWrite(pumpPin, HIGH);
  delay(3000);
  digitalWrite(pumpPin, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  currentTime = millis();

  if (currentTime - lastLightCheck >= lightInterval){
    lastLightCheck = currentTime;

    currentLight = lightMeter.readLightLevel();
    Serial.print("Current Light Level (Lux): ");
    Serial.println(currentLight);

    //If light level is dangerous and curtian is open, close it
    if (currentLight >= dangerLight && curtainOpen){
      curtain("close");
    }
    //If light level is not dangerous and curtain is closed, open it
    if(currentLight < dangerLight && !curtainOpen){
      curtain("open");
    }
  }

  if (currentTime - lastMoistureCheck >= moistureInterval){
    lastMoistureCheck = currentTime;

    rawMoisture = analogRead(soilPin);
    percentMoisture = (rawMoisture - dry) / (wet - dry) * 100;
    percentMoisture = abs(percentMoisture);
    Serial.print("Soil Moisture %: ");
    Serial.println(percentMoisture);

    //If moisture is below watering threshold or we are in a watering cycle
    //water the plant and reduce the time between checking moisture during watering cycle
    if (percentMoisture <= waterThreshold || watering){
      water();
      moistureInterval = 300000;
    }

    //If we are in a watering cycle and the moisture is now at a good level, stop watering
    //and return to default interval
    if (percentMoisture >= afterWater && watering){
      watering = false;
      moistureInterval = defaultMoistureInterval;
    }
  }
}
