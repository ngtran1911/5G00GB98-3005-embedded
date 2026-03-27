
/*
EXERCISE 1 - 4
int const LED_13 = 13;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_13, OUTPUT);
  Serial.begin(9600);
}

// the loop function runs over and over again forever
void loop() {
//For 10Hz at 30% Duty Cycle:
  int onTime = 30; //30% of 100ms
  int offTime = 70; //70% of 100ms

  calculate(onTime, offTime);

  digitalWrite(LED_13, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(onTime);                      // wait for a second
  digitalWrite(LED_13, LOW);   // turn the LED off by making the voltage LOW
  delay(offTime);                      // wait for a second
}
void calculate(float highTime, float lowTime){
  float dutyCycle = (highTime / (highTime + lowTime)) * 100;
  Serial.print("Duty Cycle: ");
  Serial.print(dutyCycle);
  Serial.println("%");
}
*/

/*EXERCISE 5
1.Max frequency with delay()function is 1ms. If you use delay(1) for HIGH and
delay(1) for LOW, the total period is 2ms, resulting in 500 Hz. While delayMicroseconds() can go higher
(up to 100-200 kHz), the overhead of the digital Write function and the loop itself limits
the "clean" maximun frequency to much less than the proecessor's clock speed.

2. WHAT IS A "BLOCKING" FUNCTION?
A blocking function stops the execution of the rest of the program until that 
specific task is finished. When delay(1000) is called, the Arduino processor 
essentially "twiddles its thumbs" and waits for the timer to finish, doing 
almost nothing else in the meantime.

3. PROBLEMS RESULTING FROM BLOCKING:
-Unresponsiveness: The Arduino cannot read sensors or check if a button is pressed while it is stuck in a delay.
-Multi-tasking issues: You cannot blink two LEDs at different rates easily because one delay will pause the entire program.
- Timing inaccuracy: If you have other code running, the time it takes to execute that code adds to the delay, making the frequency lower than intended
*/

//EXERCISE 6: Serial.println = it prints on the next line. Serial.print = it prints in the same line.

/*
EXCERCIZE 7-8 
int const INPUT_PIN = 4;
int const LED13 = 13;
int pin_state;

void setup(){
  Serial.begin(9600);
  pinMode(INPUT_PIN, INPUT);
  pinMode(LED13, OUTPUT);
  
}

void loop(){
pin_state = digitalRead(INPUT_PIN);
  Serial.print("Digital Pin 4 State: ");
  Serial.println(pin_state);

  digitalWrite(LED13, pin_state);

  delay(200);
}
*/
/*
EXERCISE 9 
 -Is 3V3 HIGH or LOW?: The 3.3V signal is regarded as a HIGH state. 
 - More than 3V is regrade as high 
 * ---------------------------------------------------------

EXERCISE 10
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
 /*

 EXERCISE 14 - 16

 #include <LiquidCrystal.h>

const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
const int analogInPin = A2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  Serial.begin(9600);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("hello, world!");
}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  lcd.print(millis() / 1000);
}
 
 
 */
/* 
EXERCISE 17-19
 
 #include <LiquidCrystal.h>

 const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
 const int analogInPin = A2;
 const int digitalInPin = 8;

 LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(digitalInPin, INPUT_PULLUP);
}

void loop () {

  int anaValue = analogRead(analogInPin);
  int digValue = digitalRead(digitalInPin);

  int duty = map(anaValue, 0, 1023, 0, 100);
  int freq = 200;

  /*logic for ex 18:*/
  if (anaValue <200) {
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("Warning: ");
    lcd.setCursor(1, 1);
    lcd.print("Value too low");
  }
  else if(anaValue > 1000){
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("Warning: ");
    lcd.setCursor(1, 1);
    lcd.print("Value too high!");
    delay(1000);

  }
  else {
    lcd.clear();
    //Dig and Ana
    lcd.setCursor(0, 0);
    lcd.print("Dig.:");
    lcd.print(digValue);
    
    lcd.setCursor(8, 0);
    lcd.print("Ana.:");
    lcd.print(anaValue);

    //Dut and Fr
    lcd.setCursor(0, 1);
    lcd.print("dut:");
    lcd.print(duty);
    lcd.print("%");
    
    lcd.setCursor(8, 1);
    lcd.print("fr:");
    lcd.print(freq);
    lcd.print("Hz");

    //Print to Monitor
    Serial.print("Analog: ");
    Serial.println(anaValue);

    delay(200);
  }
  
}

 
