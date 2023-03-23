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
	static JSModbusWrapper& GetInstance() {
		static JSModbusWrapper instance;
		return instance;
	}
	void SetView(View* view);
	JSValueRef LoadPortsFunc(JSContextRef ctx);
	JSValueRef ConnectFunc(JSContextRef ctx);
private:
	JSModbusWrapper() {}
	JSModbusWrapper(const JSModbusWrapper& other) = delete;
	JSModbusWrapper& operator=(const JSModbusWrapper& other) = delete;
	ModbusFunctions modbus;
	View* view;
};

#endif // !
