# Arduino vMix tally

This project contains the firmware for a tally system based on an Arduino esp8266 and the vMix TCP API. 
Pictures of the tally can be found in the Pictures folder.  

## Installation

### Hardware

For this project two pieces of hardware are needed.  
The first is a [WeMos® D1 Mini V2](https://www.banggood.com/WeMos-D1-Mini-V2-NodeMcu-4M-Bytes-Lua-WIFI-Internet-Of-Things-Development-Board-Based-ESP8266-p-1115398.html) and the second is a [Wemos® Matrix LED Shield V1.0.0](https://www.banggood.com/Wemos-Matrix-LED-Shield-V1_0_0-For-WEMOS-D1-Mini-p-1196300.html).  
Additionally a 3D printed case can be used to protect the hardware. A design for this can be found in the Case folder of this repository.  

#### Solder instructions

* Make sure you use the connector that comes with the WEMOS. <img src="/Pictures/Connectors.jpg" alt="Connectors" width="50">
* Solder the short site on the WEMOS.  
* Position the LED Display on the connector you just soldered on the WEMOS with just over 1 – 1,5-millimeter space between the boards. <img src="/Pictures/SolderExample.jpg" alt="Solder example" width="50">
* Depending on the USB cable you use you may need to widen up the hole for the connector in the case with a file.  
* Click the assembly in the case.  

### Software

#### 1. Install Arduino IDE

Download the Arduino IDE from the [Arduino website](https://www.arduino.cc/en/main/software) and install it.  
After the installation is complete go to File > Preferences and add http://arduino.esp8266.com/stable/package_esp8266com_index.json to the additional Board Manager URL. Go to Tools > Board > Board Manager, search for *esp8266* and install the latest version. After the installation go to Tools > Board and select *Wemos D1 R2* as your default board.  

#### 2. Copy libraries

All libraries included in the Libaries folder are required for this project. Please copy them to your own Arduino libraries folder (default path: *user/documents/Arduino/libraries*).  

#### 3. Uploading static files

Connect the Arduino to the computer with a USB cable.  
The static files in the Arduino-vMix-Tally/data folder must be uploaded using the Tools > ESP8266 Sketch Data Upload in the Arduino IDE.  

#### 4. Uploading firmware

Upload the Arduino-vMix-Tally/Arduino-vMix-Tally.ino from this repository to the Arduino by pressing the Upload button. After the upload the tally will restart in Connecting mode (see the Muliple states section).  

## Getting Started

### Multiple states

There are three states in which a tally can find itself.  

#### 1. Connecting

| Symbol | Meaning      | Led intensity | Example                                               |
|--------|--------------|---------------|-------------------------------------------------------|
| C      | Connecting   | 100%          |<img src="/Pictures/Connecting.jpg" alt="C" width="50">|

In this state the tally is trying to connect to WiFi and vMix based on the settings.  

#### 2. Access point

| Symbol | Meaning      | Led intensity | Example                                               |
|--------|--------------|---------------|-------------------------------------------------------|
| S      | Settings     | 100%          |<img src="/Pictures/AP%20mode.jpg" alt="S" width="50"> |

In this state the tally was unable to connect to WiFi and it turned itself to access point mode. It can be accessed by connecting to the WiFi network with the SSID *vMix_Tally_#* (# is the tally number) and password *vMix_Tally_#_access* (# is the tally number). Once connected the settings can be changed by going to the webpage on IP address 192.168.4.1.  

#### 3. Tally

| Symbol | Meaning      | Led intensity | Example                                                    |
|--------|--------------|---------------|------------------------------------------------------------|
| P      | Preview      | 28.5%         |<img src="/Pictures/Tally%20preview.jpg" alt="P" width="50">|
| L      | Live/program | 100%          |<img src="/Pictures/Tally%20live.jpg" alt="L" width="50">   |
| .      | Off          | 28.5%         |<img src="/Pictures/Tally%20off.jpg" alt="Off" width="50">  |

In this state the tally is connected to WiFi and vMix. It detects new tally states and shows them using the led matrix.  

### Settings

Network and tally settings can be edited on the built-in webpage. To access the webpage connect to the same WiFi network and navigate to the IP address or the devicename(*vmix_tally_#.home*, # is the tally number) in a browser.  
On this webpage the WiFi SSID, WiFi password, vMix hostname and tally number can be changed. It also shows some basic information of the device.  

## Things to keep in mind

1. Make sure to use a power cable that does not support data when using a USB port on a camera. This can cause connecting issues in the camera.  
2. The Arduino is completely dependent on a good WiFi signal.  
3. The tally must be connected to the same network as the vMix instance.  
4. The LED matrix can only display in red. The difference in color in the pictures above is due to a difference in LED intensity. 

## Footnote

Please note all images are for illustration purpose. Actual results may vary.  
Feel free open issues on this repository for bugs or feature requests.  
If you want to create your own version of this repository then a reference is appreciated.  
Special thanks to André for the 3D casing and the solder instructions.  
