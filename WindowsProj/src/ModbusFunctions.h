#ifndef MODBUSFUNCTIONS_H
#define MODBUSFUNCTIONS_H
#include <vector>
#include <modbus.h>
#include <iostream>
#include <string>
class ModbusFunctions
{
public:
	std::vector<std::string> getSerialPorts();
	modbus_t* ctx;

};

#endif