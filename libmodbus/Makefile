output: modbus.o
	gcc modbus.o -o output

modbus.o: modbus.c
	gcc -I.  modbus.c modbus-data.c modbus-rtu.c modbus-tcp.c