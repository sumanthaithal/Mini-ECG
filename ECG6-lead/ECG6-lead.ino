#include <SPI.h>
#include <CircularBuffer.h>
#include <MAX30100.h>
#include <MAX30100_BeatDetector.h>
#include <MAX30100_Filters.h>
#include <MAX30100_PulseOximeter.h>
#include <MAX30100_Registers.h>
#include <MAX30100_SpO2Calculator.h>


#include <Wire.h>
#include "MAX30100_PulseOximeter.h"

#define REPORTING_PERIOD_MS     50
 


PulseOximeter pox;


uint32_t tsLastReport = 0;

/*
  DRDYB       D4
  SDO         D12
  SDI         D13 
  SCLK        D14
  CSB         D15
  ALARMB      D2
*/
int32_t t1;
int32_t t2;
int32_t t3;
#include "Plotter.h"
Plotter p;
#include <SPI.h>
static const int spiClk = 1000000; // 1 MHz

const int pin_DRDYB = 6;  
const int pin_ALARMB = 5; 
const int pin_MISO = 12;  
const int pin_MOSI = 11; 
const int pin_SCLK = 13; 
const int pin_SS = 10; 

int32_t c1;
int32_t c2;
int32_t c3;

double cd1;
double cd2;
double cd3;

int32_t getValFromChannel(int channel)
{
  byte x1;
  byte x2;
  byte x3;

  switch (channel)
  {
  case 1:
    x1 = 0x37;
    x2 = 0x38;
    x3 = 0x39;
    break;
  case 2:
    x1 = 0x3A;
    x2 = 0x3B;
    x3 = 0x3C;
    break;
  case 3:
    x1 = 0x3D;
    x2 = 0x3E;
    x3 = 0x3F;
    break;
  }
  int32_t val;
  val = readRegister(x1);
  val = (val << 8) | readRegister(x2);
  val = (val << 8) | readRegister(x3);

  return val;
}

void setup_ECG_2_channel()
{
  writeRegister(0x01, 0x11);
  writeRegister(0x02, 0x19);
  writeRegister(0x0A, 0x07);
  writeRegister(0x0C, 0x04);
  writeRegister(0x12, 0x04);
  writeRegister(0x14, 0x24);
  writeRegister(0x21, 0x02);
  writeRegister(0x22, 0x02);
  writeRegister(0x23, 0x02);
  writeRegister(0x27, 0x08);
  writeRegister(0x2F, 0x30);
  writeRegister(0x00, 0x01);
}
void setup_ECG_3_channel()
{
  writeRegister(0x01, 0x11);
  writeRegister(0x02, 0x19);
  writeRegister(0x03, 0x2E); 
  writeRegister(0x0A, 0x07);
  writeRegister(0x0C, 0x04);
  writeRegister(0x0D, 0x01);
  writeRegister(0x0E, 0x02);
  writeRegister(0x0F, 0x03);
  writeRegister(0x10, 0x01);
  writeRegister(0x12, 0x04);
  writeRegister(0x21, 0x02);
  writeRegister(0x22, 0x02);
  writeRegister(0x23, 0x02);
  writeRegister(0x24, 0x02); 
  writeRegister(0x27, 0x08);
  writeRegister(0x2F, 0x70);
  writeRegister(0x00, 0x01);
}

byte readRegister(byte reg)
{
  byte data;
  reg |= 1 << 7;
  SPI.beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  digitalWrite(pin_SS, LOW);
  SPI.transfer(reg);
  data = SPI.transfer(0);
  digitalWrite(pin_SS, HIGH);
  SPI.endTransaction();
  return data;
}

void writeRegister(byte reg, byte data)
{
  reg &= ~(1 << 7);
  SPI.beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  digitalWrite(pin_SS, LOW);
  SPI.transfer(reg);
  SPI.transfer(data);
  digitalWrite(pin_SS, HIGH);
  SPI.endTransaction();
}

void setup()
{
  if (!pox.begin()) {
       // Serial.println("FAILED");
        for(;;);
    } else {
       // Serial.println("SUCCESS");
    }

    pox.setIRLedCurrent(MAX30100_LED_CURR_14_2MA);

  pinMode(pin_DRDYB, INPUT);
  pinMode(pin_ALARMB, INPUT);
  pinMode(pin_SS, OUTPUT);

  Serial.begin(115200);
  //hspi.begin(pin_SCLK, pin_MISO, pin_MOSI, pin_SS);
  //SPI.begin(pin_SCLK, pin_MISO, pin_MOSI, pin_SS);
SPI.begin();
  setup_ECG_3_channel();

//  p.Begin();
//  p.AddTimeGraph("3 channel graph", 1000, "c3 label", cd3, "c2 label", cd2, "c1 label", cd1);
}

void loop()
{
 
  pox.update();


    
  // if (digitalRead(pin_ALARMB) == false)
  // {
  //   Serial.println("alarm raised");
  // }
   if (digitalRead(pin_DRDYB) == false)
   {

  // sampled data is located at 3 8-bit
  //--CHANNEL 1
  c1 = getValFromChannel(1);
 //  cd1 = (double)c1;
 cd1 =c1;
 //  cd1=map(cd1,6000000,6200000,-255,255);
 Serial.print(cd1);

  Serial.print(" ");
  //--CHANNEL 2
  c2 = getValFromChannel(2);
//   cd2 = (double)c2;
   cd2 =c2;
//   cd2=map(cd2,6000000,6200000,-255,255);
Serial.print(cd2);
t1=-(c1+c2)/2;
//Serial.print(t1);
//Serial.print(" ");
t2=(c1-c2)/2;
//Serial.print(t2);
//Serial.print(" ");
t3=(c2-c1)/2;
//Serial.println(t3);
  Serial.print(" ");
  //--CHANNEL 3
  c3 = getValFromChannel(3);
//   cd3 = (double)c3;
cd3 =c3;
//  cd3=map(cd3,6000000,6200000,-255,255);
  Serial.print(cd3);
  Serial.print(" ");
  // p.Plot();
   
   }  
          Serial.print(pox.getHeartRate());
        Serial.print(" ");
        Serial.println(pox.getSpO2());
  delay(5);
    tsLastReport = millis();
    
}
