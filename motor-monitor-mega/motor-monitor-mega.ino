#include "EmonLib.h"
#include <OneWire.h>
#include <DallasTemperature.h>

// UNIT ID (UNIQUE PER UNIT):
#define UNIT_ID 1137

EnergyMonitor emon1;
EnergyMonitor emon2; 
EnergyMonitor emon3; 
EnergyMonitor emon4;
EnergyMonitor emon5;
EnergyMonitor emon6;

#define VOLT_CAL_1 500
#define VOLT_CAL_2 395
#define VOLT_CAL_3 516

const int VS1_PIN = A0 ;
const int VS2_PIN = A1 ;
const int VS3_PIN = A2 ;

const float PHASE_SHIFT = 1.7;
const float CURRENT_CALIBRATION = 14;
const int DELAY = 100;
const int GRANULARITY = 29;

#define LED_PIN 13
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer;

// For metrics computation
float voltageMetrics1 = 0.00;
float voltageMetrics2 = 0.00;
float voltageMetrics3 = 0.00;
float currentMetrics1 = 0.00;
float currentMetrics2 = 0.00;
float currentMetrics3 = 0.00;
float temperatureMetrics = 0.00;

// Structs
struct Metrics {
  float value1;
  float value2;
  float value3;
};

struct MetricsModel {
  float line1Voltage;
  float line2Voltage;
  float line3Voltage;
  float line1Current;
  float line2Current;
  float line3Current;
  float temperature;
};

// WIFI FUNCTIONS:
void sendMetrics(MetricsModel metrics) {
  String dataString = String(metrics.line1Voltage) + "," +
    String(metrics.line2Voltage) + "," +
    String(metrics.line3Voltage) + "," +
    String(metrics.line1Current) + "," +
    String(metrics.line2Current) + "," +
    String(metrics.line3Current) + "," +
    String(metrics.temperature) + "\n";
  
  Serial3.write(dataString.c_str());
  Serial.print(dataString);
}

// VOLTAGE FUNCTIONS:
Metrics measureVoltage() {
  Metrics voltageMetrics;

   // Calculate all. No.of half wavelengths (crossings), time-out
  emon1.calcVI(25,DELAY);         
  emon2.calcVI(25,DELAY);
  emon3.calcVI(25,DELAY);

  //extract Vrms into Variable
  voltageMetrics.value1  = emon1.Vrms;
  voltageMetrics.value2  = emon2.Vrms; 
  voltageMetrics.value3  = emon3.Vrms;              

  // Serial.print("Voltage 1 : ");  
  // Serial.println(voltageMetrics.value1);
  // Serial.print("Voltage 2 : ");  
  // Serial.println(voltageMetrics.value2);
  // Serial.print("Voltage 3 : ");  
  // Serial.println(voltageMetrics.value3 );

  return voltageMetrics;
}

// CURRENT FUNCTIONS
Metrics measureCurrent() {
  Metrics currentMetrics;

  double irms1 = emon4.calcIrms(2000) + 0.1;  // Calculate Irms only
  double irms2 = emon5.calcIrms(2000) + 0.1;  // Calculate Irms only
  double irms3 = emon6.calcIrms(2000) + 0.1;  // Calculate Irms only

  // Serial.print("Current 1 : "); 
  // Serial.println(irms1);		       // Irms
  // Serial.print("Current 2 : "); 
  // Serial.println(irms2);		       // Irms
  // Serial.print("Current 3 : "); 
  // Serial.println(irms3);		       // Irms

  currentMetrics.value1 = irms1;
  currentMetrics.value2 = irms2;
  currentMetrics.value3 = irms3;

  return currentMetrics;
}

// TEMPERATURE FUNCTIONS:
float measureTemperature() {
  sensors.requestTemperatures(); // Send the command to get temperatures
  return getTemperature(insideThermometer); // Use a simple function to print out the data
}
// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

// function to print the temperature for a device
float getTemperature(DeviceAddress deviceAddress)
{
 
  float tempC = sensors.getTempC(deviceAddress);
  if(tempC == DEVICE_DISCONNECTED_C) 
  {
    // Serial.println("Error: Could not read temperature data");
    return 0.00;
  }
  // Serial.print("Temp C : ");
  // Serial.println(tempC);

  return tempC;
  // Serial.print(" Temp F: ");
  // Serial.println(DallasTemperature::toFahrenheit(tempC)); // Converts tempC to Fahrenheit
}

// SETUP
void setup(void)
{
  Serial.println("MOTOR MONITOR");
  Serial.println("Initializing...");
  Serial.begin(9600);
  Serial3.begin(9600);

  // LED indicator
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  // Voltage: input pin, calibration, phase_shift
  emon1.voltage(VS1_PIN, VOLT_CAL_1, PHASE_SHIFT);  
  emon2.voltage(VS2_PIN, VOLT_CAL_2, PHASE_SHIFT); 
  emon3.voltage(VS3_PIN, VOLT_CAL_3, PHASE_SHIFT); 

  // Current: input pin, calibration.
  emon4.current(A3, CURRENT_CALIBRATION);             
  emon5.current(A4, CURRENT_CALIBRATION); 
  emon6.current(A5, CURRENT_CALIBRATION); 

  // locate devices on the bus
  Serial.print("Locating devices...");
  sensors.begin();
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

  // report parasite power requirements
  Serial.print("Parasite power is: "); 
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");
  

  if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0"); 
  
  Serial.print("Device 0 Address: ");
  printAddress(insideThermometer);
  Serial.println();

  // set the resolution to 9 bit (Each Dallas/Maxim device is capable of several different resolutions)
  sensors.setResolution(insideThermometer, 9);
 
  Serial.print("Device 0 Resolution: ");
  Serial.print(sensors.getResolution(insideThermometer), DEC); 
  Serial.println();
}

// MAIN LOOP
void loop(void)
{ 
  voltageMetrics1 = 0.00;
  voltageMetrics2 = 0.00;
  voltageMetrics3 = 0.00;
  currentMetrics1 = 0.00;
  currentMetrics2 = 0.00;
  currentMetrics3 = 0.00;
  temperatureMetrics = 0.00;

  for (int i = 0; i < GRANULARITY; i++) {
    Metrics voltageMetrics = measureVoltage();
    Metrics currentMetrics = measureCurrent();

    voltageMetrics1 += voltageMetrics.value1;
    voltageMetrics2 += voltageMetrics.value2;
    voltageMetrics3 += voltageMetrics.value3;
    currentMetrics1 += currentMetrics.value1;
    currentMetrics2 += currentMetrics.value2;
    currentMetrics3 += currentMetrics.value3;
    temperatureMetrics += measureTemperature();

    if ( i == GRANULARITY - 1 ) {
      MetricsModel metrics;

      metrics.line1Voltage = voltageMetrics1/GRANULARITY;
      metrics.line2Voltage = voltageMetrics2/GRANULARITY;
      metrics.line3Voltage = voltageMetrics3/GRANULARITY;
      metrics.line1Current = currentMetrics1/GRANULARITY;
      metrics.line2Current = currentMetrics2/GRANULARITY;
      metrics.line3Current = currentMetrics3/GRANULARITY;
      metrics.temperature = temperatureMetrics/GRANULARITY;

      sendMetrics(metrics);
    }
  }
}
