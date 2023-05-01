#include "JSModbusWrapper.h"
#include <stdlib.h>
#include <errno.h>

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
	if (modbus.ctx != NULL) {
		modbus_close(modbus.ctx);
		modbus_free(modbus.ctx);
		modbus.ctx = NULL;
	}
	

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



		if (modbus.ctx == NULL) {
			connectTest = "Unable to create the libmodbus context";
			modbus_free(modbus.ctx);
			modbus.ctx = NULL;
		}
		else if (modbus_connect(modbus.ctx) == -1) {

			if (data[0] == "No Ports Found") {
				connectTest = "Connection failed: Not using a valid port.";
			}
			else {
				connectTest = "Connection failed: " + std::string(modbus_strerror(errno));
			}

			modbus_free(modbus.ctx);
			modbus.ctx = NULL;
		}
		else {
			connectTest = "Connected to " + data[0];

		}
	}   //catch EINVAL
	catch (const std::invalid_argument& ia) {
		connectTest = "Invalid argument" + std::string(ia.what());
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
	uint8_t* tab_bits = NULL;
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

	try {
		switch (stoi(data[1])) {
		case 1:
		{

			codeString = "Coils";
			tab_bits = new uint8_t[stoi(data[3])];
			if (modbus_set_slave(modbus.ctx, stoi(data[0])) == -1) {

				code = "var options = \"Set slave failed: " + std::string(modbus_strerror(errno)) + "\";"
					"document.getElementById('result').innerHTML = options;";
			}
			else {
				std::string hexReq = modbus.getHexReq(MODBUS_FC_READ_COILS, stoi(data[2]), stoi(data[3]), tab_bits);
				code += "var hexReq = \"" + hexReq + "\";"
					"try{"
					"var rawData = document.getElementById('rawData');"
					"var rawTbody = rawData.getElementsByTagName('tbody')[0];"
					"var rawRow = rawTbody.insertRow();"

					"rawRow.innerHTML = hexReq;"
					"}catch(err){"
					"document.getElementById('result').innerHTML = err.message;"
					"}";
				if (modbus_read_bits(modbus.ctx, stoi(data[2]), stoi(data[3]), tab_bits) == -1) {

					code = "var options = \"Read Coils failed: " + std::string(modbus_strerror(errno)) + "\";"
						"document.getElementById('result').innerHTML = options;";

				}
				else {
					std::string resp = modbus.getHexResp();
					
					code += "var hexResp = \"" + resp + "\";"
						"try{"
						"var rawData = document.getElementById('rawData');"
						"var rawTbody = rawData.getElementsByTagName('tbody')[0];"
						"var rawRow = rawTbody.insertRow();"

						"rawRow.innerHTML = hexResp;"
						"}catch(err){"
						"document.getElementById('result').innerHTML = err.message;"
						"}";

					code += "var options = [\"";
					
					for (int i = 0; i < stoi(data[3]); i++) {
						code += std::to_string(tab_bits[i]);
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
						"dataNum.contentEditable = true;"

						"data.innerHTML = codeString;"
						"register.innerHTML = i + " + data[2] + ";"
						"dataNum.innerHTML = options[i];"


						"}"
						"var answer = \"Read Coils succeeded\";"
						"document.getElementById('result').innerHTML = answer;"
						"}"
						"catch(err){"
						"document.getElementById('result').innerHTML = err.message;"
						"}";


					String s = view->EvaluateScript("CountRows()");

				}
			}
			break;
		}
		case 3:
		{
			tab_reg = new uint16_t[stoi(data[3])];
			codeString = "Holding Register";
			code = "";
			if (modbus_set_slave(modbus.ctx, stoi(data[0])) == -1) {

				code = "var options = \"Set slave failed: " + std::string(modbus_strerror(errno)) + "\";"
					"document.getElementById('result').innerHTML = options;";
			}
			else {
				std::string hexReq = modbus.getHexReq(MODBUS_FC_READ_HOLDING_REGISTERS, stoi(data[2]), stoi(data[3]), tab_reg);
				code += "var hexReq = \"" + hexReq + "\";"
					"try{"
					"var rawData = document.getElementById('rawData');"
					"var rawTbody = rawData.getElementsByTagName('tbody')[0];"
					"var rawRow = rawTbody.insertRow();"

					"rawRow.innerHTML = hexReq;"
					"}catch(err){"
					"document.getElementById('result').innerHTML = err.message;"
					"}";
				if (modbus_read_registers(modbus.ctx, stoi(data[2]), stoi(data[3]), tab_reg) == -1) {

					code = "var options = \"Read holding registers failed: " + std::string(modbus_strerror(errno)) + "\";"
						"document.getElementById('result').innerHTML = options;";

				}
				else {
					std::string resp = modbus.getHexResp();
					code += "var hexResp = \"" + resp + "\";"
						"try{"
						"var rawData = document.getElementById('rawData');"
						"var rawTbody = rawData.getElementsByTagName('tbody')[0];"
						"var rawRow = rawTbody.insertRow();"

						"rawRow.innerHTML = hexResp;"
						"}catch(err){"
						"document.getElementById('result').innerHTML = err.message;"
						"}";




					String s = view->EvaluateScript("CountRows()");

					uint16_t tempData[2];
					String floatsFunc = view->EvaluateScript("GetTranslatedRead()");
					std::string floats = floatsFunc.utf8().data();

					code += "if(document.getElementById('translatedCheckBox').checked){"
						"var regdata = [\"";
					for (int i = 0; i < stoi(data[3]); i += 2) {
						tempData[0] = tab_reg[i];
						tempData[1] = tab_reg[i + 1];
						if (floats == "abcd") {
							code += std::to_string(modbus_get_float_abcd(tempData));
						}
						else if (floats == "cdab") {
							code += std::to_string(modbus_get_float_cdab(tempData));
						}
						else if (floats == "badc") {
							code += std::to_string(modbus_get_float_badc(tempData));
						}
						else if (floats == "dcba") {
							code += std::to_string(modbus_get_float_dcba(tempData));
						}
						if (i != stoi(data[3]) - 1) {
							code += "\",\"";
						}

					}
					code += "\"];}";
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
						"dataNum.contentEditable = true;"


						"data.innerHTML = codeString;"
						"register.innerHTML = i + " + data[2] + ";"
						"dataNum.innerHTML = options[i];"
						"if(document.getElementById('translatedCheckBox').checked && options.length%2==0){"
						"if(i<options.length/2){"
						"var newRow2 = transBody.insertRow();"


						"var registers = newRow2.insertCell(0);"
						"var data2 = newRow2.insertCell(1);"
						"var date = newRow2.insertCell(2);"
						"var regNum = i + " + data[2] + ";"
						"var num = regNum+1;"
						"registers.innerHTML = regNum + \" and \" + num; "
						"data2.innerHTML = regdata[i];"
						"date.innerHTML = GetTime();"
						"}"
						"}"


						"}"
						"var answer = \"Read Holding Registers succeeded\";"
						"document.getElementById('result').innerHTML = answer;"
						"}"
						"catch(err){"
						"document.getElementById('result').innerHTML = err.message;"
						"}";

				}
			}
			break;
		}

		//switch cases for all function codes
		case 4:
		{

			//function 4 read input registers
			tab_reg = new uint16_t[stoi(data[3])];
			codeString = "Input Register";
			code = "";
			if (modbus_set_slave(modbus.ctx, stoi(data[0])) == -1) {

				code = "var options = \"Set slave failed: " + std::string(modbus_strerror(errno)) + "\";"
					"document.getElementById('result').innerHTML = options;";
			}
			else {
				std::string hexReq = modbus.getHexReq(MODBUS_FC_READ_INPUT_REGISTERS, stoi(data[2]), stoi(data[3]), tab_reg);
				code += "var hexReq = \"" + hexReq + "\";"
					"try{"
					"var rawData = document.getElementById('rawData');"
					"var rawTbody = rawData.getElementsByTagName('tbody')[0];"
					"var rawRow = rawTbody.insertRow();"

					"rawRow.innerHTML = hexReq;"
					"}catch(err){"
					"document.getElementById('result').innerHTML = err.message;"
					"}";
				if (modbus_read_input_registers(modbus.ctx, stoi(data[2]), stoi(data[3]), tab_reg) == -1) {

					code = "var options = \"Read input registers failed: " + std::string(modbus_strerror(errno)) + "\";"
						"document.getElementById('result').innerHTML = options;";

				}
				else {
					std::string resp = modbus.getHexResp();
					code += "var hexResp = \"" + resp + "\";"
						"try{"
						"var rawData = document.getElementById('rawData');"
						"var rawTbody = rawData.getElementsByTagName('tbody')[0];"
						"var rawRow = rawTbody.insertRow();"

						"rawRow.innerHTML = hexResp;"
						"}catch(err){"
						"document.getElementById('result').innerHTML = err.message;"
						"}";




					String s = view->EvaluateScript("CountRows()");

					uint16_t tempData[2];
					String floatsFunc = view->EvaluateScript("GetTranslatedRead()");
					std::string floats = floatsFunc.utf8().data();
					code += "if(document.getElementById('translatedCheckBox').checked){"
						"var regdata = [\"";
					for (int i = 0; i < stoi(data[3]); i += 2) {
						tempData[0] = tab_reg[i];
						tempData[1] = tab_reg[i + 1];
						if (floats == "abcd") {
							code += std::to_string(modbus_get_float_abcd(tempData));
						}
						else if (floats == "cdab") {
							code += std::to_string(modbus_get_float_cdab(tempData));
						}
						else if (floats == "badc") {
							code += std::to_string(modbus_get_float_badc(tempData));
						}
						else if (floats == "dcba") {
							code += std::to_string(modbus_get_float_dcba(tempData));
						}
						if (i != stoi(data[3]) - 1) {
							code += "\",\"";
						}

					}
					code += "\"];}";
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
						"register.innerHTML = i + " + data[2] + ";"
						"dataNum.innerHTML = options[i];"
						"if(document.getElementById('translatedCheckBox').checked && options.length%2==0){"
						"if(i<options.length/2){"
						"var newRow2 = transBody.insertRow();"


						"var registers = newRow2.insertCell(0);"
						"var data2 = newRow2.insertCell(1);"
						"var date = newRow2.insertCell(2);"

						"var regNum = i + " + data[2] + ";"
						"var num = regNum+1;"
						"registers.innerHTML = regNum + \" and \" + num; "
						"data2.innerHTML = regdata[i];"
						"date.innerHTML = GetTime()"
						"}"
						"}"


						"}"
						"var answer = \"Read Input Registers succeeded\";"
						"document.getElementById('result').innerHTML = answer;"
						"}"
						"catch(err){"

						"}";

				}
			}
			break;
		}
		case 0x0F:
		{
			tab_bits = new uint8_t[stoi(data[3])];


			if (modbus_set_slave(modbus.ctx, stoi(data[0])) == -1) {

				code = "var options = \"Set slave failed: " + std::string(modbus_strerror(errno)) + "\";"
					"document.getElementById('result').innerHTML = options;";
			}
			else {
				String s = view->EvaluateScript("GetCellData()");
				std::string str = s.utf8().data();
				for (int i = 0; i < stoi(data[3]); i++) {
					tab_bits[i] = std::stoi(str.substr(0, str.find(",")));
					str.erase(0, str.find(",") + 1);
				}
				std::string hexReq = modbus.getHexReq(MODBUS_FC_WRITE_MULTIPLE_COILS, stoi(data[2]), stoi(data[3]), tab_bits);
				code += "var hexReq = \"" + hexReq + "\";"
					"try{"
					"var rawData = document.getElementById('rawData');"
					"var rawTbody = rawData.getElementsByTagName('tbody')[0];"
					"var rawRow = rawTbody.insertRow();"

					"rawRow.innerHTML = hexReq;"
					"}catch(err){"
					"document.getElementById('result').innerHTML = err.message;"
					"}";
				if (modbus_write_bits(modbus.ctx, stoi(data[2]), stoi(data[3]), tab_bits) == -1) {
					code += "var options = \"Write coils failed: " + std::string(modbus_strerror(errno)) + "\";"
						"document.getElementById('result').innerHTML = options;";
				}
				else {
					std::string resp = modbus.getHexResp();
					code += "var hexResp = \"" + resp + "\";"
						"try{"
						"var rawData = document.getElementById('rawData');"
						"var rawTbody = rawData.getElementsByTagName('tbody')[0];"
						"var rawRow = rawTbody.insertRow();"

						"rawRow.innerHTML = hexResp;"
						"}catch(err){"
						"document.getElementById('result').innerHTML = err.message;"
						"}";
					code += "var options = \"Write coils succeeded\";"
						"document.getElementById('result').innerHTML = options;";
				}
			}
			break;
		}
		case 0x10:
		{
			tab_reg = new uint16_t[stoi(data[3])];


			if (modbus_set_slave(modbus.ctx, stoi(data[0])) == -1) {

				code = "var options = \"Set slave failed: " + std::string(modbus_strerror(errno)) + "\";"
					"document.getElementById('result').innerHTML = options;";
			}
			else {
				String s = view->EvaluateScript("GetCellData()");
				std::string str = s.utf8().data();
				for (int i = 0; i < stoi(data[3]); i++) {
					tab_reg[i] = std::stoi(str.substr(0, str.find(",")));
					str.erase(0, str.find(",") + 1);
				}
				std::string hexReq = modbus.getHexReq(MODBUS_FC_WRITE_MULTIPLE_REGISTERS, stoi(data[2]), stoi(data[3]), tab_reg);
				code += "var hexReq = \"" + hexReq + "\";"
					"try{"
					"var rawData = document.getElementById('rawData');"
					"var rawTbody = rawData.getElementsByTagName('tbody')[0];"
					"var rawRow = rawTbody.insertRow();"

					"rawRow.innerHTML = hexReq;"
					"}catch(err){"
					"document.getElementById('result').innerHTML = err.message;"
					"}";
				if (modbus_write_registers(modbus.ctx, stoi(data[2]), stoi(data[3]), tab_reg) == -1) {
					code += "var options = \"Write holding registers failed: " + std::string(modbus_strerror(errno)) + "\";"
						"document.getElementById('result').innerHTML = options;";
				}
				else {
					int count = 0;
					for (int i = 0; i < stoi(data[3]); i++) {
						if (tab_reg[i] != 0) {
							count++;
						}
					}
					std::string resp = modbus.getHexResp();
					code += "var hexResp = \"" + resp + "\";"
						"try{"
						"var rawData = document.getElementById('rawData');"
						"var rawTbody = rawData.getElementsByTagName('tbody')[0];"
						"var rawRow = rawTbody.insertRow();"

						"rawRow.innerHTML = hexResp;"
						"}catch(err){"
						"document.getElementById('result').innerHTML = err.message;"
						"}";
					code += "var options = \"Write input registers succeeded\";"
						"document.getElementById('result').innerHTML = options;";
				}
			}
			break;
		}
		}
	}
	catch (std::exception& e) {
		code += "document.getElementById('result').innerHTML = \"Error: " + std::string(e.what()) + "\";";
	}



	JSStringRef script = JSStringCreateWithUTF8CString(code.c_str());
	JSObjectRef thisObj = JSObjectMake(ctx, nullptr, nullptr);

	// Call the JavaScript function with the ports array as an argument
	JSValueRef result = JSEvaluateScript(ctx, script, thisObj, nullptr, 0, nullptr);

	// Release our string and JavaScript objects
	JSStringRelease(script);
	if (tab_reg == NULL && tab_bits == NULL) {
		return result;
	}
	else {
		if (tab_reg != NULL) {
			delete[] tab_reg;
		}
		if (tab_bits != NULL) {
			delete[] tab_bits;
		}
		return result;
	}
}




