#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <GravityTDS.h>
#include <EEPROM.h>
#include <ArduinoJson.h>

Adafruit_ADS1115 ads; // Create an instance of the ADS1115 object

const int TEMPERATURE_SENSOR_PIN = A1; // Arduino pin connected to the temperature sensor's DQ pin
const int TDS_SENSOR_PIN = A0;        // Arduino pin connected to the GravityTDS sensor's analog output pin
const int FLEX_SENSOR_PINS[] = {A2, A3, A4, A5}; // Arduino pins connected to the flex sensors

OneWire oneWire(TEMPERATURE_SENSOR_PIN);
DallasTemperature tempSensor(&oneWire);
GravityTDS gravityTds;

void setup()
{
  Serial.begin(9600);
  ads.begin(); // Initialize the ADS1115

  tempSensor.begin();
  gravityTds.setPin(TDS_SENSOR_PIN);
  gravityTds.setAref(5.0);
  gravityTds.setAdcRange(1024);
  gravityTds.begin();
}

void loop()
{
  // Read temperature, TDS value, and flex sensor values
  float temperature = readTemperature();
  float tdsValue = measureTDS(temperature);
  int flexValues[4];

  for (int i = 0; i < 4; i++) {
    flexValues[i] = readFlexSensor(FLEX_SENSOR_PINS[i]);
  }

  // Read values from ADS1115
  int adsValues[4];
  for (int i = 0; i < 4; i++) {
    adsValues[i] = readADS1115(i);
  }

  // Scale flex sensor values to a range of 0-100
  int flexScaled[4];
  for (int i = 0; i < 4; i++) {
    flexScaled[i] = map(flexValues[i], 0, 1023, 100, 0);
  }

  // Scale ADS1115 values to a range of 0-100
  int adsScaled[4];
  for (int i = 0; i < 4; i++) {
    adsScaled[i] = map(adsValues[i], 0, 32767, 100, 0);
  }

  // Print temperature, TDS value, flex sensor values, and ADS1115 values
  // printTemperature(temperature);
  // printTDSValue(tdsValue);
  // printFlexValues(flexScaled);
  // printADSValues(adsScaled);
  sendJson(temperature, tdsValue, flexScaled, adsScaled);

  delay(500);
}

// Function to read temperature from the sensor
float readTemperature()
{
  tempSensor.requestTemperatures();
  return tempSensor.getTempCByIndex(0);
}

// Function to measure TDS value
float measureTDS(float temperature)
{
  gravityTds.setTemperature(temperature);
  gravityTds.update();
  return gravityTds.getTdsValue();
}

// Function to read flex sensor value
int readFlexSensor(int pin)
{
  return analogRead(pin);
}

// Function to read ADS1115 value
int readADS1115(int channel)
{
  int16_t adcValue = ads.readADC_SingleEnded(channel);
  return adcValue;
}

// Function to print temperature
void printTemperature(float temperature)
{
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println("°C");
}

// Function to print TDS value
void printTDSValue(float tdsValue)
{
  Serial.print("TDS Value: ");
  Serial.print(tdsValue, 0);
  Serial.println("ppm");
}

// Function to print flex sensor values
void printFlexValues(int flexValues[])
{
  Serial.println("Flex Sensor Values:");
  for (int i = 0; i < 4; i++) {
    Serial.print("Sensor ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.println(flexValues[i]);
  }
}

// Function to print ADS1115 values
void printADSValues(int adsValues[])
{
  //Serial.println("ADS1115 Values:");
  for (int i = 0; i < 4; i++) {
    Serial.print("Sensor ");
    Serial.print(i + 5);
    Serial.print(": ");
    Serial.println(adsValues[i]);
  }
}

// takes in sensor data and marshals to josn before sending over serial.
void sendJson(float temperature, float ppm, float flexSensorData[4], float ads1115Data[4]) {
  StaticJsonDocument<200> jsonDoc; // you might need to adjust the size

  jsonDoc["temperature"] = temperature;  
  jsonDoc["ppm"] = ppm;

 JsonArray accelArray = jsonDoc.createNestedArray("accel_data");
  for(int i = 0; i < 4; i++) {
    accelArray.add(flexSensorData[i]);
  }
  for(int i = 0; i < 4; i++) {
    accelArray.add(ads1115Data[i]);
  }

  serializeJson(jsonDoc, Serial);
  Serial.println();
}
