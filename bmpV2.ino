#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <DHT.h>

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
float d_temp=0;
float d_hum=0;

Adafruit_BMP085 bmp;
DHT dht(DHTPIN, DHTTYPE);

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
  String h1 = "Humidity: ";
  String h2 = h1 + d_hum;
  String t1 = nl + "Temp from DHT: ";
  String t2 = t1 + d_temp;
  String t3 = t2 + " in *C" + nl;
  Serial.print(h2 + t3);
  }
  Serial.print(nl);
  delay(1000);
}
