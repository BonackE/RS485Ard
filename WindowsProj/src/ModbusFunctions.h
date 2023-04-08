#ifndef MODBUSFUNCTIONS_H
#define MODBUSFUNCTIONS_H
#include <vector>
#include <modbus.h>
#include <iostream>
#include <string>
class ModbusFunctions
{
public:
	//Returns a vector of strings containing all serial ports on the system (that are being used)
	std::vector<std::string> getSerialPorts();
	modbus_t* ctx;

};

#endif