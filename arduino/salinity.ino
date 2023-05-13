#include <Arduino.h>

const int sensorPin = A0; // analog input pin for salinity sensor
const float vRef = 5.0; // reference voltage for ADC
const float vOffset = 0.5; // voltage offset for ADC
const float k = 0.1333; // conversion factor for salinity

void setup()
{
  Serial.begin(9600); // initialize serial communication
}

void loop()
{
  float voltage = analogRead(sensorPin) * vRef / 1023.0 - vOffset; // read sensor voltage
  float salinity = voltage / k; // convert voltage to salinity
  Serial.print(salinity, 2); // send salinity value over serial interface with 2 decimal places
  Serial.println(" ppt"); // add unit of measurement
  delay(100); // wait 1/10 second before sending the next value
}
