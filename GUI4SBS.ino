/// Include Externals libs.
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

/// SSD1306 Screen Config.
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_ADDR 0x3D

Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT);

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
}

/// Init your Battery Pack Variables.
/// This is where you should link the vars with your own project.

// Cells voltage.
float CELL1 = 3.91; 
float CELL2 = 3.92;
float CELL3 = 3.93;

// Nb of charge cycle.
int CHARGECYCLE = 160;

// Temperature sensor.
int TEMPERATURE = 22;

// Remaining capacity in percent.
int REMAININGCAP = 99;

// Main voltage of the pack.
float MAINVOLTAGE = 12.62;


//// Main Loop Program. 
void loop() {

/// Init the desplay.
display.clearDisplay();
display.setTextSize(1);
display.setTextColor(WHITE);

/// Drawing the Cells.
display.fillRoundRect(90, 4, 35, 15, 2, WHITE);
display.fillRoundRect(90, 25, 35, 15, 2, WHITE);
display.fillRoundRect(90, 45, 35, 15, 2, WHITE);

/// Drawing the Cables.

// Negative Cable.
display.drawLine(80,7,90, 7, WHITE);

// Cell link between 1 & 2.
display.drawLine(86,15,90, 15, WHITE);
display.drawLine(85,16,85, 27, WHITE);
display.drawLine(86,28,90, 28, WHITE);

// Cell link between 2 & 3.
display.drawLine(86,36,90, 36, WHITE);
display.drawLine(85,37,85, 47, WHITE);
display.drawLine(86,48,90, 48, WHITE);

// Positive Cable.
display.drawLine(6,56,90, 56, WHITE);
display.drawLine(5,55,5, 8, WHITE);
display.drawLine(6,7,10, 7, WHITE);

/// Drawing the Thermometer.
display.drawRoundRect(63, 20, 7, 30, 4, WHITE);
display.drawRoundRect(60, 42, 13, 12, 5, WHITE);
display.fillRect(64, 42, 5, 2, BLACK);
display.fillRect(63, 43, 7, 4, BLACK);
display.fillRoundRect(62, 43, 9, 10, 4, WHITE);
display.fillRoundRect(65, 23, 3, 30, 4, WHITE);

// Mask for the text.
display.fillRect(60, 27, 15, 9, BLACK);

// Graduations
display.drawLine(67,41,70, 41, BLACK);
display.drawLine(67,39,70, 39, BLACK);
display.drawLine(67,37,70, 37, BLACK);
display.drawLine(67,25,70, 25, BLACK);

/// Drawing the charge cycle.
display.drawRoundRect(11, 25, 26, 13, 5, WHITE);

// Arrow up.
display.setCursor(9, 27);
display.print((char)30);
display.fillRect(11, 25, 2, 4, BLACK);

// Arrow down.
display.setCursor(34, 29);
display.print((char)31);
display.fillRect(35, 34, 2, 3, BLACK);


//// Write the text values from vars.

// Charge Cycle. 
display.setCursor(15, 28);
display.print(CHARGECYCLE);


// Main +12 voltage
display.setCursor(13, 4);
display.print((char)43);
display.setTextSize(2);
display.setCursor(18, 1);
display.print(MAINVOLTAGE, 1);
display.print("v");
display.setTextSize(1);

// Temperature.
display.setCursor(57, 28);
display.print(TEMPERATURE);
display.print((char)247);
display.print("C");


// Remain Capacity.
display.setTextSize(2);
display.setCursor(8, 40);
display.print(REMAININGCAP);
display.print((char)37);
display.setTextSize(1);


// Cells Voltage.
display.setTextColor(BLACK, WHITE);

display.setCursor(93, 8);
display.print(CELL1, 2);
display.print("v");

display.setCursor(93, 29);
display.print(CELL2, 2);
display.print("v");

display.setCursor(93, 49);
display.print(CELL3, 2);
display.print("v");

// Actualise and wait.
display.display();
delay(5000);

}
