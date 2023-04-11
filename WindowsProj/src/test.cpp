#include <iostream>
#include "MyApp.h"

int main() {
	AllocConsole();
	

	MyApp app;
	freopen("CON", "w", stdout);
	app.Run();
	
	
	FreeConsole();
	return 0;
}