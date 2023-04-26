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
std::string ModbusFunctions::getHexReq(int function, int addr, int nb){
	int req_length;
	uint8_t req[_MIN_REQ_LENGTH];

	req_length = ctx->backend->build_request_basis(ctx, function, addr, nb, req);
	req_length = ctx->backend->send_msg_pre(req, req_length);
	std::stringstream ss;
	std::string reqStr = "";
	for (int i = 0; i < req_length; i++) {
		ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(req[i]) << " ";
		
	}
	reqStr = ss.str();
	return reqStr;
	
	
}
std::string ModbusFunctions::getHexResp(uint16_t* data, int function,int resp_length) {
    std::stringstream ss;
    std::string respStr = "";

    // Copy the uint16_t data into a temporary uint8_t array
    uint8_t* resp = (uint8_t*)malloc((resp_length * 2 + 4) * sizeof(uint8_t));

    
    resp[0] =ctx->slave;
    resp[1] = function;
    int msg_len = 2;
    // Copy the uint16_t data into the message buffer as two separate uint8_t values
    for (int i = 0; i < resp_length; i++) {
        resp[msg_len++] = (data[i] >> 8) & 0xff;
        resp[msg_len++] = data[i] & 0xff;
    }

    // Calculate the CRC of the response message
    uint16_t crc = calculate_crc(resp, msg_len);

    // Add the slave ID, function code, data, and CRC to the response message

    for (int i = 0; i < msg_len; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(resp[i]) << " ";
    }
    ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(crc & 0xff) << " ";
    ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>((crc >> 8) & 0xff) << " ";
    
    respStr = ss.str();

    free(resp);
    return respStr;
}
std::string ModbusFunctions::getHexResp(uint8_t* data, int function, int resp_length) {
    std::stringstream ss;
    std::string respStr = "";

    // Calculate the number of bytes needed to store the uint8_t array
    int byte_count = resp_length / 8;
    if (resp_length % 8 != 0) {
        byte_count++;
    }

    // Allocate memory for the response message and initialize it to zero
    uint8_t* resp = (uint8_t*)calloc(byte_count + 4, sizeof(uint8_t));

    // Initialize the first two bytes of the response message
    resp[0] = ctx->slave;
    resp[1] = function;
    int msg_len = 2;

    // Copy the uint8_t data into the message buffer as individual bits
    for (int i = 0; i < resp_length; i++) {
        int byte_index = i / 8;
        int bit_index = i % 8;
        resp[msg_len + byte_index] |= (data[i] << bit_index);
    }

    // Calculate the CRC of the response message
    uint16_t crc = calculate_crc(resp, msg_len + byte_count);

    // Add the slave ID, function code, data, and CRC to the response message
    for (int i = 0; i < msg_len + byte_count; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(resp[i]) << " ";
    }
    ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(crc & 0xff) << " ";
    ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>((crc >> 8) & 0xff) << " ";

    respStr = ss.str();

    free(resp);
    return respStr;
}
uint16_t ModbusFunctions::calculate_crc(uint8_t* data, int length) {
    uint16_t crc = 0xFFFF;
    int i, j;
    for (i = 0; i < length; i++) {
        crc ^= (uint16_t)data[i];
        for (j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc >>= 1;
                crc ^= 0xA001;
            }
            else {
                crc >>= 1;
            }
        }
    }
    return crc;
}
