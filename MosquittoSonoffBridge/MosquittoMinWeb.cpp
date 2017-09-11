#include "MosquittoMinWeb.h"


MosquittoMinWeb::MosquittoMinWeb(int MaxEEPROM,char *www_username, char*www_password, int port):ESP8266xWebServer(port){
    _MaxEEPROM=MaxEEPROM;
    _www_username=www_username;
    _www_password=www_password;
    /*  / */
    this->on("/", [](void* server){
      LOG_WEB("/\n");
      MosquittoMinWeb* s = static_cast<MosquittoMinWeb *>(server);
      if(!s->authenticate(s->_www_username, s->_www_password))
                        return s->requestAuthentication();
      s->send(200, "text/plain", "Login OK");
    });
    /*  erase */
    this->on("/erase", [](void* server){
      LOG_WEB("/\n");
      MosquittoMinWeb* s = static_cast<MosquittoMinWeb *>(server);
      mEEPROM.begin(s->_MaxEEPROM);
      if(!s->authenticate(s->_www_username, s->_www_password))
                        return s->requestAuthentication();
      for(int i=0; i<s->_MaxEEPROM;i++){
          mEEPROM.write(i,0x00);
      }
      mEEPROM.end();                        
      s->send(200, "text/plain", "Erased OK");
    });
    this->begin();
    LOG_WEB("Open http://");
    LOG_WEB("%d.%d.%d.%d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);
    LOG_WEB("/ in your browser to see it working\n");
}
