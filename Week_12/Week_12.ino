/*TASK 2 

#include <TimerOne.h>
#include <LiquidCrystal.h>

volatile int seconds = 0;
const int rs = 4, en= 6, d4= 7, d5= 8, d6= 9, d7= 10;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const uint8_t buttonPIN = 2;
volatile unsigned long pulseCount = 0;

void pulseISR(){
  pulseCount++;
}

void setup() {
  Serial.begin(9600);
  lcd.begin(24, 4);
  lcd.clear();
  Timer1.initialize(1000000); //Seconds
  Timer1.attachInterrupt(interrupt_routine);

  pinMode(buttonPIN, INPUT);
  attachInterrupt (digitalPinToInterrupt(buttonPIN), pulseISR, FALLING);

}

void loop() {
  lcd.setCursor(0,0);
  lcd.print("freq: ");
  lcd.print(pulseCount);
}

void interrupt_routine() {
  seconds ++;
} */

/*TASK 3
#include <TimerOne.h>
#include <LiquidCrystal.h>


volatile int seconds = 0;
const int rs = 4, en= 6, d4= 7, d5= 8, d6= 9, d7= 10;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const uint8_t buttonPIN = 2;
volatile unsigned long firstPress = 0;
volatile unsigned long secondPress = 0;
String duration;

void pulseISR(){
  unsigned long currentTime = micros();
  if (firstPress != 0){
    duration = currentTime - firstPress;
  }
  firstPress = currentTime;
}


void setup(){
Serial.begin(9600);
lcd.begin(20, 4);
lcd.clear();
pinMode(buttonPIN, INPUT_PULLUP);
attachInterrupt(digitalPinToInterrupt(buttonPIN), pulseISR, FALLING);
}

void loop(){
lcd.setCursor(0,0);
lcd.print("Duration:      ");
lcd.setCursor(0,1);
lcd.print(duration);
lcd.print(" mcs");

delay(200);
}
*/

//TASK 4
#include <TimerOne.h>
#include <LiquidCrystal.h>
const int rs = 4, en = 6, d4 = 7, d5 = 8, d6 = 9, d7 = 10;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const uint8_t buttonPIN = 2;

volatile unsigned long pulseCount = 0;
unsigned long finalCount = 0;
float frequency = 0.0;
volatile bool tenSecondsPassed = false;

volatile unsigned long lastPulseTime = 0;
volatile unsigned long pulseDurationUs = 0;



void pulseISR(){
  unsigned long  currentTime = micros();
  if(lastPulseTime > 0){
    pulseDurationUs = currentTime - lastPulseTime;
  }
  lastPulseTime = currentTime;
  pulseCount++;
}

void timerISR(){
  finalCount = pulseCount;
  pulseCount = 0;

  frequency = (float)finalCount / 1.0;
  tenSecondsPassed = true;
}

void setup() {
  lcd.begin(20, 4); 
  lcd.clear();

  pinMode(buttonPIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPIN), pulseISR, FALLING);

  Timer1.initialize(1000000); 
  Timer1.attachInterrupt(timerISR);
}

void loop() {

    lcd.setCursor(0, 2);
    lcd.print("Duration: ");
    lcd.print((float)pulseDurationUs / 1000.0, 1);
    lcd.print(" ms");

    lcd.setCursor(0, 0);
    lcd.print("Pulses: ");
    lcd.print(pulseCount);

  if (tenSecondsPassed) {
    tenSecondsPassed = false;
    
    lcd.clear();
    
    lcd.setCursor(0, 1);
    lcd.print("Freq: ");
    lcd.print(frequency, 3); 
    lcd.print(" Hz");
  }
}