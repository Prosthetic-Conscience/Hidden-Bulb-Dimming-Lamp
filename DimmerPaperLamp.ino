#include <Wire.h>
#include <RBDdimmer.h>
#include "Adafruit_MPR121.h"

#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif
//Defines for RobotDyn AC dimmer module
#define outputPin  A2 
#define zerocross  2 on nano // D2 for boards with CHANGEBLE input pins
//#define irqPin 3 //D3 Note: Dimmer uses interrupt vextor 1, so can't use both interrupts

dimmerLamp dimmer(outputPin); //initialise port for dimmer for MEGA, Leonardo, UNO, Arduino M0, Arduino Zero

Adafruit_MPR121 cap = Adafruit_MPR121();

//Defines for MPR121 touch sensor
// Keeps track of the last pins touched
// so we know when buttons are 'released'
uint16_t lasttouched = 0;
boolean powerONOFF = true;
uint16_t currtouched = 0;

//Non-blocking delay defines
static const uint16_t period = 25;
unsigned long time_now = 0;

//Power level defines for ramp
uint16_t iTargetPower = 0;
uint16_t iCurrentPower = 0;

//bottom pwm is 35 without flicker, highest is 91
//int iOutputPower[12] = {0 /*this one is null*/, 38, 40, 42, 44, 47, 51, 55, 60, 65, 70, 85};
const int iOutputPower[12] PROGMEM = {87,70,65,60,55,51,47,44,42,40,38,0};
void setup() {
  Serial.begin(115200); 
  //pinMode(irqPin, INPUT);
  //digitalWrite(irqPin, HIGH); //enable pullup resistor
  delay(500);
  dimmer.begin(NORMAL_MODE, ON); //dimmer initialisation: name.begin(MODE, STATE) 
  Serial.println("Dimmer Program is starting...");
  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!cap.begin(0x5A)) 
    {
    Serial.println("MPR121 not found, check wiring?");
    
    }
  Serial.println("MPR121 found!"); 
  // Create an interrupt to trigger when a button
  // is hit, the IRQ pin goes low, and the function getNumber is run. 
  //attachInterrupt(0,togglePower,LOW);
  dimmer.setState(ON); //name.setState(ON/OFF);
  delay(100);
  //dimmer.setPower(45);
  dimmer.setPower(iOutputPower[6]);
  iCurrentPower = dimmer.getPower();
  iTargetPower = dimmer.getPower(); //set to same as actual power for initial setting
  Serial.print("Booted, setting Power to: ");
  Serial.println(dimmer.getPower());
}

void loop() {
if(millis() - time_now >= period)
  {
  time_now = millis();  
  currtouched = cap.touched();
  //Fade power to setting
  iCurrentPower = dimmer.getPower();
  if(iTargetPower > iCurrentPower)
    {
    dimmer.setPower(iCurrentPower + 1);  
    }
  if(iTargetPower < iCurrentPower)
    {
    dimmer.setPower(iCurrentPower - 1);  
    }
  //Check MPR121 byte and handle inputs     
  for (int i = 0; i < 12; i++)
    {
    if((currtouched & _BV(i)) && !(lasttouched & _BV(i))) 
      {
      if(i == 11) //if last contact touched turn off
        {
        if(powerONOFF == true)
          {
           dimmer.setState(OFF); //name.setState(ON/OFF);
           powerONOFF = false;
           //Serial.println("Lamp Off");
          }
        } 
      if(i != 11) //check which is touched and reference array for output power value
        {
         if(powerONOFF == false)//Only turn on and set power to value if off
            {
            dimmer.setState(ON); //name.setState(ON/OFF);
            powerONOFF = true;
            //Serial.println("Lamp On");
            dimmer.setPower(iOutputPower[i]);
            //Serial.print("Setting Power to: ");
            //Serial.println(dimmer.getPower());
            } 
          else  //else set it to fade to the value wanted
            {                
            //Serial.print("Setting Target power to: ");
            //Serial.println(iOutputPower[i]);
            iTargetPower = iOutputPower[i];   
          }
        }
      }
    }      
  lasttouched = currtouched;    
  }
}   
/*
  if ((currtouched & _BV(11)) && !(lasttouched & _BV(11))) 
    {   
    if(powerONOFF == true)
      {
       dimmer.setState(OFF); //name.setState(ON/OFF);
       powerONOFF = false;
       Serial.println("Lamp Off");
      }
   }
    //if (cap.touched() & (1 << 10)) 
    if ((currtouched & _BV(10)) && !(lasttouched & _BV(10))) 
      {
      if(powerONOFF == false)
          {
           dimmer.setState(ON); //name.setState(ON/OFF);
           powerONOFF = true;
           Serial.println("Lamp On");
          }
          //if(preVal < 38) //Will never happen
          //  {             
            dimmer.setPower(38);
            Serial.print("Setting Power to: ");
            Serial.println(dimmer.getPower());
            return;
           // }
           //if(preVal > 38) //
          //  {
          //
          //  }
          //if(preVal == 38)
          //{
          
          //}
      }    
    //if (cap.touched() & (1 << 9)) 
    if ((currtouched & _BV(9)) && !(lasttouched & _BV(9)))
      {
      if(powerONOFF == false)
        {
         dimmer.setState(ON); //name.setState(ON/OFF);
         powerONOFF = true;
         Serial.println("Lamp On");
        }
      dimmer.setPower(40);
      Serial.print("Setting Power to: ");
      Serial.println(dimmer.getPower());
      return;
      }
    //if (cap.touched() & (1 << 8))
    if ((currtouched & _BV(8)) && !(lasttouched & _BV(8))) 
      {
      if(powerONOFF == false)
          {
           dimmer.setState(ON); //name.setState(ON/OFF);
           powerONOFF = true;
           Serial.println("Lamp On");
          }  
      dimmer.setPower(42);
      Serial.print("Setting Power to: ");
      Serial.println(dimmer.getPower());
      return;
      }
    if ((currtouched & _BV(7)) && !(lasttouched & _BV(7)))
    //if (cap.touched() & (1 << 7)) 
      {
      if(powerONOFF == false)
          {
           dimmer.setState(ON); //name.setState(ON/OFF);
           powerONOFF = true;
           Serial.println("Lamp On");
          }  
      dimmer.setPower(44);
      Serial.print("Setting Power to: ");
      Serial.println(dimmer.getPower());
      return;
      }
    if ((currtouched & _BV(6)) && !(lasttouched & _BV(6)))  
    //if (cap.touched() & (1 << 6)) 
      {
      if(powerONOFF == false)
          {
           dimmer.setState(ON); //name.setState(ON/OFF);
           powerONOFF = true;
           Serial.println("Lamp On");
          }  
      dimmer.setPower(47);
      Serial.print("Setting Power to: ");
      Serial.println(dimmer.getPower());
      return;
      }
    if ((currtouched & _BV(5)) && !(lasttouched & _BV(5)))  
    //if (cap.touched() & (1 << 5)) 
      {
      if(powerONOFF == false)
          {
           dimmer.setState(ON); //name.setState(ON/OFF);
           powerONOFF = true;
           Serial.println("Lamp On");
          }  
      dimmer.setPower(51);
      Serial.print("Setting Power to: ");
      Serial.println(dimmer.getPower());
      return;
      }
    if ((currtouched & _BV(4)) && !(lasttouched & _BV(4)))  
    //if (cap.touched() & (1 << 4)) 
      {
      if(powerONOFF == false)
          {
           dimmer.setState(ON); //name.setState(ON/OFF);
           powerONOFF = true;
           Serial.println("Lamp On");
          }  
      dimmer.setPower(55);
      Serial.print("Setting Power to: ");
      Serial.println(dimmer.getPower());
      return;
      }
    if ((currtouched & _BV(3)) && !(lasttouched & _BV(3)))  
    //if (cap.touched() & (1 << 3)) 
      {
      if(powerONOFF == false)
          {
           dimmer.setState(ON); //name.setState(ON/OFF);
           powerONOFF = true;
           Serial.println("Lamp On");
          }  
      dimmer.setPower(60);
      Serial.print("Setting Power to: ");
      Serial.println(dimmer.getPower());
      return;
      }
    if ((currtouched & _BV(2)) && !(lasttouched & _BV(2)))  
    //if (cap.touched() & (1 << 2)) 
      {
      if(powerONOFF == false)
          {
           dimmer.setState(ON); //name.setState(ON/OFF);
           powerONOFF = true;
           Serial.println("Lamp On");
          }  
      dimmer.setPower(65);
      Serial.print("Setting Power to: ");
      Serial.println(dimmer.getPower());
      return;
      }
    if ((currtouched & _BV(1)) && !(lasttouched & _BV(1)))  
    //if (cap.touched() & (1 << 1)) 
      {
      if(powerONOFF == false)
        {
         dimmer.setState(ON); //name.setState(ON/OFF);
         powerONOFF = true;
         Serial.println("Lamp On");
        } 
      dimmer.setPower(70);
      Serial.print("Setting Power to: ");
      Serial.println(dimmer.getPower());
      return;
      }
    if ((currtouched & _BV(0)) && !(lasttouched & _BV(0)))    
    //if (cap.touched() & (1 << 0)) 
      {
      if(powerONOFF == false)
          {
           dimmer.setState(ON); //name.setState(ON/OFF);
           powerONOFF = true;
           Serial.println("Lamp On");
          }  
      dimmer.setPower(85);
      Serial.print("Setting Power to: ");
      Serial.println(dimmer.getPower());
      return;
      }
  } 
lasttouched = currtouched;    
}*/
