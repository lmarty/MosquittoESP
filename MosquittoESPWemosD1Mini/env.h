#ifndef env_H
#define env_H
#include <Arduino.h>
#include "mEEPROM.h"
#include <ArduinoJson.h>
#include "context.h"
class env{
  public:
    env();
    ~env();
    /* http://<AP IP>set?env={"CWIFISSID":"xxxxxx","CWIFIPSWD":"xxxxxx","WIFISSID":"xxxxx","WIFIPSWD":"xxxxxx","MQTTNAME":"xxxxxx","MQTTPSWD":"xxxxxxx","MQTTPUBKEYPATH":"xxxxxx","MQTTSUBKEYPATH":"xxxxxxx"} */
    char * CWIFISSID;
    char * CWIFIPSWD;
    char * WIFISSID;
    char * WIFIPSWD;
    char * MQTTNAME;
    char * MQTTPSWD;
    char * MQTTPUBKEYPATH;
    char * MQTTSUBKEYPATH;
};
#endif  // env_H
