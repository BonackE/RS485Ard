#include "ModbusFunctions.h"
#include <Windows.h>
#include <WinSock2.h>
#define MAX_MESSAGE_LENGTH 260
//Function only works for windows, will need to be changed for other OS
//Returns a vector of strings containing all serial ports on the system (that are being used)
typedef enum {
	_STEP_FUNCTION,
	_STEP_META,
	_STEP_DATA
} _step_t;
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
std::string ModbusFunctions::getHexReq(int function, int addr, int nb, uint16_t* src){
	if (ctx == NULL) {
		return "No Context";
	}
	int req_length;
	
	uint8_t req[MAX_MESSAGE_LENGTH];
	
	
	
	
	req_length = ctx->backend->build_request_basis(ctx, function, addr, nb, req);
	if (function == 0x10) {
		int byte_count = nb * 2;
		req[req_length++] = byte_count;
		for (int i = 0; i < nb; i++) {
			req[req_length++] = src[i] >> 8;
			req[req_length++] = src[i] & 0x00FF;
		}
	}
	req_length = ctx->backend->send_msg_pre(req, req_length);
	std::stringstream ss;
	std::string reqStr = "";
	for (int i = 0; i < req_length; i++) {
		ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(req[i]) << " ";
		
	}
	reqStr = ss.str();
	return reqStr;
	
	
}
std::string ModbusFunctions::getHexReq(int function, int addr, int nb, uint8_t* src) {
	int req_length;
	uint8_t req[MAX_MESSAGE_LENGTH];
	
	req_length = ctx->backend->build_request_basis(ctx, function, addr, nb, req);
	if (function == 0x0F) {
		int bit_check = 0;
		int pos = 0;
		int byte_count = (nb / 8) + ((nb % 8) ? 1 : 0);
		req[req_length++] = byte_count;

		for (int i = 0; i < byte_count; i++) {
			int bit;

			bit = 0x01;
			req[req_length] = 0;

			while ((bit & 0xFF) && (bit_check++ < nb)) {
				if (src[pos++])
					req[req_length] |= bit;
				else
					req[req_length] &= ~bit;

				bit = bit << 1;
			}
			req_length++;
		}
	}
	
	req_length = ctx->backend->send_msg_pre(req, req_length);
	std::stringstream ss;
	std::string reqStr = "";
	for (int i = 0; i < req_length; i++) {
		ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(req[i]) << " ";

	}
	reqStr = ss.str();
	return reqStr;


}
std::string ModbusFunctions::getHexResp() {
    std::stringstream ss;
    std::string respStr = "";

	if (ctx->response_length == 0) {
		return "No Response";
	}
	uint8_t* response = new(uint8_t[ctx->response_length]);
    

    // Add the slave ID, function code, data, and CRC to the response message

    for (int i = 0; i < ctx->response_length; i++) {
        response[i] = ctx->response[i];
    }
    //uint16_t crc = calculate_crc(response, ctx->response_length);
    for (int i = 0; i < ctx->response_length; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(response[i]) << " ";
    }

    respStr = ss.str();

	delete[] response;
    return respStr;
}

