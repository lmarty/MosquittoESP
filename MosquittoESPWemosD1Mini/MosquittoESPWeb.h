#ifndef MosquittoESPWeb_H
#define MosquittoESPWeb_H

#include <ESP8266WiFi.h>
#include "ESP8266xWebServer.h"
#include <EEPROM.h>
#include "context.h"
#include "WebSocketsClient.h"
#include "PubSubClient.h"
extern "C" {
#include "user_interface.h"
}

#ifndef DEBUG_WEB
#define LOG(...)
#endif

class MosquittoESPWeb : public ESP8266xWebServer
{
public:
  MosquittoESPWeb(int MaxEEPROM,char *www_username, char*www_password, int port);
  ~MosquittoESPWeb();
  int action;
private:
  int _MaxEEPROM;
  char* _www_username;
  char* _www_password;
};
#endif
