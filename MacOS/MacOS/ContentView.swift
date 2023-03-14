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
struct HTMLView: NSViewRepresentable {
    let htmlFileName: String
    
    func makeNSView(context: Context) -> WKWebView {
        return WKWebView()
    }
    
    func updateNSView(_ nsView: WKWebView, context: Context) {
        if let htmlFile = Bundle.main.path(forResource: htmlFileName, ofType: "html") {
            let htmlString = try? String(contentsOfFile: htmlFile, encoding: .utf8)
            nsView.loadHTMLString(htmlString ?? "", baseURL: nil)
        }
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}


