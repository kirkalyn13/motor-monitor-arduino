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

// VOLTAGE FUNCTIONS:

// TEMPERATURE FUNCTIONS:
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
  Serial.print("Temp C: ");
  Serial.print(tempC);
  Serial.print(" Temp F: ");
  Serial.println(DallasTemperature::toFahrenheit(tempC)); // Converts tempC to Fahrenheit
}

// SETUP
void setup(void)
{
  Serial.begin(9600);
  
  emon4.current(A3, 111.1);             // Current: input pin, calibration.
  emon5.current(A4, 111.1); 
  emon6.current(A5, 111.1); 

  Serial.begin(9600);
  
  emon1.voltage(VS1_PIN, VOLT_CAL_1, 1.7);  // Voltage: input pin, calibration, phase_shift
  emon2.voltage(VS2_PIN, VOLT_CAL_2, 1.7); 
  emon3.voltage(VS3_PIN, VOLT_CAL_3, 1.7); 

  // start serial port
  Serial.begin(9600);
  Serial.println("Dallas Temperature IC Control Library Demo");

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
  double Irms_1 = emon4.calcIrms(1480);  // Calculate Irms only
  Serial.println(Irms_1-0.55);		       // Irms
  double Irms_2 = emon5.calcIrms(1480);  // Calculate Irms only
  Serial.println(Irms_2-0.55);		       // Irms
  double Irms_3 = emon6.calcIrms(1480);  // Calculate Irms only
  Serial.println(Irms_3-0.55);		       // Irms


  emon1.calcVI(25,3000);         // Calculate all. No.of half wavelengths (crossings), time-out
  emon2.calcVI(25,3000);
  emon3.calcVI(25,3000);

  float supplyVoltage_1   = emon1.Vrms;
  float supplyVoltage_2   = emon2.Vrms; 
  float supplyVoltage_3   = emon3.Vrms;              //extract Vrms into Variable

  Serial.print("Voltage 1 : ");  
  Serial.println(supplyVoltage_1);


  Serial.print("Voltage 2 : ");  
  Serial.println(supplyVoltage_2);

  Serial.print("Voltage 3 : ");  
  Serial.println(supplyVoltage_3);

  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  
  // It responds almost immediately. Let's print out the data
  printTemperature(insideThermometer); // Use a simple function to print out the data
}
