// ModBusProgram.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <modbus.h>
#include <windows.h>
#include <string>
#include <vector>



union {
	float f;
	uint16_t i[2];
}register2float;
//int main() {
//
//    uint16_t tab[10];
//    modbus_t* ctx;
//    ctx = modbus_new_rtu("COM3", 9600, 'N', 8, 1);
//    if (ctx == NULL) {
//        fprintf(stderr, "Unable to create the libmodbus context\n");
//        return -1;
//    }
//
//    if (modbus_connect(ctx) == -1) {
//        fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
//        modbus_free(ctx);
//        return -1;
//    }
//    modbus_set_slave(ctx, 1);
//    modbus_read_input_registers(ctx, 0, 10, tab);
//    char x;
//    for (int i = 0; i < 10; i += 2) {
//        register2float.i[0] = tab[i];
//        register2float.i[1] = tab[i + 1];
//        printf("Register %d: %f\n", i, register2float.f);
//    }
//    std::cin >> x;
// 
//    modbus_free(ctx);
//    return 0;
//}
#define ID_BUTTON1 1001
#define ID_TEXT 1002

using namespace std;


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
modbus_t* ctx;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR pCmdLine, int nCmdShow) {
	// Register the window class.
	const wchar_t MAIN_CLASS[] = L"Tester";

	WNDCLASS wc = { };

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = MAIN_CLASS;

	RegisterClass(&wc);

	// Create window
	HWND hwnd = CreateWindowEx(
		0,                              // Optional window styles.
		MAIN_CLASS,                     // Window class
		L"Tester",    // Window text
		WS_OVERLAPPEDWINDOW,            // Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);









	if (hwnd == NULL)
	{
		return 0;
	}
	ShowWindow(hwnd, nCmdShow);

	// Run the message loop.

	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}
//Function that creates main window




LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:

		PostQuitMessage(0);
		return 0;
		//case for button press
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case ID_BUTTON1:
		{
			vector<float> data;
			uint16_t tab[10];

			


			modbus_read_input_registers(ctx, 0, 10, tab);

			for (int i = 0; i < 10; i += 2) {
				register2float.i[0] = tab[i];
				register2float.i[1] = tab[i + 1];
				data.push_back(register2float.f);
			}
			
			//make wstring of data vector
			wstring str;
			for (int i = 0; i < data.size(); i++) {
				str += to_wstring(data[i]);
				str += L" ";
			}
			
			
			SetWindowText(GetDlgItem(hwnd, ID_TEXT), str.c_str());

			



			return 0;
		}
		}
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
		RECT rect;
		GetClientRect(hwnd, &rect);
		DrawText(hdc, L"Input string of numbers seperated by spaces", -1, &rect, DT_SINGLELINE | DT_CENTER | DT_TOP);
		MoveWindow(GetDlgItem(hwnd, ID_TEXT), 0, 50, rect.right, 25, TRUE); //Change size of text box
		MoveWindow(GetDlgItem(hwnd, ID_BUTTON1), rect.right / 2 - 50, 100, 100, 25, TRUE); //Change size of button
		EndPaint(hwnd, &ps);

		return 0;

	}
	//case for creating all controls
	case WM_CREATE:
	{
		RECT rect;
		GetClientRect(hwnd, &rect);
		

		CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL, 0, 50, rect.right, 25, hwnd, (HMENU)ID_TEXT, NULL, NULL);
		
		CreateWindowEx(WS_EX_CLIENTEDGE, L"BUTTON", L"Submit", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, rect.right / 2 - 50, 100, 100, 25, hwnd, (HMENU)ID_BUTTON1, NULL, NULL);
		ctx = modbus_new_rtu("COM3", 9600, 'N', 8, 1);
		if (ctx == NULL) {
			SetWindowText(GetDlgItem(hwnd, ID_TEXT), L"Unable to create libmodbus");
			modbus_free(ctx);
			return -1;
		}

		if (modbus_connect(ctx) == -1) {
			SetWindowText(GetDlgItem(hwnd, ID_TEXT), L"Connection Failed");
			modbus_free(ctx);


			return -1;
		}
		modbus_set_slave(ctx, 1);
		return 0;
	}
	//case for updating position of text everytime window is resized
	case WM_SIZE:
	{
		RECT rect;
		GetClientRect(hwnd, &rect);
		InvalidateRect(hwnd, &rect, TRUE);
		return 0;
	}

	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
