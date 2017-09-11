#ifndef MosquittoMinWeb_H
#define MosquittoMinWeb_H

#include <ESP8266WiFi.h>
#include "ESP8266xWebServer.h"
#include "mEEPROM.h"
#include "context.h"
extern "C" {
#include "user_interface.h"
}

#ifndef DEBUG_WEB
#define LOG_WEB(...)
#endif
class MosquittoMinWeb : public ESP8266xWebServer
{
public:
  MosquittoMinWeb(int MaxEEPROM,char *www_username, char*www_password, int port);
  ~MosquittoMinWeb();
private:
  int _MaxEEPROM;
  char* _www_username;
  char* _www_password;
};
#endif //MosquittoMinWeb_H

