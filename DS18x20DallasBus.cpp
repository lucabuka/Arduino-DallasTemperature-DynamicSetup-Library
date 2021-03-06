/**************************************************************************/
/*!
    @file     DS18x20DallasBus.cpp
    @author   Lucabuka

	The main goals of this library are:
	
	1) DINAMICALLY manage one or more OneWire bus containing DS18x20 Temperature 
	Sensor 
		Applications using the library will be able to DEFINE the hardware 
		configuration (Pin Used, Device Description/Location), Device ADDR, etc) 
		in the setup() (as opposito to have to define it at compile-time).

		The lib includes methods to LOAD the cfg from a JSON file.

	2) Internally manage, for each device, "Current" and "Previous" temperatures. 
		The getData() method will return FALSE|TRUE indicating if "Temperature 
		has changed from last Read".  
		Useful to write Monitor/Telemetry applications where you need to send 
		messages to server only if temperature changes


		
    @section  HISTORY

	 v1.1 - Release Update - 2018-05-21
		- Added suport to manage current/previous temperature read for a device
		- Added support for device-level EPSILON 
					  
    v1.0 - First release - 2018-05-08


 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

*/
/**************************************************************************/

#include "DS18x20DallasBus.h"


DS18x20DallasBus::DS18x20DallasBus() 
: ow(99), ds(&ow)  // Constructor initializer list (OneWire and DallasTemperature obj)
{
  this->trcStream=NULL;
  this->dbgStream=NULL;
}


void  DS18x20DallasBus::begin(uint8_t use_pin, const char* txt, int numerical_id){
  pin=use_pin;
  ow.begin(use_pin);     // Assign pin to OneWire bus
  strncpy(descr, txt, DEVICE_DESCR_LEN);
  id=numerical_id;
  ds.begin();  

  devicesNum=0;
}


int   DS18x20DallasBus::addDevice(unsigned char id, const char* descr, DeviceAddress addr, int prec, float eps){
  if(ds.isConnected(addr)) {
    ds.setResolution(addr,prec);
    // Check that ensor is correctly configured
    int read_prec = ds.getResolution(addr) ;
    if( read_prec != prec) {
      return(2); // Wrong precision setting or precision not set
    }
  } else {
    return(1); // Address not found on bus
  }
  if(devicesNum >= MAX_DEVICES_ON_BUS){
    return(3); // MAX DEVICE number reached - change the DEFINE and compile
  }
  
  // Add device to list
  DS18x20DallasBus::device[devicesNum].id = id;
  strncpy(DS18x20DallasBus::device[devicesNum].descr,descr,DEVICE_DESCR_LEN);
  memcpy(DS18x20DallasBus::device[devicesNum].addr, addr, sizeof(DeviceAddress));
  DS18x20DallasBus::device[devicesNum].prec = prec;
  DS18x20DallasBus::device[devicesNum].t_now = TEMPERATURE_UNDEFINED ;
  DS18x20DallasBus::device[devicesNum].t_prev = TEMPERATURE_UNDEFINED ; 
  DS18x20DallasBus::device[devicesNum].epsilon = eps;
  
  devicesNum++;
  
  return(0);
}

int   DS18x20DallasBus::addDevice(unsigned char id, const char* descr, const char* addr_str, int prec, float eps){
  DeviceAddress hex_addr;
  parseDeviceAddress(addr_str, ',', hex_addr, 8, 16);
  return(addDevice(id, descr, hex_addr, prec, eps));
}

int DS18x20DallasBus::getDeviceIdxById(unsigned char id){
  for(uint8_t i=0; i<MAX_DEVICES_ON_BUS; i++){
	if(device[i].id == id) {
     return(i);
    }
  }
  return(-1);
}

/***
int  DS18x20DallasBus::deviceGetData(byte idx, float *T, float *Tp){
  if(idx < 0 || idx >= devicesNum){
    return(-1); // 
  }
  device[idx].t_prev = device[idx].t_now;
  device[idx].t_now = ds.getTempC(device[idx].addr);

  *T  = device[idx].t_now;
  *Tp = device[idx].t_prev;
  
  return( (*T == *Tp)? 0  : 1) ; 
}
***/
int  DS18x20DallasBus::deviceGetData(byte idx, float *T, float *Tp) {
  if(idx < 0 || idx >= devicesNum){
    return(-1); // 
  }

  device[idx].t_now = ds.getTempC(device[idx].addr);	// Get new value from device
  *T  = device[idx].t_now;		// Set returned Value: T(now)
  *Tp = device[idx].t_prev;	// Set returned Value: T(prev)
  
  int differs =  (abs(device[idx].t_now - device[idx].t_prev) <= device[idx].epsilon)? 0 : 1 ;

  //  If Now-Prev differs for less than "Epsilon", NO UPDATES in "prev" --see doc !
  if(differs) {  
    device[idx].t_prev = device[idx].t_now; 
  }
  
  return(differs) ; 
}



float DS18x20DallasBus::getTempCById(unsigned char id){

  for(uint8_t i=0; i<MAX_DEVICES_ON_BUS; i++){
	if(device[i].id == id) {
     return(ds.getTempC(device[i].addr));
    }
  }
  return(DEVICE_DISCONNECTED_C);
}



void  DS18x20DallasBus::requestTemperatures(void){
    ds.requestTemperatures();
}

float DS18x20DallasBus::getTempC(unsigned char * addr){
  return(ds.getTempC(addr));
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


void DS18x20DallasBus::setTraceStream(Stream & outStream){
  this->trcStream = &outStream; 
}

void DS18x20DallasBus::setDebugStream(Stream & outStream){
  this->dbgStream = &outStream; 
}

Stream * DS18x20DallasBus::getDebugStream(void){
  return(this->dbgStream);
}

Stream * DS18x20DallasBus::getTraceStream(void){
  return(this->trcStream);
}

bool DS18x20DallasBus::getDebugState(void){
  return((this->dbgStream != NULL) ? true : false);
}

bool DS18x20DallasBus::getTraceState(void){
  return((this->trcStream != NULL) ? true : false);
}


void DS18x20DallasBus::dump(Stream &out) {
    out.print("\nDS18x20DallasBus - Dump :\n");

	 out.print("  Bus id:["); out.print(id) ; out.print("] descr:["); out.print(descr) ; out.println("]");
	 out.print("  pin:["); out.print(pin) ; out.println("]");
	 out.print("  devicesNum:["); out.print(devicesNum) ; out.println("]");


    for (unsigned char j = 0; j < devicesNum ; j++) {
		out.print("    Device id:["); out.print(device[j].id) ; 
		out.print("] descr:["); out.print(device[j].descr) ;
		out.print("] addr:["); out.print(getDeviceAddressStr(device[j].addr)); 
		out.print("] prec:["); out.print(device[j].prec);
		out.print("] epsilon:["); out.print(device[j].epsilon);
		out.print("] t_now:["); out.print(device[j].t_now);
		out.print("] t_prev:["); out.print(device[j].t_prev); out.println("]");
	 }
    out.println("");
    return;
}






/**************************************************************************/
/*!
    @brief  Class: DS18x20DallasBusJson
    @brief  loadConfig(const JsonObject& Json_Bus)
*/
/**************************************************************************/
int DS18x20DallasBusJson::loadConfig(const JsonObject& Json_Bus) {
  int retVal = 0;
  bool debug=getDebugState();
  bool trace=getTraceState(); 
  Stream * debugStream = getDebugStream();
  Stream * traceStream = getTraceStream();


  // - Initialize Bus 
  begin( Json_Bus["pin"], Json_Bus["descr"], Json_Bus["id"]);
  
  // - Add Devices found in configuration 
  for (unsigned char j = 0; j < Json_Bus["devicesNum"] ; j++) {
    int err, printOnErr=0;
    DeviceAddress hex_addr;
    parseDeviceAddress(Json_Bus["device"][j]["addr"], ',', hex_addr, 8, 16);

    uint8_t device_id = j;
	 const char* id_found = Json_Bus["device"][j]["id"];
    if(id_found != nullptr) {
	   device_id = Json_Bus["device"][j]["id"];
	 }
    
	 float device_eps = 0.0;
	 const char* eps_found = Json_Bus["device"][j]["epsilon"];
    if(eps_found != nullptr) {
	   device_eps = Json_Bus["device"][j]["epsilon"];
	 }

    err = addDevice(device_id, Json_Bus["device"][j]["descr"], hex_addr, Json_Bus["device"][j]["prec"],device_eps );
    if (err) {
      retVal++;
		if(debug) debugStream->print("  !!! ERROR from addDevice():["); debugStream->print(err); debugStream->print("] !!! ");
      printOnErr=1;
    } 
    if((debug && printOnErr) || trace) {
		if( !printOnErr) debugStream = getTraceStream();

      const char* busDescr = Json_Bus["descr"];
      const char* descr = Json_Bus["device"][j]["descr"];
      const char* addr  = Json_Bus["device"][j]["addr"];
      const char* prec  = Json_Bus["device"][j]["prec"];
      float eps  = Json_Bus["device"][j]["epsilon"];
      
      debugStream->print(" ...ADDING DEVICE: Bus["); debugStream->print(busDescr) ; 
      debugStream->print("]-Device["); debugStream->print(device_id) ; debugStream->print("][");
      debugStream->print(descr) ; debugStream->print("][");
      debugStream->print(addr) ; debugStream->print("][");
      debugStream->print(prec) ; debugStream->print("] Epsilon:[");
      debugStream->print(device_eps) ; debugStream->println("]");
    }

  }
  if(trace) dump(*traceStream);
	
  return(retVal);
}

