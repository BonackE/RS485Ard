#include "ModbusFunctions.h"
#include <Windows.h>
//Function only works for windows, will need to be changed for other OS
//Returns a vector of strings containing all serial ports on the system (that are being used)
std::vector<std::string> ModbusFunctions::getSerialPorts() {
	std::vector<std::string> port_list;

	

	DWORD test;
	TCHAR lptargetpath[5000];

	for (int i = 0; i < 255; i++) {
		std::string num = std::to_string(i);
		std::string port = "COM" + num;

		test = QueryDosDevice(port.c_str(), lptargetpath, 5000);
		if (test != 0) {
			port_list.push_back(port);
		}
	}
	if (port_list.size() == 0) {
		port_list.push_back("No Ports Found");
	}

	return port_list;

}
//Function to return a string of hex values from a uint16_t array
std::string getHexCode(uint16_t* values, int size) {
	
	//Converts the uint16_t array to a string of hex values
	std::string hexCode = "";
	for (int i = 0; i < size; i++) {
		std::stringstream stream;
		stream << std::hex << values[i];
		std::string result(stream.str());
		hexCode += result;
	}
	return hexCode;

}