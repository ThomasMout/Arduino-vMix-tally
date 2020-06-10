/*
  vMix wireless tally
  Copyright 2019 Thomas Mout
  Streaming and Recording Modifications Copyright Andrew Gilbett 2020
*/

#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <Adafruit_GFX.h>
#include <WEMOS_Matrix_GFX.h>
#include "FS.h"

// Constants
const int SsidMaxLength = 64;
const int PassMaxLength = 64;
const int HostNameMaxLength = 64;
const int TallyNumberMaxValue = 64;

// Settings object
struct Settings
{
  char ssid[SsidMaxLength];
  char pass[PassMaxLength];
  char hostName[HostNameMaxLength];
  int tallyNumber;
  int tallyType;    //1=Tally, 2=Recording 3=Streaming
  int tallyGlyph;   //1=Glyphs, 2=All on/All Off
};

// Default settings object
Settings defaultSettings = {
  "ssid default",
  "pass default",
  "hostname default",
  1,
  1,
  1
};

Settings settings;

// HTTP Server settings
ESP8266WebServer httpServer(80);
char deviceName[32];
int status = WL_IDLE_STATUS;
bool apEnabled = false;
char apPass[64];

// vMix settings
int port = 8099;

// LED settings
MLED matrix(4);

// Tally info
char currentState = -1;
const char tallyStateOff = 0;
const char tallyStateProgram = 1;
const char tallyStatePreview = 2;

// LED characters
static const uint8_t PROGMEM C[] = {B00000000, B01111110, B11111111, B10000001, B10000001, B11000011, B01000010, B00000000};
static const uint8_t PROGMEM L[] = {B00000000, B11111111, B11111111, B11000000, B11000000, B11000000, B11000000, B00000000};
static const uint8_t PROGMEM P[] = {B00000000, B11111111, B11111111, B00010001, B00010001, B00011111, B00001110, B00000000};
static const uint8_t PROGMEM R[] = {B00000000, B11111111, B11111111, B00010001, B00110001, B01011111, B10001110, B00000000};
static const uint8_t PROGMEM S[] = {B00000000, B01001100, B11011110, B10010010, B10010010, B11110110, B01100100, B00000000};
static const uint8_t PROGMEM SETUP[] = {B00110111, B11110101, B00111101, B11100000, B10011111, B11110101, B11110101, B01100000};
static const uint8_t PROGMEM DOTS[] = {B10000001, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B10000001};
static const uint8_t PROGMEM FULL[] = {B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111};

// The WiFi client
WiFiClient client;
int timeout = 10;
int delayTime = 10000;

// Time measure
int interval = 5000;
int pollinterval = 1000;
unsigned long lastCheck = 0;
unsigned long lastPoll = 0;

// Load settings from EEPROM
void loadSettings()
{
  Serial.println("------------");
  Serial.println("Loading settings");

  long ptr = 0;

  for (int i = 0; i < SsidMaxLength; i++)
  {
    settings.ssid[i] = EEPROM.read(ptr);
    ptr++;
  }

  for (int i = 0; i < PassMaxLength; i++)
  {
    settings.pass[i] = EEPROM.read(ptr);
    ptr++;
  }

  for (int i = 0; i < HostNameMaxLength; i++)
  {
    settings.hostName[i] = EEPROM.read(ptr);
    ptr++;
  }

  settings.tallyNumber = EEPROM.read(ptr);
  ptr++;

  settings.tallyType = EEPROM.read(ptr);
  ptr++;

  settings.tallyGlyph = EEPROM.read(ptr);

  if (strlen(settings.ssid) == 0 || strlen(settings.pass) == 0 || strlen(settings.hostName) == 0 || settings.tallyNumber == 0 || settings.tallyType == 0)
  {
    Serial.println("No settings found");
    Serial.println("Loading default settings");
    settings = defaultSettings;
    saveSettings();
    restart();
  }
  else
  {
    Serial.println("Settings loaded");
    printSettings();
    Serial.println("------------");
  }
}

// Save settings to EEPROM
void saveSettings()
{
  Serial.println("------------");
  Serial.println("Saving settings");

  long ptr = 0;

  for (int i = 0; i < 512; i++)
  {
    EEPROM.write(i, 0);
  }

  for (int i = 0; i < SsidMaxLength; i++)
  {
    EEPROM.write(ptr, settings.ssid[i]);
    ptr++;
  }

  for (int i = 0; i < PassMaxLength; i++)
  {
    EEPROM.write(ptr, settings.pass[i]);
    ptr++;
  }

  for (int i = 0; i < HostNameMaxLength; i++)
  {
    EEPROM.write(ptr, settings.hostName[i]);
    ptr++;
  }

  EEPROM.write(ptr, settings.tallyNumber);
  ptr++;

  EEPROM.write(ptr, settings.tallyType);
  ptr++;

  EEPROM.write(ptr, settings.tallyGlyph);

  EEPROM.commit();

  Serial.println("Settings saved");
  printSettings();
  Serial.println("------------");
}

// Print settings
void printSettings()
{
  Serial.println();
  Serial.print("SSID: ");
  Serial.println(settings.ssid);
  Serial.print("SSID Password: ");
  Serial.println(settings.pass);
  Serial.print("vMix hostname: ");
  Serial.println(settings.hostName);
  Serial.print("Tally number: ");
  Serial.println(settings.tallyNumber);
  Serial.print("Tally Type: ");
  Serial.println(settings.tallyType);
  Serial.print("Tally Glyphs: ");
  Serial.println(settings.tallyGlyph);
}

// Set led intensity from 0 to 7
void ledSetIntensity(int intensity)
{
  matrix.intensity = intensity;
}

// Set LED's off
void ledSetOff()
{
  matrix.clear();
  matrix.writeDisplay();
}

// Draw L(ive) with LED's
void ledSetProgram()
{
  matrix.clear();
  if(settings.tallyGlyph==2)
  {
    matrix.drawBitmap(0, 0, FULL, 8, 8, LED_ON);
  }
  else
  {
    matrix.drawBitmap(0, 0, L, 8, 8, LED_ON);
  }
  ledSetIntensity(7);
  matrix.writeDisplay();
}

// Draw P(review) with LED's
void ledSetPreview()
{
  matrix.clear();
  if(settings.tallyGlyph==2)
  {
    matrix.drawBitmap(0, 0, DOTS, 8, 8, LED_ON);
    ledSetIntensity(1);
  }
  else
  {
    matrix.drawBitmap(0, 0, P, 8, 8, LED_ON);
    ledSetIntensity(2);
  }
  matrix.writeDisplay();
}

// Draw C(onnecting) with LED's
void ledSetConnecting()
{
  matrix.clear();
  matrix.drawBitmap(0, 0, C, 8, 8, LED_ON);
  ledSetIntensity(7);
  matrix.writeDisplay();
}

// Draw R(ecording) with LED's
void ledSetRecording()
{
  matrix.clear();
  if(settings.tallyGlyph==2)
  {
    matrix.drawBitmap(0, 0, FULL, 8, 8, LED_ON);
  }
  else
  {
    matrix.drawBitmap(0, 0, R, 8, 8, LED_ON);
  }
  ledSetIntensity(7);
  matrix.writeDisplay();
}

// Draw S(treaming) with LED's
void ledSetStreaming()
{
  matrix.clear();
  if(settings.tallyGlyph==2)
  {
    matrix.drawBitmap(0, 0, FULL, 8, 8, LED_ON);
  }
  else
  {
    matrix.drawBitmap(0, 0, S, 8, 8, LED_ON);
  }
  ledSetIntensity(7);
  matrix.writeDisplay();
}

// Draw SETUP with LED's
void ledSetSettings()
{
  matrix.clear();
  matrix.drawBitmap(0, 0, SETUP, 8, 8, LED_ON);
  ledSetIntensity(3);
  matrix.writeDisplay();
}

// Set tally to off
void tallySetOff()
{
  Serial.println("Tally off");

  ledSetOff();
}

// Set tally to program
void tallySetProgram()
{
  Serial.println("Tally program");

  ledSetOff();
  ledSetProgram();
}

// Set tally to preview
void tallySetPreview()
{
  Serial.println("Tally preview");

  ledSetOff();
  ledSetPreview();
}

// Set tally to connecting
void tallySetConnecting()
{
  ledSetOff();
  ledSetConnecting();
}

// Set tally to recording
void tallySetRecording()
{
  Serial.println("Start Recording");

  ledSetOff();
  ledSetRecording();
}

// Set tally to recording
void tallySetStreaming()
{
  Serial.println("Start Streaming");

  ledSetOff();
  ledSetStreaming();
}
// Handle incoming data
void handleData(String data)
{
  // Check if server data is tally data
  if (data.indexOf("TALLY") == 0)
  {
    char newState = data.charAt(settings.tallyNumber + 8);

    // Check if tally state has changed
    if (currentState != newState)
    {
      currentState = newState;

      switch (currentState)
      {
        case '0':
          tallySetOff();
          break;
        case '1':
          tallySetProgram();
          break;
        case '2':
          tallySetPreview();
          break;
        default:
          tallySetOff();
      }
    }
  }
  // Check if server data is Recording data
  else if (data.indexOf("XMLTEXT") == 0 && settings.tallyType==2)
  {
    char newState = data.charAt(11);

    // Check if tally state has changed
    if (currentState != newState)
    {
      currentState = newState;

      switch (currentState)
      {
        case 'F':
          tallySetOff();
          break;
        case 'T':
          tallySetRecording();
          break;
        default:
          tallySetOff();
      }
    }
  }
    // Check if server data is Streaming data
  else if (data.indexOf("XMLTEXT") == 0 && settings.tallyType==3)
  {
    char newState = data.charAt(11);

    // Check if tally state has changed
    if (currentState != newState)
    {
      currentState = newState;

      switch (currentState)
      {
        case 'F':
          tallySetOff();
          break;
        case 'T':
          tallySetStreaming();
          break;
        default:
          tallySetOff();
      }
    }
  }
  else
  {
    Serial.print("Response from vMix: ");
    Serial.println(data);
  }
}

// Start access point
void apStart()
{
  ledSetSettings();
  Serial.println("AP Start");
  Serial.print("AP SSID: ");
  Serial.println(deviceName);
  Serial.print("AP password: ");
  Serial.println(apPass);

  WiFi.mode(WIFI_AP);
  WiFi.hostname(deviceName);
  WiFi.softAP(deviceName, apPass);
  delay(100);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("IP address: ");
  Serial.println(myIP);

  apEnabled = true;
}

// Hanle http server root request
void rootPageHandler()
{
  String response_message = "<!DOCTYPE html>";
  response_message += "<html lang='en'>";
  response_message += "<head>";
  response_message += "<title>" + String(deviceName) + "</title>";
  response_message += "<meta name='viewport' content='width=device-width, initial-scale=1, shrink-to-fit=no'>";
  response_message += "<meta charset='utf-8'>";
  response_message += "<link rel='icon' type='image/x-icon' href='favicon.ico'>";
  response_message += "<link rel='stylesheet' href='styles.css'>";
  response_message += "<style>body {width: 100%;height: 100%;padding: 25px;}</style>";
  response_message += "</head>";

  response_message += "<body class='bg-light'>";

  if(settings.tallyType==3)
  {
    response_message += "<h1>vMix Streaming Tally " + String(settings.tallyNumber) + "</h1>";
  }
  else if(settings.tallyType==2)
  {
    response_message += "<h1>vMix Recording Tally " + String(settings.tallyNumber) + "</h1>";
  }
  else
  {
    response_message += "<h1>vMix tally " + String(settings.tallyNumber) + "</h1>";
  }

  response_message += "<div data-role='content' class='row'>";

  response_message += "<div class='col-md-6'>";
  response_message += "<h2>Settings</h2>";
  response_message += "<form action='/save' method='post' enctype='multipart/form-data' data-ajax='false'>";

  response_message += "<div class='form-group row'>";
  response_message += "<label for='ssid' class='col-sm-4 col-form-label'>SSID</label>";
  response_message += "<div class='col-sm-8'>";
  response_message += "<input id='ssid' class='form-control' type='text' size='64' maxlength='64' name='ssid' value='" + String(settings.ssid) + "'>";
  response_message += "</div></div>";

  response_message += "<div class='form-group row'>";
  response_message += "<label for='ssidpass' class='col-sm-4 col-form-label'>SSID password</label>";
  response_message += "<div class='col-sm-8'>";
  response_message += "<input id='ssidpass' class='form-control' type='text' size='64' maxlength='64' name='ssidpass' value='" + String(settings.pass) + "'>";
  response_message += "</div></div>";

  response_message += "<div class='form-group row'>";
  response_message += "<label for='hostname' class='col-sm-4 col-form-label'>vMix hostname</label>";
  response_message += "<div class='col-sm-8'>";
  response_message += "<input id='hostname' class='form-control' type='text' size='64' maxlength='64' name='hostname' value='" + String(settings.hostName) + "'>";
  response_message += "</div></div>";

  response_message += "<div class='form-group row'>";
  response_message += "<label for='tallyType' class='col-sm-4 col-form-label'>Tally Type</label>";
  response_message += "<div class='col-sm-8'>";
  response_message += "<select id='tallyType' class='form-control' type='list' list='tallyType' name='tallyType'>";
  if(settings.tallyType==3)
  {
    response_message += "<option value='1'>Tally</option>";
    response_message += "<option value='2'>Recording</option>";
    response_message += "<option value='3' selected>Streaming</option>";
  }
  else if(settings.tallyType==2)
  {
    response_message += "<option value='1'>Tally</option>";
    response_message += "<option value='2' selected>Recording</option>";
    response_message += "<option value='3'>Streaming</option>";
  }
  else
  {
    response_message += "<option value='1' selected>Tally</option>";
    response_message += "<option value='2'>Recording</option>";
    response_message += "<option value='3'>Streaming</option>";
  }
  response_message += "</select>";
  response_message += "</div></div>";

  response_message += "<div class='form-group row'>";
  response_message += "<label for='tallyGlyph' class='col-sm-4 col-form-label'>Tally Glyph</label>";
  response_message += "<div class='col-sm-8'>";
  response_message += "<select id='tallyGlyph' class='form-control' type='list' list='tallyGlyph' name='tallyGlyph'>";
  if(settings.tallyGlyph==2)
  {
    response_message += "<option value='1'>Glyphs</option>";
    response_message += "<option value='2' selected>All Leds</option>";
  }
  else
  {
    response_message += "<option value='1' selected>Glyphs</option>";
    response_message += "<option value='2'>All Leds</option>";
  }
  response_message += "</select>";
  response_message += "</div></div>";

  response_message += "<div class='form-group row'>";
  response_message += "<label for='inputnumber' class='col-sm-4 col-form-label'>Input number (1-1000)</label>";
  response_message += "<div class='col-sm-8'>";
  response_message += "<input id='inputnumber' class='form-control' type='number' size='64' min='0' max='1000' name='inputnumber' value='" + String(settings.tallyNumber) + "'>";
  response_message += "</div></div>";

  response_message += "<input type='submit' value='SAVE' class='btn btn-primary'></form>";
  response_message += "</div>";

  response_message += "<div class='col-md-6'>";
  response_message += "<h2>Device information</h2>";
  response_message += "<table class='table'><tbody>";

  if(settings.tallyType==3)
  {
    response_message += "<tr><th>Mode</th><td>Streaming</td></tr>";
  }
  else if(settings.tallyType==2)
  {
    response_message += "<tr><th>Mode</th><td>Recording</td></tr>";
  }
  else
  {
    response_message += "<tr><th>Mode</th><td>Tally</td></tr>";
  }

  char ip[13];
  sprintf(ip, "%d.%d.%d.%d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);
  response_message += "<tr><th>IP</th><td>" + String(ip) + "</td></tr>";

  response_message += "<tr><th>MAC</th><td>" + String(WiFi.macAddress()) + "</td></tr>";
  response_message += "<tr><th>Signal Strength</th><td>" + String(WiFi.RSSI()) + " dBm</td></tr>";
  response_message += "<tr><th>Device Name</th><td>" + String(deviceName) + "</td></tr>";

  if (WiFi.status() == WL_CONNECTED)
  {
    response_message += "<tr><th>Status</th><td>Connected</td></tr>";
  }
  else
  {
    response_message += "<tr><th>Status</th><td>Disconnected</td></tr>";
  }

  if (apEnabled)
  {
    sprintf(ip, "%d.%d.%d.%d", WiFi.softAPIP()[0], WiFi.softAPIP()[1], WiFi.softAPIP()[2], WiFi.softAPIP()[3]);
    response_message += "<tr><th>AP</th><td>Active (" + String(ip) + ")</td></tr>";
  }
  else
  {
    response_message += "<tr><th>AP</th><td>Inactive</td></tr>";
  }
  response_message += "</tbody></table>";
  response_message += "</div>";
  response_message += "</div>";

  response_message += "</body>";
  response_message += "</html>";

  httpServer.sendHeader("Connection", "close");
  httpServer.send(200, "text/html", String(response_message));
}

// Settings POST handler
void handleSave()
{
  bool doRestart = false;

  httpServer.sendHeader("Location", String("/"), true);
  httpServer.send(302, "text/plain", "Redirected to: /");

  if (httpServer.hasArg("ssid"))
  {
    if (httpServer.arg("ssid").length() <= SsidMaxLength)
    {
      httpServer.arg("ssid").toCharArray(settings.ssid, SsidMaxLength);
      doRestart = true;
    }
  }

  if (httpServer.hasArg("ssidpass"))
  {
    if (httpServer.arg("ssidpass").length() <= PassMaxLength)
    {
      httpServer.arg("ssidpass").toCharArray(settings.pass, PassMaxLength);
      doRestart = true;
    }
  }

  if (httpServer.hasArg("hostname"))
  {
    if (httpServer.arg("hostname").length() <= HostNameMaxLength)
    {
      httpServer.arg("hostname").toCharArray(settings.hostName, HostNameMaxLength);
      doRestart = true;
    }
  }

  if (httpServer.hasArg("inputnumber"))
  {
    if (httpServer.arg("inputnumber").toInt() > 0 and httpServer.arg("inputnumber").toInt() <= TallyNumberMaxValue)
    {
      settings.tallyNumber = httpServer.arg("inputnumber").toInt();
      doRestart = true;
    }
  }

  if (httpServer.hasArg("tallyType"))
  {
    if (httpServer.arg("tallyType").toInt() >= 0 and httpServer.arg("tallyType").toInt() <= 3)
    {
      settings.tallyType = httpServer.arg("tallyType").toInt();
      doRestart = true;
    }
  }

    if (httpServer.hasArg("tallyGlyph"))
  {
    if (httpServer.arg("tallyGlyph").toInt() > 0 and httpServer.arg("tallyGlyph").toInt() < 3)
    {
      settings.tallyGlyph = httpServer.arg("tallyGlyph").toInt();
      doRestart = true;
    }
  }

  if (doRestart == true)
  {
    restart();
  }
}

// Connect to WiFi
void connectToWifi()
{
  Serial.println();
  Serial.println("------------");
  Serial.println("Connecting to WiFi");
  Serial.print("SSID: ");
  Serial.println(settings.ssid);
  Serial.print("Passphrase: ");
  Serial.println(settings.pass);

  int timeout = 15;

  WiFi.mode(WIFI_STA);
  WiFi.hostname(deviceName);
  WiFi.begin(settings.ssid, settings.pass);

  Serial.print("Waiting for connection.");
  while (WiFi.status() != WL_CONNECTED and timeout > 0)
  {
    delay(1000);
    timeout--;
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("Success!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Device name: ");
    Serial.println(deviceName);
    Serial.println("------------");
  }
  else
  {
    if (WiFi.status() == WL_IDLE_STATUS)
      Serial.println("Idle");
    else if (WiFi.status() == WL_NO_SSID_AVAIL)
      Serial.println("No SSID Available");
    else if (WiFi.status() == WL_SCAN_COMPLETED)
      Serial.println("Scan Completed");
    else if (WiFi.status() == WL_CONNECT_FAILED)
      Serial.println("Connection Failed");
    else if (WiFi.status() == WL_CONNECTION_LOST)
      Serial.println("Connection Lost");
    else if (WiFi.status() == WL_DISCONNECTED)
      Serial.println("Disconnected");
    else
      Serial.println("Unknown Failure");

    Serial.println("------------");
    apStart();
  }
}

// Connect to vMix instance
void connectTovMix()
{
  Serial.print("Connecting to vMix on ");
  Serial.print(settings.hostName);
  Serial.print("...");

  if (client.connect(settings.hostName, port))
  {
    Serial.println(" Connected!");
    Serial.println("------------");
    
    tallySetOff();

    // Subscribe to the tally events
    if(settings.tallyType==1)
    {
      client.println("SUBSCRIBE TALLY");
    }
  }
  else
  {
    Serial.println(" Not found!");
  }
}

void restart()
{
  saveSettings();

  Serial.println();
  Serial.println();
  Serial.println("------------");
  Serial.println("------------");
  Serial.println("RESTART");
  Serial.println("------------");
  Serial.println("------------");
  Serial.println();
  Serial.println();

  start();
}

void start()
{
  tallySetConnecting();
  
  loadSettings();
  if(settings.tallyType==3)
  {
    sprintf(deviceName, "vMix_Stream_Tally_%d", settings.tallyNumber);
  }
  else if(settings.tallyType==2)
  {
    sprintf(deviceName, "vMix_Record_Tally_%d", settings.tallyNumber);
  }
  else
  {
    sprintf(deviceName, "vMix_Tally_%d", settings.tallyNumber);
  }
  
  sprintf(apPass, "%s%s", deviceName, "_access");

  connectToWifi();

  if (WiFi.status() == WL_CONNECTED)
  {
    connectTovMix();
  }
}

void setup()
{
  Serial.begin(9600);
  EEPROM.begin(512);
  SPIFFS.begin();

  httpServer.on("/", HTTP_GET, rootPageHandler);
  httpServer.on("/save", HTTP_POST, handleSave);
  httpServer.serveStatic("/", SPIFFS, "/", "max-age=315360000");
  httpServer.begin();

  start();
}

void loop()
{
  httpServer.handleClient();

  if (millis() > lastPoll + pollinterval & settings.tallyType>1)
  {
      switch (settings.tallyType)
      {
        case 2:
          client.println("XMLTEXT vmix/recording");
          break;
        case 3:
          client.println("XMLTEXT vmix/streaming");
          break;
      }
    lastPoll = millis();
  }
  while (client.available())
  {
    String data = client.readStringUntil('\r\n');
    handleData(data);
  }

  if (!client.connected() && !apEnabled && millis() > lastCheck + interval)
  {
    tallySetConnecting();

    client.stop();

    connectTovMix();
    lastCheck = millis();
  }
}
