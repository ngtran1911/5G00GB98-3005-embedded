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


// TASK 3
#include <LiquidCrystal.h>
#include <Ethernet.h>

#define MAC_6    0x73
static uint8_t mymac[6] = { 0x44, 0x76, 0x58, 0x10, 0x00, MAC_6 };

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
  byte connection = 1;
  connection = Ethernet.begin(mymac);
  
  Serial.print(F("\nW5100 Revision "));
  if (connection == 0) {
    Serial.println(F("Failed to access Ethernet controller"));
  }
  
  Serial.println(F("Setting up DHCP"));
  Serial.print("Connected with IP: ");
  Serial.println(Ethernet.localIP());
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
  total -= readings[readIndex];
  readings[readIndex] = newValue;
  total += newValue;
  readIndex = (readIndex + 1) % NUM_READINGS;
  if (samplesCollected < NUM_READINGS) samplesCollected++;
  return total / samplesCollected;
}

void setup() {
  pinMode(signalPin, INPUT);
  lcd.begin(16, 2);

  // Take first real reading to pre-fill buffer, avoiding cold-start bias
  unsigned long highTime, lowTime;
  do {
    highTime = pulseIn(signalPin, HIGH, 50000); // timeout 50,000 µs = 50 ms
    lowTime  = pulseIn(signalPin, LOW,  50000); // measures full period together
  } while (highTime == 0 || lowTime == 0);

  float initHumidity = humidityFromFrequency(1000000.0 / (highTime + lowTime));
  for (int i = 0; i < NUM_READINGS; i++) readings[i] = initHumidity;
  total = initHumidity * NUM_READINGS;
  samplesCollected = NUM_READINGS;

  lcd.print("Stabilizing...");
  delay(1000);
  lcd.clear();

  fetchIP();
}

void loop() {
  unsigned long highTime = pulseIn(signalPin, HIGH, 50000); // timeout 50,000 µs = 50 ms
  unsigned long lowTime  = pulseIn(signalPin, LOW,  50000);

  if (highTime > 0 && lowTime > 0) {
    // pulseIn returns microseconds; sum is the full period.
    // frequency (Hz) = 1,000,000 µs/s ÷ period (µs)
    float frequency = 1000000.0 / (highTime + lowTime);
    float avg = updateAverage(humidityFromFrequency(frequency));

    lcd.setCursor(0, 0);
    lcd.print("Humidity:       ");
    lcd.setCursor(0, 1);
    lcd.print(avg, 1);
    lcd.print(" %      ");
  } else {
    lcd.setCursor(0, 0);
    lcd.print("No signal       ");
    lcd.setCursor(0, 1);
    lcd.print("                ");
  }

  delay(200);
}

