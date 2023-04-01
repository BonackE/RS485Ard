//
//  ContentView.swift
//  MacOS
//
//  Created by Elijah Bonack on 3/13/23.
//

import SwiftUI
import WebKit
import IOKit
import IOKit.usb
import IOKit.serial

struct ContentView: View {
    var body: some View {
        HTMLView(htmlFileName: "app")
    }
}

func getSerialPorts() -> [String] {
    
    // Get the available serial ports
    let paths = ["/dev"]
    var serialPorts = [String]()

    for path in paths {
        do {
            let contents = try FileManager.default.contentsOfDirectory(atPath: path)
            for item in contents {
                if item.hasPrefix("tty.") {
                    serialPorts.append(item)
                }
            }
        } catch {
            print("Error getting contents of directory: \(error)")
        }
    }

    // Return the array of strings
    return serialPorts
}

struct HTMLView: NSViewRepresentable {
    
    let htmlFileName: String
    
    func makeNSView(context: Context) -> WKWebView {
        let webView = WKWebView()
        
        let userContentController = webView.configuration.userContentController
        userContentController.add(context.coordinator, name: "getJsonData")
        userContentController.add(context.coordinator, name: "error")
        userContentController.add(context.coordinator, name: "console")
        userContentController.add(context.coordinator, name: "LoadPorts")
        userContentController.add(context.coordinator, name: "Connect")
        
        
        return webView
    }
    
    func updateNSView(_ nsView: WKWebView, context: Context) {
        if let htmlFile = Bundle.main.path(forResource: htmlFileName, ofType: "html") {
            let htmlString = try? String(contentsOfFile: htmlFile, encoding: .utf8)
            nsView.loadHTMLString(htmlString ?? "", baseURL: URL(string: "http://localhost/")!)
        }
    }

    func makeCoordinator() -> Coordinator {
        return Coordinator(modbus: Modbus())
    }
    
    class Coordinator: NSObject, WKScriptMessageHandler {
        var modbus = Modbus() // Store the modbus object as a member of the Coordinator

        init(modbus: Modbus) {
            super.init();
        }
        
        func userContentController(_ userContentController: WKUserContentController, didReceive message: WKScriptMessage) {
            // define a struct that matches the format of the JSON data
            struct ModbusConfig : Codable {
                let serialPort: String
                let baudRate: Int32
                let dataBits: Int32
                let stopBits: Int32
                let parity: CChar
                let slaveId: Int32
                let functionCode: Int32
                let startAddr: Int32
                let numCoils: Int32
            }
            
            // display errors from HTMLView
            if message.name == "error" {
                let error = (message.body as? [String: Any])?["message"] as? String ?? "unknown"
                print("JavaScript error: \(error)")
            }
            // display console output from HTMLView
            if message.name == "console", let data = message.body as? String {
                
                print("JavaScript console: \(data)")
            }
            // load available serial ports
            if message.name == "LoadPorts", let webView = message.webView {
    
                let ports = getSerialPorts()
                print("ports found: \(ports)")
                                
                let script = "handlePortsData(\(ports));"
                webView.evaluateJavaScript(script) { result, error in
                    if let error = error {
                        print("Error: \(error.localizedDescription)")
                    } else {
                        print("Result: \(result ?? "")")
                    }
                }
            }
            // connect to the modbus
            if message.name == "Connect", let jsonString = message.body as? String {
                
                // use the 'JSONDecoder' class to decode the JSON data into the 'ModbusConfig' struct
                let jsonData = jsonString.data(using: .utf8)!
                print(jsonData)
                let decoder = JSONDecoder()
                do {
                    let config = try decoder.decode(ModbusConfig.self, from: jsonData)
                    modbus = Modbus(device: config.serialPort, baudRate: config.baudRate, parity: config.parity,
                                    dataBits: config.dataBits, stopBits: config.stopBits)
                    modbus_set_slave(modbus.modbus, config.slaveId)

                } catch {
                    print("Error decoding JSON data: \(error)")
                }
                

            }
        }
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}


