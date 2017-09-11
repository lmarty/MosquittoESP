#include "env.h"
env::env(){
 /* http://<AP IP>set?env={"CWIFISSID":"xxxxxx","CWIFIPSWD":"xxxxxx","WIFISSID":"xxxxx","WIFIPSWD":"xxxxxx","MQTTNAME":"xxxxxx","MQTTPSWD":"xxxxxxx","MQTTPUBKEYPATH":"xxxxxx","MQTTSUBKEYPATH":"xxxxxxx","MQTTPUBIDPATH":"xxxxxx"} */
  const size_t bufferSize = JSON_OBJECT_SIZE(9) + 190;
  DynamicJsonBuffer jsonBuffer(bufferSize);
  
  uint8_t *cenv0 = (uint8_t *)"{\"CWIFISSID\":\"xxxxx\",\"CWIFIPSWD\":\"xxxxx\",\"WIFISSID\":\"MOSESPWSSMQTT\",\"WIFIPSWD\":\"MOSESPWSSMQTT\",\"MQTTNAME\":\"xxxxx\",\"MQTTPSWD\":\"xxxxx\",\"MQTTPUBKEYPATH\":\"xxxxx\",\"MQTTSUBKEYPATH\":\"xxxxx\",\"MQTTPUBIDPATH\":\"xxxxx\"}";
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
      MQTTPUBIDPATH = strdup(jenv["MQTTPUBIDPATH"]); 
      break;
    } else {
      cenv = cenv0;
    }
  }
    LOG_ENV("env---{\"CWIFISSID\":\"%s\",\"CWIFIPSWD\":\"%s\",\"WIFISSID\":\"%s\",\"WIFIPSWD\":\"%s\",\"MQTTNAME\":\"%s\",\"MQTTPSWD\":\"%s\",\"MQTTPUBKEYPATH\":\"%s\",\"MQTTSUBKEYPATH\":\"%s\",\"MQTTSUBKEYPATH\":\"%s\"}\n",
            CWIFISSID,CWIFIPSWD,WIFISSID,WIFIPSWD,MQTTNAME,MQTTPSWD,MQTTPUBKEYPATH,MQTTSUBKEYPATH,MQTTPUBIDPATH);
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
boolean env::verify(String env){
  /* http://<AP IP>set?env={"CWIFISSID":"xxxxxx","CWIFIPSWD":"xxxxxx","WIFISSID":"xxxxx","WIFIPSWD":"xxxxxx","MQTTNAME":"xxxxxx","MQTTPSWD":"xxxxxxx","MQTTPUBKEYPATH":"xxxxxx","MQTTSUBKEYPATH":"xxxxxxx",,"MQTTPUBIDPATH":"xxxxxx"} */
  const size_t bufferSize = JSON_OBJECT_SIZE(9) + 190;
  DynamicJsonBuffer jsonBuffer(bufferSize);
  boolean result=true;
  JsonObject& root = jsonBuffer.parseObject(env.c_str());
  
  if(root.containsKey("CWIFISSID")){
    if(strlen(root["CWIFISSID"])==0) result==false;
  } else {
    result==false;
  }
  if(root.containsKey("CWIFIPSWD")){
    if(strlen(root["CWIFIPSWD"])==0) result==false;
  } else {
    result==false;
  }
  if(root.containsKey("WIFISSID")){
    if(strlen(root["WIFISSID"])==0) result==false;
  } else {
    result==false;
  }
  if(root.containsKey("WIFIPSWD")){
    if(strlen(root["WIFIPSWD"])==0) result==false;
  } else {
    result==false;
  }
  if(root.containsKey("MQTTNAME")){
    if(strlen(root["MQTTNAME"])==0) result==false;
  } else {
    result==false;
  }
  if(root.containsKey("MQTTPSWD")){
    if(strlen(root["MQTTPSWD"])==0) result==false;
  } else {
    result==false;
  }
  if(root.containsKey("MQTTPUBKEYPATH")){
    if(strlen(root["MQTTPUBKEYPATH"])==0) result==false;
  } else {
    result==false;
  }
  if(root.containsKey("MQTTSUBKEYPATH")){
    if(strlen(root["MQTTSUBKEYPATH"])==0) result==false;
  } else {
    result==false;
  }
  if(root.containsKey("MQTTPUBIDPATH")){
    if(strlen(root["MQTTPUBIDPATH"])==0) result==false;
  } else {
    result==false;
  }
  return result;
}


