/* * TAMK Software Engineering - Exercises 1-8
 * Combined Script: Blink, Duty Cycle, Frequency, and Digital Input
 */
/*
const int LED_PIN = 13;
const int INPUT_PIN = 4;

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  pinMode(INPUT_PIN, INPUT);
}

void loop() {
  // 30% Duty Cycle = 30ms ON / 70ms OFF.
  int onTime = 30; 
  int offTime = 70; 

  calculate(onTime, offTime);

  digitalWrite(LED_PIN, HIGH);
  delay(onTime);
  digitalWrite(LED_PIN, LOW);
  delay(offTime);

  int pin_state = digitalRead(INPUT_PIN);
  Serial.print("Digital Pin 4 State: ");
  Serial.println(pin_state);
}

void calculate(float highTime, float lowTime) {
  float period = highTime + lowTime;
  float dutyCycle = (highTime / period) * 100;
  float frequency = 1000.0 / period; 

  Serial.print("Duty Cycle: ");
  Serial.print(dutyCycle);
  Serial.print("% | Frequency: ");
  Serial.print(frequency);
  Serial.println(" Hz");
}
*/
/* * ---------------------------------------------------------
 * ANSWERS TO THEORY QUESTIONS
 * ---------------------------------------------------------
 * EXERCISE 5: 
 * - Max Frequency?
  T=1/f => T and f have reverse relationship => highest f(Frequency) = lowest T (period= onTime(1ms) + offTime(1ms)= 2ms = 0.002s)=> f = 1/T = 1/0.002 = 500hz  
 * - Blocking func: the func stop the code from running until its code is done
 * - Problems: Unresponsiveness and inability to multitask.
 *
 * EXERCISE 6: 
 * - Serial.print: Prints on the same line.
 * - Serial.println: Moves to a new line after printing.
 *
 * EXERCISE 9 :
 * - Is 3V3 HIGH or LOW?: The 3.3V signal is regarded as a HIGH state. 
 * - More than 3V is regrade as high 
 * ---------------------------------------------------------

 * EXERCISE 10
    int potPin = A2 ;
    int value = 0 ;

    void setup()
    {
      Serial.begin(9600);
      pinMode(A2,INPUT);
    }

    void loop()
    {
      value = analogRead(potPin);
      Serial.println(value);
      delay(200);
    }
 * --------------------------------------------------------- 
 * EXERCISE 11
 - adjust the trimmer to the left return the min num :0 
 - adjust the trimmer to the right return the max num :1023
 * ---------------------------------------------------------


 */

 /*TASK 1 - ANALOG MEASUREMENT AND LCD

#include <LiquidCrystal.h>

// 1. PIN DEFINITIONS
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
const int analogInPin = A2;
const int ledPin = 9;   // Pin for the External LED

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  
  // Define Pin 9 as an OUTPUT for the LED
  pinMode(ledPin, OUTPUT); 
}

void loop() {
  // 2. READ VALUES
  int adcValue = analogRead(analogInPin);
  
  // 3. MATH: Convert ADC (0-1023) to Voltage (0-5V)
  float voltage = adcValue * (5.0 / 1023.0);

  // 4. SERIAL MONITOR
  Serial.print("ADC: ");
  Serial.print(adcValue);
  Serial.print(" | Voltage: ");
  Serial.println(voltage);

  // 5. LCD DISPLAY
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ADC Value: ");
  lcd.print(adcValue);
  
  lcd.setCursor(0, 1);
  lcd.print("Voltage: ");
  lcd.print(voltage);
  lcd.print("V");

  // 6. LED LOGIC (Task: ON if > 4V)
  if (voltage > 4.0) {
    digitalWrite(ledPin, HIGH); // Turn LED ON
  } else {
    digitalWrite(ledPin, LOW);  // Turn LED OFF
  }

}
*/
/* TASK 2 - AVERAGING

#include <LiquidCrystal.h>

// PIN DEFINITIONS
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
const int analogInPin = A2;
const int ledPin = 9;

// GLOBAL VARIABLES
int sampleCount = 100; 

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// THE AVERAGING FUNCTION
// Type is float, passed parameter is int
float getAverageVoltage(int count) {
  long sum = 0; 
  
  for (int i = 0; i < count; i++) {
    sum += analogRead(analogInPin); 
  }
  
  float averageADC = (float)sum / count; 
  float voltage = averageADC * (5.0 / 1023.0); 
  
  return voltage; // Return the averaged value to main loop()
}

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // 1. Single Measurement (From Task 1)
  int rawADC = analogRead(analogInPin);
  float singleVoltage = rawADC * (5.0 / 1023.0);

  // 2. Averaged Measurement (Task 2)
  // Calling the function and passing our global sampleCount
  float avgVoltage = getAverageVoltage(sampleCount);

  // 3. LED Control (Still based on Task 1 voltage or Avg? Usually Avg is better!)
  if (avgVoltage > 4.0) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }

  // 4. LCD Display
  lcd.clear();
  
  // Row 0: Single Voltage
  lcd.setCursor(0, 0);
  lcd.print("Single: ");
  lcd.print(singleVoltage);
  lcd.print("V");
  
  // Row 1: Averaged Voltage (Requirement: Display below Task 1)
  lcd.setCursor(0, 1);
  lcd.print("Avg:    ");
  lcd.print(avgVoltage);
  lcd.print("V");
 
}
*/
/*
 // TASK 3 - VOLTAGE SCALING

#include <LiquidCrystal.h>

// PIN DEFINITIONS
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
const int analogInPin = A2;
const int ledPin = 9;

// GLOBAL VARIABLES
int sampleCount = 100; // This must be outside of any function

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// AVERAGING FUNCTION (Task 2)
float getAverageVoltage(int count) {
  long sum = 0;
  for (int i = 0; i < count; i++) {
    sum += analogRead(analogInPin);
  }
  float averageADC = (float)sum / count;
  return averageADC * (5.0 / 1023.0);
}

// SCALING FUNCTION (Task 3)
int scaleVoltage(float v) {
  // Formula: 0V->50, 3V->200, 5V->300
  return (v * 50) + 50;
}

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  
  int rawADC = analogRead(analogInPin);
  float singleVoltage = rawADC * (5.0 / 1023.0);

  float avgVoltage = getAverageVoltage(sampleCount);

  int scaledVal = scaleVoltage(avgVoltage);

  lcd.clear();
  
  // Row 0: Single Voltage and Scaled Value (Top Right)
  lcd.setCursor(0, 0);
  lcd.print("S:");
  lcd.print(singleVoltage, 1); // 1 decimal place
  
  lcd.setCursor(12, 0); // Move to the end of the first row
  lcd.print(scaledVal);

  // Row 1: Averaged Voltage
  lcd.setCursor(0, 1);
  lcd.print("Avg: ");
  lcd.print(avgVoltage);
  lcd.print("V");

  // LED Logic
  if (avgVoltage > 4.0) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }

}
*/

// TASK 4 - AND AND SERIAL PRINT EXECUTION TIME
#include <LiquidCrystal.h>

const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
const int analogInPin = A2; 
const int ledPin = 9;

int sampleCount = 100;   
unsigned long duration = 0;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7); // Declared globally

float getAverageVoltage(int count) {
  unsigned long startTime = micros(); 
  long sum = 0;
  for (int i = 0; i < count; i++) {
    sum += analogRead(analogInPin);

    Serial.println("hieno juttu, kiitos paljon");
   
  }
  unsigned long endTime = micros();
  duration = endTime - startTime;
  
  float averageADC = (float)sum / count;
  return averageADC * (5.0 / 1023.0);
}

int scaleVoltage(float v) {
  return (v * 50) + 50;
}


void setup() {
  Serial.begin(9600); // Change this to 4800 or 115200 later for Task 4
  lcd.begin(16, 2);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // Calculations
  int rawADC = analogRead(analogInPin);
  float singleVoltage = rawADC * (5.0 / 1023.0);
  float avgVoltage = getAverageVoltage(sampleCount);
  int scaledVal = scaleVoltage(avgVoltage);

  // LCD Update
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("S:"); lcd.print(singleVoltage, 1);
  lcd.setCursor(12, 0);
  lcd.print(scaledVal);

  lcd.setCursor(0, 1);
  lcd.print("Avg:"); lcd.print(avgVoltage, 2);
  lcd.setCursor(9, 1);
  lcd.print("T:"); lcd.print(duration);

  // LED logic
  if (avgVoltage > 4.0) digitalWrite(ledPin, HIGH);
  else digitalWrite(ledPin, LOW);


}