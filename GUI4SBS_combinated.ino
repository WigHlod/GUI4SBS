/// Define SDA SMbus Interface.
#define SDA_PORT PORTC
#define SDA_PIN 4

/// Define SCL SMBus Interface.
#define SCL_PORT PORTC
#define SCL_PIN 5

// Load Library.
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <SoftI2CMaster.h>

// Define SPI OLED pins.
#define OLED_MOSI     3
#define OLED_CLK      2
#define OLED_DC       5
#define OLED_CS       6
#define OLED_RST      4

// Smart Battery Commands.
#define TEMP                     0x08
#define VOLTAGE                  0x09
#define CURRENT                  0x0A
#define RELATIVE_SOC             0x0D
#define ABSOLUTE_SOC             0x0E
#define REMAINING_CAPACITY       0x0F
#define FULL_CHARGE_CAPACITY     0x10
#define CYCLE                    0x17
#define DESIGN_CAPACITY          0x18
#define DESIGN_VOLTAGE           0x19
#define CELL3_VOLTAGE            0x3D
#define CELL2_VOLTAGE            0x3E
#define CELL1_VOLTAGE            0x3F

// Data Buffer Vars.
float C1backup;
float C1actual;

float C2backup;
float C2actual;

float C3backup;
float C3actual;

float VOLTAGEbackup;
float VOLTAGEactual;

int CURRENTbackup;
int CURRENTactual;

int RELATIVE_SOCbackup;
int RELATIVE_SOCactual;

int CYCLEbackup;
int CYCLEactual;

int TEMPbackup;
int TEMPactual;

// SMBus over I2C Buffer. 
#define bufferLen 32
uint8_t i2cBuffer[bufferLen];

// Generate the OLED display.
Adafruit_SH1106G display = Adafruit_SH1106G(128, 64,OLED_MOSI, OLED_CLK, OLED_DC, OLED_RST, OLED_CS);

// Define Battery controller I2C/SMBus Address.
byte deviceAddress = 0x0B;

void setup()   {
  // Init Serial output.
  Serial.begin(9600);

  // Start OLED
  display.begin(0, true);

  // Clear the Diplay buffer.
  display.clearDisplay();
   
   while (!Serial) {    
    ;
  }

  // Init the I2C Bus.
  i2c_init();

  // Default Display .. awaiting the SBS data input.
  display.setTextColor(SH110X_WHITE); 
  display.setTextSize(1);
  display.setCursor(10, 10);
  display.print("Read SBS Data ...");
  
  // Show on display.
  display.display();
  delay(1000);    
}

// Function fechWord.
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

// Process Data from I2C/SMBus.
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
display.clearDisplay();

//// OLED Screen GUI.

/// Drawing the Cells.
display.fillRoundRect(90, 4, 35, 15, 2, SH110X_WHITE);
display.fillRoundRect(90, 25, 35, 15, 2, SH110X_WHITE);
display.fillRoundRect(90, 45, 35, 15, 2, SH110X_WHITE);


/// Drawing the Cables.

// Negative Cable.
display.drawLine(38,7,90, 7, SH110X_WHITE);

// Cell link between 1 & 2.
display.drawLine(86,15,90, 15, SH110X_WHITE);
display.drawLine(85,16,85, 27, SH110X_WHITE);
display.drawLine(86,28,90, 28, SH110X_WHITE);

// Cell link between 2 & 3.
display.drawLine(86,36,90, 36, SH110X_WHITE);
display.drawLine(85,37,85, 47, SH110X_WHITE);
display.drawLine(86,48,90, 48, SH110X_WHITE);

// Positive Cable.
display.drawLine(20,56,90, 56, SH110X_WHITE);
display.drawLine(6,56,8, 56, SH110X_WHITE);
display.drawLine(5,55,5, 8, SH110X_WHITE);
display.drawLine(6,7,8, 7, SH110X_WHITE);

/// Drawing the Thermometer.
display.drawRoundRect(63, 17, 7, 27, 4, SH110X_WHITE);
display.drawRoundRect(60, 39, 13, 9, 5, SH110X_WHITE);
display.fillRect(64, 39, 5, 2, SH110X_BLACK);
display.fillRect(63, 40, 7, 4, SH110X_BLACK);
display.fillRoundRect(62, 40, 9, 7, 4, SH110X_WHITE);
display.fillRoundRect(65, 20, 3, 27, 4, SH110X_WHITE);
display.drawLine(65,48,67,48, SH110X_WHITE);

// Mask for the text.
display.fillRect(60, 27, 15, 9, SH110X_BLACK);

// Graduations.
display.drawLine(67,37,70, 37, SH110X_BLACK);
display.drawLine(67,25,70, 25, SH110X_BLACK);
display.drawLine(67,23,70, 23, SH110X_BLACK);

/// Drawing the charge cycle.
display.drawRoundRect(14, 16, 26, 13, 5, SH110X_WHITE);

display.setTextColor(SH110X_WHITE, SH110X_BLACK);

// Arrow up.
display.setCursor(12, 18);
display.print((char)30);
display.fillRect(14, 16, 2, 4, SH110X_BLACK);

// Arrow down.
display.setCursor(37, 18);
display.print((char)31);
display.fillRect(38, 23, 2, 3, SH110X_BLACK);


// Init color & size.
display.setTextColor(SH110X_WHITE); 
display.setTextSize(1);

/// Pupulate the GUI with Data.

// Main +12.x voltage.
display.setCursor(11, 4);
display.print((char)43);
display.setTextSize(1);
display.setCursor(16, 4);
display.setTextColor(SH110X_WHITE, SH110X_BLACK);

// Retrieve the value.
float VOLTAGEmain = ((float)fetchWord(VOLTAGE)/1000);
// Check if the value is in range.
if (VOLTAGEmain > 10 && VOLTAGEmain < 15) {
// If the value is correct store it in the two next vars.  
VOLTAGEbackup = VOLTAGEmain;
VOLTAGEactual = VOLTAGEmain;
// Then Display the value.
display.print((VOLTAGEactual), 1);
} 
else
{
// If the value is not correct, reuuse the last good value from backup.  
VOLTAGEactual = VOLTAGEbackup;
// Then Display the value.
display.print((VOLTAGEactual), 1);
}
// Voltage symbol.
display.print("v");

// I/O Current
display.setCursor(11, 53);

// Retrieve the value.
int CURRENTmain = (fetchWord(CURRENT));
// Check if the value is in range.
if (CURRENTmain > -5000 && CURRENTmain < 5000) {
// If the value is correct store it in the two next vars. 
CURRENTbackup = CURRENTmain;
CURRENTactual = CURRENTmain;
}
else
{
// If the value is not correct, reuuse the last good value from backup.  
CURRENTactual = CURRENTbackup;
}

// Define Up & Down Arrow char from hex code.
int CharUp = 0x18;
int CharDown = 0x19;
int CharLine = 0x2D;

// Show a up arrow is current is rising.
if (CURRENTactual > 0) {
display.write(CharUp);
}

// Show a down arrow if current is drain.
if (CURRENTactual < 0) {
display.write(CharDown);
}

// Remove the negative sign (transform to positive).
if(CURRENTbackup<0)
{
CURRENTbackup=-CURRENTbackup;
}

// Print Current on display.
display.print(CURRENTbackup);
display.print("mA");

// Temperature.
display.setCursor(57, 28);
// Get the temp in celsius.
unsigned int tempk = fetchWord(TEMP);
int TEMPmain = ((float)tempk/10.0-273.15);

// Check if cycle count value is correct and display it. 
if (TEMPmain > -10 && TEMPmain < 70) {
TEMPbackup = TEMPmain;
TEMPactual = TEMPmain;
display.print(TEMPactual);
} 
else
{
// If not display the last good value instead.
TEMPactual = TEMPbackup;
display.print(TEMPactual);
}

display.print((char)247);
display.print("C");

// Remain Capacity.
display.setTextSize(2);
display.setCursor(8, 33);

int RELATIVE_SOCmain = (fetchWord(RELATIVE_SOC));
if (RELATIVE_SOCmain >= 0  && RELATIVE_SOCmain <= 100) {
RELATIVE_SOCactual = RELATIVE_SOCmain;
RELATIVE_SOCbackup = RELATIVE_SOCmain;
display.print(RELATIVE_SOCactual);
} 
else
{
// If not display the last good value instead.
RELATIVE_SOCactual=RELATIVE_SOCbackup;
display.print(RELATIVE_SOCactual);
}
display.print((char)37);
display.setTextSize(1);


/// Charge Cycle. 
display.setCursor(18, 19);
// Check if cycle count value is correct and display it. 
int CYCLEmain = (fetchWord(CYCLE));
if (CYCLEmain >= 0 && CYCLEmain < 5000) {
CYCLEbackup = CYCLEmain;
CYCLEactual = CYCLEmain;
display.print(CYCLEactual);
} 
else
{
// If not display the last good value instead.
CYCLEactual = CYCLEbackup;
display.print(CYCLEactual);
}

/// Cells Voltage.

// Store Cells voltage in vars.
float C1main = ((float)fetchWord(CELL1_VOLTAGE)/1000);
float C2main = ((float)fetchWord(CELL2_VOLTAGE)/1000);
float C3main = ((float)fetchWord(CELL3_VOLTAGE)/1000);
display.setTextColor(SH110X_BLACK, SH110X_WHITE);

display.setCursor(93, 8);
// Check if cell voltage value is correct and display it. 
if (C1main > 3 && C1main < 5) {
C1backup = C1main;
C1actual = C1main;
display.print(C1actual);
display.println("v");
} 
else
{
// If not display the last good value instead.
C1actual=C1backup;
display.print(C1actual);
display.println("v");
}

display.setCursor(93, 29);
// Check if cell voltage value is correct and display it. 
if (C2main > 3 && C2main < 5) {
C2backup = C2main;
C2actual = C2main;
display.print(C2actual);
display.println("v");
} 
else
{
// If not display the last good value instead.
C2actual=C2backup;
display.print(C2actual);
display.println("v");
}

display.setCursor(93, 49);
// Check if cell voltage value is correct and display it. 
if (C3main > 3 && C3main < 5) {
C3backup = C3main;
C3actual = C3main;
display.print(C3actual);
display.println("v");
} 
else
{
// If not display the last good value instead.
C3actual=C3backup;
display.print(C3actual);
display.println("v");
}

// Print the whole display for 5 sec then clear it.
display.display();

//// SERIAL DATA OUTPUT >IF NEEDED.

//Serial.println("");
//Serial.println("======== GENERAL ========");
//Serial.println("");

//Serial.print("Voltage: ");
//Serial.print(VOLTAGEactual);
//Serial.println("V" );

// This SBS Battery controller return value in Watt not Mah so value must be x2 
//Serial.print("Capacity: " );
//Serial.print(fetchWord(REMAINING_CAPACITY)*2);
//Serial.print("/" );
//Serial.print(fetchWord(DESIGN_CAPACITY)*2);
//Serial.println("mAh " );

// This data is still readable in the Console output.
Serial.print("Remaining: " );
Serial.print(RELATIVE_SOCactual);
Serial.println("%" );

//Serial.print("Recharge Cycle: ");
//Serial.println(CYCLEactual);

//Serial.print("Temp: ");
//Serial.print(TEMPactual);
//Serial.println("°C" );

  
//Serial.print("In/Out Current: " );
//Serial.print(CURRENTactual);
//Serial.println(" (mA)" );

//Serial.println("");
//Serial.println("========= CELLS =========");
//Serial.println("");
-
//Serial.print("C1 Voltage: ");
//Serial.print(C1actual);
//Serial.println("V" );
//Serial.print("C2 Voltage: ");
//Serial.print(C2actual);
//Serial.println("V" );
//Serial.print("C3 Voltage: ");
//Serial.print(C3actual);
//Serial.println("V" );

delay(5000);
display.clearDisplay();

}
