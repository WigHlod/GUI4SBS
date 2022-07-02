/// Define SDA Interface.
#define SDA_PORT PORTC
#define SDA_PIN 4

/// Define SCL Interface.
#define SCL_PORT PORTC
#define SCL_PIN 5

/// Include Externals libs.
#include <Wire.h>
#include <SoftI2CMaster.h>

// standard I2C address for Smart Battery packs.
byte deviceAddress = 11;

// Standard Smart Battery commands.
#define BATTERY_MODE             0x03
#define TEMPERATURE              0x08
#define VOLTAGE                  0x09
#define CURRENT                  0x0A
#define RELATIVE_SOC             0x0D
#define ABSOLUTE_SOC             0x0E
#define REMAINING_CAPACITY       0x0F
#define FULL_CHARGE_CAPACITY     0x10

#define TIME_TO_FULL             0x13
#define CHARGING_CURRENT         0x14
#define CHARGING_VOLTAGE         0x15
#define BATTERY_STATUS           0x16
#define CYCLE_COUNT              0x17
#define DESIGN_CAPACITY          0x18
#define DESIGN_VOLTAGE           0x19

#define SPEC_INFO                0x1A
#define MFG_DATE                 0x1B
#define SERIAL_NUM               0x1C
#define MFG_NAME                 0x20   
#define DEV_NAME                 0x21   
#define CELL_CHEM                0x22   
#define MFG_DATA                 0x23   
#define MFG_DATA3B               0x4F   
#define CELL3_VOLTAGE            0x3D
#define CELL2_VOLTAGE            0x3E
#define CELL1_VOLTAGE            0x3F
#define STATE_OF_HEALTH          0x54

#define PF_ALERT                 0x52
#define PF_STATUS                0x46

#define bufferLen 32
uint8_t i2cBuffer[bufferLen];

void setup() {
  Serial.begin(9600);
  
   while (!Serial) {    
    ;
  }

  //Serial.println("Serial Initialized");
  
  i2c_init();
}

int fetchWord(byte func)
{
  i2c_start(deviceAddress<<1 | I2C_WRITE);
  i2c_write(func);
  i2c_rep_start(deviceAddress<<1 | I2C_READ);
  byte b1 = i2c_read(false);
  byte b2 = i2c_read(true);
  i2c_stop();
  return (int)b1|((( int)b2)<<8);
}

uint8_t i2c_smbus_read_block ( uint8_t command, uint8_t* blockBuffer, uint8_t blockBufferLen ) 
{
  uint8_t x, num_bytes;
  i2c_start((deviceAddress<<1) + I2C_WRITE);
  i2c_write(command);
  i2c_rep_start((deviceAddress<<1) + I2C_READ);
  num_bytes = i2c_read(false); // num of bytes; 1 byte will be index 0
  num_bytes = constrain(num_bytes,0,blockBufferLen-2); // room for null at the end
  for (x=0; x<num_bytes-1; x++) { // -1 because x=num_bytes-1 if x<y; last byte needs to be "nack"'d, x<y-1
    blockBuffer[x] = i2c_read(false);
  }
  blockBuffer[x++] = i2c_read(true); // this will nack the last byte and store it in x's num_bytes-1 address.
  blockBuffer[x] = 0; // and null it at last_byte+1
  i2c_stop();
  return num_bytes;
}


void loop() {

uint8_t length_read = 0;
Serial.println("======== Global Data ========");
Serial.println(""); 

Serial.print("Manufacturer Name: ");
//  display.print("Manu: ");
 
length_read = i2c_smbus_read_block(MFG_NAME, i2cBuffer, bufferLen);
//display.println(Serial.write(i2cBuffer, length_read));
Serial.write(i2cBuffer, length_read);
Serial.println(" ");

String formatted_date = "Manufacture Date: ";
int mdate = fetchWord(MFG_DATE);
int mday = B00011111 & mdate;
int mmonth = mdate>>5 & B00001111;
int myear = 1980 + (mdate>>9 & B01111111);
formatted_date += myear;
formatted_date += "-";
formatted_date += mmonth;
formatted_date += "-";
formatted_date += mday;
Serial.println(formatted_date);

Serial.print("Device Name: ");
length_read = i2c_smbus_read_block(DEV_NAME, i2cBuffer, bufferLen);
Serial.write(i2cBuffer, length_read);
Serial.println("");

Serial.print("Serial Number: ");
Serial.println(fetchWord(SERIAL_NUM));

Serial.print("Chemistry Type: ");
length_read = i2c_smbus_read_block(CELL_CHEM, i2cBuffer, bufferLen);
Serial.write(i2cBuffer, length_read);
Serial.println("");

Serial.print("Design Capacity: " );
Serial.print(fetchWord(DESIGN_CAPACITY));
Serial.println(" (mAh)" );
       
Serial.print("Current Capacity: " );
Serial.print(fetchWord(FULL_CHARGE_CAPACITY));
Serial.println(" (mAh)" );
  
Serial.print("Design Voltage: " );
Serial.print(fetchWord(DESIGN_VOLTAGE));
Serial.println(" (mV)" );

Serial.println("");
Serial.println("======== Charge Data ========" );
Serial.println("");

Serial.print("Max Charging Current: ");
Serial.print(fetchWord(CHARGING_CURRENT));
Serial.println(" (mA)" );
  
Serial.print("Max Charging Voltage: ");
Serial.print(fetchWord(CHARGING_VOLTAGE));
Serial.println(" (mV)" );

Serial.print("Cycle Charging Count: ");
Serial.println(fetchWord(CYCLE_COUNT));

//Serial.println("");
//Serial.println("======== Pack State ========" );
//Serial.println("");

//Serial.print("Battery Mode (BIN): 0b");
//Serial.println(fetchWord(BATTERY_MODE),BIN);

//Serial.print("Battery Status (BIN): 0b");
//Serial.println(fetchWord(BATTERY_STATUS),BIN);

//Serial.println("");
//Serial.println("======== State ========" );
//Serial.println("");

Serial.println("");

Serial.print("Temp: ");
//unsigned int tempk = fetchWord(TEMPERATURE);
//Serial.print((float)tempk/10.0-273.15);
Serial.println(" °C" );

Serial.println("");

delay(5000);

}
