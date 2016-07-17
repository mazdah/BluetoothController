//
//  SecondViewController.swift
//  Bluetooth
//
//  Created by Mick on 12/20/14.
//  Copyright (c) 2014 MacCDevTeam LLC. All rights reserved.
//
//  Modified by Mazdah on 2016.06.19
//

import UIKit
import CoreBluetooth

class ViewController: UIViewController, CBCentralManagerDelegate, CBPeripheralDelegate {
    
    @IBOutlet private weak var connectDeviceLabel: UILabel!
    @IBOutlet private weak var textView: UITextView!
    @IBOutlet private weak var buttonUp: UIButton!
    @IBOutlet private weak var buttonDown: UIButton!
    @IBOutlet private weak var buttonLeft: UIButton!
    @IBOutlet private weak var buttonRight: UIButton!
    @IBOutlet private weak var reset: UIButton!
    
    @IBOutlet private weak var moveSwitch: UISwitch!
    @IBOutlet private weak var rotateSwitch: UISwitch!
    @IBOutlet private weak var twistSwitch: UISwitch!
    @IBOutlet private weak var tiltSwitch: UISwitch!
    
    private var centralManager: CBCentralManager?
    private var discoveredPeripheral: CBPeripheral?
    private var txCharacteristic:CBCharacteristic?

    var timer: NSTimer!
    var speedAmmo = 100
    
    var isMove = true
    var isRotate = false
    var isTwist = false
    var isTilt = false
    
    // And somewhere to store the incoming data
    private let data = NSMutableData()
    
    private let actionProtocol = MakeActionProtocol()
    
    
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // Start up the CBCentralManager
        centralManager = CBCentralManager(delegate: self, queue: nil)
        
    }
    
    override func viewWillDisappear(animated: Bool) {
        super.viewWillDisappear(animated)
        
        print("Stopping scan")
        centralManager?.stopScan()
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    /** centralManagerDidUpdateState is a required protocol method.
     *  Usually, you'd check for other states to make sure the current device supports LE, is powered on, etc.
     *  In this instance, we're just using it to wait for CBCentralManagerStatePoweredOn, which indicates
     *  the Central is ready to be used.
     */
    func centralManagerDidUpdateState(central: CBCentralManager) {
        print("\(#line) \(#function)")
        
        guard central.state  == .PoweredOn else {
            // In a real app, you'd deal with all the states correctly
            return
        }
        
        // The state must be CBCentralManagerStatePoweredOn...
        // ... so start scanning
        scan()
    }
    
    /** Scan for peripherals - specifically for our service's 128bit CBUUID
     */
    func scan() {
        
        //scanForPeripheralsWithServices의 첫번째 인자는 CBUUID가 아니라 CBUUID의 배열이다...애플 API 다시 확인해볼 것!
        centralManager?.scanForPeripheralsWithServices(
            nil, options: [
                CBCentralManagerScanOptionAllowDuplicatesKey : NSNumber(bool: true)
            ]
        )
        
        print("Scanning started")
    }
    
    /** This callback comes whenever a peripheral that is advertising the TRANSFER_SERVICE_UUID is discovered.
     *  We check the RSSI, to make sure it's close enough that we're interested in it, and if it is,
     *  we start the connection process
     */
    func centralManager(central: CBCentralManager, didDiscoverPeripheral peripheral: CBPeripheral, advertisementData: [String : AnyObject], RSSI: NSNumber) {
        
        // Reject any where the value is above reasonable range
        // Reject if the signal strength is too low to be close enough (Close is around -22dB)
        
        //        if  RSSI.integerValue < -15 && RSSI.integerValue > -35 {
        //            println("Device not at correct range")
        //            return
        //        }
        
        print("Discovered \(peripheral.name) at \(RSSI)")
        // Ok, it's in range - have we already seen it?
        
        if discoveredPeripheral != peripheral {
            // Save a local copy of the peripheral, so CoreBluetooth doesn't get rid of it
            discoveredPeripheral = peripheral
            
            // And connect
            print("Connecting to peripheral \(peripheral)")
            
            centralManager?.connectPeripheral(peripheral, options: nil)
            
            if peripheral.name == nil {
                connectDeviceLabel?.text = "No name (" + discoveredPeripheral!.identifier.UUIDString + ")\n"
            } else {
                connectDeviceLabel?.text = discoveredPeripheral!.name! + "(" + discoveredPeripheral!.identifier.UUIDString + ")\n"
            }
            
        }
    }
    
    /** If the connection fails for whatever reason, we need to deal with it.
     */
    func centralManager(central: CBCentralManager, didFailToConnectPeripheral peripheral: CBPeripheral, error: NSError?) {
        print("Failed to connect to \(peripheral). (\(error!.localizedDescription))")
        
        cleanup()
    }
    
    /** We've connected to the peripheral, now we need to discover the services and characteristics to find the 'transfer' characteristic.
     */
    func centralManager(central: CBCentralManager, didConnectPeripheral peripheral: CBPeripheral) {
        print("Peripheral Connected")
        
        // Stop scanning
        centralManager?.stopScan()
        print("Scanning stopped")
        
        // Clear the data that we may already have
        data.length = 0
        
        // Make sure we get the discovery callbacks
        peripheral.delegate = self
        
        // Search only for services that match our UUID
        peripheral.discoverServices([transferServiceUUID])
    }
    
    /** The Transfer Service was discovered
     */
    func peripheral(peripheral: CBPeripheral, didDiscoverServices error: NSError?) {
        guard error == nil else {
            print("Error discovering services: \(error!.localizedDescription)")
            cleanup()
            return
        }
        
        guard let services = peripheral.services else {
            return
        }
        
        // Discover the characteristic we want...
        
        // Loop through the newly filled peripheral.services array, just in case there's more than one.
        for service in services {
            peripheral.discoverCharacteristics([transferCharacteristicUUID], forService: service)
        }
    }
    
    /** The Transfer characteristic was discovered.
     *  Once this has been found, we want to subscribe to it, which lets the peripheral know we want the data it contains
     */
    func peripheral(peripheral: CBPeripheral, didDiscoverCharacteristicsForService service: CBService, error: NSError?) {
        // Deal with errors (if any)
        guard error == nil else {
            print("Error discovering services: \(error!.localizedDescription)")
            cleanup()
            return
        }
        
        
        guard let characteristics = service.characteristics else {
            return
        }
        
        // Again, we loop through the array, just in case.
        for characteristic in characteristics {
            // And check if it's the right one
            if characteristic.UUID.isEqual(transferCharacteristicUUID) {
                // If it is, subscribe to it
                peripheral.setNotifyValue(true, forCharacteristic: characteristic)
                txCharacteristic = characteristic
            }
        }
        // Once this is complete, we just need to wait for the data to come in.
    }
    
    /** This callback lets us know more data has arrived via notification on the characteristic
     */
    func peripheral(peripheral: CBPeripheral, didUpdateValueForCharacteristic characteristic: CBCharacteristic, error: NSError?) {
        guard error == nil else {
            print("Error discovering services: \(error!.localizedDescription)")
            return
        }
        
        guard let stringFromData = NSString(data: characteristic.value!, encoding: NSUTF8StringEncoding) else {
            print("Invalid data")
            return
        }
        
        // Have we got everything we need?
        if stringFromData.isEqualToString("EOM") {
            // We have, so show the data,
            textView.text = String(data: data.copy() as! NSData, encoding: NSUTF8StringEncoding)
            
            // Cancel our subscription to the characteristic
            peripheral.setNotifyValue(false, forCharacteristic: characteristic)
            
            // and disconnect from the peripehral
            centralManager?.cancelPeripheralConnection(peripheral)
        } else {
            // Otherwise, just add the data on to what we already have
            data.appendData(characteristic.value!)
            
            // Log it
            print("Received: \(stringFromData)")
            textView.text = textView.text + (stringFromData as String) + "\n"
        }
    }
    
    /** The peripheral letting us know whether our subscribe/unsubscribe happened or not
     */
    func peripheral(peripheral: CBPeripheral, didUpdateNotificationStateForCharacteristic characteristic: CBCharacteristic, error: NSError?) {
        print("Error changing notification state: \(error?.localizedDescription)")
        
        // Exit if it's not the transfer characteristic
        guard characteristic.UUID.isEqual(transferCharacteristicUUID) else {
            return
        }
        
        // Notification has started
        if (characteristic.isNotifying) {
            print("Notification began on \(characteristic)")
        } else { // Notification has stopped
            print("Notification stopped on (\(characteristic))  Disconnecting")
            centralManager?.cancelPeripheralConnection(peripheral)
        }
    }
    
    /** Once the disconnection happens, we need to clean up our local copy of the peripheral
     */
    func centralManager(central: CBCentralManager, didDisconnectPeripheral peripheral: CBPeripheral, error: NSError?) {
        print("Peripheral Disconnected")
        discoveredPeripheral = nil
        
        // We're disconnected, so start scanning again
        scan()
    }
    
    /** Call this when things either go wrong, or you're done with the connection.
     *  This cancels any subscriptions if there are any, or straight disconnects if not.
     *  (didUpdateNotificationStateForCharacteristic will cancel the connection if a subscription is involved)
     */
    private func cleanup() {
        // Don't do anything if we're not connected
        // self.discoveredPeripheral.isConnected is deprecated
        guard discoveredPeripheral?.state == .Connected else {
            return
        }
        
        // See if we are subscribed to a characteristic on the peripheral
        guard let services = discoveredPeripheral?.services else {
            cancelPeripheralConnection()
            return
        }
        
        for service in services {
            guard let characteristics = service.characteristics else {
                continue
            }
            
            for characteristic in characteristics {
                if characteristic.UUID.isEqual(transferCharacteristicUUID) && characteristic.isNotifying {
                    discoveredPeripheral?.setNotifyValue(false, forCharacteristic: characteristic)
                    // And we're done.
                    return
                }
            }
        }
    }
    
    private func cancelPeripheralConnection() {
        // If we've got this far, we're connected, but we're not subscribed, so we just disconnect
        centralManager?.cancelPeripheralConnection(discoveredPeripheral!)
    }
    
    @IBAction func lognPressForward(sender: UILongPressGestureRecognizer) {
        let gestureState = sender.state;
        
        switch (gestureState) {
        case .Began :
            print("Began")
            
            if isMove {
                timer = NSTimer.scheduledTimerWithTimeInterval(1.0, target: self, selector: #selector(ViewController.sendMessage(_:)), userInfo: NSData(bytes:actionProtocol.goForward(), length:6), repeats: true)
            } else if isTilt {
                
            }
        case .Changed :
            print("Changed")
            
        case .Ended :
            print("Ended")
            if isMove {
                stopTimer()
            } else if isTilt {
                
            }
        default :
            print("Default")
        }
    }
    
    @IBAction func longPressLeft(sender: UILongPressGestureRecognizer) {
        print("longPressLeft");
        let gestureState = sender.state;
        
        switch (gestureState) {
        case .Began :
            print("Began")
            if isMove {
                timer = NSTimer.scheduledTimerWithTimeInterval(1.0, target: self, selector: #selector(ViewController.sendMessage(_:)), userInfo: NSData(bytes:actionProtocol.goLeft(), length:6), repeats: true)
            } else if isRotate {
                timer = NSTimer.scheduledTimerWithTimeInterval(1.0, target: self, selector: #selector(ViewController.sendMessage(_:)), userInfo: NSData(bytes:actionProtocol.turnLeft(), length:6), repeats: true)
            } else if isTwist {
                sendData(NSData(bytes:actionProtocol.twistLeft(), length:6))
            } else if isTilt {
                
            }
            
        case .Changed :
            print("Changed")
            
        case .Ended :
            print("Ended")
            
            if isMove {
                stopTimer()
            } else if isRotate {
                stopTimer()
            } else if isTwist {
                sendData(NSData(bytes:actionProtocol.reset(), length:6))
            } else if isTilt {
                
            }
        default :
            print("Default")
        }
    }
    
    @IBAction func longPressRight(sender: UILongPressGestureRecognizer) {
        print("longPressRight");
        let gestureState = sender.state;
        
        switch (gestureState) {
        case .Began :
            print("Began")
            
            if isMove {
                timer = NSTimer.scheduledTimerWithTimeInterval(1.0, target: self, selector: #selector(ViewController.sendMessage(_:)), userInfo: NSData(bytes:actionProtocol.goRight(), length:6), repeats: true)
            } else if isRotate {
                timer = NSTimer.scheduledTimerWithTimeInterval(1.0, target: self, selector: #selector(ViewController.sendMessage(_:)), userInfo: NSData(bytes:actionProtocol.turnRight(), length:6), repeats: true)
            } else if isTwist {
                sendData(NSData(bytes:actionProtocol.twistRight(), length:6))
            } else if isTilt {
                
            }
        case .Changed :
            print("Changed")
            
        case .Ended :
            print("Ended")
            
            if isMove {
                stopTimer()
            } else if isRotate {
                stopTimer()
            } else if isTwist {
                sendData(NSData(bytes:actionProtocol.reset(), length:6))
            } else if isTilt {
                
            }
        default :
            print("Default")
        }
    }
    
    @IBAction func longPressBack(sender: UILongPressGestureRecognizer) {
        let gestureState = sender.state;
        
        switch (gestureState) {
        case .Began :
            print("Began")
            
            if isMove {
                timer = NSTimer.scheduledTimerWithTimeInterval(1.0, target: self, selector: #selector(ViewController.sendMessage(_:)), userInfo: NSData(bytes:actionProtocol.goBackward(), length:6), repeats: true)
            } else if isTilt {
                
            }
        case .Changed :
            print("Changed")
            
        case .Ended :
            print("Ended")
            if isMove {
                stopTimer()
            } else if isTilt {
                
            }
        default :
            print("Default")
        }
    }
    
    @IBAction func moveToggle(sender: UISwitch) {
        if sender.on {
            buttonUp.enabled = true
            buttonDown.enabled = true
            
            isMove = true
            isRotate = false
            isTwist = false
            isTilt = false
            
            rotateSwitch.setOn(false, animated: true)
            twistSwitch.setOn(false, animated: true)
            tiltSwitch.setOn(false, animated: true)
        } else {
            if !rotateSwitch.on && !twistSwitch.on && !tiltSwitch.on {
                moveSwitch.setOn(true, animated: false)
            }
        }
    }
    
    @IBAction func rotateToggle(sender: UISwitch) {
        if sender.on {
            isMove = false
            isRotate = true
            isTwist = false
            isTilt = false
            
            buttonUp.enabled = false
            buttonDown.enabled = false
            
            moveSwitch.setOn(false, animated: true)
            twistSwitch.setOn(false, animated: true)
            tiltSwitch.setOn(false, animated: true)
        } else {
            if !moveSwitch.on && !twistSwitch.on && !tiltSwitch.on {
                moveSwitch.setOn(true, animated: false)
            } else {
                buttonUp.enabled = true
                buttonDown.enabled = true
            }
        }
    }
    
    @IBAction func twistToggle(sender: UISwitch) {
        if sender.on {
            isMove = false
            isRotate = false
            isTwist = true
            isTilt = false
            
            buttonUp.enabled = false
            buttonDown.enabled = false
            
            moveSwitch.setOn(false, animated: true)
            rotateSwitch.setOn(false, animated: true)
            tiltSwitch.setOn(false, animated: true)
        } else {
            if !rotateSwitch.on && !moveSwitch.on && !tiltSwitch.on {
                moveSwitch.setOn(true, animated: false)
            } else {
                buttonUp.enabled = true
                buttonDown.enabled = true
            }
        }
    }
    
    @IBAction func tiltToggle(sender: UISwitch) {
        if sender.on {
            isMove = false
            isRotate = false
            isTwist = false
            isTilt = true
            
            moveSwitch.setOn(false, animated: true)
            rotateSwitch.setOn(false, animated: true)
            twistSwitch.setOn(false, animated: true)
        } else {
            if !rotateSwitch.on && !twistSwitch.on && !moveSwitch.on {
                moveSwitch.setOn(true, animated: false)
            }
        }
    }
    
    /*
    @IBAction func buttonUpTouch (sender: UIButton) {
        
        timer = NSTimer.scheduledTimerWithTimeInterval(1.0, target: self, selector: #selector(ViewController.sendMessage(_:)), userInfo: NSData(bytes:actionProtocol.goForward(), length:6), repeats: true)
    }
    
    @IBAction func buttonDownTouch (sender: UIButton) {
        timer = NSTimer.scheduledTimerWithTimeInterval(1.0, target: self, selector: #selector(ViewController.sendMessage(_:)), userInfo: NSData(bytes:actionProtocol.goBackward(), length:6), repeats: true)
    }
    
    @IBAction func buttonLeftTouch (sender: UIButton) {
        timer = NSTimer.scheduledTimerWithTimeInterval(1.0, target: self, selector: #selector(ViewController.sendMessage(_:)), userInfo: NSData(bytes:actionProtocol.goLeft(), length:6), repeats: true)
    }
    
    @IBAction func buttonRightTouch (sender: UIButton) {
        timer = NSTimer.scheduledTimerWithTimeInterval(1.0, target: self, selector: #selector(ViewController.sendMessage(_:)), userInfo: NSData(bytes:actionProtocol.goRight(), length:6), repeats: true)
    }
    
    @IBAction func buttonUpRelease (sender: UIButton) {
        print("buttonup Release!!!")
        stopTimer();
    }
    
    @IBAction func buttonDownRelease (sender: UIButton) {
        print("buttonDown Release!!!")
        stopTimer();
    }
    
    @IBAction func buttonLeftRelease (sender: UIButton) {
        print("buttonLeft Release!!!")
        stopTimer();
    }
    
    @IBAction func buttonRightRelease (sender: UIButton) {
        print("buttonRight Release!!!")
        stopTimer();
    }
    */
    
    @IBAction func resetTouched (sender: UIButton) {
        print("buttonRight Release!!!")
        sendData(NSData(bytes:actionProtocol.reset(), length:6))
    }
    
    func sendMessage (timer: NSTimer) {
        let msg = timer.userInfo as! NSData
        print(msg)
        sendData(msg)
    }
    
    func sendData (msg: NSData) {
        if discoveredPeripheral?.state == CBPeripheralState.Connected {
            print("connected : sendMessage");
        } else {
            print("Not connected");
        }
        
        discoveredPeripheral?.writeValue(msg, forCharacteristic: txCharacteristic!, type: CBCharacteristicWriteType.WithoutResponse)
//        if let data = message.dataUsingEncoding(NSUTF8StringEncoding) {
//            connectedPeripheral!.writeValue(data, forCharacteristic: writeCharacteristic!, type: writeType)
//        }
        
    }
    
    func stopTimer () {
        timer?.invalidate()
    }
    
    override func touchesBegan(touches: Set<UITouch>, withEvent event: UIEvent?) {
        
        textView.resignFirstResponder()
        
    }
}

