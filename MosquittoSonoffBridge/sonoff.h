/*
 *  -> 8266 sends
 *  <- 8266 receives
 * 
 * Learning Key (0xA1)          learning action (8266 request)
 * 8266 -> 0xAA 0xA1 0x55
 * MCU  <- 0xAA 0xA0 0x55
 * 
 * Learning Timeout (0xA2)      timeout exits (MCU request)
 * MCU  <- 0xAA 0xA2 0x55
 * 8266 -> 0xAA 0xA0 0x55
 * 
 * Learning Success (0xA3)      learning success  (MCU request)
 * MCU  <- 0XAA 0XA3 Tsync Tlow Thigh 24bit data 0x55
 * 8266 -> 0xAA 0xA0 0x55
 * 
 * Receive Key Value (0xA4)     forward RF Remote key value (MCU request)
 * MCU  <- 0XAA 0XA4 Tsync Tlow Thigh 24bit data 0x55
 * 8266 -> 0xAA 0xA0 0x55
 * 
 * Transmit Key Value (0XA5)      transmit key value (8266 request)
 * 8266 -> 0XAA 0XA5 Tsync Tlow Thigh 24bit data 0x55
 * MCU  <- 0xAA 0xA0 0x55
 * 
 * 
 * Tsyn: uint16 type, it means code synchronization time, unit: us, MSB order (high first send)
 * Tlow: uint16 type, it means low level time, unit: us, MSB order (high first send)
 * Thigh: uint16 type, it means high level time, unit: us, MSB order (high first send)
 * 
 */
#define LOG_RF(format,...) console(format, __VA_ARGS__)
void console(char *,...);
#include <Arduino.h>
#include <stdint.h>

#ifndef sonoff_H_
#define sonoff_H_
class sonoff {
  public:
      sonoff();
      ~sonoff();
      void LearningKey();  //(0xA1)
      void TransmitKey(uint16_t Tsyn, uint16_t Tlow, uint16_t Thigh,uint32_t key );  //(0XA5)
      int MCUMessages();
      void sendAck();
      uint16_t Tsyn;
      uint16_t Tlow;
      uint16_t Thigh;
      uint32_t key;
  private:
      /* structure to receive learned keys or forwarded keys */
      int MCUrequestState;     
      void sendAction(char a);
      uint8_t buffer[12];
      int counter;
      int length;
      int last;
};
#endif /* sonoff_H_ */

/* **** Example which copy received key on telenet connected clients if any ****/
/*
#include <ESP8266WiFi.h>

#include "sonoff.h"

//how many clients should be able to telnet to this ESP8266
#define MAX_SRV_CLIENTS 1
const char* ssid = "NETGEAR28";
const char* password = "vanillariver618";

WiFiServer server(23);
WiFiClient serverClients[MAX_SRV_CLIENTS];

int count;

sonoff sn;

void setup() {
  count=0;
  Serial1.begin(115200);
  WiFi.begin(ssid, password);
  Serial1.print("\nConnecting to "); Serial1.println(ssid);
  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED && i++ < 20) delay(500);
  if(i == 21){
    Serial1.print("Could not connect to"); Serial1.println(ssid);
    while(1) delay(500);
  }
  //start UART and the server
  Serial.begin(19200);
  server.begin();
  server.setNoDelay(true);
  
  Serial1.print("Ready! Use 'telnet ");
  Serial1.print(WiFi.localIP());
  Serial1.println(" 23' to connect");
}

void console(char *format, ...){
  char buf[512];
  va_list args;
  va_start(args, format);
  vsnprintf(buf,512,format,args);
  va_end(args);
  size_t len = strlen(buf);
  for(int i = 0; i < MAX_SRV_CLIENTS; i++){
      if (serverClients[i] && serverClients[i].connected()){
        serverClients[i].write((uint8_t*)buf, len);
        delay(1);
      }
  }
}

void loop() {
  uint8_t i;
  //check if there are any new clients
  if (server.hasClient()){
    for(i = 0; i < MAX_SRV_CLIENTS; i++){
      //find free/disconnected spot
      if (!serverClients[i] || !serverClients[i].connected()){
        if(serverClients[i]) serverClients[i].stop();
        serverClients[i] = server.available();
        Serial1.print("New client: "); Serial1.print(i);
        continue;
      }
    }
    //no free/disconnected spot so reject
    WiFiClient serverClient = server.available();
    serverClient.stop();
  }
  //check clients for data
  for(i = 0; i < MAX_SRV_CLIENTS; i++){
    if (serverClients[i] && serverClients[i].connected()){
      if(serverClients[i].available()){
        //get data from the telnet client and push it to the UART
        while(serverClients[i].available()) Serial.write(serverClients[i].read());
      }
    }
  }

  int r = sn.MCUMessages();
  if((r!=0)&(r!=-1)){
    if((r==4)||(r==5)){
        // key received
        char tmp[512];
        snprintf(tmp,512,"Tsyn(%d),Tlow(%d),Thigh(%d),key(%d)\r\n",sn.Tsyn, sn.Tlow, sn.Thigh, sn.key);
        size_t len = strlen(tmp);
        for(i = 0; i < MAX_SRV_CLIENTS; i++){
          if (serverClients[i] && serverClients[i].connected()){
            serverClients[i].write((uint8_t*)tmp, len);
            delay(1);
          }
        }
        // send Ack message...
        sn.sendAck();        
    }
  }

  if(r==-1){  // flush any incoming data we take one key at a time
    //check UART for data
    if(Serial.available()){
      char tmp[4];
      snprintf(tmp,4,"%02x ",Serial.read());
      size_t len = strlen(tmp); // Serial.available();
      // uint8_t sbuf[len];
      // Serial.readBytes(sbuf, len);
      //push UART data to all connected telnet clients
      for(i = 0; i < MAX_SRV_CLIENTS; i++){
        if (serverClients[i] && serverClients[i].connected()){
          serverClients[i].write((uint8_t*)tmp, len);
          delay(1);
        }
      }
      count++;
      if (count>=80){
        count=0;
        serverClients[i].write((uint8_t*)"\r\n", 2);
      }
    }
  }
  
}

*/
