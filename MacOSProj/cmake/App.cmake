set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(MODBUS_INCLUDES /opt/homebrew/Cellar/libmodbus/3.1.10/include/modbus)

find_library(MODBUS NAMES modbus)
find_package(wxWidgets REQUIRED COMPONENTS net core base)
include(${wxWidgets_USE_FILE})
