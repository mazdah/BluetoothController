//
//  TargetActionController.swift
//  BluetoothControll
//
//  Created by Mazdah on 2016. 6. 24..
//  Copyright © 2016년 mazdah. All rights reserved.
//

import Foundation

class MakeActionProtocol {
    var buffer: [UInt8] = [0x00, 0x00, 0x00, 0x00, 0x00, 0x00]
    var speed: UInt8 = 0x00
    var distance: UInt8 = 0x00


    init() {
        
    }
    
    // 방향 이동 : 전, 후, 좌, 우
    func goForward() -> [UInt8] {
        buffer = [0x72, 0x61, 0x6D, 0x66, speed, distance]
        return buffer
    }

    func goBackward() -> [UInt8] {
        buffer = [0x72, 0x61, 0x6D, 0x62, speed, distance]
        return buffer
    }

    func goLeft() -> [UInt8] {
        buffer = [0x72, 0x61, 0x6D, 0x6C, speed, distance]
        return buffer
    }

    func goRight() -> [UInt8] {
        buffer = [0x72, 0x61, 0x6D, 0x72, speed, distance]
        return buffer
    }

    // 회전 : 좌, 우
    func turnLeft() -> [UInt8] {
        buffer = [0x72, 0x61, 0x72, 0x6C, speed, distance]
        return buffer
    }

    func turnRight() -> [UInt8] {
        buffer = [0x72, 0x61, 0x72, 0x72, speed, distance]
        return buffer
    }

    // UP/DOWN : 위, 아래
    func  turnUp() -> [UInt8] {
        buffer = [0x72, 0x61, 0x65, 0x75, speed, distance]
        return buffer
    }

    func turnDown() -> [UInt8] {
        buffer = [0x72, 0x61, 0x65, 0x64, speed, distance]
        return buffer
    }

    // 기울이기 : 전, 후, 좌, 우
    func  tiltForward() -> [UInt8] {
        buffer = [0x72, 0x61, 0x73, 0x66, speed, distance]
        return buffer
    }

    func tiltBackward() -> [UInt8] {
        buffer = [0x72, 0x61, 0x73, 0x62, speed, distance]
        return buffer
    }

    func  tiltLeft() -> [UInt8] {
        buffer = [0x72, 0x61, 0x73, 0x6c, speed, distance]
        return buffer
    }

    func tiltRight() -> [UInt8] {
        buffer = [0x72, 0x61, 0x73, 0x72, speed, distance]
        return buffer
    }

    // 수평(몸통이 수평인 채)기울이기 : 전, 후, 좌, 우
    func  horizontalTiltForward() -> [UInt8] {
        buffer = [0x72, 0x61, 0x68, 0x66, speed, distance]
        return buffer
    }

    func horizontalTiltBackward() -> [UInt8] {
        buffer = [0x72, 0x61, 0x68, 0x62, speed, distance]
        return buffer
    }

    func  horizontalTiltLeft() -> [UInt8] {
        buffer = [0x72, 0x61, 0x68, 0x6c, speed, distance]
        return buffer
    }

    func horizontalTiltRight() -> [UInt8] {
        buffer = [0x72, 0x61, 0x68, 0x72, speed, distance]
        return buffer
    }

    // 비틀기 : 좌, 우
    func skewLeft() -> [UInt8] {
        buffer = [0x72, 0x61, 0x73, 0x6C, speed, distance]
        return buffer
    }

    func skewRight() -> [UInt8] {
        buffer = [0x72, 0x61, 0x73, 0x72, speed, distance]
        return buffer
    }
}