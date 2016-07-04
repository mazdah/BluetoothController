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

int frontLeftPin2 = 6;
int frontRightPin2 = 7;
int rearLeftPin2 = 8;
int rearRightPin2 = 9;

int frontLeftAngle = 90;
int frontRightAngle = 90;
int rearLeftAngle = 90;
int rearRightAngle = 90;

int frontLeftAngle2 = 85;
int frontRightAngle2 = 85;
int rearLeftAngle2 = 85;
int rearRightAngle2 = 85;

Servo frontLeft;
Servo frontRight;
Servo rearLeft;
Servo rearRight;

Servo frontLeft2;
Servo frontRight2;
Servo rearLeft2;
Servo rearRight2;

bool stepSide = true; // true frontLeft, false front right
bool start = true;

//int angle = 0; // servo position in degreesint
//SoftwareSerial BTSerial(2, 3); // SoftwareSerial(RX, TX)

char type[1];
char target[1];
char action[1];
char _direction[1];;
char _speed[1];
char distance[1];

void setup()

{

    Serial.begin(9600);

    frontLeft.attach(frontLeftPin);
    frontRight.attach(frontRightPin);
    rearLeft.attach(rearLeftPin);
    rearRight.attach(rearRightPin);

    frontLeft2.attach(frontLeftPin2);
    frontRight2.attach(frontRightPin2);
    rearLeft2.attach(rearLeftPin2);
    rearRight2.attach(rearRightPin2);

    frontLeft.write(90);
    delay(500);
    frontRight.write(90);
    delay(500);
    rearLeft.write(90);
    delay(500);
    rearRight.write(90);

    frontLeft2.write(85);
    delay(500);
    frontRight2.write(85);
    delay(500);
    rearLeft2.write(85);
    delay(500);
    rearRight2.write(85);
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
    frontLeft.write(90);
    delay(500);
    frontRight.write(90);
    delay(500);
    rearLeft.write(90);
    delay(500);
    rearRight.write(90);

    frontLeft2.write(85);
    delay(500);
    frontRight2.write(85);
    delay(500);
    rearLeft2.write(85);
    delay(500);
    rearRight2.write(85);
}

// 로봇 전진
void goForward() {
    Serial.println("go forward");

    if (stepSide) {
        stepSide = false;

        if (start) {
            start = false;
            int level = 1;
            for (rearRightAngle = 90, frontRightAngle = 90, rearRightAngle2 = 85;
                rearRightAngle > 44, frontRightAngle < 136;
                rearRightAngle--, frontRightAngle++, rearRightAngle2 += (2*level)) {

                if (rearRightAngle2 == 131) {
                    level = -1;
                }

                rearRight.write(rearRightAngle);
                delay(15);
                frontRight.write(frontRightAngle);
                delay(15);
                rearRight2.write(rearRightAngle2);
                delay(15);
            }
        } else {
            int level = 1;
            for (rearRightAngle = 90, frontLeftAngle = 45, frontLeftAngle2 = 85, frontRightAngle = 90, rearRightAngle2 = 85, rearLeftAngle = 135;
            rearRightAngle < 46, frontLeftAngle < 91, frontRightAngle < 136, rearLeftAngle > 89;
            rearRightAngle--, frontLeftAngle++, frontRightAngle++, rearLeftAngle--, frontLeftAngle2 += (2*level), rearRightAngle2 += (2*level)) {

                if (frontLeftAngle2 == 131) {
                    level = -1;
                }

                frontLeft.write(frontLeftAngle);
                delay(15);
                frontRight.write(frontRightAngle);
                delay(15);
                frontLeft2.write(frontLeftAngle2);
                delay(15);
                rearLeft.write(rearLeftAngle);
                delay(15);
                rearRight.write(rearRightAngle);
                delay(15);
                rearRight2.write(rearRightAngle2);
                delay(15);
            }
        }
    } else {
        stepSide = true;
        int level = -1;
        for (rearRightAngle = 45, frontLeftAngle = 90, rearLeftAngle2 = 85, frontRightAngle2 = 85, frontRightAngle = 135, rearLeftAngle = 90;
        rearRightAngle < 91, frontLeftAngle < 46, frontRightAngle < 91, rearLeftAngle < 136;
        rearRightAngle++, frontLeftAngle--, rearLeftAngle2 += (2*level), frontRightAngle2 += (2*level), frontRightAngle--, rearLeftAngle++) {

        if (rearLeftAngle2 == 39) {
            level = 1;
        }

        frontLeft.write(frontLeftAngle);
        delay(15);
        frontRight.write(frontRightAngle);
        delay(15);
        rearLeft2.write(rearLeftAngle2);
        delay(15);
        rearLeft.write(rearLeftAngle);
        delay(15);
        rearRight.write(rearRightAngle);
        delay(15);
        frontRight2.write(frontRightAngle2);
        delay(15);
        }
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
