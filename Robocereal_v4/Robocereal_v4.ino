#include <DS3232RTC.h>
#include <Arduino.h>
#include "fauxmoESP.h"
#include <NewPing.h>
#include <Average.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TimeLib.h>
#include <NtpClientLib.h>
#include <ESP8266WiFi.h>
#include <TimeAlarms.h>
#include <ESP8266Ping.h>
#include <Wire.h>
#include <WebSocketsServer.h>
#include <Hash.h>
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`
#include <EEPROM.h>

#define YOUR_WIFI_SSID "BTHub6-5M6T"
#define YOUR_WIFI_PASSWD "kngQurHTu9yU"
const char* robopassword = "cereal"; // <===================== your robocereal Password

//define websockets server object
WebSocketsServer webSocket = WebSocketsServer(301); //port defined here

//define NTP server
const char* ntpServer = "80.87.128.222";//"uk.pool.ntp.org";

//initiate screen and settings
SSD1306  display(0x3c, D3, D5); // this also sets wire for D3 and D5

int screenW = 128;
int screenH = 64;
int clockCenterX = screenW / 2;
int clockCenterY = ((screenH - 16) / 2) + 16; // top yellow part is 16 px height

//setrobocereal logo
#define WiFi_Logo_width 128
#define WiFi_Logo_height 64

//robocereal logo
const char WiFi_Logo_bits[] PROGMEM = {
  0x00, 0xF0, 0xFF, 0x01, 0xC0, 0x7F, 0x00, 0xFC, 0x3F, 0x00, 0xF0, 0x0F,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xE8, 0xFF, 0x03, 0xE0, 0xFF, 0x00, 0xF9,
  0x7F, 0x00, 0xF8, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xD8, 0xFF, 0x03,
  0xE0, 0xFF, 0x00, 0xF3, 0x7F, 0x00, 0xF8, 0x1F, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xB8, 0xFF, 0x0B, 0xC8, 0x7F, 0x00, 0xF7, 0xFF, 0x01, 0xF3, 0xCF,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x1C, 0x1C, 0x00, 0x07, 0x0F,
  0xC0, 0x83, 0x07, 0xE0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x1E,
  0x3E, 0x80, 0x0F, 0x0F, 0xC0, 0x83, 0x07, 0xF0, 0x01, 0x00, 0x00, 0x00,
  0x00, 0x78, 0x00, 0x1F, 0x3E, 0x80, 0x0F, 0x0F, 0xC0, 0x83, 0x07, 0xF0,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x1F, 0x3E, 0x80, 0x0F, 0x0F,
  0xC0, 0x83, 0x07, 0xF0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x1F,
  0x3E, 0x80, 0x0F, 0x0F, 0xC0, 0xC3, 0x07, 0xF0, 0x01, 0x00, 0x00, 0x00,
  0x00, 0x78, 0x00, 0x1F, 0x3E, 0x80, 0x0F, 0x0F, 0xC0, 0xC3, 0x07, 0xF0,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x1F, 0x3E, 0x80, 0x0F, 0x0F,
  0xC0, 0xC3, 0x07, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x1E,
  0x1E, 0x80, 0x0F, 0x07, 0xC0, 0xC3, 0x03, 0xE0, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x98, 0xFF, 0x0C, 0x0C, 0x00, 0x07, 0xF3, 0x9F, 0x81, 0x01, 0xC0,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xFF, 0x05, 0x04, 0x00, 0x00, 0xF8,
  0x3F, 0x80, 0x00, 0x40, 0x00, 0x00, 0x02, 0x00, 0x00, 0xC0, 0xFF, 0x01,
  0x00, 0x00, 0x03, 0xFC, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00,
  0x00, 0x98, 0xFF, 0x00, 0x06, 0x80, 0x07, 0xF8, 0x9F, 0x81, 0x01, 0x60,
  0x00, 0x00, 0x1F, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x0F, 0x80, 0x87, 0x03,
  0xC0, 0xC1, 0x01, 0xF0, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x7C, 0x0F, 0x00,
  0x1F, 0xC0, 0x87, 0x07, 0xE0, 0xC1, 0x03, 0xF8, 0x00, 0x80, 0x1F, 0x00,
  0x00, 0x7C, 0x1F, 0x00, 0x1F, 0xC0, 0x87, 0x0F, 0xF0, 0xE1, 0x03, 0xF8,
  0x00, 0x80, 0x1F, 0x00, 0x00, 0x3C, 0x3F, 0x00, 0x1F, 0xC0, 0x87, 0x0F,
  0xF0, 0xE1, 0x03, 0xF8, 0x00, 0x80, 0x1F, 0x00, 0x00, 0x3E, 0x7F, 0x00,
  0x1F, 0xC0, 0x87, 0x0F, 0xF0, 0xE1, 0x03, 0xF8, 0x00, 0xC0, 0x1F, 0x00,
  0x00, 0x3E, 0x7E, 0x00, 0x1F, 0xC0, 0x87, 0x07, 0xF0, 0xE1, 0x03, 0xF8,
  0x00, 0xC0, 0x0F, 0x00, 0x00, 0x3E, 0xFC, 0x00, 0x1F, 0xC0, 0xC7, 0x07,
  0xF0, 0xE1, 0x03, 0xF8, 0x00, 0xC0, 0x0F, 0x00, 0x00, 0x3E, 0xF8, 0x03,
  0x0F, 0x80, 0xC3, 0x07, 0xF0, 0xE1, 0x01, 0x70, 0x00, 0xE0, 0x0F, 0x00,
  0x00, 0x3E, 0xF0, 0x03, 0xE6, 0x3F, 0xC0, 0xF3, 0xE7, 0xC0, 0xFC, 0x07,
  0x00, 0xE0, 0x0F, 0x00, 0x00, 0x0E, 0xF0, 0x07, 0xF0, 0x7F, 0xC0, 0xF9,
  0x4F, 0x00, 0xFE, 0x0F, 0x1C, 0xE0, 0x0F, 0x00, 0x00, 0x06, 0xE0, 0x07,
  0xF0, 0x7F, 0xC0, 0xFC, 0x0F, 0x00, 0xFE, 0x0F, 0x3E, 0xF0, 0x07, 0x00,
  0x00, 0x02, 0xC0, 0x07, 0xE0, 0x3F, 0x00, 0xFE, 0x07, 0x00, 0xFC, 0x07,
  0x7E, 0xF0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0xE0, 0x7F, 0xF0, 0x07, 0x00, 0x00, 0x00, 0x00, 0xF8,
  0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x7F, 0xF8, 0x07, 0x00,
  0x00, 0x00, 0x00, 0xFE, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC,
  0x3F, 0xF8, 0x03, 0x00, 0x00, 0x00, 0x80, 0xFF, 0x0F, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0xFF, 0x3F, 0xFC, 0x03, 0x00, 0x00, 0x00, 0xC0, 0x9F,
  0x1F, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x00, 0x7F, 0x3F, 0xFC, 0x03, 0x00,
  0x00, 0x00, 0xE0, 0x0F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x83, 0x3F,
  0x3F, 0xFE, 0x33, 0x00, 0x00, 0x00, 0xF0, 0x07, 0x1F, 0x00, 0x00, 0x07,
  0x00, 0xFC, 0x83, 0x1F, 0x1F, 0xFE, 0x31, 0x00, 0x00, 0x00, 0xF8, 0x03,
  0x1F, 0x00, 0x80, 0x07, 0x01, 0xFE, 0xC3, 0x1F, 0x1F, 0xFF, 0x39, 0x00,
  0x00, 0xE0, 0xFD, 0x01, 0x1F, 0x00, 0xC0, 0xC7, 0x03, 0xBF, 0xC3, 0x8F,
  0x1F, 0xFF, 0x1D, 0x00, 0x00, 0xF8, 0xFF, 0x81, 0x1F, 0x00, 0xC0, 0xFF,
  0x83, 0x9F, 0xE3, 0x8F, 0x9F, 0xFF, 0x0F, 0x00, 0x00, 0xFC, 0xFF, 0x80,
  0x1F, 0x00, 0xC0, 0xFF, 0x83, 0x9F, 0xE3, 0xCF, 0xCF, 0xFD, 0x0F, 0x00,
  0x00, 0x0F, 0xFE, 0x83, 0x0F, 0xC0, 0xC3, 0xFF, 0xC3, 0xCF, 0xF1, 0xC7,
  0xEF, 0xFC, 0x07, 0x00, 0x00, 0x07, 0xFF, 0xC7, 0x0F, 0xE0, 0x87, 0xFF,
  0xE1, 0xEF, 0xF9, 0xC7, 0xFF, 0xFC, 0x03, 0x00, 0x80, 0x03, 0x7F, 0xE7,
  0x07, 0xF8, 0x87, 0xFB, 0xE1, 0xEF, 0xF8, 0xE7, 0x7F, 0xFC, 0x03, 0x00,
  0x80, 0x81, 0x7F, 0xE0, 0x03, 0xFC, 0x8F, 0xF1, 0xF1, 0x7F, 0xFC, 0xFF,
  0x3F, 0xFC, 0x01, 0x00, 0x80, 0x81, 0x3F, 0xF0, 0x03, 0x7E, 0xCF, 0xF9,
  0xF1, 0x7F, 0xFE, 0xFF, 0x3F, 0x78, 0x00, 0x00, 0x80, 0xC1, 0x3F, 0xF8,
  0x01, 0x3F, 0xC7, 0xF8, 0xF9, 0x1F, 0xFE, 0xFF, 0x1F, 0x00, 0x00, 0x00,
  0x80, 0xC3, 0x3F, 0xFC, 0x00, 0x3F, 0xE7, 0xFC, 0xFD, 0x0F, 0xF7, 0xDF,
  0x0F, 0x00, 0x00, 0x00, 0x80, 0xC7, 0x3F, 0x7E, 0x80, 0xBF, 0xE7, 0xFC,
  0xFC, 0x87, 0xF3, 0xCF, 0x07, 0x00, 0x00, 0x00, 0x00, 0xDF, 0xFF, 0x1F,
  0x80, 0xDF, 0x73, 0xFE, 0xFE, 0xC7, 0xF3, 0x07, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xFE, 0xFF, 0x07, 0xC4, 0xDF, 0x31, 0xFE, 0xFF, 0xFF, 0xE1, 0x03,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xFF, 0x01, 0xCE, 0xFF, 0x39, 0xFE,
  0xF7, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x3F, 0x00,
  0xEE, 0xFF, 0x38, 0xFE, 0xE3, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xC0, 0x3F, 0x00, 0xE7, 0x7F, 0x1C, 0xFF, 0xC1, 0x1F, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x3F, 0x00, 0xF7, 0x3F, 0x0E, 0xFF,
  0x01, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x3F, 0x80,
  0xF7, 0x0F, 0x07, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xC0, 0x7F, 0x80, 0xE7, 0x8F, 0x07, 0x3E, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xFF, 0xC0, 0xE3, 0xFF, 0x03, 0x08,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xFF, 0xF3,
  0xC3, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x80, 0xFF, 0xFF, 0xC1, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xFF, 0xFF, 0x01, 0x3F, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xFF, 0xFF,
  0x00, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0x3F, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x1F,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xC0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00
};

//define averages for sensors
Average<float> sonar1Average(30);
Average<float> sonar2Average(30);
Average<float> tempAverage(30);
Average<float> eTapeAverage(30);

//define variables for sensors
float cerealLevel;
boolean bowlPresent;
float milkTemperature;
float milkLevel;

//define memory variables
int alarmSet;
int cerealTimeHrs;
int cerealTimeMins;
int cerealPortion;
int milkPortion;

//set EEPROM allocation for variables
int eepromHrs = 0; //hrs (00 to 23)
int eepromMins = 1; //mins (00 to 59)
int eepromOnOff = 2; //alarm on/off (0/1)
int eepromCerealPortion = 3; //cereal portion (0-3)
int eepromMilkPortion = 4; //milk portion (0-3)

//ultrasonic sensor configeration
#define PING_PIN1  D6  // Arduino pin tied to both trigger and echo pins on the ultrasonic sensor.
#define PING_PIN2  D2  // Arduino pin tied to both trigger and echo pins on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar1(PING_PIN1, PING_PIN1, MAX_DISTANCE); // NewPing setup of pin and maximum distance.
NewPing sonar2(PING_PIN2, PING_PIN2, MAX_DISTANCE); // NewPing setup of pin and maximum distance.

// eTape Configuration values:
#define SERIES_RESISTOR     2200    // Value of the series resistor in ohms.   
#define SENSOR_PIN          A0      // Analog pin which is connected to the sensor.
#define ZERO_VOLUME_RESISTANCE    5320.00    // Resistance value (in ohms) when no liquid is present.
#define CALIBRATION_RESISTANCE    2030.00    // Resistance value (in ohms) when liquid is at max line.
#define CALIBRATION_VOLUME        1000.00    // Volume (in any units) when liquid is at max line.

//temperature sensor configuration
#define ONE_WIRE_BUS 2  // DS18B20 pin
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
float temp;

//cereal timer setup
AlarmId alarms1;

//boolean for app button
boolean appButton = true;

//define fauxmo for alexa
fauxmoESP fauxmo;
boolean alexaOn = false;

void setup()
{
  //initiate serial connection
  Serial.begin(115200);
  Serial.println();

  //set relay outputs
  pinMode(D1, OUTPUT);     // Solenoid - IN2 on relay
  pinMode(D7, OUTPUT);     // Pump and TEC - IN1 on relay
  pinMode(D8, OUTPUT);     // Motor - IN3 on relay
  digitalWrite(D1, LOW);
  digitalWrite(D7, LOW);
  digitalWrite(D8, LOW);

  //diplay logo at logon
  display.init();
  display.flipScreenVertically();
  display.drawXbm(0, 0, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
  display.display();
  display.setTextAlignment(TEXT_ALIGN_CENTER);

  //connect to internet

  Serial.print("Connecting to ");
  Serial.println(YOUR_WIFI_SSID);
  //WiFi.mode(WIFI_STA);
  WiFi.begin(YOUR_WIFI_SSID, YOUR_WIFI_PASSWD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("IP assigned by DHCP is ");
  Serial.println(WiFi.localIP());
  delay(2000);

  //pinging NTP server to check internet connection

  for (int i = 0; i <= 10; i++) {
    Serial.print("Pinging host ");
    Serial.println(ntpServer);

    if (Ping.ping(ntpServer)) {
      Serial.println("Success!!");

      Serial.println("Acquiring NTP time");
      NTP.begin(ntpServer, 0, true); //Start NTP Client
      while (timeStatus() != timeSet) {
        // Wait for time sync. Make yourself sure it can connect to WiFi
        Serial.print(".");
        delay(500);
      }
      Serial.println();
      //set time on RTC
      RTC.set(now());
      break;
    } else {
      Serial.println("Error :(");
    }
    delay(500);
  }

  if (timeStatus() != timeSet) {
    //get time from RTC only
    Serial.println("Getting time from RTC");
    setSyncProvider(RTC.get);
    if (timeStatus() != timeSet) {
      Serial.println("Unable to sync with the RTC");
      //what now? Connect via app and sync time that way?
    } else {
      Serial.println("RTC has set the system time");
    }
  }

  //read eeprom memory
  EEPROM.begin(512);
  cerealTimeHrs = EEPROM.read(eepromHrs);
  cerealTimeMins = EEPROM.read(eepromMins);
  alarmSet = EEPROM.read(eepromOnOff);
  cerealPortion = EEPROM.read(eepromCerealPortion);
  milkPortion = EEPROM.read(eepromMilkPortion);
  Serial.println();
  Serial.println("EEPROM memory settings:");
  Serial.println("Alarm Hrs " + String(cerealTimeHrs));
  Serial.println("Alarm Mins " + String(cerealTimeMins));
  Serial.println("Alarm On/Off " + String(alarmSet));
  Serial.println("Cereal Portion " + String(cerealPortion));
  Serial.println("Milk Portion " + String(milkPortion));
  Serial.println();

  //sensor activation alarm
  Alarm.timerRepeat(0, 0, 59, alarmtimer1);
  //cereal repeat alarm
  alarms1 = Alarm.alarmRepeat(cerealTimeHrs, cerealTimeMins, 0, alarmRepeat1);

  //set up fauxmo for alexa
  fauxmo.enable(true);
  unsigned char device_1 = fauxmo.addDevice("Cereal");
  fauxmo.onMessage([](unsigned char device_id, const char * device_name, bool state) {
    Serial.printf("[MAIN] Device #%d (%s) state: %s\n", device_id, device_name, state ? "ON" : "OFF");
    alexaOn = true;
  });

  //initiate websocket server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  //check levels at startup
  alarmtimer1();
}

time_t prevDisplay = 0; // when the digital clock was displayed


void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:

      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);

        //communicate current state of robocereal
        webSocket.broadcastTXT("HM#" + String(cerealTimeHrs) +"." + String(cerealTimeMins)); //Hrs
        webSocket.broadcastTXT("C#" + String(cerealLevel)); //Cereal level 1
        webSocket.broadcastTXT("V#" + String(milkLevel)); //Volume 1
        webSocket.broadcastTXT("T#" + String(milkTemperature)); //Milk temperature
        webSocket.broadcastTXT("K#" + String(cerealPortion)); //Milk portion
        webSocket.broadcastTXT("P#" + String(milkPortion)); //Cereal portion

        if (alarmSet == 0){
          webSocket.broadcastTXT("ASOFF#"); //Alarm state
        } else if (alarmSet == 1){
          webSocket.broadcastTXT("ASON#"); //Alarm state
        }
      }
      break;

    case WStype_TEXT:
      {
        //extract text
        String text = String((char *) &payload[0]);

        //Check password
        if (text.substring(0, 3) == "PW#") {
          String passreceived = text.substring(text.indexOf('#') + 1);

          if (passreceived == robopassword) {
            webSocket.sendTXT(num, "P#G", length);
            Serial.println(String(webSocket.remoteIP(num).toString() + " has logged in."));
          }
          else {
            webSocket.sendTXT(num, "P#B", length);
            Serial.println(String(webSocket.remoteIP(num).toString() + " bass pass."));
          }
        }

        //make cereal button pressed
        if (text == "MC") {
          appButton = true;
          alarmRepeat1();
        }

        //Milk Cereal level handling (Milk Cereal = MC)
        if (text.substring(0, 3) == "MC#") {
          cerealPortion = text.substring(text.indexOf('#') + 1, text.indexOf('#') + 2).toInt();
          milkPortion = text.substring(text.indexOf('.') + 1, text.indexOf('.') + 2).toInt();

          //write to EEPROM
          EEPROM.write(eepromCerealPortion, cerealPortion);
          EEPROM.write(eepromMilkPortion, milkPortion);
          EEPROM.commit();

          //Broadcast
          webSocket.broadcastTXT("K#" + String(cerealPortion)); //Milk portion
          webSocket.broadcastTXT("P#" + String(milkPortion)); //Cereal portion

          //Serial print
          Serial.println("Cereal portion changed to: " + String(cerealPortion));
          Serial.println("Milk portion changed to: " + String(milkPortion));

        }

        //Alarm handling (Hours Minutes = HM)
        if (text.substring(0, 3) == "HM#") {
          if (text.substring(text.indexOf('#') + 1, text.indexOf('#') + 2) == "0" ) {
            cerealTimeHrs = text.substring(text.indexOf('#') + 2, text.indexOf('#') + 3).toInt();
          } else {
            cerealTimeHrs = text.substring(text.indexOf('#') + 1, text.indexOf('#') + 3).toInt();
          }

          if (text.substring(text.indexOf('.') + 1, text.indexOf('.') + 2) == "0" ) {
            cerealTimeMins = text.substring(text.indexOf('.') + 2, text.indexOf('.') + 3).toInt();
          } else {
            cerealTimeMins = text.substring(text.indexOf('.') + 1, text.indexOf('.') + 3).toInt();
          }
          //change alarm time
          Serial.println("Alarm Time changed to HM#" + String(cerealTimeHrs) + "." + String(cerealTimeMins));
          Serial.println("alarm on");
          
          Alarm.free(alarms1);
          alarms1 = dtINVALID_ALARM_ID;
          alarms1 = Alarm.alarmRepeat(cerealTimeHrs, cerealTimeMins, 0, alarmRepeat1);

          alarmSet = 1;

          //write to EEPROM
          EEPROM.write(eepromHrs, cerealTimeHrs);
          EEPROM.write(eepromMins, cerealTimeMins);
          EEPROM.write(eepromOnOff, alarmSet);
          EEPROM.commit();

          //let app know
          webSocket.broadcastTXT("ASON#");
          webSocket.broadcastTXT("HM#" + String(cerealTimeHrs) +"." + String(cerealTimeMins)); //Hrs
        }

        //Alarm Set (AS) OFF
        if (text.substring(0, 3) == "AO#") {
          Serial.println("alarm off");
          alarmSet = 0;
          EEPROM.write(eepromOnOff, alarmSet);
          EEPROM.commit();

          //let app know
          webSocket.broadcastTXT("ASOFF#");
        }
        
        //Alarm Set (AS) ON
        if (text.substring(0, 3) == "AN#") {
          Serial.println("alarm on");
          alarmSet = 1;
          EEPROM.write(eepromOnOff, alarmSet);
          EEPROM.commit();

          //let app know
          webSocket.broadcastTXT("ASON#");
        }

      }
      //webSocket.sendTXT(num, payload, length);
      webSocket.broadcastTXT(payload, length);
      break;

    case WStype_BIN:
      hexdump(payload, length);
      // echo data back to browser
      webSocket.sendBIN(num, payload, length);
      break;
  }
}

void alarmRepeat1() {

  //is alarm set?
  if (alarmSet == 0 && alexaOn == false && appButton == false) {
    return;
  }

  //check if bowl is there!
  int count1 = 0;
  float sonar2AverageResult;

  //collect 10 cycles of each measurement
  while (count1 != 10) {
    Alarm.delay(50);
    webSocket.loop(); //handle websocket between pings
    sonar2Average.push(sonar2.ping_cm());
    count1 += 1;
  }

  sonar2AverageResult = sonar2Average.mode();
  Serial.print("Ping2: ");
  Serial.print(sonar2AverageResult); // Send ping, get distance in cm and print result (0 = outside set distance range)
  Serial.println("cm");

  //clear
  sonar2Average.clear();

  if (sonar2AverageResult < 8) {
    bowlPresent = true;
    webSocket.broadcastTXT("BOK");
  } else {
    bowlPresent = false;
    webSocket.broadcastTXT("NB"); //broadcast No Bowl
    alexaOn = false;
    appButton = false;
    //include some sort of debouncing?
    return;
  }

  Serial.println("Making cereal");
  //Diplay screen!!!
  //dispense cereal
  digitalWrite(D8, HIGH);   // Motor
  int cerealDispense = cerealPortion * 8;
  int milkDispense = milkPortion * 40;
  for (int i = 0; i <= cerealDispense; i++) {
    //portion length here
    Alarm.delay(499);
    delay(1);
    Serial.println(i);
  }
  digitalWrite(D8, LOW);
  //dispense milk
  digitalWrite(D1, HIGH);   // Solenoid
  for (int i = 0; i <= milkDispense; i++) {
    //portion length here
    Alarm.delay(499);
    delay(1);
  }
  digitalWrite(D1, LOW);
  webSocket.broadcastTXT("DONE");

  //turn off alexa and button press
  alexaOn = false;
  appButton = false;

  //include some sort of debouncing?


  //recheck levels
  alarmtimer1;
  //now resync with RTC if possible
}

void alarmtimer1() {
  //check cereal, milk and bowl levels
  int count1 = 0;
  float resistance;

  //collect 30 cycles of each measurement
  while (count1 != 30) {
    //handle websockets each loop
    webSocket.loop();
    fauxmo.handle();
    //if alexa activated while timer run then stop
    if (alexaOn == true) {
      return;
    }

    //read ultrasonic sensors
    Alarm.delay(50);                      // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
    webSocket.loop(); //handle websocket between pings
    fauxmo.handle();
    if (alexaOn == true) {
      return;
    }

    sonar1Average.push(sonar1.ping_cm());
    Alarm.delay(50);
    webSocket.loop(); //handle websocket between pings
    fauxmo.handle();
    if (alexaOn == true) {
      return;
    }

    sonar2Average.push(sonar2.ping_cm());
    //read eTape
    resistance = readResistance(SENSOR_PIN, SERIES_RESISTOR);
    eTapeAverage.push(resistance);
    //read thermometer
    DS18B20.requestTemperatures();
    tempAverage.push(DS18B20.getTempCByIndex(0));

    //update time if need be
    if (timeStatus() != timeNotSet) {
      if (now() != prevDisplay) { //update the display only if time has changed
        prevDisplay = now();
        digitalClockDisplay();
        display.display();
      }
    }

    count1 += 1;
  }
  //print out bowl and cereal

  cerealLevel = sonar1Average.mode();
  Serial.print("Ping1: ");
  Serial.print(cerealLevel); // Send ping, get distance in cm and print result (0 = outside set distance range)
  Serial.println("cm");

  //print out milk temp
  milkTemperature = (tempAverage.mean());
  Serial.print("Temperature: ");
  Serial.println(milkTemperature);
  //print out milk volume
  Serial.print("Resistance: ");
  Serial.print(eTapeAverage.mode(), 2);
  Serial.println(" ohms");
  milkLevel = resistanceToVolume(eTapeAverage.mode(), ZERO_VOLUME_RESISTANCE, CALIBRATION_RESISTANCE, CALIBRATION_VOLUME);
  Serial.print("Calculated volume: ");
  Serial.println(milkLevel);

  //clear averages for next run
  sonar1Average.clear();
  tempAverage.clear();
  eTapeAverage.clear();

  //if milk too warm turn on relay - introduce count if longer on off cycle desired
  if (milkTemperature > 10) {
    if (digitalRead(D7) == HIGH) {
      digitalWrite(D7, LOW);
    } else {
      digitalWrite(D7, HIGH);
    }
  } else {
    digitalWrite(D7, LOW);
  }

  //broadcast cereal/milk levels here
  webSocket.broadcastTXT("C#" + String(cerealLevel)); //Cereal level 1
  webSocket.broadcastTXT("V#" + String(milkLevel)); //Volume 1
  webSocket.broadcastTXT("T#" + String(milkTemperature)); //Milk temperature
}

void loop()
{
  //handle alexa
  if (alexaOn == true) {
    alarmRepeat1();
  }

  //handle websockets
  webSocket.loop();

  //handle alexa
  fauxmo.handle();

  //zero alarm delay, is this needed?
  Alarm.delay(0);

  //update display
  if (timeStatus() != timeNotSet) {
    if (now() != prevDisplay) { //update the display only if time has changed
      prevDisplay = now();
      digitalClockDisplay();
      display.display();
    }
  }
}


//set clock display for time
void digitalClockDisplay()
{
  // digital clock display of the time
  String timenow = String(hour()) + ":" + twoDigits(minute()) + ":" + twoDigits(second());
  Serial.println(timenow + " " + day() + "." + month() + "." + year());

  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawString(clockCenterX, 6 , "IP:" + WiFi.localIP().toString());
  display.setFont(ArialMT_Plain_24);
  display.drawString(clockCenterX, clockCenterY, timenow );

}

String twoDigits(int digits) {
  if (digits < 10) {
    String i = '0' + String(digits);
    return i;
  }
  else {
    return String(digits);
  }
}


//calculate float for etape resistance
float readResistance(int pin, int seriesResistance) {
  // Get ADC value.
  float resistance = analogRead(pin);
  // Convert ADC reading to resistance.
  resistance = (1023.0 / resistance) - 1.0;
  resistance = seriesResistance / resistance;
  return resistance;
}
float resistanceToVolume(float resistance, float zeroResistance, float calResistance, float calVolume) {
  if (resistance > zeroResistance || (zeroResistance - calResistance) == 0.0) {
    // Stop if the value is above the zero threshold, or no max resistance is set (would be divide by zero).
    return 0.0;
  }
  // Compute scale factor by mapping resistance to 0...1.0+ range relative to maxResistance value.
  float scale = (zeroResistance - resistance) / (zeroResistance - calResistance);
  // Scale maxVolume based on computed scale factor.
  return calVolume * scale;
}
