#ifndef JSMODBUSWRAPPER_H
#define JSMODBUSWRAPPER_H
#include <vector>
#include <iostream>
#include <string>


#include "ModbusFunctions.h"
#include "MyApp.h"
using namespace ultralight;
class JSModbusWrapper
{
public:
	//Create singleton
	static JSModbusWrapper& GetInstance() {
		static JSModbusWrapper instance;
		return instance;
	}
	
	~JSModbusWrapper() {}
	//Set the view for current program context
	void SetView(View* view);
	//Function return for loading ports in HTML
	JSValueRef LoadPortsFunc(JSContextRef ctx);
	//Returns whether the connection was successful in HTML
	JSValueRef ConnectFunc(JSContextRef ctx);
	//Returns all data from the modbus device (Only function 4 capable right now)
	JSValueRef RequestFunc(JSContextRef ctx);

private:
	JSModbusWrapper() {
		modbus = ModbusFunctions();
		view = nullptr;
	}
	JSModbusWrapper(const JSModbusWrapper& other) = delete;
	JSModbusWrapper& operator=(const JSModbusWrapper& other) = delete;
	ModbusFunctions modbus;
	View* view;

};

#endif // !
