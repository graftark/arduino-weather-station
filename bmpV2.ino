#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <SPI.h>
#include "DHT.h"
#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>

#define DHTPIN 2
#define DHTTYPE DHT11

//vars for bmp
float tempC=0;
float tempF=0;
float pressure=0;
float InHg=0;
float Am=0;
float Af=0;

//vars for dht
float d_temp;
float d_hum;

Adafruit_BMP085 bmp;
DHT dht(DHTPIN, DHTTYPE);
//Set up radio
RF24 radio(9,10);
// Topology
// Radio pipe address for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0XF0F0F0F0D2LL };

// ROLES 
typedef enum { role_sender = 1, role_receiver } role_e;
// debug friendly role names
const char* role_friendly_name[] = { "invalid","Sender","Receiver" };
// default role
role_e role = role_receiver;
  
void setup() {
  // Initialize Serial Comm
  Serial.begin(9600);
  // Initialize BMP
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
  // Initialize DHT
  dht.begin();

}

void loop() {
  // Add new lines easy
  String nl = "\n";
  /*************************
   * Code for BMP180       *
   *************************/
  // Get temp and convert to F
  tempC=bmp.readTemperature();
  tempF=((tempC*9)/5)+32;
  Serial.print("Temperature: ");
  Serial.print(tempF + nl);
  // Get the barometric pressure
  pressure=bmp.readPressure();
  InHg=pressure*0.000295333727;
  String p1 = "Pressure: ";
  String p2 = p1 + InHg;
  Serial.print(p2 + " In Hg" + nl);
  // Get the altitude
  Am = bmp.readAltitude(101520);
  Af = Am*3.28084;
  String a1 = "Altitude: ";
  String a2 = a1 + Af;
  Serial.print(a2 + " ft." + nl);
  /**************************
   * Code for DHT-11        *
   **************************/
  if(isnan(d_temp)||isnan(d_hum)){
    Serial.println("DHT read failure");
  }else{
  d_temp = dht.readTemperature(true);
  d_hum = dht.readHumidity();
  
  float d_hi = dht.computeHeatIndex(d_temp,d_hum);
  float x_hi = dht.computeHeatIndex(tempF,d_hum);
  
  String h1 = "Humidity: ";
  String h2 = h1 + d_hum;
  String h3 = h2 + " %\t ";
  String t1 = nl + "Temp from DHT: ";
  String t2 = t1 + d_temp;
  String t3 = t2 + " in *F" + nl;
  String i1 = "Heat Index: ";
  String i2 = i1 + d_hi;
  String i3 = i2 + " *F" + nl;
  String x1 = "Heat Index using BMP Temp: ";
  String x2 = x1 + x_hi;
  String x3 = x2 + " *F" + nl;
  
  Serial.print(h3 + t3 + i3 + x3);
  }
  Serial.print(nl);
  delay(1000);
}
