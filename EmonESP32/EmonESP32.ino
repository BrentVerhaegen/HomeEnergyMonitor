// EmonLibrary examples openenergymonitor.org, Licence GNU GPL V3
#include "EmonLib.h"    // Include Emon Library
#include <WiFi.h>
#include <HTTPClient.h>

#define GridVoltage 230
#define MeasuringSamples 813000/2
#define EmonCMS_API_Key "1837ba36ce9334841fc0c743f00265f2"

const char* ssid = "telenet-90C70";
const char* password =  "sYX8KQNpEuSb";

double measurePowerGrid();
double measurePowerSolar();
void sendHTTPPost(String, double);

EnergyMonitor grid;   // Create an instance for the power grid
EnergyMonitor solar;    // Create an instance for the solar panels            

void setup() {
  analogReadResolution(10);   //Doesn't work well with 12bit ADCValue for now
  Serial.begin(9600);

  /* Setup EnergyMonitor instances */
  grid.current(32,31.675);    //(Pin number on ESP32, calibration value)
  solar.current(34,5.15);
  
  delay(4000); // Needed before initializing the WiFi

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { //Check for the connection
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
}

void loop() {
   /* Variables for main loop */ 
  double PowerGrid;
  double PowerSolar;
  
  if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status 
    PowerGrid = measurePowerGrid();
    sendHTTPPost("GridPower", PowerGrid);
    PowerSolar = measurePowerSolar();
    sendHTTPPost("SolarPower", PowerSolar);
  }
  else{
    Serial.println("No longer connected to WiFi. Trying to reconnect...");
    delay(1000);   
  }
}

double measurePowerGrid(){
    double IrmsGrid = grid.calcIrms(MeasuringSamples);    //(Amount of measuring cycles, for now approx. 5 sec)
    double PowerGrid = IrmsGrid * GridVoltage;

    if(PowerGrid < 40){
      PowerGrid = 0;
    }
    
    return PowerGrid;
}

double measurePowerSolar(){
    double IrmsSolar = solar.calcIrms(MeasuringSamples);
    double PowerSolar = IrmsSolar * GridVoltage;

    if(PowerSolar < 20){
      PowerSolar = 0;
    }
      
    return PowerSolar;
}

void sendHTTPPost(String input, double power){
  HTTPClient http;   
  http.begin("http://192.168.0.203/emoncms/input/post?");  //Specify destination for HTTP request
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");             //Specify content-type header
  
  int httpResponseCode = http.POST("node=Power&fulljson={\"" + input + "\":" + power + "}&apikey=" + EmonCMS_API_Key);   //Send the actual POST request

  Serial.println("Sent the POST with the following data:  Input = " + input + " Power = " + power + " with the following answer :");  
  
  if(httpResponseCode>0){
    String response = http.getString();                       //Get the response to the request
    Serial.println(httpResponseCode);   //Print return code
    Serial.println(response);           //Print request answer
    Serial.println();
  }
  else{
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
    Serial.println();
  }
  http.end();  //Free resources
}
