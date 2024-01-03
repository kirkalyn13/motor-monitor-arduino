#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h> 

// UNIT ID:
const int UNIT_ID = 1137; // Unique per unit; Change as needed.

const char* ssid     = ""; // To be changed to a generic value.
const char* password = ""; // To be changed to a generic value.
const char* host = "motor-monitor-backend.onrender.com"; // Backend server URL
const String userAgent = "motor-monitor-arduino/1.0";
const int port = 443;
const int timeout = 60000; // RTO at 60 seconds
const int serial_wait = 100;
WiFiClientSecure wifiClient;

struct MetricsModel {
  float line1Voltage;
  float line2Voltage;
  float line3Voltage;
  float line1Current;
  float line2Current;
  float line3Current;
  float temperature;
};

MetricsModel parseMetrics(const String& input) {
  MetricsModel metrics;

  // Using c_str() to get a char array for strtok
  char buffer[input.length() + 1];
  strcpy(buffer, input.c_str());

  // Tokenizing the input string using strtok
  char* token = strtok(buffer, ",");
  
  // Mapping tokens to struct fields
  metrics.line1Voltage = atof(token);
  token = strtok(nullptr, ",");
  metrics.line2Voltage = atof(token);
  token = strtok(nullptr, ",");
  metrics.line3Voltage = atof(token);
  token = strtok(nullptr, ",");
  metrics.line1Current = atof(token);
  token = strtok(nullptr, ",");
  metrics.line2Current = atof(token);
  token = strtok(nullptr, ",");
  metrics.line3Current = atof(token);
  token = strtok(nullptr, ",");
  metrics.temperature = atof(token);

  return metrics;
}

bool postRequest(String method, String endpoint)
{
  bool isSuccess = false;
  bool finishedHeaders = false;
  bool currentLineIsBlank = true;
  bool gotResponse = false;
  long now;

  String headers = "";
  String body = "";

  if (wifiClient.connect(host, port)) {
    // Format POST Request
    wifiClient.println("POST " + endpoint + " HTTP/1.1");
    wifiClient.print("Host: "); wifiClient.println(host);
    wifiClient.println("Content-Type: application/x-www-form-urlencoded");
    wifiClient.println("User-Agent: " + userAgent);
    wifiClient.println("");

    now = millis();
    while (millis() - now < timeout) {
      while (wifiClient.available()) {
        char c = wifiClient.read();
        // Serial.print(c); // For testing only

        if (finishedHeaders) {
          body = body + c; 
        } else {
          if (currentLineIsBlank && c == '\n') {
            finishedHeaders = true;
          }
          else {
            headers = headers + c;
          }
        }

        if (c == '\n') {
          currentLineIsBlank = true;
        } else if (c != '\r') {
          currentLineIsBlank = false;
        }

        gotResponse = true;

      }
      if (gotResponse) {
        isSuccess = true;
        break;
      }
    }
  }

  return isSuccess;
}

// Change args to MetricsModel when migrated
void sendMetrics(String dataString) 
{
  MetricsModel metrics = parseMetrics(dataString);
  String queryParams = "line1Voltage=" + String(metrics.line1Voltage) + "&"
  + "line2Voltage=" + String(metrics.line2Voltage) + "&"
  + "line3Voltage=" + String(metrics.line3Voltage) + "&"
  + "line1Current=" + String(metrics.line1Current) + "&"
  + "line2Current=" + String(metrics.line2Current) + "&"
  + "line3Current=" + String(metrics.line3Current) + "&"
  + "temperature=" + String(metrics.temperature);
  String endpoint = "/api/v1/metrics/" + String(UNIT_ID) + "?" + queryParams;

  bool result = postRequest("POST", endpoint);
  if (result == true) {
    Serial.print("Success: " + dataString);
  } else {
    Serial.print("Error Occured: " + dataString);
  }
}

void initializeWifi()
{
  delay(1000);
  WiFi.mode(WIFI_OFF);        
  WiFi.mode(WIFI_STA);        
  
  WiFi.begin(ssid, password); 
  Serial.println("");

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  wifiClient.setInsecure();
}


void setup()
{
  Serial.begin(9600);
  initializeWifi();
}



void loop()
{
  if ( Serial.available() ) { 
    String data = Serial.readString();
    sendMetrics(data);
  }
  delay(serial_wait);
}