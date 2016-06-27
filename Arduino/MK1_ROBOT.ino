/* Originally posted on www.bbangpan.com

Program Description: HC-06 bluetooth 2.0 / HM-10 BLE test code

DataPin : 2,3 to TX, RX of HC-06(cross connection)

Tested on : Arduino 1.0.6, Arduino UNO R3, HC-06 or HM-10 BLE



Copyright (c) 2015 www.bbangpan.com. All rights reserved.

This program can be used for any non-commercial purpose freely. */



//#include <SoftwareSerial.h>
#include <Servo.h>

int frontLeftPin = 2;
int frontRightPin = 3;
int rearLeftPin = 4;
int rearRightPin = 5;

int frontLeftAngle = 90;
int frontRightAngle = 90;
int rearLeftAngle = 90;
int rearRightAngle = 90;

Servo frontLeft;
Servo frontRight;
Servo rearLeft;
Servo rearRight;

bool stepSide = true; // true frontLeft, false front right
bool start = true;

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

    frontLeft.attach(frontLeftPin);
    frontRight.attach(frontRightPin);
    rearLeft.attach(rearLeftPin);
    rearRight.attach(rearRightPin);

    frontLeft.write(135);
    delay(500);
    frontRight.write(45);
    delay(500);
    rearLeft.write(90);
    delay(500);
    rearRight.write(90);
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
            } else if (action[0] == 0x72) {
                if (_direction[0] == 0x62) {
                    turnLeft();
                } else if (_direction[0] == 0x72) {
                    turnRight();
                }
            } else if (action[0] == 0x69) {
                reset();
            }
        }
    }



    while (Serial.available()){ // if Serial has input(from serial monitor)

        byte data = Serial.read();

        Serial3.write(data); // write it to BT

    }



}

void reset () {
    start = true;
    frontLeft.write(135);
    delay(500);
    frontRight.write(45);
    delay(500);
    rearLeft.write(90);
    delay(500);
    rearRight.write(90);
}

// 로봇 전진
void goForward() {
    Serial.println("go forward");

    if (stepSide) {
        stepSide = false;

        if (start) {
            start = false;
            for (frontLeftAngle = 135; frontLeftAngle < 91; frontLeftAngle--) {
                frontLeft.write(frontLeftAngle);
                delay(15);
            }
            delay(500);

            for (rearRightAngle = 90; rearRightAngle < 136; rearRightAngle++) {
                rearRight.write(rearRightAngle);
                delay(15);
            }
            delay(500);
        } else {
            for (frontLeftAngle = 135, rearRightAngle = 90, frontRightAngle = 90, rearLeftAngle = 45;
            frontLeftAngle > 91, rearRightAngle < 136, frontRightAngle > 44, rearLeftAngle < 91;
            frontLeftAngle--, rearRightAngle++, frontRightAngle--, rearLeftAngle++) {
                frontLeft.write(frontLeftAngle);
                delay(15);
                rearRight.write(rearRightAngle);
                delay(15);
                frontRight.write(frontRightAngle);
                delay(15);
                rearLeft.write(rearLeftAngle);
            }
            delay(500);
        }
    } else {
        stepSide = true;
        for (frontRightAngle = 45, rearLeftAngle = 90, frontLeftAngle = 90, rearRightAngle = 135;
        frontRightAngle < 91, rearLeftAngle > 44, frontLeftAngle < 136, rearRightAngle > 91;
        frontRightAngle++, rearLeftAngle--, frontLeftAngle++, rearRightAngle--) {
            frontRight.write(frontRightAngle);
            delay(15);
            rearLeft.write(rearLeftAngle);
            delay(15);
            frontLeft.write(frontLeftAngle);
            delay(15);
            rearRight.write(rearRightAngle);
        }
        delay(500);
    }
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

void turnLeft() {
    Serial.println("turn left");
}

void turnRight() {
    Serial.println("turn right");
}
