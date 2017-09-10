#include "env.h"
env::env(){
 /* http://<AP IP>set?env={"CWIFISSID":"xxxxxx","CWIFIPSWD":"xxxxxx","WIFISSID":"xxxxx","WIFIPSWD":"xxxxxx","MQTTNAME":"xxxxxx","MQTTPSWD":"xxxxxxx","MQTTPUBKEYPATH":"xxxxxx","MQTTSUBKEYPATH":"xxxxxxx"} */
  const size_t bufferSize = JSON_OBJECT_SIZE(8) + 190;
  DynamicJsonBuffer jsonBuffer(bufferSize);
  
  uint8_t *cenv0 = (uint8_t *)"{\"CWIFISSID\":\"xxxxx\",\"CWIFIPSWD\":\"xxxxx\",\"WIFISSID\":\"MESPWSSMQTT\",\"WIFIPSWD\":\"esp8266\",\"MQTTNAME\":\"xxxxx\",\"MQTTPSWD\":\"xxxxx\",\"MQTTPUBKEYPATH\":\"xxxxx\",\"MQTTSUBKEYPATH\":\"xxxxx\"}";
  uint8_t *cenv = cenv0;

  if((mEEPROM.read(0)==0xAA)&&(mEEPROM.read(1)==0x55)){  // eeprom marker to know if initilized by user
    cenv = mEEPROM.getDataPtrRO()+2;
  }

  
  while(1){
    JsonObject& jenv = jsonBuffer.parseObject(cenv);
    if(jenv.containsKey("CWIFISSID")){
      CWIFISSID = strdup(jenv["CWIFISSID"]);
      CWIFIPSWD = strdup(jenv["CWIFIPSWD"]);
      WIFISSID = strdup(jenv["WIFISSID"]);
      WIFIPSWD = strdup(jenv["WIFIPSWD"]);
      MQTTNAME = strdup(jenv["MQTTNAME"]);
      MQTTPSWD = strdup(jenv["MQTTPSWD"]);
      MQTTPUBKEYPATH = strdup(jenv["MQTTPUBKEYPATH"]);
      MQTTSUBKEYPATH = strdup(jenv["MQTTSUBKEYPATH"]);
      break;
    } else {
      cenv = cenv0;
    }
  }
    LOG_ENV("env---{\"CWIFISSID\":\"%s\",\"CWIFIPSWD\":\"%s\",\"WIFISSID\":\"%s\",\"WIFIPSWD\":\"%s\",\"MQTTNAME\":\"%s\",\"MQTTPSWD\":\"%s\",\"MQTTPUBKEYPATH\":\"%s\",\"MQTTSUBKEYPATH\":\"%s\"}\n",
            CWIFISSID,CWIFIPSWD,WIFISSID,WIFIPSWD,MQTTNAME,MQTTPSWD,MQTTPUBKEYPATH,MQTTSUBKEYPATH);
}
env::~env(){
  free(CWIFISSID);
  free(CWIFIPSWD);
  free(WIFISSID);
  free(WIFIPSWD);
  free(MQTTNAME);
  free(MQTTPSWD);
  free(MQTTPUBKEYPATH);
  free(MQTTSUBKEYPATH);
}

