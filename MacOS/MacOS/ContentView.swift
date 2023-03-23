//
//  ContentView.swift
//  MacOS
//
//  Created by Elijah Bonack on 3/13/23.
//

import SwiftUI
import WebKit

struct ContentView: View {
    var body: some View {
        HTMLView(htmlFileName: "app")
    }
}

func getDevices() -> Data? {
    if let path = Bundle.main.path(forResource: "devices", ofType: "json") {
        do {
            let data = try Data(contentsOf: URL(fileURLWithPath: path), options: .mappedIfSafe)
            print("successfully read data: \(data)")
            
            return data
        } catch {
            print("Error reading devices.json: \(error)")
        }
    } else {
        print("Could not find device.json file.")
    }
    return nil
}

struct HTMLView: NSViewRepresentable {
    
    let htmlFileName: String
    
    func makeNSView(context: Context) -> WKWebView {
        let webView = WKWebView()
        
        let userContentController = webView.configuration.userContentController
        userContentController.add(context.coordinator, name: "getJsonData")
        userContentController.add(context.coordinator, name: "error")
        
        
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
        func userContentController(_ userContentController: WKUserContentController, didReceive message: WKScriptMessage) {
            if message.name == "getJsonData", let webView = message.webView {
                let jsonData = getDevices()!
                let str = String(data: jsonData, encoding: .utf8)!
                let jsonStr = str.replacingOccurrences(of: "\n", with: "");                

                print("jsonStr: \(jsonStr)")
   
                let script = "handleJsonData('\(jsonStr)');"
                webView.evaluateJavaScript(script) { result, error in
                    if let error = error {
                        print("Error: \(error.localizedDescription)")
                    } else {
                        print("Result: \(result ?? "")")
                    }
                }
            }
            if message.name == "error" {
                let error = (message.body as? [String: Any])?["message"] as? String ?? "unknown"
                print("JavaScript error: \(error)")
            }
        }
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}


