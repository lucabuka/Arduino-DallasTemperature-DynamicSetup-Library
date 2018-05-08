# Arduino/ESP32 Library to DINAMICALLY manage one or more DallasTemperature->OneWire Bus


The main goal of this library is to DINAMICALLY define one or more 
OneWire bus containing DS18x20 Temperature Sensor.  
There is a Class for the BUS properties (Pin number, name, etc) and a
a contained Structure for the DEVICE (Id, Descr/Location, ADDR, Precision)

Applications using the library will be able to DEFINE the hardware 
configuration (Pin Used, Device Description/Location), Device ADDR, etc) 
in the setup() (as opposito to have to define it at compile-time).

The lib includes methods to LOAD the cfg from a JSON file.
>> You just need to create the JSON file and call locadConfig() !
	
### So what ?  
Using the library is possible, for example, to write an Application with the following meta-code:
- READ cfg from JSON file at startup.
- Communicate via WiFI to:
    - Send temperature data to MQTT a server	
    - Receive commands (ex: NEW CFG FILE: Save it and reload cfg)

This means your app can manage situations like:  
- REPLACE a DEVICE on the bus (new device have a different ADDR)
- ADD Devices on a BUS (the App can automatically start to transmit data for the new device)  

with no need to Change-Compile-Upload the code.

## Required libs:
 - OneWire from Paul Stoffregen
 - DallasTemperature
 - ArduinoJson (https://arduinojson.org/)
		

## Platforms:
- Arduino 
- Arduino Core for ESP32 (https://github.com/espressif/arduino-esp32)


## Useful Tools:
 - ESP32 Sketch Data Upload for Arduino IDE:
    https://github.com/me-no-dev/arduino-esp32fs-plugin  
    Used to upload the JSON CFG file on the SPIFFS file system of the ESP32 in the Json_SPIFFS example  


## Implementation Notes

In the current version of the library there are some "FIXED SIZE" arrays:

- MAX_DEVICES_ON_BUS 5:
    - Max number of devices on a single Bus (fixed-size array in "DS18x20DallasBus" class) 
- BUS_DESCR_LEN 12:
    - Max Lenght for the Bus string Description (char descr[BUS_DESCR_LEN] in "DS18x20DallasBus" class)
- DEVICE_DESCR_LEN 12:
    - Max Lenght for the DEVICE string Description" (char descr[DEVICE_DESCR_LEN] in "DS18x20DallasBus.device[i]" struct)] 

You can change this values to match your needings (and board's memory constrains) at the top of DS18x20DallasBus.h



## JSON CFG FILE:  

  Each BUS is described with:  
```
"OneWireBus": [
    {	"descr":"Room 1",
    "pin":25,
	"devicesNum":2,
	"device": [...]
    }
```
- "descr": Description used to identify the singe BUS (ex: Room_1: Bus where the sensor in Room_1 are connected)  
- "pin": hardware pin where the bus is connected   
- "devicesNum": number of devices listed in the "device" array below.  
- "device": array of Temperature Sensors --see below  




Each DEVICE in a bus is described with:  
```
    {  
    "descr":  "Window 1",  
    "addr": "0x28,0xFF,0x08,0xA8,0x14,0x14,0x0,0xC0",  
    "prec":9  
    }  
```
- "descr": identify the single Sensor  
- "prec": device precision requested (9-12)  
>>    If you don't know the addresses for a device, run one of the   
>>    "scan" examples that comes with OneWire/Dallas libs  


### UPLOAD the JSON file:  
    The JSON file created on the development computer could be sent   
    to the ESP32 SPIFF file system using a "pluging tool"   
    for the arduino IDE (ESP32 Sketch Data Upload)  
    See:   
    https://github.com/me-no-dev/arduino-esp32fs-plugin   



## Website


You can find the latest version of the library at
https://github.com/lucabuka/Arduino-DallasTemperature-DynamicSetup-Library

# License

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA



2018-05-08: Ver 1.0 - Luca (lucabuka AT gmail DOT com)


