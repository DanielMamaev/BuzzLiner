#define KEEP_SETTINGS 1
#define RESET_SETTINGS 0

#define RX 14
#define TX 12

#define PIN_LED LED_BUILTIN

#define EEPROM_addr_NAME_R 1
#define EEPROM_addr_ip_mode 13
#define EEPROM_ip_0 14
#define EEPROM_ip_1 15
#define EEPROM_ip_2 16
#define EEPROM_ip_3 17
#define EEPROM_sn_0 18
#define EEPROM_sn_1 19
#define EEPROM_sn_2 20
#define EEPROM_sn_3 21
#define EEPROM_gw_0 22
#define EEPROM_gw_1 23
#define EEPROM_gw_2 24
#define EEPROM_gw_3 25
#define EEPROM_addr_baud 26
#define EEPROM_addr_soft_baud 30
#define EEPROM_addr_set_pass 34

unsigned long baud = 9600;
unsigned long soft_baud = 9600;
unsigned long list_baud[] = {300, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 74880, 115200, 230400, 250000, 500000, 1000000, 2000000};

String NAME_R = "NEWR";
String ap_pass = "password";

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266SSDP.h>

#include <ESP8266HTTPUpdateServer.h>
ESP8266HTTPUpdateServer httpUpdater;

#include <EEPROM.h>

#include <WiFiManager.h>
WiFiManager wifiManager;

#include <FS.h>
File fsUploadFile;

ESP8266WebServer HTTP(80);

#include <ArduinoJson.h>
StaticJsonDocument<400> doc;

//-----------------------------------TCP--------------------
#include <algorithm> // std::min
#define RXBUFFERSIZE 1024
#define STACK_PROTECTOR 512 // bytes
// how many clients should be able to telnet to this ESP8266
#define MAX_SRV_CLIENTS 4
const int port = 23;
WiFiServer server(port);
WiFiClient serverClients[MAX_SRV_CLIENTS];

//---------------- SOFTSERIAL
#include <SoftwareSerial.h>
SoftwareSerial mySerial(RX, TX); // RX, TX
// SoftwareSerial Serial_lora(RX_lora, TX_lora); // RX, TX

//---КОМАНДА ДЛЯ UBOX
//для m8n включение сырых данных + nmea на uart1, скорость 9600, все спутники
byte msg[] = {181, 98, 6, 62, 12, 0, 0, 32, 32, 1, 0, 10, 32, 0, 1, 0, 0, 0, 188, 93, 181, 98, 6, 62, 12, 0, 0, 32, 32, 1, 6, 8, 16, 0, 1, 0, 0, 0, 176, 31, 181, 98, 6, 1, 8, 0, 3, 15, 0, 1, 0, 1, 0, 0, 35, 44, 181, 98, 6, 1, 8, 0, 3, 16, 0, 1, 0, 1, 0, 0, 36, 51, 181, 98, 6, 1, 8, 0, 1, 32, 0, 1, 0, 1, 0, 0, 50, 147};

// отключения nmea, остается ubx, на uart1, скорость 9600
byte msg1[] = {181, 98, 6, 0, 20, 0, 1, 0, 0, 0, 208, 8, 0, 0, 128, 37, 0, 0, 7, 0, 1, 0, 0, 0, 0, 0, 160, 169};
byte msg2[] = {181, 98, 6, 0, 1, 0, 1, 8, 34};
byte msg3[] = {181, 98, 10, 4, 0, 0, 14, 52};
//

int freq_flash = 1000;

boolean serial_on = true;
bool flag_con = false;
byte ip_mode = 0;
unsigned long check_ip, reconnect_timer;
IPAddress ip_temp;

#include "Serial.h"
#include "FileSys.h"
#include "HTTP_init.h"
#include "SSDP.h"
#include "LED.h"
#include "TCP_Svr.h"

bool flag_wifi = false;
String temp_ssid = "";
String temp_psk = "";

IPAddress _ip;
IPAddress _gw;
IPAddress _sm;
//*********************** SETUP *******************
void setup()
{
  pinMode(PIN_LED, OUTPUT);

  Serial.begin(baud);
  Serial.println(F("\nHello!"));
  Serial.setTimeout(100);
  Serial.setRxBufferSize(RXBUFFERSIZE);

  EEPROM.begin(512);

  if (RESET_SETTINGS)
  {
    EEPROM.write(0, 0);
    EEPROM.commit();
  }

  if (KEEP_SETTINGS)
  {
    if (EEPROM.read(0) != 1)
    {
      EEPROM.write(0, 1);
      EEPROM.commit();
      EEPROM_write();
    }
    else
      EEPROM_read();
  }

  mySerial.begin(soft_baud);
  mySerial.setTimeout(100);

  mySerial.write(msg, 88);
  delay(100);
  mySerial.write(msg1, 28);
  delay(100);
  mySerial.write(msg2, 9);
  delay(100);
  mySerial.write(msg3, 8);
  delay(100);

  //---ВЫТАСКИВАЕМ ИЗ ПАМЯТИ IP, MASK, GATEWAY
  if (ip_mode == 1)
  {
    _ip = IPAddress(EEPROM.read(EEPROM_ip_0), EEPROM.read(EEPROM_ip_1), EEPROM.read(EEPROM_ip_2), EEPROM.read(EEPROM_ip_3));
    _gw = IPAddress(EEPROM.read(EEPROM_gw_0), EEPROM.read(EEPROM_gw_1), EEPROM.read(EEPROM_gw_2), EEPROM.read(EEPROM_gw_3));
    _sm = IPAddress(EEPROM.read(EEPROM_sn_0), EEPROM.read(EEPROM_sn_1), EEPROM.read(EEPROM_sn_2), EEPROM.read(EEPROM_sn_3));
    Serial.println(_ip);
    Serial.println(_gw);
    Serial.println(_sm);
    wifiManager.setSTAStaticIPConfig(_ip, _gw, _sm);
    WiFi.config(_ip, _gw, _sm);
  }
  wifiManager.setConfigPortalBlocking(false);

 
  if ((WiFi.SSID() == "") && (WiFi.psk() == ""))
  {
    WiFi.softAP(NAME_R, ap_pass);
  }
  else
  {
    WiFi.begin(WiFi.SSID(), WiFi.psk());
  }

  server.begin();
  server.setNoDelay(true);

  FS_init();

  saveJson();

  SSDP_init();

  HTTP_init();

  delay(1000);
  check_ip = millis();

  Serial.println("OK");
}

//*************** LOOP***************
void loop()
{
  wifiManager.process();
  HTTP.handleClient();

  if (millis() - check_ip > 2000)
  {
    if (WiFi.status() == WL_CONNECTED and WiFi.localIP() != ip_temp)
    {
      ip_temp = WiFi.localIP();
      WiFi.softAP(NAME_R + "_" + String(ip_temp[0]) + "." + String(ip_temp[1]) + "." + String(ip_temp[2]) + "." + String(ip_temp[3]), "password");
      flag_wifi = false;
    }

    if (WiFi.status() == WL_NO_SSID_AVAIL and flag_wifi == false)
    {

      temp_ssid = WiFi.SSID();
      temp_psk = WiFi.psk();
      WiFi.disconnect();
      WiFi.softAP(NAME_R, ap_pass);
      flag_wifi = true;
      reconnect_timer = millis();
    }
    check_ip = millis();
  }

  if (flag_wifi)
  {
    if (millis() - reconnect_timer > 30000)
    {
      WiFi.begin(temp_ssid, temp_psk);
      reconnect_timer = millis();
      flag_wifi = false;
    }
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    TCP_void();
    tcp_led();
  }

  if (serial_on and !flag_con)
  {
    if (mySerial.available())
      Serial.write(mySerial.read());
  }

  indi();
  ser_con();
}

void EEPROM_read()
{
  Serial.println(F(""));
  Serial.println(F("Start read EEPROM"));

  EEPROM.get(EEPROM_addr_NAME_R, NAME_R);
  Serial.println("Name receiver: " + NAME_R);

  EEPROM.get(EEPROM_addr_ip_mode, ip_mode);
  Serial.println("IP mode: " + String(ip_mode));

  Serial.println("IP: " + String(EEPROM.read(EEPROM_ip_0)) + "." + String(EEPROM.read(EEPROM_ip_1)) + "." + String(EEPROM.read(EEPROM_ip_2)) + "." + String(EEPROM.read(EEPROM_ip_3)));
  Serial.println("Gateway: " + String(EEPROM.read(EEPROM_gw_0)) + "." + String(EEPROM.read(EEPROM_gw_1)) + "." + String(EEPROM.read(EEPROM_gw_2)) + "." + String(EEPROM.read(EEPROM_gw_3)));
  Serial.println("Subnet: " + String(EEPROM.read(EEPROM_sn_0)) + "." + String(EEPROM.read(EEPROM_sn_1)) + "." + String(EEPROM.read(EEPROM_sn_2)) + "." + String(EEPROM.read(EEPROM_sn_3)));

  EEPROM.get(EEPROM_addr_baud, baud);
  Serial.println("Serial baud: " + String(baud));

  EEPROM.get(EEPROM_addr_soft_baud, soft_baud);
  Serial.println("SoftSerial baud: " + String(soft_baud));

  EEPROM.get(EEPROM_addr_set_pass, ap_pass);
  Serial.println("Password: " + ap_pass);

  Serial.println(F("End read EEPROM"));
}

void EEPROM_write()
{
  Serial.println(F(""));
  Serial.println(F("Start write EEPROM"));

  Serial.println("Name receiver: " + NAME_R);
  EEPROM.put(EEPROM_addr_NAME_R, NAME_R);

  Serial.println("IP mode: " + String(ip_mode));
  EEPROM.put(EEPROM_addr_ip_mode, ip_mode);

  for (byte i = 14; i < 26; i++)
  {
    EEPROM.put(i, 255);
  }

  Serial.println("Serial baud: " + String(baud));
  EEPROM.put(EEPROM_addr_baud, baud);

  Serial.println("SoftSerial baud: " + String(soft_baud));
  EEPROM.put(EEPROM_addr_soft_baud, soft_baud);

  Serial.println("Password: " + ap_pass);
  EEPROM.put(EEPROM_addr_set_pass, ap_pass);

  Serial.println(F("End write EEPROM"));
  EEPROM.commit();
}

void saveJson()
{
  doc["name"] = NAME_R;
  doc["baud"] = baud;
  doc["soft_baud"] = soft_baud;
  doc["ip"] = _ip.toString();
  doc["gw"] = _gw.toString();
  doc["sm"] = _sm.toString();

  String output_json;
  serializeJson(doc, output_json);
  Serial.println(output_json);

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile)
  {
    Serial.println("Failed to open config file for writing");
    configFile.close();
    return;
  }
  else
  {
    serializeJson(doc, configFile);
    configFile.close();
  }
}
