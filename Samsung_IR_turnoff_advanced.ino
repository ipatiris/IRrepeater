/* IRremoteESP8266: IRsendDemo - demonstrates sending IR codes with IRsend.
 *
 * Version 1.1 January, 2019
 * Based on Ken Shirriff's IrsendDemo Version 0.1 July, 2009,
 * Copyright 2009 Ken Shirriff, http://arcfn.com
 *
 * An IR LED circuit *MUST* be connected to the ESP8266 on a pin
 * as specified by kIrLed below.
 *
 * TL;DR: The IR LED needs to be driven by a transistor for a good result.
 *
 * Suggested circuit:
 *     https://github.com/crankyoldgit/IRremoteESP8266/wiki#ir-sending
 *
 * Common mistakes & tips:
 *   * Don't just connect the IR LED directly to the pin, it won't
 *     have enough current to drive the IR LED effectively.
 *   * Make sure you have the IR LED polarity correct.
 *     See: https://learn.sparkfun.com/tutorials/polarity/diode-and-led-polarity
 *   * Typical digital camera/phones can be used to see if the IR LED is flashed.
 *     Replace the IR LED with a normal LED if you don't have a digital camera
 *     when debugging.
 *   * Avoid using the following pins unless you really know what you are doing:
 *     * Pin 0/D3: Can interfere with the boot/program mode & support circuits.
 *     * Pin 1/TX/TXD0: Any serial transmissions from the ESP8266 will interfere.
 *     * Pin 3/RX/RXD0: Any serial transmissions to the ESP8266 will interfere.
 *   * ESP-01 modules are tricky. We suggest you use a module with more GPIOs
 *     for your first time. e.g. ESP-12 etc.
 */

#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

#define sw 14 //pin at push button is connected
unsigned long myTime;

//Time that board is going to sleep
int sleeptime = 60000;//60 seconds
int lastButtonState = HIGH;   // the previous reading from the input pin
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
bool buttonState = HIGH; //saving state of the switch
byte tapCounter; //for saving no. of times the switch is pressed
int timediff; //for saving the time in between each press and release of the switch
bool flag1, flag2; //just two variables
long double presstime, releasetime; //for saving millis at press and millis at release

const int led= 12;
int en_ldo = 5;

const uint16_t kIrLed = 4;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).

IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.

// Example of data captured by IRrecvDumpV2.ino
uint16_t shutdowncommand[67] = {4592, 4456,  620, 1650,  598, 1648,  598, 1650,  598, 526,  598, 526,  598, 526,  598, 526,  598, 526,  598, 1628,  620, 1648,  598, 1650,  598, 526,  598, 526,  598, 526,  598, 526,  598, 526,  598, 526,  598, 1650,  598, 526,  598, 526,  598, 526,  598, 524,  598, 526,  598, 526,  598, 1650,  598, 526,  598, 1648,  600, 1648,  598, 1650,  598, 1650,  598, 1648,  598, 1650,  598};
uint16_t pausecommand[67] = {4618, 4432,  646, 1624,  624, 1624,  622, 1624,  624, 500,  622, 500,  624, 500,  624, 500,  624, 500,  624, 1626,  622, 1624,  624, 1624,  622, 500,  624, 500,  624, 500,  624, 500,  624, 500,  624, 500,  624, 1626,  624, 500,  624, 1626,  622, 500,  624, 500,  622, 1626,  624, 500,  624, 1626,  622, 500,  624, 1624,  624, 500,  624, 1626,  622, 1624,  624, 500,  624, 1626,  622};



void setup() {
  Serial.begin(115200);
  
  pinMode(sw, INPUT); //setting pin 5 as input with internal pull up resistor
  
  pinMode(en_ldo,OUTPUT);
  digitalWrite(en_ldo,HIGH);
  pinMode(led,OUTPUT);
  digitalWrite(led,LOW);
  
  irsend.begin();
  

}

void loop() {

  int reading = digitalRead(sw);

  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;
    }
  }
  //Serial.println(buttonState);

  //when switch is pressed
  if (buttonState == 0 && flag2 == 0)
  {
    presstime = millis(); //time from millis fn will save to presstime variable
    flag1 = 0;
    flag2 = 1;
    tapCounter++; //tap counter will increase by 1
    //delay(10); //for avoiding debouncing of the switch
  }
  //when sw is released
  if (buttonState == 1 && flag1 == 0)
  {
    releasetime = millis(); //time from millis fn will be saved to releasetime var
    flag1 = 1;
    flag2 = 0;

    timediff = releasetime - presstime; //here we find the time gap between press and release and stored to timediff var

  }

  if ((millis() - presstime) > 400 && buttonState == 1) //wait for some time and if sw is in release position
  {
    if (tapCounter == 1) //if tap counter is 1
    {
      if (timediff >= 400) //if time diff is larger than 400 then its a hold
      {
        Serial.println("Hold");
        hold(); //fn to call when the button is hold
      }
      else //if timediff is less than 400 then its a single tap
      {
        Serial.println("single tap");
        singleTap(); //fn to call when the button is single taped
      }
    }
    else if (tapCounter == 2 ) //if tapcounter is 2
    {
      if (timediff >= 400) // if timediff is greater than  400 then its single tap and hold
      {
        Serial.println("single tap and hold");
        tapAndHold(); //fn to call when the button is single tap and hold
      }
      else // if timediff is less than 400 then its just double tap
      {
        Serial.println("double tap");
        doubleTap(); //fn to call when doubletap
      }
    }
    else if (tapCounter == 3) //if tapcounter is 3 //then its triple tap
    {
      Serial.println("triple tap");
      tripleTap(); //fn to call when triple tap
    }
    else if (tapCounter == 4) //if tapcounter is 4 then its 4 tap
    {
      Serial.println("four tap");
      fourTap();//fn to call when four tap
    }
    tapCounter = 0;
  }
  lastButtonState = reading;
  myTime= millis();
  if (myTime >= sleeptime) 
   {
    digitalWrite(en_ldo,LOW);
  }
}
void nolight()
{
}
void singleTap()
{
  delay(100);  
  irsend.sendRaw(pausecommand, 67, 38);  // Send a raw data capture at 38kHz.
  delay(100);
}
void doubleTap()
{
  delay(100);  
  irsend.sendRaw(shutdowncommand, 67, 38);  // Send a raw data capture at 38kHz.
  delay(100);
}
void tripleTap()
{
digitalWrite(en_ldo,LOW);
}
void fourTap()
{


}
void hold()
{

}
void tapAndHold()
{

}
