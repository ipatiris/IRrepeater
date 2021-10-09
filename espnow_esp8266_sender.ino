/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-esp8266-nodemcu-arduino-ide/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <ESP8266WiFi.h>
#include <espnow.h>
#include <IRsend.h>
#include <Arduino.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRutils.h>
const uint16_t kIrLedPin = 4;
const uint16_t kRecvPin = 13;

const uint16_t kCaptureBufferSize = 1024;
IRsend irsend(kIrLedPin);
// kTimeout is the Nr. of milli-Seconds of no-more-data before we consider a
// message ended.
const uint8_t kTimeout = 50;  // Milli-Seconds

// kFrequency is the modulation frequency all messages will be replayed at.
const uint16_t kFrequency = 38000;  // in Hz. e.g. 38kHz.

// The IR receiver.
IRrecv irrecv(kRecvPin, kCaptureBufferSize, kTimeout, false);
// Somewhere to store the captured message.
decode_results results;

// REPLACE WITH RECEIVER MAC Address
uint8_t broadcastAddress[] = {0x18, 0xFE, 0x34, 0xE1, 0x9D, 0x9B};
// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  
  uint32_t b;
  //uint16_t c;

} struct_message;

// Create a struct_message called myData
struct_message myData;

unsigned long lastTime = 0;  
unsigned long timerDelay = 2000;  // send readings timer

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  irrecv.enableIRIn();  // Start up the IR receiver.
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}
 
void loop() {

 if (irrecv.decode(&results)) {  // We have captured something.

    uint32_t k=results.value;
    myData.b=k;
//    // Send message via ESP-NOW
    esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(results));
    delay(50);
    irrecv.resume();
  
  }
    // Set values to send
    



   
    
  
}
