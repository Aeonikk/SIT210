#include "DHT.h"
#include <BH1750.h>
#include <Wire.h>
#include <WiFiNINA.h>
#include "secrets.h"
#include "ThingSpeak.h"

DHT dht(2, DHT11);
BH1750 lightMeter;

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

void setup() {
  Serial.begin(9600);
  while(!Serial);
  dht.begin();
  Wire.begin();
  lightMeter.begin();
  ThingSpeak.begin(client);
}

void loop() {
  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network
      Serial.print(".");
      delay(2000);     
    } 
    Serial.println("\nConnected.");
  }

  float temp = dht.readTemperature();
  float light = lightMeter.readLightLevel();

  ThingSpeak.setField(1, temp);
  ThingSpeak.setField(2, light);

  Serial.print(F("Temperature: "));
  Serial.print(temp);
  Serial.println(F("°C"));

  Serial.print(F("Light: "));
  Serial.print(light);
  Serial.println(F("lx"));

  // write to the ThingSpeak channel 
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  delay(60000);
}
