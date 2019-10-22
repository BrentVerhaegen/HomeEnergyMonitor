// EmonLibrary examples openenergymonitor.org, Licence GNU GPL V3

#include "EmonLib.h"                   // Include Emon Library
EnergyMonitor emon1;                   // Create an instance

double Irms = 0;

void setup()
{  
  analogReadResolution(10);
  Serial.begin(9600);
  //emon1.current(32, 31.675);             // Current: input pin, calibration. (For 30A/1V sensor)
  //emon1.current(32, 5.15);             // Current: input pin, calibration. (For 5A/1V sensor, Max 1600w)

}

void loop()
{
    double duration = millis();
    double Irms = emon1.calcIrms((813000/2));  // Calculate Irms only ( double calcIrms(unsigned int NUMBER_OF_SAMPLES);)
    duration = (millis() - duration);
  
  Serial.print(Irms*230.0);         //power
  Serial.print(" / ");
  Serial.println(Irms);          
  //delay(300);
  
  //Serial.println(duration);
}
