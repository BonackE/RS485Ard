#ifndef MODBUSFUNCTIONS_H
#define MODBUSFUNCTIONS_H

#include <vector>
#include <modbus.h>
#include <modbus-private.h>
#include <modbus-rtu.h>
#include <modbus-rtu-private.h>
#include <time.h>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
class ModbusFunctions
{
public:
	//Returns a vector of strings containing all serial ports on the system (that are being used)
	std::vector<std::string> getSerialPorts();
	std::string getHexReq(int function,int addr,int nb, uint16_t* src);
	std::string getHexReq(int function, int addr, int nb, uint8_t* src);
	std::string getHexResp();
	modbus_t* ctx;

};

#endif