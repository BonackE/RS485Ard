//
//  MacOSApp.swift
//  MacOS
//
//  Created by Elijah Bonack on 3/13/23.
//
import SwiftUI

@main
struct MacOSApp: App {
    var modbus:Modbus
    init(){
        modbus = Modbus(device:"/dev/ttys001",baudRate:9600,parity: 0x4E,dataBits:8,stopBits: 1)
    }
    var body: some Scene {
        WindowGroup {
            ContentView()
                .environmentObject(modbus) // pass the Modbus instance to the ContentView
        }
    }
    
}

class Modbus : ObservableObject {
    var modbus: OpaquePointer?

    init() {
        
    }
    
    init(device: String, baudRate: Int32, parity: CChar, dataBits: Int32, stopBits: Int32){

        modbus = modbus_new_rtu(device,baudRate,parity,dataBits,stopBits)

        if modbus == nil{
            print("failed")
        }
        
        
    }
}

