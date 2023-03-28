//
//  ContentView.swift
//  MacOS
//
//  Created by Elijah Bonack on 3/13/23.
//

import SwiftUI
import WebKit
import IOKit.serial

struct ContentView: View {
    var body: some View {
        HTMLView(htmlFileName: "app")
    }
}

//func getDevices() -> Data? {
//    if let path = Bundle.main.path(forResource: "devices", ofType: "json") {
//        do {
//            let data = try Data(contentsOf: URL(fileURLWithPath: path), options: .mappedIfSafe)
//            print("successfully read data: \(data)")
//
//            return data
//        } catch {
//            print("Error reading devices.json: \(error)")
//        }
//    } else {
//        print("Could not find device.json file.")
//    }
//    return nil
//}

func getSerialPorts() -> [String] {
    let matchingDict = IOServiceMatching(kIOSerialBSDServiceValue)
    var serialIterator: io_iterator_t = 0
    var serialPortArray = [String]()
    
    let kernResult = IOServiceGetMatchingServices(kIOMainPortDefault, matchingDict, &serialIterator)
    guard kernResult == KERN_SUCCESS else {
        print("IOServiceGetMatchingServices error:", kernResult)
        return serialPortArray
    }
    
    var portService: io_object_t = 1
    repeat {
        portService = IOIteratorNext(serialIterator)
        if portService == 0 {
            break
        }
        if let path = IORegistryEntryCreateCFProperty(portService, kIOCalloutDeviceKey as CFString, kCFAllocatorDefault, 0).takeRetainedValue() as? String {
            serialPortArray.append(path)
        }
        IOObjectRelease(portService)
    } while true
    
    IOObjectRelease(serialIterator)
    return serialPortArray
}

struct HTMLView: NSViewRepresentable {
    
    let htmlFileName: String
    
    func makeNSView(context: Context) -> WKWebView {
        let webView = WKWebView()
        
        let userContentController = webView.configuration.userContentController
        userContentController.add(context.coordinator, name: "getJsonData")
        userContentController.add(context.coordinator, name: "error")
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
        return Coordinator()
    }
    
    class Coordinator: NSObject, WKScriptMessageHandler {
//        let modbus = Modbus() // Store the modbus object as a member of the Coordinator
//
//        init(modbus: Modbus) {
//            self.modbus = modbus
//        }
        
        func userContentController(_ userContentController: WKUserContentController, didReceive message: WKScriptMessage) {
//            if message.name == "getJsonData", let webView = message.webView {
//                let jsonData = getDevices()!
//                let str = String(data: jsonData, encoding: .utf8)!
//                let jsonStr = str.replacingOccurrences(of: "\n", with: "");
//
//                print("jsonStr: \(jsonStr)")
//
//                let script = "handleJsonData('\(jsonStr)');"
//                webView.evaluateJavaScript(script) { result, error in
//                    if let error = error {
//                        print("Error: \(error.localizedDescription)")
//                    } else {
//                        print("Result: \(result ?? "")")
//                    }
//                }
//            }
            if message.name == "error" {
                let error = (message.body as? [String: Any])?["message"] as? String ?? "unknown"
                print("JavaScript error: \(error)")
            }
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
            if message.name == "Connect" {
                
            }
        }
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}


