# RS485Ard
A software that provides communication with MODBUS devices through RTU connection with either RS-485 or RS-232 Protocol.
This uses 2 libraries, which is Libmodbus and Ultralight
Ultralight is supplied to create an ease of access GUI for Windows.

MacOS does not provide Ultralight and is in Swift language.

Prerequisites:
  - Need CMake to create Windows portion of the program.

To install:
  1. Make a build directory in the main file.
  2. Run the following commands:
    cmake ..
    cmake --build . --config Release
  3. That's it, from there, feel free to edit the code all you want. This is an open source project.

