/* Originally posted on www.bbangpan.com

Program Description: HC-06 bluetooth 2.0 / HM-10 BLE test code

DataPin : 2,3 to TX, RX of HC-06(cross connection)

Tested on : Arduino 1.0.6, Arduino UNO R3, HC-06 or HM-10 BLE



Copyright (c) 2015 www.bbangpan.com. All rights reserved.

This program can be used for any non-commercial purpose freely. */



//#include <SoftwareSerial.h>
//#include <Servo.h>

//int servoPin = 9;
//int servoPin2 = 6;
//int servoPin3 = 10;
//int servoPin4 = 11;

//bool servoState = true;

//Servo servo;
//Servo servo2;
//Servo servo3;
//Servo servo4;

//int angle = 0; // servo position in degreesint
//SoftwareSerial BTSerial(2, 3); // SoftwareSerial(RX, TX)

char type[1];
char target[1];
char action[1];
char _direction[1];
char _speed[1];
char distance[1];

void setup()

{

    Serial.begin(9600);

    //  servo.attach(servoPin);
    //  servo2.attach(servoPin2);
    //  servo3.attach(servoPin3);
    //  servo4.attach(servoPin4);
    //  Serial.println("Hello!");


    // set the data rate for the BT port

    Serial3.begin(9600);

}

void loop(){

    while (Serial3.available()){ // if BT sends something
        byte rtype = Serial3.readBytes(type, 1);
        byte rtarget = Serial3.readBytes(target, 1);
        byte raction = Serial3.readBytes(action, 1);
        byte r_direction = Serial3.readBytes(_direction, 1);
        byte r_speed = Serial3.readBytes(_speed, 1);
        byte rdistance = Serial3.readBytes(distance, 1);

    //    if (data == 0x66) {
    //      if (servoState) {
    //        servoState = false;
    //        servo.write(135);
    //        servo3.write(135);
    //        servo2.write(45);
    //        servo4.write(45);
    //        delay(15);
    //      } else {
    //        servoState = true;
    //        servo.write(45);
    //        servo3.write(45);
    //        servo2.write(135);
    //        servo4.write(135);
    //        delay(15);
    //      }
    //    }

        if (type[0] == 0x72 && target[0] == 0x61) {
            if (action[0] == 0x6D) {
                if (_direction[0] == 0x66) {
                    goForward();
                } else if (_direction[0] == 0x62) {
                    goBackward();
                } else if (_direction[0] == 0x6C) {
                    goLeft();
                } else if (_direction[0] == 0x72) {
                    goRight();
                }
            }
        }
    }

    while (Serial.available()){ // if Serial has input(from serial monitor)
        byte data = Serial.read();
        Serial3.write(data); // write it to BT
    }
}

// 로봇 전진
void goForward() {
    Serial.println("go forward");
}

// 로봇 후진
void goBackward() {
    Serial.println("go backward");
}

// 로봇 좌로 이동
void goLeft() {
    Serial.println("go left");
}

// 로봇 우로 이동
void goRight() {
    Serial.println("go right");
}