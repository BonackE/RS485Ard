#ifndef MODBUSFUNCTIONS_H
#define MODBUSFUNCTIONS_H

#include <vector>
#include <modbus.h>
#include <modbus-private.h>

#include <iostream>
#include <string>
class ModbusFunctions
{
public:
	//Returns a vector of strings containing all serial ports on the system (that are being used)
	std::vector<std::string> getSerialPorts();
	std::string getHexReq(uint16_t* values, int size);
	modbus_t* ctx;

};

#endif