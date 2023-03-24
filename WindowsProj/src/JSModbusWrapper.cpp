#include "JSModbusWrapper.h"
using namespace ultralight;
void JSModbusWrapper::SetView(View* view) {
    this->view = view;
}
JSValueRef JSModbusWrapper::LoadPortsFunc(JSContextRef ctx){

    std::vector<std::string> ports = modbus.getSerialPorts();
	

    // Create a JavaScript array and populate it with the port strings
    std::string code = "var ports = [\"";
    for (int i = 0; i < ports.size(); i++) {
        
        code += ports[i];
        if (i < ports.size()-1) {
            code += "\",\"";
        }
    }

    code += "\"];"
		"var docPorts =  document.getElementsByName('serialPorts');"
        "for(var i = 0; i < ports.length;i++){"
        "var optn = document.createElement('OPTION');"
        "optn.text = ports[i];"
        "optn.value = ports[i];"
        "docPorts[0].options.add(optn);}";

    // Create our string of JavaScript and substitute the placeholder with the ports array
    const char* str = code.c_str();




    JSStringRef script = JSStringCreateWithUTF8CString(str);

    // Create a JavaScript object that holds the `this` value
    JSObjectRef thisObj = JSObjectMake(ctx, nullptr, nullptr);

    // Call the JavaScript function with the ports array as an argument
    JSValueRef result = JSEvaluateScript(ctx, script, thisObj, nullptr, 0, nullptr);

    // Release our string and JavaScript objects
    JSStringRelease(script);


	return result;
}

JSValueRef JSModbusWrapper::ConnectFunc(JSContextRef ctx) {
    String origStr = view->EvaluateScript("GetOptions()");
    std::string str = origStr.utf8().data();
    std::vector<std::string> data;
    std::string temp;
    for (int i = 0; i < str.length(); i++) {
        if (str[i] == ',') {
            if (temp[0] == ',') {
                temp.erase(0, 1);
            }

            data.push_back(temp);
            temp = "";
        }
        temp += str[i];
    }
    temp.erase(0, 1);
    data.push_back(temp);
    std::string connectTest = "A";
	//prepend a / to data[0] to make it a valid path
    
    
    try {
    modbus.ctx = modbus_new_rtu(data[0].c_str(), stoi(data[1]), data[4][0], stoi(data[2]), stoi(data[3]));
    
    if (modbus.ctx == NULL) {
		connectTest = "Unable to create the libmodbus context";
    }
    if (modbus_connect(modbus.ctx) == -1) {
        connectTest = "Failed to connected";
        modbus_free(modbus.ctx);
    }
    else {
        connectTest = "Connected to " + data[0];
    }
}   //catch EINVAL
	catch (const std::invalid_argument& ia) {
		connectTest = "Invalid argument";
	}
    
    
	
    //Replace data[1] with slave ID using modbus_get_slave(modbus.ctx)
    std::string code = "var options = \"" + connectTest + " " + data[1] + "\";"
        "document.getElementById('result').innerHTML = options;";

	JSStringRef script = JSStringCreateWithUTF8CString(code.c_str());
    JSObjectRef thisObj = JSObjectMake(ctx, nullptr, nullptr);

    // Call the JavaScript function with the ports array as an argument
    JSValueRef result = JSEvaluateScript(ctx, script, thisObj, nullptr, 0, nullptr);

    // Release our string and JavaScript objects
    JSStringRelease(script);


    return result;
    
}




