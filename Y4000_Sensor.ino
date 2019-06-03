#include <ModbusMaster.h>

#define RS485Transmit    HIGH
#define RS485Receive     LOW
// Pin control receive and transmit mode rs485
#define SSDirection 8

// Define sensor register for y4000 yosemitech
#define DO1 0
#define DO2 1
#define TUR1 2
#define TUR2 3
#define CT1 4
#define CT2 5
#define PH01 6
#define PH02 7
#define TEMP1 8
#define TEMP2 9
#define ORP1 10
#define ORP2 11
#define CHL1 12
#define CHL2 13
#define BGA1 14
#define BGA2 15

// instantiate ModbusMaster object
ModbusMaster node;

// Variables to store sensors values
float dO = 0, tur = 0, ct = 0, ph = 0, temp = 0, orp = 0, chl = 0, bga = 
0;

void preTransmission()
{
  digitalWrite(SSDirection, RS485Transmit);
}

void postTransmission()
{
  digitalWrite(SSDirection, RS485Receive);
}

float readFloatValue(uint32_t byte01, uint32_t byte23)
{
  int32_t realbyte0,realbyte1,realbyte2,realbyte3;
  uint8_t S;
  int32_t E,M;
  float D;
  realbyte3 = highByte(byte01);
  realbyte2 = lowByte(byte01);
  realbyte1 = highByte(byte23);
  realbyte0 = lowByte(byte23);
  if((realbyte0 & 0x80) == 0)
  {
    S = 0; //Positive
  }
  else
  {
    S = 1; //Negative
  }
  E = (((uint8_t)(realbyte0 << 1)) | (realbyte1 & 0x80) >> 7) - 127;
  M = ((realbyte1&0x7f) << 16) | (realbyte2<< 8)| realbyte3;
  D = pow(-1,S)*(1.0 + M/pow(2,23))* pow(2,E);
  return D;
}

void setup()
{
  pinMode(SSDirection, OUTPUT);
  
  // Init the transceiver mode
  digitalWrite(SSDirection, RS485Receive);
  
  
  // Initialize Modbus communication baud rate
  Serial.begin(9600);
  Serial1.begin(9600);

  // Communicate with Modbus slave ID 1 over Serial (port 1)
  node.begin(1, Serial1);

  // Call back allow us to configure RS485 transceiver correctly
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  // Active brush of Yosemitech Y4000 sensor
  uint8_t transBuffer = node.setTransmitBuffer(0x00, 0x0000);
  uint8_t activeBrush = node.writeMultipleRegisters(0x2F00,0);
  node.clearTransmitBuffer();
}


void loop()
{
  uint8_t result = node.readHoldingRegisters(0x2601,16);

  //Read data respond from Y4000 sensor
  if (result == node.ku8MBSuccess)
  {
    dO = readFloatValue(node.getResponseBuffer(DO1), 
node.getResponseBuffer(DO2));
    tur = readFloatValue(node.getResponseBuffer(TUR1), 
node.getResponseBuffer(TUR2));
    ct = readFloatValue(node.getResponseBuffer(CT1), 
node.getResponseBuffer(CT2));
    ph = readFloatValue(node.getResponseBuffer(PH01), 
node.getResponseBuffer(PH02));
    temp = readFloatValue(node.getResponseBuffer(TEMP1), 
node.getResponseBuffer(TEMP2));
    orp = readFloatValue(node.getResponseBuffer(ORP1), 
node.getResponseBuffer(ORP2));
    chl = readFloatValue(node.getResponseBuffer(CHL1), 
node.getResponseBuffer(CHL2));
    bga = readFloatValue(node.getResponseBuffer(BGA1), 
node.getResponseBuffer(BGA2));
  }

  //Send data to serial port in dictionary form
  Serial.print('{');
  Serial.print("'DO': ");
  Serial.print(dO);
  Serial.print(", 'Tur': ");
  Serial.print(tur);
  Serial.print(", 'CT': ");
  Serial.print(ct);
  Serial.print(", 'pH': ");
  Serial.print(ph);
  Serial.print(", 'Temp': ");
  Serial.print(temp);
  Serial.print(", 'Orp': ");
  Serial.print(orp);
  Serial.print(", 'Chl': ");
  Serial.print(chl);
  Serial.print(", 'BGA': ");
  Serial.print(bga);
  Serial.println('}');

  delay(3000);
}
