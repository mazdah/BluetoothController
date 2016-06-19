/* Originally posted on www.bbangpan.com

Program Description: HC-06 bluetooth 2.0 / HM-10 BLE test code

DataPin : 2,3 to TX, RX of HC-06(cross connection)

Tested on : Arduino 1.0.6, Arduino UNO R3, HC-06 or HM-10 BLE

 

Copyright (c) 2015 www.bbangpan.com. All rights reserved.

This program can be used for any non-commercial purpose freely. */

 

#include <SoftwareSerial.h>
#include <Servo.h>

int servoPin = 9;
int servoPin2 = 6;
int servoPin3 = 10;
int servoPin4 = 11;

bool servoState = true;

Servo servo;
Servo servo2;
Servo servo3;
Servo servo4;

int angle = 0; // servo position in degreesint
SoftwareSerial BTSerial(2, 3); // SoftwareSerial(RX, TX)

 

void setup()

{

  Serial.begin(9600);

  servo.attach(servoPin);
  servo2.attach(servoPin2);
  servo3.attach(servoPin3);
  servo4.attach(servoPin4);
  Serial.println("Hello!");

 

// set the data rate for the BT port

  BTSerial.begin(9600);

}

void loop(){

  while (BTSerial.available()){ // if BT sends something
    byte data = BTSerial.read();

    if (data == 0x66) {
      if (servoState) {
        servoState = false;
        servo.write(135);
        servo3.write(135); 
        servo2.write(45); 
        servo4.write(45); 
        delay(15); 
      } else {
        servoState = true;
        servo.write(45);
        servo3.write(45); 
        servo2.write(135); 
        servo4.write(135); 
        delay(15);
      }
    }
    
    Serial.write(data); // write it to serial(serial monitor)

  }

 

  while (Serial.available()){ // if Serial has input(from serial monitor)
  
    byte data = Serial.read();
    
    BTSerial.write(data); // write it to BT
  
  }

 

}
