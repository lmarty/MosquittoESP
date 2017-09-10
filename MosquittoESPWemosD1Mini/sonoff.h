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
 * #include <Arduino.h>
 * sonoff sn;
 * 
 * setup(){
 *    Serial.Begin(19200);
 * }
 * 
 * loop(){
 *    if(sn.MCUMessages()==4){
 *      // key received
 *    }
 *    delay(100);
 * }
 * 
 */
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
      int handshake;
      uint16_t Tsyn;
      uint16_t Tlow;
      uint16_t Thigh;
      uint32_t key;
  private:
      /* structure to receive learned keys or forwarded keys */
      int MCUrequestState;     
      void sendAction(char a);
};
#endif /* sonoff_H_ */
