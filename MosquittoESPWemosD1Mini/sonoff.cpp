#include "sonoff.h"
sonoff::sonoff(){
  MCUrequestState=0;
  counter=0;
  length=0;
  last=millis();
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
 * 0: need to loop
 * -1: error
 */
int sonoff::MCUMessages(){
  int result=0;
  if(MCUrequestState==0){
    last = millis();
  }  
  while((result==0)&&(Serial.available())&&(millis()-last < 10*1000UL)){
    switch(MCUrequestState){
      case 0:  // message border trigger
        {
          uint8_t c = Serial.read();
          LOG_RF("%02x : ",c);
          if(c==0xAA){
            buffer[counter]=c;
            counter++;
            MCUrequestState=1;
            last=millis();           
          }       
        }
        break;
      case 1:
        {
          uint8_t c = Serial.read();
          LOG_RF("%02x ",c);
          buffer[counter]=c;
          counter++;
          switch(c){
            case 0xA0: //return action
              length=3;
              MCUrequestState=2;
              break;
            case 0xA2: //timeout exits
              length=3;
              MCUrequestState=3;
              break;
            case 0xA3: // learning success
              length=12;
              MCUrequestState=4;
              break;
            case 0xA4: // forward RF Remote key value
             length=12;
             MCUrequestState=5;
             break;
            default:
              MCUrequestState=0;
              counter=0;
              length=0;
              result = -1; // error   
              break;        
          }
        }
        break;
      case 2:
      case 3:
      case 4:  
      case 5: // read Serial into buffer according to count up to length
        {          
          uint8_t c = Serial.read();
          LOG_RF("%02x ",c);
          buffer[counter]=c;
          counter++;
          if(counter==length){ 
            result = -1;      
            switch(MCUrequestState){
                case 2:
                case 3:
                  if(buffer[2]==0x55){
                    result = MCUrequestState;
                  }
                  break;
                case 4:
                case 5:
                  if(buffer[11]==0x55){
                    this->Tsyn = buffer[2]<<8|buffer[3];
                    this->Tlow = buffer[4]<<8|buffer[5];
                    this->Thigh= buffer[6]<<8|buffer[7];
                    this->key = buffer[8]<<16|buffer[9]<<8|buffer[10];
                    result = MCUrequestState;
                  }
                  break;
            }
          }
        }
        break;
    }
  }
  if((MCUrequestState!=0)&&(millis()-last>10 *1000UL)){
    LOG_RF("timeout!!!%d",millis()-last);
    result=-1;
  }
  if(result!=0){
    // state to start again
    counter=0;
    length=0;
    MCUrequestState=0;                 
  }
  return result;
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

