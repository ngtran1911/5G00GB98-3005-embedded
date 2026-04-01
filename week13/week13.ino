/* TASK 1

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
  attachInterrupt(digitalPinToInterrupt(buttonPIN), pulseISR, FALLING); // (interrupt num : int, ISR, mode)
                                                                        //ISR:the ISR to call when the interrupt occurs; this function must take no parameters and return nothing
                                                                        //mode : define when the interrupt shouuld be triggered 
                                                                        //LOW to trigger the interrupt whenever the pin is low,
                                                                        //CHANGE to trigger the interrupt whenever the pin changes value
                                                                        //RISING to trigger when the pin goes from low to high,
                                                                        //FALLING for when the pin goes from high to low.
  Timer1.initialize(1000000); // sets the timer period to 1 second (1,000,000 microseconds)
  Timer1.attachInterrupt(timerISR); //when 1 second is up, call timerISR
}

void loop() {

    lcd.setCursor(0, 2);// (column, row)
    lcd.print("Duration: ");
    lcd.print((float)pulseDurationUs / 1000.0,1);
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
*/

// TASK 2

// #include <LiquidCrystal.h>

// // PIN ASSIGNMENTS
// const int signalPin = 5;
// const int rs = 4, en = 6, d4 = 7, d5 = 8, d6 = 9, d7 = 10;
// LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// // AVERAGING SETTINGS
// const int NUM_READINGS = 10;
// float readings[NUM_READINGS];
// int readIndex = 0;
// float total = 0;
// int samplesCollected = 0;

// // Converts sensor output frequency (Hz) to relative humidity (%).
// // Linear calibration formula derived from sensor datasheet:
// //   At ~7000 Hz → ~94% RH  (514 - 0.06*7000 = 94)
// //   At ~5140 Hz → ~206% RH → clamped to 100%
// //   Slope: -0.06 %RH per Hz  (humidity decreases as frequency increases)
// //   Intercept: 514 (y-intercept of the calibration line)
// // Result is clamped to [0, 100] to discard out-of-range readings.
// float humidityFromFrequency(float freq) {
//   return constrain((-0.06 * freq) + 514, 0, 100);
// }

// float updateAverage(float newValue) {
//   total -= readings[readIndex];
//   readings[readIndex] = newValue;
//   total += newValue;
//   readIndex = (readIndex + 1) % NUM_READINGS;
//   if (samplesCollected < NUM_READINGS) samplesCollected++;
//   return total / samplesCollected;
// }

// void setup() {
//   pinMode(signalPin, INPUT);
//   lcd.begin(16, 2);

//   // Take first real reading to pre-fill buffer, avoiding cold-start bias
//   unsigned long highTime, lowTime;
//   do {
//     highTime = pulseIn(signalPin, HIGH, 50000); // timeout 50,000 µs = 50 ms
//     lowTime  = pulseIn(signalPin, LOW,  50000); // measures full period together
//   } while (highTime == 0 || lowTime == 0);

//   float initHumidity = humidityFromFr
//   equency(1000000.0 / (highTime + lowTime));
//   for (int i = 0; i < NUM_READINGS; i++) readings[i] = initHumidity;
//   total = initHumidity * NUM_READINGS;
//   samplesCollected = NUM_READINGS;

//   lcd.print("Stabilizing...");
//   delay(1000);
//   lcd.clear();
// }

// void loop() {
//   unsigned long highTime = pulseIn(signalPin, HIGH, 50000); // timeout 50,000 µs = 50 ms
//   unsigned long lowTime  = pulseIn(signalPin, LOW,  50000);

//   if (highTime > 0 && lowTime > 0) {
//     // pulseIn returns microseconds; sum is the full period.
//     // frequency (Hz) = 1,000,000 µs/s ÷ period (µs)
//     float frequency = 1000000.0 / (highTime + lowTime);
//     float avg = updateAverage(humidityFromFrequency(frequency));

//     lcd.setCursor(0, 0);
//     lcd.print("Humidity:       ");
//     lcd.setCursor(0, 1);
//     lcd.print(avg, 1);
//     lcd.print(" %      ");
//   } else {
//     lcd.setCursor(0, 0);
//     lcd.print("No signal       ");
//     lcd.setCursor(0, 1);
//     lcd.print("                ");
//   }

//   delay(200);
// }


// // TASK 3
// #include <LiquidCrystal.h>
// #include <Ethernet.h>

// #define MAC_6    0x73
// static uint8_t mymac[6] = { 0x44, 0x76, 0x58, 0x10, 0x00, MAC_6 };

// // PIN ASSIGNMENTS
// const int signalPin = 5;
// const int rs = 4, en = 6, d4 = 7, d5 = 8, d6 = 9, d7 = 10;
// LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// // AVERAGING SETTINGS
// const int NUM_READINGS = 10;
// float readings[NUM_READINGS];
// int readIndex = 0;
// float total = 0;
// int samplesCollected = 0;

// void fetchIP();

// void fetchIP() {
//   Serial.println(F("Setting up DHCP..."));
//   lcd.clear();
//   lcd.print(F("Connecting..."));

//   if (Ethernet.begin(mymac) == 0) {
//     Serial.println(F("DHCP failed — no Ethernet"));
//     lcd.clear();
//     lcd.print(F("Ethernet failed"));
//     return;
//   }

//   Serial.print(F("Connected with IP: "));
//   Serial.println(Ethernet.localIP());
//   lcd.clear();
//   lcd.print(F("IP:"));
//   lcd.setCursor(0, 1);
//   lcd.print(Ethernet.localIP());
//   delay(1500);
// }

// // Converts sensor output frequency (Hz) to relative humidity (%).
// // Linear calibration formula derived from sensor datasheet:
// //   At ~7000 Hz → ~94% RH  (514 - 0.06*7000 = 94)
// //   At ~5140 Hz → ~206% RH → clamped to 100%
// //   Slope: -0.06 %RH per Hz  (humidity decreases as frequency increases)
// //   Intercept: 514 (y-intercept of the calibration line)
// // Result is clamped to [0, 100] to discard out-of-range readings.
// float humidityFromFrequency(float freq) {
//   return constrain((-0.06 * freq) + 514, 0, 100);
// }

// float updateAverage(float newValue) {
//   total -= readings[readIndex];
//   readings[readIndex] = newValue;
//   total += newValue;
//   readIndex = (readIndex + 1) % NUM_READINGS;
//   if (samplesCollected < NUM_READINGS) samplesCollected++;
//   return total / samplesCollected;
// }

// void setup() {
//   Serial.begin(9600);
//   pinMode(signalPin, INPUT);
//   lcd.begin(20, 4);

//   // Take first real reading to pre-fill buffer, avoiding cold-start bias
//   unsigned long highTime, lowTime;
//   do {
//     highTime = pulseIn(signalPin, HIGH, 50000); // timeout 50,000 µs = 50 ms
//     lowTime  = pulseIn(signalPin, LOW,  50000); // measures full period together
//   } while (highTime == 0 || lowTime == 0);

//   float initHumidity = humidityFromFrequency(1000000.0 / (highTime + lowTime));
//   for (int i = 0; i < NUM_READINGS; i++) readings[i] = initHumidity;
//   total = initHumidity * NUM_READINGS;
//   samplesCollected = NUM_READINGS;

//   lcd.print("Stabilizing...");
//   delay(1000);
//   lcd.clear();

//   fetchIP();
// }

// void loop() {
//   unsigned long highTime = pulseIn(signalPin, HIGH, 50000); // timeout 50,000 µs = 50 ms
//   unsigned long lowTime  = pulseIn(signalPin, LOW,  50000);

//   if (highTime > 0 && lowTime > 0) {
//     // pulseIn returns microseconds; sum is the full period.
//     // frequency (Hz) = 1,000,000 µs/s ÷ period (µs)
//     float frequency = 1000000.0 / (highTime + lowTime);
//     float avg = updateAverage(humidityFromFrequency(frequency));

//     lcd.setCursor(0, 0);
//     lcd.print("Humidity:       ");
//     lcd.setCursor(0, 1);
//     lcd.print(avg, 1);
//     lcd.print(" %      ");
//   } else {
//     lcd.setCursor(0, 0);
//     lcd.print("No signal       ");
//     lcd.setCursor(0, 1);
//     lcd.print("                ");
//   }

//   lcd.setCursor(0, 2);
//   lcd.print("IP: ");
//   lcd.print(Ethernet.localIP());

//   delay(200);
// }





// TASK 3
#include <LiquidCrystal.h>
#include <Ethernet.h>
#include <PubSubClient.h>

#define outTopic   "ICT4_out_2020" // Topic to which the message is sent


#define MAC_6    0x73
static uint8_t mymac[6] = { 0x44, 0x76, 0x58, 0x10, 0x00, MAC_6 };

byte server[] = { 10,6,0,23 }; // MQTT server IP address
unsigned int Port = 1883;  // MQTT server port
EthernetClient ethClient; // Client object from Ethernet library
PubSubClient client(server, Port, ethClient); // Create PubSubClient object

const char* clientId = "a731fsd4";

// PIN ASSIGNMENTS
const int signalPin = 5;
const int rs = 4, en = 6, d4 = 7, d5 = 8, d6 = 9, d7 = 10;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// AVERAGING SETTINGS
const int NUM_READINGS = 10;
float readings[NUM_READINGS];
int readIndex = 0;
float total = 0;
int samplesCollected = 0;

void fetchIP();

void fetchIP() {
  Serial.println(F("Setting up DHCP..."));
  lcd.clear();
  lcd.print(F("Connecting..."));

  if (Ethernet.begin(mymac) == 0) {
    Serial.println(F("DHCP failed — no Ethernet"));
    lcd.clear();
    lcd.print(F("Ethernet failed"));
    return;
  }

  Serial.print(F("Connected with IP: "));
  Serial.println(Ethernet.localIP());
  lcd.clear();
  lcd.print(F("IP:"));
  lcd.setCursor(0, 1);
  lcd.print(Ethernet.localIP());
  delay(1500);
}

// Converts sensor output frequency (Hz) to relative humidity (%).
// Linear calibration formula derived from sensor datasheet:
//   At ~7000 Hz → ~94% RH  (514 - 0.06*7000 = 94)
//   At ~5140 Hz → ~206% RH → clamped to 100%
//   Slope: -0.06 %RH per Hz  (humidity decreases as frequency increases)
//   Intercept: 514 (y-intercept of the calibration line)
// Result is clamped to [0, 100] to discard out-of-range readings.
float humidityFromFrequency(float freq) {
  return constrain((-0.06 * freq) + 514, 0, 100);
}

float updateAverage(float newValue) {
  total -= readings[readIndex];// remove the old value from the array 
  readings[readIndex] = newValue; // replace old with new
  total += newValue;//add new value to the total
  readIndex = (readIndex + 1) % NUM_READINGS;// readIndex never exceed 9
  if (samplesCollected < NUM_READINGS) samplesCollected++;
  return total / samplesCollected;
}

void setup() {
  Serial.begin(9600); //9600 bits per seconds : speed of communicationre
  pinMode(signalPin, INPUT);// receive signal
  lcd.begin(20, 4);

  // Take first real reading to pre-fill buffer, avoiding cold-start bias
  unsigned long highTime, lowTime; // var to store time measure 
  do {
    highTime = pulseIn(signalPin, HIGH, 50000); // timeout 50,000 µs = 50 ms
    lowTime  = pulseIn(signalPin, LOW,  50000); // measures full period together
  } while (highTime == 0 || lowTime == 0); //repeated until find out high time or lowTime # 0
                                            //got a real signal! → stop

  float initHumidity = humidityFromFrequency(1000000.0 / (highTime + lowTime));
  for (int i = 0; i < NUM_READINGS; i++){
    readings[i] = initHumidity;}
  total = initHumidity * NUM_READINGS; // for calc the average 
  samplesCollected = NUM_READINGS;

  lcd.print("Stabilizing...");
  delay(1000);
  lcd.clear();

  fetchIP();
}

void send_MQTT_message() {
    if (!client.connected()) { // Check if connected to MQTT broker
        connect_MQTT_server(); // If not connected, call connect function
    }
    if (client.connected()) {
        client.publish(outTopic, "Hello this is Vivian sendddding");
        Serial.println("Message sent to MQTT server.");
    } else {
        Serial.println("Failed to send message: not connected to MQTT server."); // Not connected -> error message
    }
}

void connect_MQTT_server() { 
    Serial.println("Connecting to MQTT"); // Print informational message
    if (client.connect(clientId)) { // Attempt to connect to MQTT broker
        Serial.println("Connected OK"); // Connection successful
    } else {
        Serial.println("Connection failed."); // Connection failed
    }    
}


double minVal = 9999.9;
double maxVal = -9999.9;

void handleTemperature() {
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
    lcd.setCursor(0, 3);
    lcd.print("Now temp: ");
    lcd.print(dat, 1); // 1 decimal place for neatness
    lcd.print("C   ");
}

void loop() {
  unsigned long highTime = pulseIn(signalPin, HIGH, 50000); // timeout 50,000 µs = 50 ms
  unsigned long lowTime  = pulseIn(signalPin, LOW,  50000);// pulseIn measure how long a pulse last on a pin

  if (highTime > 0 && lowTime > 0) {
    // pulseIn returns microseconds; sum is the full period.
    // frequency (Hz) = 1,000,000 µs/s ÷ period (µs)
    float frequency = 1000000.0 / (highTime + lowTime);
    float avg = updateAverage(humidityFromFrequency(frequency));

    lcd.setCursor(0, 0);
    lcd.print("Humidity: ");
    lcd.print(avg, 1);
    lcd.print(" %");
  } else {
    lcd.setCursor(0, 0);
    lcd.print("No signal");
    lcd.setCursor(0, 1);
    lcd.print("                ");
  }

  lcd.setCursor(0, 2);
  lcd.print("IP: ");
  lcd.print(Ethernet.localIP());// Adruino connects to the network by purple cable in class and get an IP address

  send_MQTT_message(); 
  handleTemperature();
  delay(5000); // run every 5s,wait 5s then repeat
}
