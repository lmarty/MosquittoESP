#include "sonoff.h"
sonoff::sonoff(){
  MCUrequestState=0;
  handshake=0;
}
sonoff::~sonoff(){
  
}
void sonoff::sendAck()
{
  sendAction('0xA0');
}
void sonoff::sendAction(char a){
  Serial.write(0xAA);  // Start of Text
  Serial.write(a);  // action
  Serial.write(0x55);  // End of Text
}
void sonoff::LearningKey(){
  sendAction('0xA1');
}
/*
 * return 
 * 0: call again, 
 * -1:not an MCU message, 
 * 2: Learning Timeout, 
 * 3:Learning Success, Time & key available, 
 * 4:Receive Key Value, Time & key available
 */
int sonoff::MCUMessages(){
  int result=1;  
    while(result==1){ // till result==1, things to process...
      if(Serial.available()){
        switch(MCUrequestState){
          case 0 :
            if(Serial.peek()=='xAA'){
              Serial.read();
              MCUrequestState=1; // triggered an MCU request
            } else{
              MCUrequestState=0; // for next time
              result= -1;  // not an MCU request or an error
            }
            break;            
          case 1:
            switch(Serial.read()){
              case '0xA2':            // learning timeout                
                MCUrequestState=2;
                break;
              case '0xA3':            // learning success                
                 MCUrequestState=3;
                break;
              case '0xA4':            // Receive Key Value                
                MCUrequestState=4;
                break;
            }
            break;
          case 2: // Learning Timeout        
            if(Serial.peek()=='0x55'){
              Serial.read();
              MCUrequestState=0; // for next time
              result=2;
            }else{
              MCUrequestState=0; // for next time
              result= -1;  // not an MCU request or an error
            }
            break;
          case 3: // Learning Success 1st step
            // need now to read Tsync, Tlow 24bit data and 0x55
            this->Tsyn=Serial.read()<<8;
            MCUrequestState=30;
            break;
          case 30:// Learning Success 2nd step
            this->Tsyn=this->Tsyn&Serial.read();
            MCUrequestState=31;
            break;
          case 31:
            this->Tlow=Serial.read()<<8;
            MCUrequestState=32;
            break;
          case 32:
            this->Tlow=this->Tlow&Serial.read();
            MCUrequestState=33;
            break;
          case 33:
            this->Thigh=Serial.read()<<8;
            MCUrequestState=34;
            break;
          case 34:
            this->Thigh=this->Thigh&Serial.read();
            MCUrequestState=35;
            break;
          case 35:
            this->Thigh=Serial.read()<<16;
            MCUrequestState=36;
            break;
          case 36:
            this->Thigh=this->Thigh&(Serial.read()<<8);
            MCUrequestState=37;
            break;
          case 37:
            this->Thigh=this->Thigh&Serial.read();
            MCUrequestState=38;
            break;
          case 38:
            if(Serial.peek()=='0x55'){
              Serial.read();
              MCUrequestState=0; // for next time
              result=3;
            }else{
              MCUrequestState=0; // for next time
              result= -1;  // not an MCU request or an error
            }
           break;
          case 4: //forward RF Remote key value
            this->Tsyn=Serial.read()<<8;
            MCUrequestState=40;
            break;
          case 40:// Learning Success 2nd step
            this->Tsyn=this->Tsyn&Serial.read();
            MCUrequestState=41;
            break;
          case 41:
            this->Tlow=Serial.read()<<8;
            MCUrequestState=42;
            break;
          case 42:
            this->Tlow=this->Tlow&Serial.read();
            MCUrequestState=43;
            break;
          case 43:
            this->Thigh=Serial.read()<<8;
            MCUrequestState=44;
            break;
          case 44:
            this->Thigh=this->Thigh&Serial.read();
            MCUrequestState=45;
            break;
          case 45:
            this->Thigh=Serial.read()<<16;
            MCUrequestState=46;
            break;
          case 46:
            this->Thigh=this->Thigh&(Serial.read()<<8);
            MCUrequestState=47;
            break;
          case 47:
            this->Thigh=this->Thigh&Serial.read();
            MCUrequestState=48;
            break;
          case 48:
            if(Serial.peek()=='0x55'){
              Serial.read();
              MCUrequestState=0; // for next time
              result=4;
            }else{
              MCUrequestState=0; // for next time
              result= -1;  // not an MCU request or an error
            }
           break;
         }
      }
      else {
        return 0; // need to loop again, responsibility of the caller
      }
  }
}
void sonoff::TransmitKey(uint16_t Tsyn, uint16_t Tlow, uint16_t Thigh,uint32_t key ){ 
  Serial.write(0xAA);  // Start of Text
  Serial.write('0XA5');  // TransmitKey
  Serial.write((char)((Tsyn&0xFF00)>>8));
  Serial.write((char)(Tsyn&0x00FF));
  Serial.write((char)(Tlow&0xFF00)>>8);
  Serial.write((char)(Tlow&0x00FF));
  Serial.write((char)(Thigh&0xFF00)>>8);
  Serial.write((char)(Thigh&0x00FF));
  Serial.write((char)((key&0x00FF0000)>>16));
  Serial.write((char)((key&0x0000FF00)>>8));
  Serial.write((char)((key&0x000000FF)));
  Serial.write(0x55);  // End of Text  
}

