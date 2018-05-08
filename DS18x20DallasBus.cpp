/**************************************************************************/
/*!
    @file     DS18x20DallasBus.cpp
    @author   Lucabuka

	The main goal of this library is to DINAMICALLY manage one or more 
	OneWire bus containing DS18x20 Temperature Sensor.
	There is a Class for the BUS properties (Pin number, name, etc) and a
	a contained class for the DEVICE (Id, Descr/Location, ADDR, Precision)

	Applications using the library will be able to DEFINE the hardware 
	configuration (Pin Used, Device Description/Location), Device ADDR, etc) 
	in the setup() (as opposito to have to define it at compile-time).

	The lib includes methods to LOAD the cfg from a JSON file.
	-- You just need to create the JSON file and call locadConfig() !
	
	So what ?	
	 Using the library is possible, for example, to write an Application that:
		- READ cfg from JSON file at startup.
		- Communicate via WiFI to:
			- Send temperature data to MQTT a server	
			- Receive commands (ex: NEW CFG FILE: Save it and reload cfg)

	This means your app can manage situations like:
		- REPLACE a DEVICE on the bus (new device have a different ADDR)
		- ADD Devices on a BUS (the App can automatically start to transmit 
			data for the new device)

		
    @section  HISTORY

    v1.0 - First release - 2018-05-08
		  

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

*/
/**************************************************************************/

#include "DS18x20DallasBus.h"



// https://playground.arduino.cc/Code/Hierarchy
// Usare per fare inherit a DallasTemperature ed avere sue f() accessibili (...?...)

DS18x20DallasBus::DS18x20DallasBus() 
: ow(99), ds(&ow)  // Constructor initializer list (OneWire and DallasTemperature obj)
{
  ; // nothing else to do
}


void  DS18x20DallasBus::begin(uint8_t use_pin, const char* txt){
  pin=use_pin;
  ow.begin(use_pin);     // Assign pin to OneWire bus
  strncpy(descr, txt, DEVICE_DESCR_LEN);
  ds.begin();  

  devicesNum=0;
}


int   DS18x20DallasBus::addDevice(unsigned char id, const char* descr, DeviceAddress addr, int prec){
  int errcode=0;
  if(ds.isConnected(addr)) {
    ds.setResolution(addr,prec);
    // Check that ensor is correctly configured
    int read_prec = ds.getResolution(addr) ;
    if( read_prec != prec) {
      return(2); // Wrong precision setting or precision not set
    }
  } else {
    return(1);
  }
  if(devicesNum >= MAX_DEVICES_ON_BUS){
    return(3); // MAX DEVICE number reached - change the DEFINE and compile
  }
  
  // Add device to list
  DS18x20DallasBus::device[devicesNum].id = id;
  strncpy(DS18x20DallasBus::device[devicesNum].descr,descr,DEVICE_DESCR_LEN);
  memcpy(DS18x20DallasBus::device[devicesNum].addr, addr, sizeof(DeviceAddress));
  DS18x20DallasBus::device[devicesNum].prec = prec;
  
  devicesNum++;
  
  return(0);
}

int   DS18x20DallasBus::addDevice(unsigned char id, const char* descr, const char* addr_str, int prec){
  DeviceAddress hex_addr;
  parseDeviceAddress(addr_str, ',', hex_addr, 8, 16);
  return(addDevice(id, descr, hex_addr, prec));
}


void  DS18x20DallasBus::requestTemperatures(void){
    ds.requestTemperatures();
}


float DS18x20DallasBus::getTempC(unsigned char * addr){
  return(ds.getTempC(addr));
}

float DS18x20DallasBus::getTempC(unsigned char id){
  return(ds.getTempC(device[id].addr));
}

/*
 * Convert the address string read from Json cfg file in a 
 * "DeviceAdress" (uint8_t DeviceAddress[8])
 */
void DS18x20DallasBus::parseDeviceAddress(const char* str, char sep, DeviceAddress bytes, int maxBytes, int base) {
    for (int i = 0; i < maxBytes; i++) {
        bytes[i] = strtoul(str, NULL, base);  // Convert byte
        str = strchr(str, sep);               // Find next separator
        if (str == NULL || *str == '\0') {
            break;                            // No more separators, exit
        }
        str++;                                // Point to next character after separator
    }
}
/*
 * Convert the address string read from Json cfg file in a 
 * "DeviceAdress" (uint8_t DeviceAddress[8])
 */
char * DS18x20DallasBus::getDeviceAddressStr(DeviceAddress addr) {
  static char retStr[25];
  sprintf(retStr,"%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x",
                  addr[0],addr[1],addr[2],addr[3],
                  addr[4],addr[5],addr[6],addr[7] );
  return(retStr);
}



/**************************************************************************/
/*!
    @brief  Class: DS18x20DallasBusJson
    @brief  loadConfig(const JsonObject& Json_Bus, uint8_t verbose)
*/
/**************************************************************************/
int DS18x20DallasBusJson::loadConfig(const JsonObject& Json_Bus, uint8_t verb) {
  int retVal = 0;
  
  // - Initialize Bus 
  begin( Json_Bus["pin"], Json_Bus["descr"]);
  
  // - Add Devices found in configuration 
  for (unsigned char j = 0; j < Json_Bus["devicesNum"] ; j++) {
    int err, printOnErr=0;
    DeviceAddress hex_addr;
    parseDeviceAddress(Json_Bus["device"][j]["addr"], ',', hex_addr, 8, 16);
    err = addDevice( j, Json_Bus["device"][j]["descr"], hex_addr, Json_Bus["device"][j]["prec"]);
    if (err) {
      retVal++;
     if(verb){
        printOnErr=1;
        Serial.print("  !!! ERROR=["); Serial.print(err); Serial.print("] !!! ");
      }
    } 
    if(verb == 2 || printOnErr) {
      const char* busDescr = Json_Bus["descr"];
      const char* descr = Json_Bus["device"][j]["descr"];
      const char* addr  = Json_Bus["device"][j]["addr"];
      const char* prec  = Json_Bus["device"][j]["prec"];
      
      Serial.print(" ...ADDING DEVICE: Bus["); Serial.print(busDescr) ; 
      Serial.print("]-Device["); Serial.print(j) ; Serial.print("][");
      Serial.print(descr) ; Serial.print("][");
      Serial.print(addr) ; Serial.print("][");
      Serial.print(prec) ; Serial.println("]");
    }

  }

  return(retVal);
}


/**************************************************************************/
/*!
    @brief  Class: DS18x20DallasBusJson
    @brief  loadConfig(const JsonObject& Json_Bus)
*/
/**************************************************************************/
int DS18x20DallasBusJson::loadConfig(const JsonObject& Json_Bus) {
  return(loadConfig(Json_Bus,0));
 
}


