#include "EmonLib.h"                   // Include Emon Library
EnergyMonitor emon4;                   // Create an instance
EnergyMonitor emon5;
EnergyMonitor emon6;

#include "EmonLib.h"             // Include Emon Library

#define VOLT_CAL_1 407
#define VOLT_CAL_2 395
#define VOLT_CAL_3 510

EnergyMonitor emon1;             // Create an instance
EnergyMonitor emon2; 
EnergyMonitor emon3; 

const int VS1_PIN = A0 ;
const int VS2_PIN = A1 ;
const int VS3_PIN = A2 ;


// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// arrays to hold device address
DeviceAddress insideThermometer;

const float PHASE_SHIFT = 1.7;
const float CURRENT_CALIBRATION = 14.5;
const int DELAY = 1000;

// VOLTAGE FUNCTIONS:
void measureVoltage() {
   // Calculate all. No.of half wavelengths (crossings), time-out
  emon1.calcVI(25,DELAY);         
  emon2.calcVI(25,DELAY);
  emon3.calcVI(25,DELAY);

  //extract Vrms into Variable
  float supplyVoltage_1   = emon1.Vrms;
  float supplyVoltage_2   = emon2.Vrms; 
  float supplyVoltage_3   = emon3.Vrms;              

  Serial.print("Voltage 1 : ");  
  Serial.println(supplyVoltage_1);


  Serial.print("Voltage 2 : ");  
  Serial.println(supplyVoltage_2);

  Serial.print("Voltage 3 : ");  
  Serial.println(supplyVoltage_3);
}

// CURRENT FUNCTIONS
void measureCurrent() {
  double Irms_1 = emon4.calcIrms(CURRENT_CALIBRATION);  // Calculate Irms only
  Serial.print("Current 1 : "); 
  Serial.println(Irms_1);		       // Irms
  double Irms_2 = emon5.calcIrms(CURRENT_CALIBRATION);  // Calculate Irms only
  Serial.print("Current 2 : "); 
  Serial.println(Irms_2);		       // Irms
  double Irms_3 = emon6.calcIrms(CURRENT_CALIBRATION);  // Calculate Irms only
  Serial.print("Current 3 : "); 
  Serial.println(Irms_3);		       // Irms
}

// TEMPERATURE FUNCTIONS:
void measureTemperature() {
  sensors.requestTemperatures(); // Send the command to get temperatures
  printTemperature(insideThermometer); // Use a simple function to print out the data
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
void printTemperature(DeviceAddress deviceAddress)
{
 
  float tempC = sensors.getTempC(deviceAddress);
  if(tempC == DEVICE_DISCONNECTED_C) 
  {
    Serial.println("Error: Could not read temperature data");
    return;
  }
  Serial.print("Temp C : ");
  Serial.println(tempC);
  // Serial.print(" Temp F: ");
  // Serial.println(DallasTemperature::toFahrenheit(tempC)); // Converts tempC to Fahrenheit
}

// SETUP
void setup(void)
{
  Serial.println("MOTOR MONITOR");
  Serial.println("Initializing...");
  Serial.begin(9600);

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
  //measureVoltage();
  measureCurrent();
 // measureTemperature();
}
