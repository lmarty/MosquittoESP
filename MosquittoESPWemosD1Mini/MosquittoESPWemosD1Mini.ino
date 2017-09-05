/*
 * WebSocketClientSSL.ino
 *
 *  Created on: 10.12.2015
 *
 *  note SSL is only possible with the ESP8266
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
#include <Arduino.h>
#include <NtpClientLib.h>

#include <ESP8266WiFi.h>

// #include <ESP8266WebServer.h>

#define DEBUG_ESP_PORT Serial;

#include "WebSocketsClient.h"
#include "PubSubClient.h"
#include <Hash.h>
#include <Ticker.h>
#include <ArduinoJson.h>

Ticker showtime;

int myState;
unsigned long last;
unsigned long first;

WebSocketsClient *webSocket;
PubSubClient *client;

#define STA_SSID "NETGEAR28"
#define STA_PASS "vanillariver618"

#define USE_SERIAL Serial

const char* www_username = "admin";
const char* www_password = "esp8266";

void getTime(){
   Serial.printf("epoch=%d\n",sntp_get_current_timestamp());
}


void setup() {

  pinMode(led, OUTPUT);
  myState = 0;
  last=millis();
  USE_SERIAL.begin(115200);
  USE_SERIAL.setDebugOutput(true);
  uint32_t free = system_get_free_heap_size();
  Serial.printf("free mem = %d\n");

    Serial.println("WIFI Off\n");
    WiFi.mode(WIFI_OFF);
    delay(100);

    WiFi.mode(WIFI_STA);
    WiFi.begin(STA_SSID, STA_PASS);

    Serial.println();
    Serial.println();
    Serial.print("Wait for WiFi... ");

    int count=0;
    while(WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
        count++;
        if(count>30){
          // switch to AP...
        }
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

/*
    server.on("/", [](){
      if(!server.authenticate(www_username, www_password))
        return server.requestAuthentication();
      server.send(200, "text/plain", "Login OK");
    });
    server.begin();
    */

    Serial.print("Open http://");
    Serial.print(WiFi.localIP());
    Serial.println("/ in your browser to see it working");

    delay(500);

    showtime.attach(60,getTime);
    sntp_stop();
    sntp_setservername(0,"pool.ntp.org");
    sntp_init();  
    sntp_stop();
    sntp_set_timezone(0);
    sntp_init();   

    webSocket=NULL;
    client=NULL;

    setWSMQTT();

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
    if(WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected!");
        delay(5000); // let it recover!!!
    } else {
      if(webSocket && client){
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
                uint32_t free = system_get_free_heap_size();
                Serial.printf("free mem = %d\n");
            }
          }
        }      
      }
      // server.handleClient();
    }
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    // payload not garanteed to be valid after this call...
    // this is called by webSocket.loop()...
    switch(type) {
        case WStype_DISCONNECTED:
            USE_SERIAL.printf("[WSc] Disconnected!\n");
            delete(client);
            setWSMQTT();
            myState=0;  // reset first MQTT activity
            first=last=millis();  // reset time publishing
            break;
        case WStype_CONNECTED:
            {
                USE_SERIAL.printf("[WSc] Connected to url: %s\n",  payload);
                Serial.printf("Connecting to mqtt now...");
                int r = random(10000, 2000000);
                char buf[50];
                snprintf(buf,50,"ESP8266Client.%d",r);
                while(client->connect(buf,"gbrault","gb080556")){
                  Serial.printf("Not able to connect to MQTT yet...");
                  // need to do some client connect again...
                  delay(100);
                }
            }
            break;
        case WStype_TEXT:
            USE_SERIAL.printf("[WSc] get text: %s\n", payload);
            // not used by this application
            break;
        case WStype_BIN:
            USE_SERIAL.printf("[WSc] Read_BIN get binary length: %u\n", length);
            hexdump(payload, length);
            
            webSocket->setBuffer(payload, length);  // to prepare reading from MQTT
            if(client->state()==MQTT_CONNECTING){
              int res;
              while(res = client->connectRead()!=2){ 
                  if(res==1){
                    // connected!
                    webSocket->printType(payload[0]&0xF0);
                    Serial.println("connected to MQTT...");                
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
  // payload not valid after this call
  // this is called by client.process() ... which will be called by webSocket.loop()
  DynamicJsonBuffer jsonBuffer;
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  JsonObject& root = jsonBuffer.parseObject(payload);
  String value = root["payload"];
  Serial.print(value.c_str()[0]);
  Serial.println();
  // Switch on the LED if an 1 was received as first character
  if (value.c_str()[0] == '1') {
    digitalWrite(led, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(led, HIGH);  // Turn the LED off by making the voltage HIGH
  }

  uint32_t free = system_get_free_heap_size();
  Serial.printf("free mem = %d\n");
}

