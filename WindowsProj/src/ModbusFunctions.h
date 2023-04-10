#ifndef MODBUSFUNCTIONS_H
#define MODBUSFUNCTIONS_H
#pragma comment(lib,"modbus.lib")
#include <vector>
#include <modbus.h>
#include <iostream>
#include <string>
#include <sstream>
class ModbusFunctions
{
public:
	//Returns a vector of strings containing all serial ports on the system (that are being used)
	std::vector<std::string> getSerialPorts();
	std::string getHexCode(uint16_t* values, int size);
	modbus_t* ctx;

};

#endif