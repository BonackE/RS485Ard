#include "ModbusFunctions.h"
#include <Windows.h>
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