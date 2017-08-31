/*
 * WebSocketClientSSL.ino
 *
 *  Created on: 10.12.2015
 *
 *  note SSL is only possible with the ESP8266
 *
 */
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

WebSocketsClient webSocket;
PubSubClient client(webSocket);

#define STA_SSID "NETGEAR28"
#define STA_PASS "vanillariver618"

#define USE_SERIAL Serial

// ESP8266WebServer server(80);

const char* www_username = "admin";
const char* www_password = "esp8266";

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {


    switch(type) {
        case WStype_DISCONNECTED:
            USE_SERIAL.printf("[WSc] Disconnected!\n");
            WiFi.mode(WIFI_OFF);
            break;
        case WStype_CONNECTED:
            {
                USE_SERIAL.printf("[WSc] Connected to url: %s\n",  payload);
				
			          // send message to server when Connected
				        // webSocket.sendTXT("Connected");
                  Serial.printf("Connecting to mqtt now...");
                  int r = random(10000, 2000000);
                  char buf[50];
                  snprintf(buf,50,"ESP8266Client.%d",r);
                 client.connect(buf,"gbrault","gb080556");
            }
            break;
        case WStype_TEXT:
            USE_SERIAL.printf("[WSc] get text: %s\n", payload);

			// send message to server
			// webSocket.sendTXT("message here");
            break;
        case WStype_BIN:
            USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
            hexdump(payload, length);
            webSocket.setBuffer(payload, length);
            if(client.state()==MQTT_CONNECTING){
              int res = client.connectRead(); 
              if(res==1){
                // connected!
                Serial.println("connected to MQTT...");                
              } else {
                  Serial.print("failed, rc=");
                  Serial.print(client.state());
                  Serial.println(" try again in 5 seconds");
                  // Wait 5 seconds before retrying
                  delay(5000);                
              }
            } else {
              client.process();  
            }            
            break;
    }

}

void getTime(){
   Serial.printf("epoch=%d\n",sntp_get_current_timestamp());
}


void setup() {

  pinMode(2, OUTPUT);
  myState = 0;
  last=millis();
    // USE_SERIAL.begin(921600);
    USE_SERIAL.begin(115200);

    //Serial.setDebugOutput(true);
    USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

      for(uint8_t t = 4; t > 0; t--) {
          USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
          USE_SERIAL.flush();
          delay(1000);
      }
  uint32_t free = system_get_free_heap_size();
  Serial.printf("free mem = %d\n");

    Serial.println("WIFI Off\n");
    WiFi.mode(WIFI_OFF);
    delay(10000);

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
          WiFi.forceSleepBegin(); ESP.restart();
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
    

    webSocket.setAuthorization("gbrault","gb080556");
    //https://www.grc.com/fingerprints.htm
    webSocket.beginSSL("nupc.org", 443, "/mqtt","C0 C7 EE F9 99 FD 51 E3 6B 13 B4 BC C4 89 59 CD B7 86 66 EA","mqtt");
    webSocket.onEvent(webSocketEvent);

    client.setCallback(callback);

}

void callback(char* topic, byte* payload, unsigned int length) {
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
    digitalWrite(2, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(2, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}


void loop() {
    if(WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected!");
        setup();
    } else {
      webSocket.loop();
      client.loop();
      if(client.state()==MQTT_CONNECTED){
        if(myState==0){
          // Once connected, publish an announcement... only once
          client.publish("Home/test1", "{\"username\":\"gbrault\",\"payload\":1}");
          // ... and resubscribe
          client.subscribe("Home/test1");   
          myState=1;
        }
      }
      unsigned long t = millis();
      if ((t - last > 30*1000UL)){
          char buf[256];
          snprintf(buf,256,"{\"username\":\"gbrault\",\"payload\":%d}",sntp_get_current_timestamp());
          client.publish("Home/time", buf);
          last = t;
          uint32_t free = system_get_free_heap_size();
          Serial.printf("free mem = %d\n");
      }
      // server.handleClient();
    }
}
