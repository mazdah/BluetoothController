//
//  ViewController.swift
//  BluetoothControll
//
//  Created by Mazdah on 2016. 6. 17..
//  Copyright © 2016년 mazdah. All rights reserved.
//
import CoreBluetooth
import UIKit

class ViewController: UIViewController, CBCentralManagerDelegate {

    var manager:CBCentralManager!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
        
        manager = CBCentralManager(delegate: self, queue: nil)
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }

    func centralManager(central: CBCentralManager,
                                   didDiscover peripheral: CBPeripheral,
                                               advertisementData: [String : AnyObject],
                                               rssi RSSI: NSNumber) {
        
        print("Peripheral = \(peripheral)")
        
    }
    
    func centralManagerDidUpdateState(central: CBCentralManager) { // x\
        
        
        var consoleMsg = ""
        switch(central.state){ // 상태에 따른 콘솔 메시지를 출력한다.
            
        case .PoweredOff :
            consoleMsg = "BLE is powered Off"
            
        case .PoweredOn :
            consoleMsg = "BLE is powered On"
            manager.scanForPeripheralsWithServices(nil, options: nil) // PowerOn 상태이면 장치를 검색한다.
            
        case .Resetting :
            consoleMsg = "BLE is Resetting"
            
        case .Unauthorized :
            consoleMsg = "BLE is Unautorized"
            
        case .Unknown :
            consoleMsg = "BLE is Unknown"
            
        case .Unsupported:
            consoleMsg = "BLE is Unsupported"
            
        }
        
        print("\(consoleMsg)")
        
        
    }
    
}

