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

#include <Arduino.h>
#include <IRsend.h>
#include <IRremoteESP8266.h>
#include <IRutils.h>

const uint16_t kIrLedPin = 14;


// GPIO to use to control the IR LED circuit. Recommended: 4 (D2).


// As this program is a special purpose capture/resender, let's use a larger
// than expected buffer so we can handle very large IR messages.
// i.e. Up to 512 bits.
const uint16_t kCaptureBufferSize = 1024;

// kTimeout is the Nr. of milli-Seconds of no-more-data before we consider a
// message ended.
const uint8_t kTimeout = 50;  // Milli-Seconds

// kFrequency is the modulation frequency all messages will be replayed at.
const uint16_t kFrequency = 38000;  // in Hz. e.g. 38kHz.

// The IR transmitter.
IRsend irsend(kIrLedPin);

decode_results results;
// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {

    uint32_t b;
    //uint16_t c;

} struct_message;

// Create a struct_message called myData
struct_message myData;

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&myData, incomingData, sizeof(myData));
 uint32_t klaidi= myData.b;
Serial.println(klaidi,HEX);
   irsend.sendRC5(klaidi,12,1);
               

}
 
void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  irsend.begin();       // Start up the IR sender.
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  
}
