#include "JSModbusWrapper.h"
#include <cstdlib>

using namespace ultralight;
union {
    uint16_t i[2];
    float data;
    
}reg2float;
void JSModbusWrapper::SetView(View* view) {
    this->view = view;
    
}
JSValueRef JSModbusWrapper::LoadPortsFunc(JSContextRef ctx) {

    std::vector<std::string> ports = modbus.getSerialPorts();


    // Create a JavaScript array and populate it with the port strings
    std::string code = "var ports = [\"";
    for (int i = 0; i < ports.size(); i++) {

        code += ports[i];
        if (i < ports.size() - 1) {
            code += "\",\"";
        }
    }

    code += "\"];"
        "var docPorts =  document.getElementsByName('serialPort');"
        "if(docPorts[0].children.length > 0){"
        "for(var i = 0; i < docPorts[0].children.length;i++){"
		"docPorts[0].remove(0);}"
        "}"
        "for(var i = 0; i < ports.length;i++){"
        "var optn = document.createElement('OPTION');"
        "optn.text = ports[i];"
        "optn.value = ports[i];"
        "docPorts[0].options.add(optn);}";

    // Create our string of JavaScript and substitute the placeholder with the ports array





    JSStringRef script = JSStringCreateWithUTF8CString(code.c_str());

    // Create a JavaScript object that holds the `this` value
    JSObjectRef thisObj = JSObjectMake(ctx, nullptr, nullptr);

    // Call the JavaScript function with the ports array as an argument
    JSValueRef result = JSEvaluateScript(ctx, script, thisObj, nullptr, 0, nullptr);

    // Release our string and JavaScript objects
    JSStringRelease(script);


	return result;
}
//Should return whether the connection was successful in JS
JSValueRef JSModbusWrapper::ConnectFunc(JSContextRef ctx) {
    modbus_close(modbus.ctx);
    modbus_free(modbus.ctx);
    modbus.ctx = NULL;
    
    String JSCode = view->EvaluateScript("GetOptions()");
    std::string str = JSCode.utf8().data();
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
    
    //Attempt to make a new RTU context
    try {
        
            modbus.ctx = modbus_new_rtu(data[0].c_str(), stoi(data[1]), data[4][0], stoi(data[2]), stoi(data[3]));
            int debug =modbus_set_debug(modbus.ctx, TRUE);
           
            printf("%d\n", debug);
        
        
    if (modbus.ctx == NULL) {
		connectTest = "Unable to create the libmodbus context";
        modbus_free(modbus.ctx);
    }
    else if (modbus_connect(modbus.ctx) == -1) {
		

		connectTest = "Connection failed: " + std::string(modbus_strerror(errno));
        
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
    std::string code = "var options = \"" + connectTest + "\";"
        "document.getElementById('result').innerHTML = options;";

	JSStringRef script = JSStringCreateWithUTF8CString(code.c_str());
    JSObjectRef thisObj = JSObjectMake(ctx, nullptr, nullptr);

    // Call the JavaScript function with the ports array as an argument
    JSValueRef result = JSEvaluateScript(ctx, script, thisObj, nullptr, 0, nullptr);

    // Release our string and JavaScript objects
    JSStringRelease(script);


    return result;
    
}

//Should return the data in the table
JSValueRef JSModbusWrapper::RequestFunc(JSContextRef ctx) {
    String JSCode = view->EvaluateScript("GetRequest()");
    std::string str = JSCode.utf8().data();
    std::vector<std::string> data;
    std::string temp;
    std::string code = "";
    std::string codeString;

    uint16_t* tab_reg = NULL;
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
    switch (stoi(data[1])) {
        //switch cases for all function codes
    case 4:

        
        //function 4 read input registers
		tab_reg = (uint16_t*)malloc(sizeof(uint16_t) * stoi(data[3]));
        codeString = "Input Register";
        if (modbus_set_slave(modbus.ctx, stoi(data[0])) == -1) {
            
			code = "var options = \"Set slave failed: " + std::string(modbus_strerror(errno)) + "\";"
				"document.getElementById('result').innerHTML = options;";
        }
        if (modbus_read_input_registers(modbus.ctx, stoi(data[2]), stoi(data[3]), tab_reg) == -1) {
            printf("%d\n", errno);
			code = "var options = \"Read input registers failed: " + std::string(modbus_strerror(errno)) + "\";"
				"document.getElementById('result').innerHTML = options;";
            
        }
        else {
            





            String s = view->EvaluateScript("CountRows()");
			
            uint16_t tempData[2];

            code += "var regdata = [\"";
            for (int i = 0; i < stoi(data[3]); i+=2) {
                tempData[0] = tab_reg[i];
                tempData[1] = tab_reg[i + 1];
                code += std::to_string(modbus_get_float_cdab(tempData));
                if (i != stoi(data[3]) - 1) {
                    code += "\",\"";
                }

            }
            code += "\"];";
			code += "var options = [\"";
			for (int i = 0; i < stoi(data[3]); i++) {
				code += std::to_string(tab_reg[i]);
				if (i != stoi(data[3]) - 1) {
					code += "\",\"";
				}
			}
            code += "\"];"
                "try{"
                "var table = document.getElementById('dataTable');"
                "var tbody = table.getElementsByTagName('tbody')[0];"
                "var transTable = document.getElementById('translatedData');"
                "var transBody = transTable.getElementsByTagName('tbody')[0];"

                "var codeString = \"" + codeString + "\";"

                "for(var i = 0 ; i < options.length ; i++) {"
                "var newRow = tbody.insertRow();"

                "var data = newRow.insertCell(0);"
                "var register = newRow.insertCell(1);"
                "var dataNum = newRow.insertCell(2);"


                "data.innerHTML = codeString;"
                "register.innerHTML = i;"
                "dataNum.innerHTML = options[i];"
                "if(i<options.length/2){"
                "var newRow2 = transBody.insertRow();"


                "var registers = newRow2.insertCell(0);"
                "var data2 = newRow2.insertCell(1);"


                "var num = i+1;"
                "registers.innerHTML = i + \" and \" + num; "
                "data2.innerHTML = regdata[i];"
                "}"
                

                "}"
                
                "}"
                "catch(error){"
                
                "}"
                ;
            
		}
        }
    
    
    
	JSStringRef script = JSStringCreateWithUTF8CString(code.c_str());
	JSObjectRef thisObj = JSObjectMake(ctx, nullptr, nullptr);

	// Call the JavaScript function with the ports array as an argument
	JSValueRef result = JSEvaluateScript(ctx, script, thisObj, nullptr, 0, nullptr);

	// Release our string and JavaScript objects
	JSStringRelease(script);
    if (tab_reg == NULL) {
        return result;
    }
    else {
        free(tab_reg);
        return result;
    }
}




