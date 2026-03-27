
/*EXERCISE 1 - 4
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

/*EXERCISE 5:
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
-Multi-tasking Issues: You cannot blink two LEDs at different rates easily because one delay will pause the entire program.
- Timing Inaccuracy: If you have other code running, the time it takes to execute that code adds to the delay, making the frequency lower than intended
*/

//EXERCISE 6: Serial.println = it prints on the next line. Serial.print = it prints in the same line.

/* EXCERSIZE 7-8 */
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

