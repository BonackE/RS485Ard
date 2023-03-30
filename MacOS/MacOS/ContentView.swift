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


