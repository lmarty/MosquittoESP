#ifndef MosquittoESPWeb_H
#define MosquittoESPWeb_H

#include <ESP8266WiFi.h>
#include "ESP8266xWebServer.h"
#include "mEEPROM.h"
#include "context.h"
#include "WebSocketsClient.h"
#include "PubSubClient.h"
extern "C" {
#include "user_interface.h"
}
#include <ArduinoJson.h>

#ifndef DEBUG_WEB
#define LOG_WEB(...)
#endif

/*
 * This server is alive while in AP mode to configure paratemeters to have the ESP device able to connect and publish/subscribe to a Mosquittoaut server
 * list of parameters
 * ==================
 * WIFI Access point to connect as a client
 *    CWIFISSID, CWIFIPSWD
 * WIFI AP Identifier and password
 *    WIFISSID (MosquittoESP), WIFIPSWD (esp8266)
 * MQTT client name and password
 *    MQTTNAME, MQTTPSWD
 * MQTT key access path where to publish a key when received by the gateway
 *    MQTTPUBKEYPATH  (like... HOME/key)
 * MQTT key access path to subscribe a key and transmitt it to the gateway 
 *    MQTTSUBKEYPATH
 * http://<AP IP>set?env={"CWIFISSID":"xxxxxx","CWIFIPSWD":"xxxxxx","WIFISSID":"xxxxx","WIFIPSWD":"xxxxxx","MQTTNAME":"xxxxxx","MQTTPSWD":"xxxxxxx","MQTTPUBKEYPATH":"xxxxxx","MQTTSUBKEYPATH":"xxxxxxx"}
 */

class MosquittoESPWeb : public ESP8266xWebServer
{
public:
  MosquittoESPWeb(int MaxEEPROM,char *www_username, char*www_password, int port);
  ~MosquittoESPWeb(); 
  static boolean verify(String env);
private:
  int _MaxEEPROM;
  char* _www_username;
  char* _www_password;
};
#endif
