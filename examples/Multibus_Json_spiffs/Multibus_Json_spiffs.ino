/**************************************************************


  Meta-code of this example:
    - Configure the hardware loading a JSON file in the SPIFFS file system         
    - Loop trough the configured Buses
        - RequestTemperature() on the Bus (every device)
        - Show the temperatures and "timing" for each Device

  Hardware Setup:
    - ESP32 Board ONLY -- Uses SPIFFS File System
      - It can be ported to Arduino removing dependencies from SPIFFS
        and using SD Card  
    - 2 x Dallas Temperature Bus (connected to 2 Digital Pins)
    - 5 x DS18x20 Temperature Sensors on Bus_0
        2 on on Bus_0
        3 on on Bus_1

  TO BE ABLE TO RUN THIS EXAMPLE: -> SPIFFS, ./data/appConfig.json
  ----------------------------------------------------------------
  This example READ buses cfg from a JSON File
      ./data/appConfig.json
    
  !!! You HAVE TO MODIFY the Json file with your specific 
  !!! hardware configuration (Pin used, Devices ADDR, etc), 
  !!! and UPLOAD the modified file in the SPIFFS file system 
  !!! of the ESP32 board.

  If you are using the Arduino IDE, an easy way to do it 
  is installing a TOOL for the Arduin IDE: 
    "ESP32 Sketch Data Upload for Arduino IDE" 
    https://github.com/me-no-dev/arduino-esp32fs-plugin 
  When you UPLOAD the ./data dir content remember to CLOSE
  the Serial Monitor (or the Upload will fail)

  PLEASE NOTE:
    If you've never use before the SPIFFS file system on
    the board you are using, the UPLOAD and the SPIFFS.begin()
    in the setup() will FAIL (Mount Failed)
    You need to format the file system. To do so CHANGE in the setup()
    the call to SPIFFS.begin() with SPIFFS.begin(1).
 
   NOTE: Precision
     Measured "delays" for different "Precision" setting
     of the Temperature Sensors (DS18B20):
         9 Bit:  86 mSec
        10 Bit: 169 mSec
        11 Bit: 336 mSec
        12 Bit: 669 mSec

 **************************************************************/

#include "FS.h"
#include "SPIFFS.h"
#include <ArduinoJson.h>

#include "DS18x20DallasBus.h"

#define APP_CFG_FNAME "/appConfig.json" // Application Config File
#define MAX_BUS_NUMBER 4

// Allocate 4 DS18x20DallasBus objects (Top level representation of theuuuuuuuuuu bus and contained devices)
DS18x20DallasBusJson Bus[MAX_BUS_NUMBER];
int totDallaBusFound=0;


void setup(void) {
  Serial.begin(115200);
  
  // Open ESP32 internal file system where cfg file is stored
  if(!SPIFFS.begin()){  // this FAILS if the file system was never formatted
                        // Calling SPIFFS.begin(1) will FORMAT the FS on opening error
    Serial.println("SPIFFS Mount Failed");
    return;
  }
  int err=loadConfiguration(SPIFFS, APP_CFG_FNAME, & totDallaBusFound );
  if(err){
    Serial.print("loadConfiguration erro:");
    Serial.println(err);
  }

}


void loop() {

  unsigned long t0, t1;

  for (int i = 0; i < totDallaBusFound; i++) {
    Serial.println("--------------------------------------------------");
    Serial.printf("Bus:[%d][%s] - Requesting temperatures...", Bus[i].id, Bus[i].descr);
    t0 = millis();
    Bus[i].requestTemperatures();
    t1 = millis();
    Serial.printf("DONE in [%ld] msec\n", t1 - t0);

    for (int j = 0; j < Bus[i].devicesNum ; j++) {

      Serial.printf("\tdevice[%d]:[%s] ", Bus[i].device[j].id, Bus[i].device[j].descr );
      t0 = millis();
      float Tnow=0,Tprev=0;   
      int hasChanged=Bus[i].deviceGetData(j, &Tnow, &Tprev );
      Serial.printf("Tnow:[%f] Tprev:[%f] - hasChanged[%d] - Using Epsilon:[%f]",Tnow, Tprev, hasChanged, Bus[i].device[j].epsilon  );   
      
      t1 = millis();
      Serial.printf(" - getData() [%ld] msec\n", t1 - t0);
    }
  }

  delay(5000);
}




int loadConfiguration(fs::FS &fs, const char * path, int * bus_tot_number) {
  int retVal=0;

  Serial.printf("\nReading file: %s\r\n", path);

  File file = fs.open(path);
  if(!file || file.isDirectory()){
      Serial.println("- failed to open file for reading");
      return(1);
  }
  Serial.println("App Settings read from file:");

  // Allocate the memory pool on the stack.
  // Don't forget to change the capacity to match your JSON document.
  // Use arduinojson.org/assistant to compute the capacity.
  StaticJsonBuffer<4096> jsonBuffer;
  
  // Parse the root object
  JsonObject &root = jsonBuffer.parseObject(file);
  if (!root.success())
    Serial.println(F("!!!ERROR: Failed to read file - No devices configured !!! - Pls check if file exist and its syntax"));

  file.close();
  //root.prettyPrintTo(Serial);

  int n_bus = root["OneWire"]["OneWireBus"].size();
  if(n_bus > MAX_BUS_NUMBER){
    Serial.printf("\n !!!CONFIGURATION ERROR: Found [%d] bus - Compile-time MAX_BUS_NUMBER=[%d]!!!\n",n_bus,MAX_BUS_NUMBER);
    n_bus = MAX_BUS_NUMBER;
  }
  // Set number of buses found in cfg file
  *bus_tot_number = n_bus;

  for (int i = 0; i < n_bus; i++) {
    Bus[i].setDebugStream(Serial); // activate debug output (error messages)
    Bus[i].setTraceStream(Serial); // activate Trace output (trace activity and internal status messages)    
    retVal+=Bus[i].loadConfig(root["OneWire"]["OneWireBus"][i]);
  }

  return retVal;
}




