/*
 * MosquittoESPWemosD1Mini.ino
 *
 *  Created on: 06.09.2017
 *
 *  Websocket MQTT SSL
 *
 */
#include "context.h"
/* for WEMOS D1 Mini */
#define led 2
#define button 16
/* for WEMOS D1 Mini */
 
extern "C" {
#include "user_interface.h"
}
#include <umm_malloc/umm_malloc.h>
#include <Arduino.h>
#include <NtpClientLib.h>
#include "mEEPROM.h"

#include <ESP8266WiFi.h>

#include "WebSocketsClient.h"
#include "PubSubClient.h"
#include <Hash.h>
// #include <Ticker.h>
#include <ArduinoJson.h>
#include "MosquittoESPWeb.h"
#include "env.h"

WebSocketsClient *webSocket;
PubSubClient *client;

// Ticker showtime;

int myState;
unsigned long last;
unsigned long first;
int wifi_mode;
#define eeprom 512

MosquittoESPWeb *server;
env *oenv; 

/*
void getTime(){
   LOG("epoch=%d\n",sntp_get_current_timestamp());
}
*/

void printMem(){
  size_t size = umm_free_heap_size();
  uint32_t free = system_get_free_heap_size();
  LOG("free mem = %d -- %d\n",free,size);
  console("free mem = %d -- %d\n",free,size);
}

void console(char *format, ...){
  char buf[MQTT_MAX_PACKET_SIZE+1];
  va_list args;
  va_start(args, format);
  vsnprintf(buf,MQTT_MAX_PACKET_SIZE,format,args);
  va_end(args);
  unsigned long t = millis();
  if(webSocket && client){
    if(client &&  (client->state()==MQTT_CONNECTED) && (t - first > 30*1000UL)){    
        client->publish("Home/console",buf);
    }
  }
}


void setup() {

  mEEPROM.begin(eeprom);

  Serial.begin(115200);
  Serial.setDebugOutput(true);
  // Serial.setDebugOutput(false);

  LOG_SERIAL("\n");
  LOG_SERIAL("\n");
  LOG_SERIAL("\n");
  LOG_SERIAL("\n");
  delay(5000);
  
  uint8_t *ptr = mEEPROM.getDataPtrRO();
  LOG("%02x:%02x\n",*ptr,*(ptr+1));
  LOG("EEPROM=%s\n",ptr+2);
  oenv = new env();

  
  wifi_mode=0;   // no WIFI
  pinMode(led, OUTPUT);
  myState = 0;
  last=millis();
  client=NULL;
  webSocket=NULL;
  server=NULL;
 
    printMem();
    LOG_SERIAL("WIFI Off\n");
    WiFi.disconnect();  
    WiFi.mode(WIFI_OFF); 
    WiFi.disconnect();  
    delay(500);

  if(strcmp(oenv->CWIFISSID,"xxxxx")!=0){
    WiFi.mode(WIFI_STA);
    // WiFi.disconnect();  
    delay(500);
    WiFi.begin(oenv->CWIFISSID, oenv->CWIFIPSWD);

    LOG_SERIAL("\n");
    LOG_SERIAL("\n");
    LOG_SERIAL("Wait for WiFi STA mode... ");

    int count=0;
    while(WiFi.status() != WL_CONNECTED) {
        LOG_SERIAL(".");
        delay(500);
        count++;
        if(count>30){
          // switch to AP...
          WiFi.mode(WIFI_AP);          
          WiFi.softAPdisconnect(true);
          delay(500);
          LOG_SERIAL("AP SSID(%s),PWD(%s)\n",oenv->WIFISSID, oenv->WIFIPSWD);
          WiFi.softAP(oenv->WIFISSID, oenv->WIFIPSWD);
          wifi_mode=2;
          server = new MosquittoESPWeb(eeprom,oenv->WIFISSID, oenv->WIFIPSWD,80);      
          printMem();
          break;
        }
    }
  } else{
    // switch to AP...
    WiFi.mode(WIFI_AP);          
    WiFi.softAPdisconnect(true);
    delay(500);
    LOG_SERIAL("AP SSID(%s),PWD(%s)\n",oenv->WIFISSID, oenv->WIFIPSWD);
    WiFi.softAP(oenv->WIFISSID, oenv->WIFIPSWD);
    wifi_mode=2;
    server = new MosquittoESPWeb(eeprom,oenv->WIFISSID, oenv->WIFIPSWD,80);      
    printMem();
  }

    if(wifi_mode==0) wifi_mode=1; // STA Mode

    LOG_SERIAL("\n");
    LOG_SERIAL("WiFi connected\n");
    LOG_SERIAL("IP address: \n");
    LOG_SERIAL("%d:%d:%d:%d",WiFi.localIP()[0],WiFi.localIP()[1],WiFi.localIP()[2],WiFi.localIP()[3]);

if(wifi_mode==1){
    // showtime.attach(60,getTime);
    sntp_stop();
    sntp_setservername(0,"pool.ntp.org");
    sntp_init();  
    sntp_stop();
    sntp_set_timezone(0);
    sntp_init();
    setWSMQTT();
    printMem();
    mEEPROM.end();
}
}

void setWSMQTT(){
    webSocket = new WebSocketsClient();
  
    if(webSocket){
      webSocket->setAuthorization("gbrault","gb080556");
      //https://www.grc.com/fingerprints.htm
      webSocket->beginSSL("nupc.org", 443, "/mqtt","C0 C7 EE F9 99 FD 51 E3 6B 13 B4 BC C4 89 59 CD B7 86 66 EA","mqtt");
      webSocket->onEvent(webSocketEvent);
 
      client = new PubSubClient(webSocket);
      if(client){
        client->setCallback(callback);
 
      } else{
        LOG("could not create PubSubClient!!!\n");
      }
    } else {
      LOG("could not create WebSocketsClient!!!\n");
    }
}

void loop() {
    if((wifi_mode!=2)&&(WiFi.status() != WL_CONNECTED)) {
        LOG_SERIAL("WiFi not connected!\n");
        delay(5000); // let it recover!!!
    } else {
        if(wifi_mode!=2){
          if( webSocket && client){
            webSocket->loop();   // will call webSocketEvent depending on state  ... handle client reconnection ...
            client->loop();      // just maintain the MQTTPINGREQ watchdog
            if(client->state()==MQTT_CONNECTED){
              if(myState==0){
                // Once connected, publish an announcement... only once
                client->publish("Home/test1", "{\"username\":\"gbrault\",\"payload\":1}");
                // ... and resubscribe
                client->subscribe("Home/test1");   
                myState=1;
              } else {
                unsigned long t = millis();
                if ((t - last > 5*1000UL)&&(t - first > 30*1000UL)){
                    char buf[256];
                    snprintf(buf,256,"{\"username\":\"gbrault\",\"payload\":%d}",sntp_get_current_timestamp());
                    client->publish("Home/time", buf);
                    last = t;
                    console("Topic(%s),=%s","Home/time",buf);
                    printMem();
                }
              }
            }      
          }
        } else {
              if(server){
                  server->handleClient();
              }          
        }
    }
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    // payload not garanteed to be valid after this call...
    // this is called by webSocket.loop()...
    switch(type) {
        case WStype_DISCONNECTED:
            LOG("[WSc] Disconnected!\n");
            delete(client);
            setWSMQTT();
            myState=0;  // reset first MQTT activity
            first=last=millis();  // reset time publishing
            break;
        case WStype_CONNECTED:
            {
                LOG("[WSc] Connected to url: %s\n",  payload);
                LOG("Connecting to mqtt now...");
                int r = random(10000, 2000000);
                char buf[50];
                snprintf(buf,50,"ESP8266Client.%d",r);
                while(client->connect(buf,oenv->MQTTNAME,oenv->MQTTPSWD)){
                  LOG("Not able to connect to MQTT yet...");
                  // need to do some client connect again...
                  delay(100);
                }
            }
            break;
        case WStype_TEXT:
            LOG("[WSc] get text: %s\n", payload);
            // not used by this application
            break;
        case WStype_BIN:
            LOG("[WSc] Read_BIN get binary length: %u\n", length);
            hexdump(payload, length);
            
            webSocket->setBuffer(payload, length);  // to prepare reading from MQTT
            if(client->state()==MQTT_CONNECTING){
              int res;
              while(res = client->connectRead()!=2){ 
                  if(res==1){
                    // connected!
                    webSocket->printType(payload[0]&0xF0);
                    LOG("connected to MQTT...");                
                    myState=0;  // reset first MQTT activity
                    first=last=millis(); // reset time publishing
                    // how to reset the MQTTPINGREQ?
                    client->resetPingRequest();
                    break;
                  }
                  delay(100);
              }
              if(res==2){
                Serial.println("failed to connected to MQTT...");    
                // TODO: what to do?   
              }
            } else {
              client->process();  
            }            
            break;
    }

}

void callback(char* topic, byte* payload, unsigned int length) {
  LOG("Message arrived [");
  LOG(topic);
  LOG("] ");
  for (int i = 0; i < length; i++) {
    LOG("%c",(char)payload[i]);
  }
  LOG("\n");
  unsigned long t = millis();
  // payload not valid after this call
  // this is called by client.process() ... which is called by webSocket.loop()
  if(strcmp(topic,"Home/test1")==0){
      const size_t bufferSize = JSON_OBJECT_SIZE(2) + 30;
      DynamicJsonBuffer jsonBuffer(bufferSize);
      
      JsonObject& root = jsonBuffer.parseObject(payload);
      int value = root["payload"];
      LOG("value=(%d)\n",value);
      // Switch on the LED if an 1 was received as first character
      if (value == 1) {
        digitalWrite(led, LOW);   // Turn the LED on (Note that LOW is the voltage level
        // but actually the LED is on; this is because
        // it is acive low on the ESP-01)
      } else {
        digitalWrite(led, HIGH);  // Turn the LED off by making the voltage HIGH
      }
  }
  printMem();
}

