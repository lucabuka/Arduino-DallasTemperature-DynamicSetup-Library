# Arduino/ESP32 Library to DINAMICALLY manage one or more DallasTemperature->OneWire Bus


The main goals of this library are:
- DINAMICALLY define one or more DallasTemperature->OneWire bus containing DS18x20 Temperature Sensor  
    Hardware Pin, Device ID/Description/Location, Device ADDR, etc, will be loaded from a JSON 
    configuration file  in the setup() (as opposite to have to define them at compile-time).
>> You just need to create the JSON file and call loadConfig() !

- Internally manage, for each device, "Current" and "Previous" temperatures. 
    The getData() method will return FALSE|TRUE indicating if "Temperature has changed from last Read".  
    This is useful to write Monitor/Telemetry applications where you need to send messages to the 
    server only if temperature changes
>>    For each device is possible to SET a value defining the threshold in temperature variations you want to ignore EPSILON).    
>>   Example:   Set EPSILON=2.0 for a device 
>>          deviceGetData() will return FALSE (no changes) even if the CURRENT temperature 
>>          read is 1 degree higher than the PREVIOUS read.
>>          When the difference reach 2.0 degree, deviceGetData() will return TRUE.

	

### So what ?  
Using the library is possible, for example, to write an Application with the following meta-code:
- READ cfg from JSON file at startup.
- Communicate via WiFI to:
    - Send _ON_CHANGE_ temperature data to a MQTT server	
    - Receive commands (ex: Receive a new CFG JSON file => Save it => reload cfg)

This means your app _ with no need to Change-Compile-Upload the code_ can manage situations like:  
- REPLACE/ADD DEVICES (new device have a different ADDR)
- Change setting for exixting devices (Precision, Epsilon)
- Change settings for the Application (Delay between temperature checks, etc)



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
    {
    "id": 1	
    "descr":"Room 1",
    "pin":25,
	"devicesNum":2,
	"device": [...]
    }
```
- "id": numerical ID to identify the single BUS (ex: 1)  
- "descr": Description used to identify the singe BUS (ex: Room_1: Bus where the sensor in Room_1 are connected)  
- "pin": hardware pin where the bus is connected   
- "devicesNum": number of devices listed in the "device" array below.  
- "device": array of Temperature Sensors --see below  




Each DEVICE in a bus is described with:  
```
    {  
    "id":3      
    "descr":  "Window 1",  
    "addr": "0x28,0xFF,0x08,0xA8,0x14,0x14,0x0,0xC0",  
    "prec":9,  
    "epsilon": 0.5
    }  
```
- "id": __OPTIONAL_ Numerical ID of the Sensor (thermometer)
- "descr": identify the single Sensor  
- "prec": device precision requested (9-12)  
- "epsilon": __OPTIONAL_ Ignore variation in sensor read smaller than EPSLON
            if [abs("now"-"prev") < epsilon] -> getData() returns NO CHANGE   

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


