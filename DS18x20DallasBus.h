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

#define TEMPERATURE_UNDEFINED	-200.0

class DS18x20DallasBus {


private:
  // Note: 
  //    ow and ds objects will be created in constructor's initialiser list
  OneWire               ow;                   // OneWire Object 
  Stream *  trcStream; 
  Stream *  dbgStream;


public: 
  DallasTemperature     ds;                   // Dallas Object 
  DS18x20DallasBus();
  int							id;						 // Bus numerical Id 
  char                  descr[BUS_DESCR_LEN]; // Bus descr (ex: "Room_1") 
  uint8_t               pin;                  // Hardware pin for the bus  
  int                   devicesNum;           // number of devices on the bus 

  struct               {
                            unsigned char   id;             // Numeric Id
                            char            descr[DEVICE_DESCR_LEN]; // Descr (ex: "WaterIn") 
                            DeviceAddress   addr;           // Address
                            int             prec;           // Precision
									 float           t_now;				// Last temperature recorded
									 float           t_prev;			// Previous temperature recorded
									 float           epsilon;			
														// Used to return "Temperature has changed from last getData()? 0|1"
														// if [abs("now"-"prev") < epsilon] => getData() returns NO CHANGE   
                        } device[MAX_DEVICES_ON_BUS]  ;		

  void  begin(uint8_t, const char*, int);
  int   addDevice(unsigned char, const char*, DeviceAddress, int, float);
  int   addDevice(unsigned char, const char*, const char*, int, float);

  int   getDeviceIdxById(unsigned char);

  int   deviceGetData(byte idx, float *T, float *Tp);	

  float getTempCById(unsigned char id);

  void  requestTemperatures();
  float getTempC(unsigned char * addr);

  char* getDeviceAddressStr(DeviceAddress);
  void  parseDeviceAddress(const char*, char, DeviceAddress, int,int);
  void      setDebugStream(Stream &);
  void      setTraceStream(Stream &);
  Stream *  getTraceStream(void);
  Stream *  getDebugStream(void);
  bool      getTraceState(void);
  bool      getDebugState(void);
//  int	  loadConfig(const JsonObject&);  
//  int   loadConfig(const JsonObject& , Stream *);

  void  dump(Stream &); 
};


/*
 * Derive from DS18x20DallasBus & add loadConfig method 
 * to read the Bus configuration from a Json Object 
 * 
 */
class DS18x20DallasBusJson : public DS18x20DallasBus

{
 public:
  int loadConfig(const JsonObject& );

 private:

};

#endif

