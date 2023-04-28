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
	std::string getHexReq(int function,int addr,int nb);
	std::string getHexResp(uint16_t* data, int functin,int resp_length);
	std::string getHexResp(uint8_t* data, int function, int resp_length);
	std::string getHexRespWrite(uint16_t* data, int function, int resp_length, int start_addr, int num_regs_written);
	uint16_t calculate_crc(uint8_t* data, int length);
	modbus_t* ctx;

};

#endif