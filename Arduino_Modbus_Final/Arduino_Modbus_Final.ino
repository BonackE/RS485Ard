#include <ArduinoRS485.h>
#include <ArduinoModbus.h>
#include <OneWire.h>
#include <DallasTemperature.h>
//Sample Code for Peripheral Sensor Inputs
union {
  float f;
  uint16_t i[2];
} float2register;


const int INTERVAL_MS = 15000;
unsigned long previousMillis = 0;
float volt_read, temp_read, pH_read, cond_read, turb_read;                      
float Voltage, Temperature, pH, Conductivity, Turbidity;
const int numCoils = 10;
const int numDiscreteInputs = 10;
const int numHoldingRegisters = 10;
const int numInputRegisters = 10;
OneWire oneWire(5); // Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature tempSensor(&oneWire);
void setup(){
  Serial.begin(9600);
  while(!Serial);

  if(!ModbusRTUServer.begin(1,9600)){
    Serial.println("Failed to start");
    while(1);

  }
  
  ModbusRTUServer.configureCoils(0x00,numCoils);
  ModbusRTUServer.configureDiscreteInputs(0x00,numDiscreteInputs);
  ModbusRTUServer.configureHoldingRegisters(0x00,numHoldingRegisters);
  ModbusRTUServer.configureInputRegisters(0x00,numInputRegisters);
  
  pinMode(13, OUTPUT);
  tempSensor.begin();
  tempSensor.setResolution(12);
}

void loop(){    
  //Voltage - Registers 00 and 01
                     //set pull-up on analog pin
      //  volt_read = 2.25;          //read the input pin
       //add code to calcuate voltage
       //add code to store voltage in modbus registers 00 and 01
    unsigned long currentMillis = millis();
    if(currentMillis-previousMillis >= INTERVAL_MS){
 

      previousMillis = currentMillis;
      digitalWrite(A0,LOW);
      volt_read = analogRead(0);
      Voltage = (volt_read*3.3/1024)*1000;
      float2register.f = Voltage;
      ModbusRTUServer.inputRegisterWrite(0, float2register.i[1]);
      ModbusRTUServer.inputRegisterWrite(1, float2register.i[0]);

      

      tempSensor.requestTemperatures();
    Temperature = tempSensor.getTempCByIndex(0) + 1.5;
    float2register.f = Temperature;

      ModbusRTUServer.inputRegisterWrite(2,float2register.i[1] );
      ModbusRTUServer.inputRegisterWrite(3, float2register.i[0]);

      
    }

    ModbusRTUServer.poll();

  delay(100);

  
}

