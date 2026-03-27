/*TASK 2

#include <LiquidCrystal.h>

// Nano Pins
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Global Variables for Phase 2
double minVal = 9999.9;
double maxVal = -9999.9;

void setup() {
    Serial.begin(9600);
    lcd.begin(16, 2); // Change to (20, 4) if you have the large screen
    lcd.print("Sensor Init...");
    delay(1000);
    lcd.clear();
}

void loop() {
    // Read the sensor
    uint16_t val = analogRead(A0);
    
    // Formula: (ADC * 5 / 1024) * 100 
    double dat = (double)val * (5.0 / 10.24);

    // Update Min/Max (only if sensor is actually connected/above 0)
    if (dat > 0.1) { 
        if (dat < minVal) minVal = dat;
        if (dat > maxVal) maxVal = dat;
    }

    // Row 1: Current Temp
    lcd.setCursor(0, 0);
    lcd.print("Now: ");
    lcd.print(dat, 1); // 1 decimal place for neatness
    lcd.print("C   ");

    // Row 2: Min Value
    lcd.setCursor(0, 1);
    lcd.print("Min:");
    lcd.print(minVal, 1);
    lcd.print("C ");

    // Row 3 
    lcd.setCursor(10, 1); // Putting Max on the same row if using 16x2
    lcd.print("Mx:");
    lcd.print(maxVal, 1);
    
    // Serial Monitor for Phase tracking
    Serial.print("Data: ");
    Serial.println(dat);

    delay(800);
}*/


// TASK 3A - Button Test
#include <LiquidCrystal.h>

// Nano Pins
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Global Variables for Phase 2
double minVal = 9999.9;
double maxVal = -9999.9;
const int buttonPin = 8;   // Button connected to pin 8

void setup() {
    Serial.begin(9600);
    pinMode(buttonPin, INPUT_PULLUP);   // Set button pin as input
}

void loop() {

    // Read button state
    int buttonState = digitalRead(buttonPin);

    if(buttonState == HIGH){
      digitalWrite(buttonPin, HIGH);
    }
    else{
      digitalWrite(buttonPin, LOW);
    }

    // Print button state to Serial Monitor
    Serial.print("Button state: ");
    Serial.println(buttonState);

    delay(200);
}

/*TASK 3B - RESETTING VALUES
#include <LiquidCrystal.h>

// Nano Pins
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Global Variables for Phase 2
double minVal = 9999.9;
double maxVal = -9999.9;
const int buttonPin = 8;   // Button connected to pin 8

void setup() {
    Serial.begin(9600);
    pinMode(buttonPin, INPUT);   // Set button pin as input
}

void loop() {

    // Read button state
    int buttonState = digitalRead(buttonPin);


    // Print button state to Serial Monitor
    Serial.print("Button state: ");
    Serial.println(buttonState);

    delay(200);
}
*/