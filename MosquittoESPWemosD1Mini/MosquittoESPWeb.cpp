#include "MosquittoESPWeb.h"

MosquittoESPWeb::MosquittoESPWeb(int MaxEEPROM,char *www_username, char*www_password, int port):ESP8266xWebServer(port){
  if(WiFi.isConnected()){
    _MaxEEPROM=MaxEEPROM;
    _www_username=www_username;
    _www_password=www_password;
    action=0;
    this->on("/", [](void* server){
      LOG("/\n");
      MosquittoESPWeb* s = static_cast<MosquittoESPWeb *>(server);
      if(!s->authenticate(s->_www_username, s->_www_password))
                        return s->requestAuthentication();
      s->send(200, "text/plain", "Login OK");
    });
    this->on("/delete", [](void* server){
      LOG("/delete\n");
      MosquittoESPWeb* s = static_cast<MosquittoESPWeb *>(server);
      if(!s->authenticate(s->_www_username, s->_www_password))
                        return s->requestAuthentication();
      s->send(200, "text/plain", "Delete OK");
    });
    this->on("/stop", [](void* server){
      LOG("/stop\n");
      MosquittoESPWeb* s = static_cast<MosquittoESPWeb *>(server);
      if(!s->authenticate(s->_www_username, s->_www_password))
                        return s->requestAuthentication();
      s->action=1;
      s->send(200, "text/plain", "Stop OK");
    });
    this->on("/start", [](void* server){
      LOG("/start\n");
      MosquittoESPWeb* s = static_cast<MosquittoESPWeb *>(server);
      if(!s->authenticate(s->_www_username, s->_www_password))
                        return s->requestAuthentication();
      s->action=0;
      s->send(200, "text/plain", "Start OK");
    });
    this->on("/set", [](void* server){
      MosquittoESPWeb* s = static_cast<MosquittoESPWeb *>(server);
      LOG("/set, args=%d\n",s->args());
      int i,l;
      if(!s->authenticate(s->_www_username, s->_www_password))
                        return s->requestAuthentication();
      // write string to EEPROM
      if(s->args()==1){
        LOG("/set, argName[0](%s) = arg[0]=%s\n",s->argName(0).c_str(),s->arg(0).c_str());
        if(strcmp(s->argName(0).c_str(),"env")==0){
          l=strlen(s->arg(0).c_str());
          if(l<s->_MaxEEPROM-1){              
            for(i=0; i<l;i++){
              EEPROM.write(i,s->arg(0).c_str()[i]);
            }
          }
          EEPROM.write(i,0);
        }
        EEPROM.commit();
      }
      s->send(200, "text/plain", "set OK");
    }); 
    this->on("/get", [](void* server){
      LOG("/get\n");
      MosquittoESPWeb* s = static_cast<MosquittoESPWeb *>(server);
      char buf[s->_MaxEEPROM];
      if(!s->authenticate(s->_www_username, s->_www_password))
                        return s->requestAuthentication();        
      // read string from EEPROM
      for(int i=0; i<s->_MaxEEPROM;i++){
        buf[i]=EEPROM.read(i);
        if(buf[i]==0) break;
      }
      s->send(200, "text/plain", buf);
    });    
    this->begin();
    LOG("Open http://");
    LOG("%d.%d.%d.%d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);
    LOG("/ in your browser to see it working");
  }
}
MosquittoESPWeb::~MosquittoESPWeb(){
    LOG("server stopped\n"); 
}

