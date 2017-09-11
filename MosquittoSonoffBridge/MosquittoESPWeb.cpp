#include "MosquittoESPWeb.h"


MosquittoESPWeb::MosquittoESPWeb(int MaxEEPROM,char *www_username, char*www_password, int port):ESP8266xWebServer(port){
    _MaxEEPROM=MaxEEPROM;
    _www_username=www_username;
    _www_password=www_password;
    /*  / */
    this->on("/", [](void* server){
      LOG_WEB("/\n");
      MosquittoESPWeb* s = static_cast<MosquittoESPWeb *>(server);
      if(!s->authenticate(s->_www_username, s->_www_password))
                        return s->requestAuthentication();
      char buf[128];
      snprintf(buf,128,"Login OK (%s)",build_str);
      s->send(200, "text/plain", buf);
    });
    /*  erase */
    this->on("/erase", [](void* server){
      LOG_WEB("/\n");
      MosquittoESPWeb* s = static_cast<MosquittoESPWeb *>(server);
      if(!s->authenticate(s->_www_username, s->_www_password))
                        return s->requestAuthentication();
      for(int i=0; i<s->_MaxEEPROM;i++){
          mEEPROM.write(i,0x00);
      }
      mEEPROM.commit();                        
      s->send(200, "text/plain", "Erased OK");
    });
    /*  set */
    this->on("/set", [](void* server){
      MosquittoESPWeb* s = static_cast<MosquittoESPWeb *>(server);
      LOG_WEB("/set, args=%d\n",s->args());
      int i,l;
      if(!s->authenticate(s->_www_username, s->_www_password))
                        return s->requestAuthentication();
      // write string to EEPROM
      if(s->args()==1){
        LOG_WEB("/set, argName[0](%s) = arg[0]=%s\n",s->argName(0).c_str(),s->arg(0).c_str());
        if(strcmp(s->argName(0).c_str(),"env")==0){
          // verify it has all the necessary parameters
          if(env::verify(s->arg(0))){
            l=strlen(s->arg(0).c_str());
            if(l<s->_MaxEEPROM-3){
              mEEPROM.write(0,0xAA);
              mEEPROM.write(1,0x55);  
              LOG_WEB("%02x:%02x, l=%d",mEEPROM.read(0),mEEPROM.read(1),l);            
              for(i=0; i<l;i++){
                mEEPROM.write(i+2,s->arg(0).c_str()[i]);
                LOG_WEB("%c",mEEPROM.read(i+2));
              }
              mEEPROM.write(i+2,0);
              LOG_WEB("\n");
              mEEPROM.commit();
            }
          }          
        }       
      }
      s->send(200, "text/plain", "set OK");
    });
    /* get */
    this->on("/get", [](void* server){
      LOG_WEB("/get\n");
      MosquittoESPWeb* s = static_cast<MosquittoESPWeb *>(server);
      char buf[s->_MaxEEPROM];
      if(!s->authenticate(s->_www_username, s->_www_password))
                        return s->requestAuthentication();        
      // read string from mEEPROM      
      LOG_WEB("_MaxEEPROM=%d\n",s->_MaxEEPROM);
      uint8_t *ptr = mEEPROM.getDataPtrRO();
      LOG_WEB("%02x:%02x\n",*ptr,*(ptr+1));
      LOG_WEB("mEEPROM=%s\n",ptr+2);
      buf[0]=0x00;
      // if((mEEPROM.read(0)==0xAA)&&(mEEPROM.read(1)==0x55)){
        for(int i=2; i<s->_MaxEEPROM-2;i++){
          buf[i-2]=mEEPROM.read(i);
          LOG_WEB("%c",buf[i-2]);
          if(buf[i-2]==0) break;
        }
        LOG_WEB("\n");
      // }
      if(strlen(buf)==0){
        s->send(200, "text/plain", "empty");
      } else{
        s->send(200, "text/plain", buf);
      }      
    });    
    this->begin();
    LOG_WEB("Open http://");
    LOG_WEB("%d.%d.%d.%d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);
    LOG_WEB("/ in your browser to see it working\n");
}
MosquittoESPWeb::~MosquittoESPWeb(){
    LOG_WEB("server stopped\n"); 
}
