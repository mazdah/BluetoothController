/* Originally posted on www.bbangpan.com

Program Description: HC-06 bluetooth 2.0 / HM-10 BLE test code

DataPin : 2,3 to TX, RX of HC-06(cross connection)

Tested on : Arduino 1.0.6, Arduino UNO R3, HC-06 or HM-10 BLE



Copyright (c) 2015 www.bbangpan.com. All rights reserved.

This program can be used for any non-commercial purpose freely.

**************************************************************
*
*  2016.07.09 Mazdah 수정
*
**************************************************************

*/



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

int minAngle = 45;                                    // 제 1 관절의 이동 범위 중 최소 각도
int maxAngle = 135;                                   // 제 1 관절의 이동 범위 중 최대 각도
int centerAngle = (minAngle + maxAngle) / 2;          // 제 1 관절의 이동 범위 중 중간 각도
int changedVal = 5;                                   // 제 1 관절의 증감 단위
int term = (maxAngle - centerAngle) / changedVal;     // 제 1 관절의 중간 각도와 최소 각도 또는 최대 각도 간의 차이 (실제 움직이는 범위

int secondStartAngle = 85;                            // 제 2 관절의 초기 각도
int secondChangeVal = 3;                              // 제 2 관절의 증감 단위
int secondHighAngle = secondStartAngle + (secondChangeVal * changedVal * (term / 2)); // 제 2 관절의 리턴 포인트 (이 각도 이후에는 초기 각도로 되돌아옴
int secondLowAngle = secondStartAngle - (secondChangeVal * changedVal * (term / 2));  //// 제 2 관절의 리턴 포인트 (이 각도 이후에는 초기 각도로 되돌아옴

Servo frontLeft;
Servo frontRight;
Servo rearLeft;
Servo rearRight;

Servo frontLeft2;
Servo frontRight2;
Servo rearLeft2;
Servo rearRight2;

bool stepPhase = true; // true frontLeft, false front right
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

    frontLeft.write(centerAngle);
    delay(15);
    frontRight.write(centerAngle);
    delay(15);
    rearLeft.write(centerAngle);
    delay(150);
    rearRight.write(centerAngle);

    frontLeft2.write(secondStartAngle);
    delay(15);
    frontRight2.write(secondStartAngle);
    delay(15);
    rearLeft2.write(secondStartAngle);
    delay(15);
    rearRight2.write(secondStartAngle);
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
    frontLeft.write(centerAngle);
    delay(15);
    frontRight.write(centerAngle);
    delay(15);
    rearLeft.write(centerAngle);
    delay(15);
    rearRight.write(centerAngle);

    frontLeft2.write(secondStartAngle);
    delay(15);
    frontRight2.write(secondStartAngle);
    delay(15);
    rearLeft2.write(secondStartAngle);
    delay(15);
    rearRight2.write(secondStartAngle);
}

// 로봇 전진
void goForward() {
    Serial.println("go forward");

    if (start) {
        start = false;

        int rearRightAngle2 = rearRight2.read();
        int tempCenter = centerAngle, tempCenter2 = centerAngle;
        int level = changedVal;

        for (int i = 0; i < term; i++) {

            rearRight.write(tempCenter -= changedVal);
            delay(15);

            frontRight.write(tempCenter2 += changedVal);
            delay(15);

            //      Serial.print("rearRightAngle2 : ");
            //      Serial.print(i);
            //      Serial.print(" : ");
            //      Serial.println(rearRightAngle2);

            if (rearRightAngle2 == secondHighAngle) {
                level = -1 * changedVal;
            }

            rearRight2.write(rearRightAngle2 += (2 * level));
        }
    } else {

        int frontLeftAngle = frontLeft.read();
        int frontRightAngle = frontRight.read();
        int rearLeftAngle = rearLeft.read();
        int rearRightAngle = rearRight.read();

        int frontLeftAngle2 = secondStartAngle;
        int frontRightAngle2 = secondStartAngle;
        int rearLeftAngle2 = secondStartAngle;
        int rearRightAngle2 = secondStartAngle;

        int tempCenterFL = centerAngle;
        int tempCenterFR = centerAngle;
        int tempCenterRL = centerAngle;
        int tempCenterRR = centerAngle;

        int tempMin = minAngle;
        int tempMax = maxAngle;

        Serial.print("frontLeftAngle : ");
        Serial.println(frontLeftAngle);

        if (stepPhase) {
            stepPhase = false;
            int level = changedVal * -1;

            for (int i = 0; i < term; i++) {
                frontLeft.write(tempCenterFL -= changedVal);
                delay(15);

                rearRight.write(tempMin += changedVal);
                delay(15);

                //frontRight2 move  85 -> 39 -> 85
                if (frontRightAngle2 == secondLowAngle) {
                    level = changedVal;
                }

                frontRight2.write(frontRightAngle2 += (secondChangeVal * level));
                frontRight.write(tempMax -= changedVal);
                delay(15);

                //rearLeft2 move  85 -> 39 -> 85
                rearLeft2.write(rearLeftAngle2 += (secondChangeVal * level));
                rearLeft.write(tempCenterRL += changedVal);
                delay(15);
            }
        } else {
            stepPhase = true;
            int level = changedVal;

            for (int i = 0; i < term; i++) {
                frontRight.write(tempCenterFR += changedVal);
                delay(15);

                rearLeft.write(tempMax -= changedVal);
                delay(15);

                if (frontLeftAngle2 == secondHighAngle) {
                    level = -1 * changedVal;
                }

                frontLeft2.write(frontLeftAngle2 += (secondChangeVal * level));
                frontLeft.write(tempMin += changedVal);
                delay(15);

                rearRight2.write(rearRightAngle2 += (secondChangeVal * level));
                rearRight.write(tempCenterRR -= changedVal);
                delay(15);
            }
        }
    }
}

// 로봇 후진
void goBackward() {
    Serial.println("go backward");

    if (start) {
        start = false;

        int frontLeftAngle2 = frontLeft2.read();
        int tempCenter = centerAngle, tempCenter2 = centerAngle;
        int level = changedVal;

        for (int i = 0; i < term; i++) {

            rearLeft.write(tempCenter += changedVal);
            delay(15);

            frontLeft.write(tempCenter2 -= changedVal);
            delay(15);

            //      Serial.print("rearRightAngle2 : ");
            //      Serial.print(i);
            //      Serial.print(" : ");
            //      Serial.println(rearRightAngle2);

            if (frontLeftAngle2 == secondHighAngle) {
                level = -1 * changedVal;
            }

            frontLeft2.write(frontLeftAngle2 += (2 * level));
        }
    } else {
        int frontLeftAngle = frontLeft.read();
        int frontRightAngle = frontRight.read();
        int rearLeftAngle = rearLeft.read();
        int rearRightAngle = rearRight.read();

        int frontLeftAngle2 = secondStartAngle;
        int frontRightAngle2 = secondStartAngle;
        int rearLeftAngle2 = secondStartAngle;
        int rearRightAngle2 = secondStartAngle;

        int tempCenterFL = centerAngle;
        int tempCenterFR = centerAngle;
        int tempCenterRL = centerAngle;
        int tempCenterRR = centerAngle;

        int tempMin = minAngle;
        int tempMax = maxAngle;

        Serial.print("frontLeftAngle : ");
        Serial.println(frontLeftAngle);

        if (stepPhase) {
            stepPhase = false;
            int level = changedVal;

            for (int i = 0; i < term; i++) {
                //frontRight2 move  85 -> 131 -> 85
                if (frontLeftAngle2 == secondHighAngle) {
                    level = changedVal * -1;
                }

                frontLeft2.write(frontLeftAngle2 += (secondChangeVal * level));
                frontLeft.write(tempCenterFL -= changedVal);
                delay(15);

                //rearRight2 move  85 -> 131 -> 85
                rearRight2.write(rearRightAngle2 += (secondChangeVal * level));
                rearRight.write(tempMin += changedVal);
                delay(15);

                frontRight.write(tempMax -= changedVal);
                delay(15);

                rearLeft.write(tempCenterRL += changedVal);
                delay(15);
            }
        } else {
            stepPhase = true;
            int level = changedVal * -1;

            for (int i = 0; i < term; i++) {
                if (frontRightAngle2 == secondLowAngle) {
                    level = changedVal;
                }

                frontRight2.write(frontRightAngle2 += (secondChangeVal * level));
                frontRight.write(tempCenterFR += changedVal);
                delay(15);

                rearLeft2.write(rearLeftAngle2 += (secondChangeVal * level));
                rearLeft.write(tempMax -= changedVal);
                delay(15);

                frontLeft.write(tempMin += changedVal);
                delay(15);

                rearRight.write(tempCenterRR -= changedVal);
                delay(15);
            }
        }
    }
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
