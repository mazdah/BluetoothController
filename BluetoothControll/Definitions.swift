//
//  Definitions.swift
//  Bluetooth
//
//  Created by Mick on 12/20/14.
//  Copyright (c) 2014 MacCDevTeam LLC. All rights reserved.
//

import CoreBluetooth

let TRANSFER_SERVICE_UUID = "FFE0"
let TRANSFER_CHARACTERISTIC_UUID = "FFE1"
let NOTIFY_MTU = 20

let transferServiceUUID = CBUUID(string: TRANSFER_SERVICE_UUID)
let transferCharacteristicUUID = CBUUID(string: TRANSFER_CHARACTERISTIC_UUID)


// protocall define

let R_CONNECT = 0x80    // 로봇에게 연결 여부를 물음
let R_READY = 0x81      // 로봇에게 기동 준비를 시킴 로봇으로부터 0x82를 돌려받음
let R_CLOSE = 0xFF      // 로봇의 기동을 중지시키고 자세를 초기화 함
let CMD_START = 0x01    // command 시작
let CMD_END = 0x04      // command 종료