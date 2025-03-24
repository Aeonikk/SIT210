#include <BH1750.h>
#include <Wire.h>
#include <WiFiNINA.h>
#include "secrets.h"

BH1750 lightMeter;

//Values from secrets.h file for connecting to WiFi
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
WiFiClient client;

//IFTTT trigger URL
char   HOST_NAME[] = "maker.ifttt.com";
String PATH_NAME   = "/trigger/light_activity/json/with/key/dCA6DO05Cyc0K9vC-auZcw";

//Previous light value
float prevLight = 0;
//Current light value
float light = 0;
//Required light
float plantLight = 25000;
//Minutes spent in required light value
float minInSun = 0;
String status;

void setup() {
  // put your setup code here, to run once:
  WiFi.begin(ssid, pass);

  Serial.begin(9600);
  while(!Serial);

  Wire.begin();
  lightMeter.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
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

  //Connect or reconnect to server
  if(!client.connected()){
    Serial.print("Attempting to connect to server: ");
    Serial.println(HOST_NAME);
    while(!client.connected()){
      client.connect(HOST_NAME, 80);
      Serial.print(".");
      delay(2000);
    }
    Serial.println("\nConnected.");
  }

  //Reading of current light level
  light = lightMeter.readLightLevel();
  
  //When plant enters 'full sunlight' send update
  if (light >= plantLight && prevLight < plantLight){
    status = "{\"update\":\"Entered sunlight :) \",\"hours in sun\": "+ String(minInSun/60) +"}";
    client.print(String("POST ") + PATH_NAME + " HTTP/1.1\r\n" +
                      "Host: " + String(HOST_NAME) + "\r\n" +
                      "Content-Type: application/json\r\n" +
                      "Content-Length: " + status.length() + "\r\n\r\n" +
                      status +"\r\n");
    prevLight = light;
    minInSun++;
  }
  //When plant exits 'full sunlight' send update
  if (light < plantLight && prevLight >= plantLight){
    status = "{\"update\":\"Exited sunlight :O \",\"hours in sun\": "+ String(minInSun/60) +"}";
    client.print(String("POST ") + PATH_NAME + " HTTP/1.1\r\n" +
                      "Host: " + String(HOST_NAME) + "\r\n" +
                      "Content-Type: application/json\r\n" +
                      "Content-Length: " + status.length() + "\r\n\r\n" +
                      status +"\r\n");
    prevLight = 0;
    minInSun = 0;
  }
  //If plant remains in 'full sunlight' increment the minutes in sun value
  if (light >= plantLight && prevLight >= plantLight){
    minInSun++;
  }
  Serial.println(light);
  delay(60000);
}
