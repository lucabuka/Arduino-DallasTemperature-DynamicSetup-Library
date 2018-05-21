/**************************************************************

Sample code to demostrate the usage of main functionality:

- Configure TWO Buses with 2 devices each

  - Demo usage 1) RequestTemperature() on Each Bus (every device) and display 
    [Current Temperature], [Previous Temperaturere], [hasChanged], [Configured Epsilon]
    
  - Demo usage 2) Get temperature for a single device on a Bus - by Device ID 
        
  Hardware Setup:
    - ESP32 Board or Arduino UNO
    - 2 x Dallas Temperature Bus (connected to 2 Digital Pins)
    - 4 x DS18x20 Temperature Sensors on Bus_0
        2 on on Bus_0
        2 on on Bus_1


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

  Serial.println("--------------------------------------------------");

  
  Serial.println("\nExample (1): Loop through Bus and request [NOW] [PREV] [HAS_CHANGED] temp for every configured device:");
  for (int i = 0; i < BUS_NUMBER; i++) {
    Serial.print("  Bus: Id["); Serial.print(Bus[i].id);Serial.print("]["); Serial.print(Bus[i].descr);Serial.print("] - Requesting temperatures...");
    
    t0 = millis();
    Bus[i].requestTemperatures();
    t1 = millis();
    Serial.print("DONE in ["); Serial.print(t1 - t0); Serial.println("] msec");
    
    for (int j = 0; j < Bus[i].devicesNum ; j++) {

      Serial.print("     device[");Serial.print(Bus[i].device[j].id);Serial.print("]:[");
      Serial.print(Bus[i].device[j].descr);Serial.print("] ");
      t0 = millis();
      
      float Tnow=0,Tprev=0;   
      int hasChanged=Bus[i].deviceGetData(j, &Tnow, &Tprev );
      Serial.print("Tnow:["); Serial.print(Tnow); Serial.print("] Tprev:["); Serial.print(Tprev);
      Serial.print("] - hasChanged:["); Serial.print(hasChanged); Serial.print("] - Using Epsilon:["); 
      Serial.print(Bus[i].device[j].epsilon); Serial.print("]");
 
      t1 = millis();
      Serial.print(" - getData() ["); Serial.print(t1 - t0); Serial.println("] msec");
    }
  }
  
  float tempC=-1000.0;
  
  // Get temperature by device ID
  Serial.print("\nExample (2): Get temperature by device ID - Bus:[0]- Device id:[7]\n    getTempCById(7): ");
  tempC = Bus[0].getTempCById(7) ;
  if (tempC == DEVICE_DISCONNECTED_C) {
    Serial.println("Error getting temperature");
  } else {
      Serial.println(tempC);
  }
 

  delay(5000);
}


int setConfiguration() {
  int err=0;

  // Bus 0: HardwarePin=25, Descr="Room 1" 
  // -------------------------------------
  Bus[0].begin( 25 , "Room 1", 1);

  // Devices on Bus 0 
  // ----------------
  // Window_1, Precision 9, Epsilon 1.5
  err += Bus[0].addDevice( 1, "Window 1", "0x28,0xFF,0x08,0xA8,0x14,0x14,0x0,0xC0", 9, 1.5);
  // Heater_1, Precision 9, Epsilon 1.5
  err += Bus[0].addDevice( 7, "Heater 1", "0x28,0xFF,0xD3,0xC1,0x14,0x14,0x0,0xED", 9, 1.5);


  // Bus 1: HardwarePin=26, Descr="Garage" 
  // ------------------------------------
  Bus[1].begin( 26 , "Garage", 2);
  
  // Devices on Bus 1 
  // ----------------
  // Window_1, Precision 9 , Epsilon 1.0
  err += Bus[1].addDevice( 1, "Window 1", "0x28,0xFF,0x09,0xAC,0x14,0x14,0x0,0x03", 9, 1.0);
  // Window_1, Precision 9 , Epsilon 0.5
  err += Bus[1].addDevice( 2, "Window 2", "0x28,0xFF,0x3B,0xCC,0x14,0x14,0x0,0x94", 9, 0.5);
  
  return(err);

}

