#ifndef CANSAT_GAGAN
#include "./src/Adafruit_GPS_Library/Adafruit_GPS.h"
#include "./src/DS3231/DS3231.h"
//#include "./src/HPMA115S0/src/hpma115s0.h"
#include <Wire.h>
#include <String.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include "SD.h"
#include "Arduino.h"
 
#define MPU9250_ADDRESS 0x68
#define MAG_ADDRESS 0x0C
 
#define GYRO_FULL_SCALE_250_DPS 0x00 
#define GYRO_FULL_SCALE_500_DPS 0x08
#define GYRO_FULL_SCALE_1000_DPS 0x10
#define GYRO_FULL_SCALE_2000_DPS 0x18
 
#define ACC_FULL_SCALE_2_G 0x00 
#define ACC_FULL_SCALE_4_G 0x08
#define ACC_FULL_SCALE_8_G 0x10
#define ACC_FULL_SCALE_16_G 0x18

#define BMP280_ADDRESS (0x76) 
#define BMP280_CHIPID (0x58)  
extern TwoWire Wire;  /**< Forward declaration of Wire object */
#define  BMP280_REGISTER_DIG_T1  0x88
#define  BMP280_REGISTER_DIG_T2  0x8A
#define  BMP280_REGISTER_DIG_T3  0x8C
#define  BMP280_REGISTER_DIG_P1  0x8E
#define  BMP280_REGISTER_DIG_P2  0x90
#define  BMP280_REGISTER_DIG_P3  0x92
#define  BMP280_REGISTER_DIG_P4  0x94
#define  BMP280_REGISTER_DIG_P5  0x96
#define  BMP280_REGISTER_DIG_P6  0x98
#define  BMP280_REGISTER_DIG_P7  0x9A
#define  BMP280_REGISTER_DIG_P8  0x9C
#define  BMP280_REGISTER_DIG_P9  0x9E
#define  BMP280_REGISTER_CHIPID  0xD0
#define  BMP280_REGISTER_VERSION  0xD1
#define  BMP280_REGISTER_SOFTRESET  0xE0
#define  BMP280_REGISTER_CAL26  0xE1/**< R calibration = 0xE1-0xF0 */
#define  BMP280_REGISTER_CONTROL 0xF4
#define  BMP280_REGISTER_CONFIG  0xF5
#define  BMP280_REGISTER_PRESSUREDATA 0xF7
#define  BMP280_REGISTER_TEMPDATA  0xFA
#define  _i2caddr 0x76

typedef struct {
  uint16_t dig_T1; /**< dig_T1 cal register. */
  int16_t dig_T2;  /**<  dig_T2 cal register. */
  int16_t dig_T3;  /**< dig_T3 cal register. */

  uint16_t dig_P1; /**< dig_P1 cal register. */
  int16_t dig_P2;  /**< dig_P2 cal register. */
  int16_t dig_P3;  /**< dig_P3 cal register. */
  int16_t dig_P4;  /**< dig_P4 cal register. */
  int16_t dig_P5;  /**< dig_P5 cal register. */
  int16_t dig_P6;  /**< dig_P6 cal register. */
  int16_t dig_P7;  /**< dig_P7 cal register. */
  int16_t dig_P8;  /**< dig_P8 cal register. */
  int16_t dig_P9;  /**< dig_P9 cal register. */
} bmp280_calib_data;

/**
 * Driver for the Adafruit BMP280 barometric pressure sensor.
 */
class Adafruit_BMP280 {
public:
  Adafruit_BMP280(TwoWire *theWire = &Wire);

  bool begin(uint8_t addr = BMP280_ADDRESS, uint8_t chipid = BMP280_CHIPID);

  float readTemperature();

  

  float readPressure(void);

  float readAltitude(float seaLevelhPa = 1013.25);

  void setSampling();
  
  TwoWire *_wire; 

private:
  /** Encapsulates the config register */
  struct config {
    /** Inactive duration (standby time) in normal mode */
    unsigned int t_sb : 3;
    /** Filter settings */
    unsigned int filter : 3;
    /** Unused - don't set */
    unsigned int none : 1;
    /** Enables 3-wire SPI */
    unsigned int spi3w_en : 1;
    /** Used to retrieve the assembled config register's byte value. */
    unsigned int get() { return (t_sb << 5) | (filter << 2) | spi3w_en; }
  };

  /** Encapsulates trhe ctrl_meas register */
  struct ctrl_meas {
    /** Temperature oversampling. */
    unsigned int osrs_t : 3;
    /** Pressure oversampling. */
    unsigned int osrs_p : 3;
    /** Device mode */
    unsigned int mode : 2;
    /** Used to retrieve the assembled ctrl_meas register's byte value. */
    unsigned int get() { return (osrs_t << 5) | (osrs_p << 2) | mode; }
  };

  void readCoefficients(void);
  void write8(byte reg, byte value);
  uint8_t read8(byte reg);
  uint16_t read16(byte reg);
  uint32_t read24(byte reg);
  int16_t readS16(byte reg);
  uint16_t read16_LE(byte reg);
  int16_t readS16_LE(byte reg);

 
  int32_t _sensorID;
  int32_t t_fine;
  bmp280_calib_data _bmp280_calib;
  config _configReg;
  ctrl_meas _measReg;
};

Adafruit_BMP280::Adafruit_BMP280(TwoWire *theWire){
  _wire = theWire;
}

bool Adafruit_BMP280::begin(uint8_t addr, uint8_t chipid) {
    Serial.println("it is starting");
    _wire->begin();
 

  if (read8(BMP280_REGISTER_CHIPID) != chipid)
    return false;

  readCoefficients();
  setSampling();
  delay(100);
  return true;
}

void Adafruit_BMP280::setSampling() {
  _measReg.mode = 0x03;
  _measReg.osrs_t = 0x04;
  _measReg.osrs_p = 0x04;

  _configReg.filter = 0x02;
  _configReg.t_sb = 0x02;

  write8(BMP280_REGISTER_CONFIG, _configReg.get());
  write8(BMP280_REGISTER_CONTROL, _measReg.get());
}

void Adafruit_BMP280::write8(byte reg, byte value) {
    _wire->beginTransmission((uint8_t)_i2caddr);
    _wire->write((uint8_t)reg);
    _wire->write((uint8_t)value);
    _wire->endTransmission();
}

uint8_t Adafruit_BMP280::read8(byte reg) {
  uint8_t value;
    _wire->beginTransmission((uint8_t)_i2caddr);
    _wire->write((uint8_t)reg);
    _wire->endTransmission();
    _wire->requestFrom((uint8_t)_i2caddr, (byte)1);
    value = _wire->read();
  return value;
}

uint16_t Adafruit_BMP280::read16(byte reg) {
  uint16_t value;
    _wire->beginTransmission((uint8_t)_i2caddr);
    _wire->write((uint8_t)reg);
    _wire->endTransmission();
    _wire->requestFrom((uint8_t)_i2caddr, (byte)2);
    value = (_wire->read() << 8) | _wire->read();
  return value;
}

uint16_t Adafruit_BMP280::read16_LE(byte reg) {
  uint16_t temp = read16(reg);
  return (temp >> 8) | (temp << 8);
}

/*!
 *   @brief  Reads a signed 16 bit value over I2C/SPI
 */
int16_t Adafruit_BMP280::readS16(byte reg) { return (int16_t)read16(reg); }

int16_t Adafruit_BMP280::readS16_LE(byte reg) {
  return (int16_t)read16_LE(reg);
}

/*!
 *  @brief  Reads a 24 bit value over I2C/SPI
 */
uint32_t Adafruit_BMP280::read24(byte reg) {
  uint32_t value;
    _wire->beginTransmission((uint8_t)_i2caddr);
    _wire->write((uint8_t)reg);
    _wire->endTransmission();
    _wire->requestFrom((uint8_t)_i2caddr, (byte)3);

    value = _wire->read();
    value <<= 8;
    value |= _wire->read();
    value <<= 8;
    value |= _wire->read();


  return value;
}

/*!
 *  @brief  Reads the factory-set coefficients
 */
void Adafruit_BMP280::readCoefficients() {
  _bmp280_calib.dig_T1 = read16_LE(BMP280_REGISTER_DIG_T1);
  _bmp280_calib.dig_T2 = readS16_LE(BMP280_REGISTER_DIG_T2);
  _bmp280_calib.dig_T3 = readS16_LE(BMP280_REGISTER_DIG_T3);

  _bmp280_calib.dig_P1 = read16_LE(BMP280_REGISTER_DIG_P1);
  _bmp280_calib.dig_P2 = readS16_LE(BMP280_REGISTER_DIG_P2);
  _bmp280_calib.dig_P3 = readS16_LE(BMP280_REGISTER_DIG_P3);
  _bmp280_calib.dig_P4 = readS16_LE(BMP280_REGISTER_DIG_P4);
  _bmp280_calib.dig_P5 = readS16_LE(BMP280_REGISTER_DIG_P5);
  _bmp280_calib.dig_P6 = readS16_LE(BMP280_REGISTER_DIG_P6);
  _bmp280_calib.dig_P7 = readS16_LE(BMP280_REGISTER_DIG_P7);
  _bmp280_calib.dig_P8 = readS16_LE(BMP280_REGISTER_DIG_P8);
  _bmp280_calib.dig_P9 = readS16_LE(BMP280_REGISTER_DIG_P9);
}

/*!
 * Reads the temperature from the device.
 * @return The temperature in degress celcius.
 */
float Adafruit_BMP280::readTemperature() {
  int32_t var1, var2;

  int32_t adc_T = read24(BMP280_REGISTER_TEMPDATA);
  adc_T >>= 4;

  var1 = ((((adc_T >> 3) - ((int32_t)_bmp280_calib.dig_T1 << 1))) *
          ((int32_t)_bmp280_calib.dig_T2)) >>
         11;

  var2 = (((((adc_T >> 4) - ((int32_t)_bmp280_calib.dig_T1)) *
            ((adc_T >> 4) - ((int32_t)_bmp280_calib.dig_T1))) >>
           12) *
          ((int32_t)_bmp280_calib.dig_T3)) >>
         14;

  t_fine = var1 + var2;

  float T = (t_fine * 5 + 128) >> 8;
  return T / 100;
}

/*!
 * Reads the barometric pressure from the device.
 * @return Barometric pressure in hPa.
 */
float Adafruit_BMP280::readPressure() {
  int64_t var1, var2, p;

  // Must be done first to get the t_fine variable set up
  readTemperature();

  int32_t adc_P = read24(BMP280_REGISTER_PRESSUREDATA);
  adc_P >>= 4;

  var1 = ((int64_t)t_fine) - 128000;
  var2 = var1 * var1 * (int64_t)_bmp280_calib.dig_P6;
  var2 = var2 + ((var1 * (int64_t)_bmp280_calib.dig_P5) << 17);
  var2 = var2 + (((int64_t)_bmp280_calib.dig_P4) << 35);
  var1 = ((var1 * var1 * (int64_t)_bmp280_calib.dig_P3) >> 8) +
         ((var1 * (int64_t)_bmp280_calib.dig_P2) << 12);
  var1 =
      (((((int64_t)1) << 47) + var1)) * ((int64_t)_bmp280_calib.dig_P1) >> 33;

  if (var1 == 0) {
    return 0; // avoid exception caused by division by zero
  }
  p = 1048576 - adc_P;
  p = (((p << 31) - var2) * 3125) / var1;
  var1 = (((int64_t)_bmp280_calib.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
  var2 = (((int64_t)_bmp280_calib.dig_P8) * p) >> 19;

  p = ((p + var1 + var2) >> 8) + (((int64_t)_bmp280_calib.dig_P7) << 4);
  return (float)p / 256;
}


float Adafruit_BMP280::readAltitude(float seaLevelhPa) {
  float altitude;

  float pressure = readPressure(); // in Si units for Pascal
  pressure /= 100;

  altitude = 44330 * (1.0 - pow(pressure / seaLevelhPa, 0.1903)) ;

  return altitude;
}

//Adafruit_BMP280 bmp;


//void setupBMP() 
//{
//  bmp.begin();
// 
//}

//void UPDATEBMP() {
//    Serial.print(bmp.readTemperature());
//    Serial.print(bmp.readPressure());
//    Serial.print(bmp.readAltitude(1013.25));
//    Serial.println();
////    delay(2000);
//}

void I2Cread(uint8_t Address, uint8_t Register, uint8_t Nbytes, uint8_t* Data)
{
// Set register address
Wire.beginTransmission(Address);
Wire.write(Register);
Wire.endTransmission();
 
// Read Nbytes
Wire.requestFrom(Address, Nbytes);
uint8_t index=0;
while (Wire.available())
Data[index++]=Wire.read();
}
 
 
// Write a byte (Data) in device (Address) at register (Register)
void I2CwriteByte(uint8_t Address, uint8_t Register, uint8_t Data)
{
// Set register address
Wire.beginTransmission(Address);
Wire.write(Register);
Wire.write(Data);
Wire.endTransmission();
}
 
// Initial time
 
// Initializations
void setupMPU()
{
// Arduino initializations
Wire.begin();
 
// Set accelerometers low pass filter at 5Hz
I2CwriteByte(MPU9250_ADDRESS,29,0x06);
// Set gyroscope low pass filter at 5Hz
I2CwriteByte(MPU9250_ADDRESS,26,0x06);
 
 
// Configure gyroscope range
I2CwriteByte(MPU9250_ADDRESS,27,GYRO_FULL_SCALE_1000_DPS);
// Configure accelerometers range
I2CwriteByte(MPU9250_ADDRESS,28,ACC_FULL_SCALE_4_G);
// Set by pass mode for the magnetometers
I2CwriteByte(MPU9250_ADDRESS,0x37,0x02);
 
// Request continuous magnetometer measurements in 16 bits
I2CwriteByte(MAG_ADDRESS,0x0A,0x16);
}
void UPDATEMPU()
{
uint8_t Buf[14];
I2Cread(MPU9250_ADDRESS,0x3B,14,Buf);
 
// Create 16 bits values from 8 bits data
 
// Accelerometer
int16_t ax=-(Buf[0]<<8 | Buf[1]);
int16_t ay=-(Buf[2]<<8 | Buf[3]);
int16_t az=Buf[4]<<8 | Buf[5];
 
// Gyroscope
int16_t gx=-(Buf[8]<<8 | Buf[9]);
int16_t gy=-(Buf[10]<<8 | Buf[11]);
int16_t gz=Buf[12]<<8 | Buf[13];
 
// Display values
 
// Accelerometer
Serial.print (ax,DEC);
Serial.print (ay,DEC);
Serial.print (az,DEC);
 
// Gyroscope
Serial.print (gx,DEC);
Serial.print (gy,DEC);
Serial.print (gz,DEC);
 
 
// _____________________
// ::: Magnetometer :::
 
 
// Read register Status 1 and wait for the DRDY: Data Ready
 
uint8_t ST1;
do
{
I2Cread(MAG_ADDRESS,0x02,1,&ST1);
}
while (!(ST1&0x01));
 
// Read magnetometer data 
uint8_t Mag[7];
I2Cread(MAG_ADDRESS,0x03,7,Mag);
 
// Create 16 bits values from 8 bits data
 
// Magnetometer
int16_t mx=-(Mag[3]<<8 | Mag[2]);
int16_t my=-(Mag[1]<<8 | Mag[0]);
int16_t mz=-(Mag[5]<<8 | Mag[4]);
 
 
// Magnetometer
Serial.print (mx+200,DEC);
Serial.print (my-70,DEC);
Serial.print (mz-700,DEC);
 
}



#endif
#define CANSAT_GAGAN
