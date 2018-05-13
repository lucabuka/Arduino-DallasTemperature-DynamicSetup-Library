#ifndef DS18x20DallasBus_h
#define DS18x20DallasBus_h

// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version


#define MAX_DEVICES_ON_BUS 5    // Max devices on a single OneWire Bus 
                                // (fixed-size array in DS_BUS struct) 
#define BUS_DESCR_LEN 12        // Max Lenght for the BUS string "ID" 
#define DEVICE_DESCR_LEN 12     // Max Lenght for the DEVICE string "ID"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <OneWire.h>
#include <DallasTemperature.h>

class DS18x20DallasBus {


private:
  // Note: 
  //    ow and ds objects will be created in constructor's initialiser list
  OneWire               ow;                   // OneWire Object 

public: 
  DallasTemperature     ds;                   // Dallas Object 
  DS18x20DallasBus();
  int							id;						 // Bus numerical Id 
  char                  descr[BUS_DESCR_LEN]; // Bus descr (ex: "Room_1") 
  uint8_t               pin;                  // Hardware pin for the bus  
  int                   devicesNum;           // number of devices on the bus 

  struct               {
                            unsigned char   id;                      // Numeric Id
                            char            descr[DEVICE_DESCR_LEN]; // Descr (ex: "WaterIn" - Temperature of the Water Inlet) 
                            DeviceAddress   addr;                    // Address
                            int             prec;                    // Precision
                        } device[MAX_DEVICES_ON_BUS]  ;

  void  begin(uint8_t, const char*, int);
  int   addDevice(unsigned char, const char*, DeviceAddress, int);
  int   addDevice(unsigned char, const char*, const char*, int);
  void  requestTemperatures();

  float getTempC(unsigned char * addr);
  float getTempC(unsigned char);

  char* getDeviceAddressStr(DeviceAddress);
  void  parseDeviceAddress(const char*, char, DeviceAddress, int,int);
};


/*
 * Derive from DS18x20DallasBus & add loadConfig method 
 * to read the Bus configuration from a Json Object 
 * 
 */
class DS18x20DallasBusJson : public DS18x20DallasBus

{
 public:
	int loadConfig(const JsonObject& , uint8_t );
	int loadConfig(const JsonObject& );

 private:
};

#endif

