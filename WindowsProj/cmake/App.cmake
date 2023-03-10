set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(APP_NAME ${CMAKE_PROJECT_NAME})
set(MODBUS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/libmodbus)
set(MODBUS_BIN ${MODBUS_DIR}/bin)
set(MODBUS_LIB ${MODBUS_DIR}/libs)
set(MODBUS_INCLUDES ${MODBUS_DIR}/includes)
set(ULTRA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/ultralight)
set(ULTRA_BIN ${ULTRA_DIR}/bin)
set(ULTRA_LIB ${ULTRA_DIR}/lib)
set(ULTRA_INCLUDES ${ULTRA_DIR}/include)
set(ULTRA_RSC_DIR ${ULTRA_DIR}/resources)
add_library(modbus SHARED IMPORTED)
add_library(UltralightCore SHARED IMPORTED)
add_library(AppCore SHARED IMPORTED)
add_library(Ultralight SHARED IMPORTED)
add_library(WebCore SHARED IMPORTED)
link_libraries(UltralightCore AppCore Ultralight WebCore)
set_target_properties(UltralightCore PROPERTIES IMPORTED_LOCATION ${ULTRA_BIN}/UltralightCore.dll)
set_target_properties(UltralightCore PROPERTIES IMPORTED_IMPLIB ${ULTRA_LIB}/UltralightCore.lib)
set_target_properties(UltralightCore PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${ULTRA_INCLUDES})
set_target_properties(AppCore PROPERTIES IMPORTED_LOCATION ${ULTRA_BIN}/AppCore.dll)
set_target_properties(AppCore PROPERTIES IMPORTED_IMPLIB ${ULTRA_LIB}/AppCore.lib)
set_target_properties(AppCore PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${ULTRA_INCLUDES})
set_target_properties(Ultralight PROPERTIES IMPORTED_LOCATION ${ULTRA_BIN}/Ultralight.dll)
set_target_properties(Ultralight PROPERTIES IMPORTED_IMPLIB ${ULTRA_LIB}/Ultralight.lib)
set_target_properties(Ultralight PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${ULTRA_INCLUDES})
set_target_properties(WebCore PROPERTIES IMPORTED_LOCATION ${ULTRA_BIN}/WebCore.dll)
set_target_properties(WebCore PROPERTIES IMPORTED_IMPLIB ${ULTRA_LIB}/WebCore.lib)
set_target_properties(WebCore PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${ULTRA_INCLUDES})


set_target_properties(modbus PROPERTIES IMPORTED_LOCATION ${MODBUS_BIN}/modbus.dll)
set_target_properties(modbus PROPERTIES IMPORTED_IMPLIB ${MODBUS_LIB}/modbus.lib)
set_target_properties(modbus PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${MODBUS_INCLUDES})
