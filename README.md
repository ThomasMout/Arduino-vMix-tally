# Arduino vMix tally
This project contains the firmware for a tally system based on an Arduino esp8266 and the vMix TCP API.

## Installation
### Hardware
For this project two pieces of hardware are needed. 
The first is a [WeMos® D1 Mini V2](https://www.banggood.com/WeMos-D1-Mini-V2-NodeMcu-4M-Bytes-Lua-WIFI-Internet-Of-Things-Development-Board-Based-ESP8266-p-1115398.html) and the second is a [Wemos® Matrix LED Shield V1.0.0](https://www.banggood.com/Wemos-Matrix-LED-Shield-V1_0_0-For-WEMOS-D1-Mini-p-1196300.html). 
Additionally a 3d printed case can be used to protect the hardware. A design for this might be added in the future.

### Software
Before first time use three steps must be taken.

##### 1. Copy libraries
All libraries included in the Libaries folder are required for this project. Please copy them to your own Arduino libraries folder (default path: *user/documents/Arduino/libraries*).

##### 2. Uploading static files
The static files in the Arduino-vMix-Tally/data folder must be uploaded using the Tools > ESP8266 Sketch Data Upload in the Arduino IDE.

##### 3. Uploading firmware
Upload the Arduino-vMix-Tally/Arduino-vMix-Tally.ino to the Arduino.

## Getting Started
### Multiple states
There are three states in which a tally can find itself.

##### 1. Connecting
| Symbol | Meaning      | Led intensity | Example                                               |
|--------|--------------|---------------|-------------------------------------------------------|
| C      | Connecting   | 100%          |<img src="/Pictures/Connecting.jpg" alt="C" width="50">|


In this state the tally is trying to connect to WiFi and vMix based on the settings.

##### 2. Access point
| Symbol | Meaning      | Led intensity | Example                                               |
|--------|--------------|---------------|-------------------------------------------------------|
| S      | Settings     | 100%          |<img src="/Pictures/AP%20mode.jpg" alt="S" width="50"> |

In this state the tally was unable to connect to WiFi and it turned itself to access point mode. It can be accessed by connecting to the WiFi network with the SSID *vMix_Tally_#* (# is the tally number) and password *vMix_Tally_#_access* (# is the tally number). Once connected the settings can be changed by going to the webpage on IP address 192.168.4.1. 

##### 3. Tally
| Symbol | Meaning      | Led intensity | Example                                                    |
|--------|--------------|---------------|------------------------------------------------------------|
| P      | Preview      | 28.5%         |<img src="/Pictures/Tally%20preview.jpg" alt="P" width="50">|
| L      | Live/program | 100%          |<img src="/Pictures/Tally%20live.jpg" alt="L" width="50">   |
| *None* | Off          | na            |<img src="/Pictures/Tally%20off.jpg" alt="Off" width="50">  |

In this state the tally is connected to WiFi and vMix. It detects new tally states and shows them using the led matrix.

### Settings
Network and tally settings can be edited on the built-in webpage. To access the webpage connect to the same WiFi network and navigate to the IP address or the devicename(*vmix_tally_#.home*, # is the tally number) in a browser.  
On this webpage the WiFi SSID, WiFi password, vMix hostname and tally number can be changed. It also shows some basic information of the device.

## Footnoot
Please note all images are for illustration purpose. Actual results may vary.  
Feel free open issues on this repository for bugs or feature requests.