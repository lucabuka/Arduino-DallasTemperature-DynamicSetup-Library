/**************************************************************

  Hardware Setup:
    - ESP32 Board or Arduino UNO
    - 2 x Dallas Temperature Bus (connected to 2 Digital Pins)
    - 4 x DS18x20 Temperature Sensors on Bus_0
        2 on on Bus_0
        2 on on Bus_1

  Application meta-code:
    - Configure the hardware in setConfiguration()
       - Initialize each Bus
       - Add Devices on each bus 
          !!! You need to change this with your specific hardware cfg
          !!! (addresses, Hardware Pins used, etc)         
    - Loop trough the configured Buses
        - RequestTemperature() on the Bus (every device)
        - Show the temperatures and "timing" for each Device

   NOTE: Precision
     Measured "delays" for different "Precision" setting
     of the Temperature Sensors (DS18B20):
         9 Bit:  86 mSec
        10 Bit: 169 mSec
        11 Bit: 336 mSec
        12 Bit: 669 mSec

 **************************************************************/
#include "DS18x20DallasBus.h"

#define BUS_NUMBER 2

// Allocate n DS18x20DallasBus objects (Top level representation of the bus and contained devices)
DS18x20DallasBus Bus[BUS_NUMBER];

void setup(void) {
  Serial.begin(115200);
  
  int err=setConfiguration();
  if(err){
    Serial.print("setConfiguration erro:");
    Serial.println(err);
  }
}


void loop() {

  unsigned long t0, t1;

  for (int i = 0; i < BUS_NUMBER; i++) {
    Serial.println("--------------------------------------------------");
    Serial.print("Bus:[");Serial.print(Bus[i].descr);Serial.print("] - Requesting temperatures...");
    
    t0 = millis();
    Bus[i].requestTemperatures();
    t1 = millis();
    Serial.print("DONE in ["); Serial.print(t1 - t0); Serial.println("] msec");
    
    for (int j = 0; j < Bus[i].devicesNum ; j++) {

      Serial.print("   device[");Serial.print(Bus[i].device[j].id);Serial.print("]:[");
      Serial.print(Bus[i].device[j].descr);Serial.print("] ");
      t0 = millis();

      float tempC = Bus[i].getTempC(Bus[i].device[j].addr);

      if (tempC == -127.00) {
        Serial.print("Error getting temperature");
      } else {
        Serial.print(tempC);
      }
      t1 = millis();
      Serial.print(" - getTempC() ["); Serial.print(t1 - t0); Serial.println("] msec");
    }
  }

  delay(5000);
}


int setConfiguration() {
  int err=0;

  // Bus 0: HardwarePin=25, Descr="Room 1" 
  // -------------------------------------
  Bus[0].begin( 25 , "Room 1");

  // Devices on Bus 0 
  // ----------------
  // Window_1, Precision 9
  err += Bus[0].addDevice( 0, "Window 1", "0x28,0xFF,0x08,0xA8,0x14,0x14,0x0,0xC0", 9);
  // Heater_1, Precision 9
  err += Bus[0].addDevice( 1, "Heater 1", "0x28,0xFF,0xD3,0xC1,0x14,0x14,0x0,0xED", 9);


  // Bus 1: HardwarePin=26, Descr="Garage" 
  // ------------------------------------
  Bus[1].begin( 26 , "Garage");
  
  // Devices on Bus 1 
  // ----------------
  // Window_1, Precision 9 
  err += Bus[1].addDevice( 0, "Window 1", "0x28,0xFF,0x09,0xAC,0x14,0x14,0x0,0x03", 9);
  // Window_1, Precision 9 
  err += Bus[1].addDevice( 1, "Window 2", "0x28,0xFF,0x3B,0xCC,0x14,0x14,0x0,0x94", 9);
  
  return(err);

}

