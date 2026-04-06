#include <LiquidCrystal.h>
#include <Ethernet.h>
#include <PubSubClient.h>

// ── NETWORK CONFIG ────────────────────────────────────────────────────────────
#define OUT_TOPIC   "ICT4_out_2020"
#define MAC_LAST    0x73
static uint8_t mymac[6] = { 0x44, 0x76, 0x58, 0x10, 0x00, MAC_LAST };
byte mqttIP[] = { 10, 6, 0, 23 };
const char* CLIENT_ID = "shortcircuit";

EthernetClient ethClient;
PubSubClient   mqtt(mqttIP, 1883, ethClient);

// ── PINS ──────────────────────────────────────────────────────────────────────
const int HUMID_PIN  = 3;   // (D3) humidity sensor (frequency output)
const int BUTTON_PIN = 2;   // mode-select button (INPUT_PULLUP, polled)
LiquidCrystal lcd(4, 5, 6, 7, 8, 9);

// ── SEND MODE ─────────────────────────────────────────────────────────────────
// 0 = temperature only | 1 = humidity only | 2 = both
uint8_t sendMode = 0;
const char* modeLabels[] = { "TEMP", "HUMID", "BOTH" };

// Custom LCD characters
byte degreeChar[8] = { 0x06, 0x09, 0x09, 0x06, 0x00, 0x00, 0x00, 0x00 };
byte dropChar[8]   = { 0x04, 0x04, 0x0A, 0x0A, 0x11, 0x11, 0x11, 0x0E };

// ── HUMIDITY AVERAGING (10-element circular buffer) ───────────────────────────
const int NUM_READINGS    = 10;
float     readings[NUM_READINGS];
int       readIndex        = 0;
float     bufTotal         = 0;
int       samplesCollected = 0;

// Task 1: send every 50 s to avoid spamming the broker.
const unsigned long SEND_INTERVAL = 50000UL;
unsigned long lastSendTime = 0;

void checkButton() {
  static int  lastState   = HIGH;       // button starts unpressed (HIGH)
  static unsigned long lastPress = 0;   // for 200 ms debounce

  int currentState = digitalRead(BUTTON_PIN);
  

  // Detect falling edge: was HIGH, now LOW → button just pressed
  if (lastState == HIGH && currentState == LOW) {
    Serial.print("Button pressed");
    unsigned long now = millis();
    if (now - lastPress > 200) {        // ignore bounces within 200 ms
      sendMode = (sendMode + 1) % 3;    // cycle 0 → 1 → 2 → 0
      lastPress = now;
    }
  }
  lastState = currentState;
}

// LM35 on A0: ADC → °C
// Formula: ADC × (5 V / 1024) × 100 mV per °C  →  ADC × 5 / 10.24
float readTemperature() {
  return analogRead(A0) * (5.0f / 10.24f);
}

// Humidity sensor calibration: RH% = -0.06 × Hz + 514, clamped [0, 100]
float humidityFromFreq(float freq) {
  return constrain(-0.06f * freq + 514.0f, 0.0f, 100.0f);
}

// Sliding average: subtract the oldest reading, add the new one, return average.  
float updateAverage(float newVal) {
  bufTotal -= readings[readIndex];
  readings[readIndex] = newVal;
  bufTotal += newVal;
  readIndex = (readIndex + 1) % NUM_READINGS;

  if (samplesCollected < NUM_READINGS) {
    samplesCollected++;
  }
  return bufTotal / samplesCollected;
}

// Measure one period from the humidity sensor and return averaged RH%.
// Returns -1 if no signal detected within the 50 ms timeout.
float readHumidity() {
  unsigned long hi = pulseIn(HUMID_PIN, HIGH, 50000);
  unsigned long lo = pulseIn(HUMID_PIN, LOW,  50000);
  if (hi == 0 || lo == 0) return -1.0f;
  float freq = 1000000.0f / (hi + lo);
  return updateAverage(humidityFromFreq(freq));
}

void connectEthernet() {
  lcd.clear();
  lcd.print("Connecting ETH..");
  
  if (Ethernet.begin(mymac) == 0) {
    lcd.clear();
    lcd.print("ETH failed!");
    Serial.println(F("DHCP failed"));
    return;
  }

  lcd.clear();
  lcd.print("IP:");
  lcd.setCursor(0, 1);
  lcd.print(Ethernet.localIP());
  Serial.print(F("IP: "));
  Serial.println(Ethernet.localIP());
  delay(1500);
}

void connectMQTT() {
  if (mqtt.connected()) {
    return;
  }

  Serial.print(F("Connecting MQTT... "));
  if (mqtt.connect(CLIENT_ID)) {
    Serial.println(F("OK")); 
  } else {
    Serial.println(F("FAILED"));
  }                        
}

void sendMQTT(float temp, float humidity) {
  char* message = format_sensor_data(humidity, temp, sendMode);
  send_MQTT_message(message);
}

char* format_sensor_data(float humidity, float temperature, int dataOption) {
    static char result[200];
    char humidityStr[20];
    char temperatureStr[20];

    dtostrf(humidity, 6, 2, humidityStr);
    dtostrf(temperature, 6, 2, temperatureStr);

    if (dataOption == 0) {
      snprintf(
          result,
          sizeof(result),
          "IOTJS={\"S_temperature_name\":\"SC_temperature\",\"S_temperature_value\":%s}",
          temperatureStr
        );
        
    } else if (dataOption == 1) {
        snprintf(
          result, 
          sizeof(result),
          "IOTJS={\"S_humidity_name\":\"SC_humidity\",\"S_humidity_value\":%s}",
          humidityStr
        );
    } else {
        snprintf(
          result,
          sizeof(result),
          "IOTJS={\"S_humidity_name\":\"SC_humidity\",\"S_humidity_value\":%s,"
          "\"S_temperature_name\":\"SC_temperature\",\"S_temperature_value\":%s}",
          humidityStr,
          temperatureStr);
    }

    return result;
}

void send_MQTT_message(char* message) {
  if (!mqtt.connected()) {
      connectMQTT();
  }
  if (mqtt.connected()) {
      mqtt.publish(OUT_TOPIC, message);
      Serial.println("Message sent to MQTT server.");
      Serial.println(message);
  } else {
      Serial.println("Failed to send message: not connected to MQTT server.");
  }
}

void printRow(int row, const char* text) {
  char buf[21];
  snprintf(buf, sizeof(buf), "%-20s", text);
  lcd.setCursor(0, row);
  lcd.print(buf);
}

void updateDisplay(float temp, float humidity) {
  char line[21];
  char val1[8], val2[8];

  // Row 0: mode + countdown
  //         col: 01234567890123456789
  //              Mode : BOTH   [42s]
  unsigned long elapsed = millis() - lastSendTime;
  int secs = (elapsed >= SEND_INTERVAL) ? 0 : (SEND_INTERVAL - elapsed) / 1000;
  snprintf(line, 21, "Mode : %-5s  [%2ds]", modeLabels[sendMode], secs);
  printRow(0, line);

  // Row 1: primary sensor
  //         col: 01234567890123456789
  //              Temp :  23.5 *C
  //              Humid:  45.2 %
  if (sendMode == 0 || sendMode == 2) {
    dtostrf(temp, 5, 1, val1);
    lcd.setCursor(0, 1);
    lcd.print("Temp : ");
    lcd.print(val1);
    lcd.write(byte(0));
    lcd.print("C       ");
  } else {
    if (humidity >= 0) {
      dtostrf(humidity, 5, 1, val1);
      snprintf(line, 21, "Humid: %s %%", val1);
    } else {
      snprintf(line, 21, "Humid:  --.- %%");
    }
    printRow(1, line);
  }

  // Row 2: second sensor (both) or blank
  if (sendMode == 2) {
    if (humidity >= 0) {
      dtostrf(humidity, 5, 1, val1);
      snprintf(line, 21, "Humid: %s %%", val1);
    } else {
      snprintf(line, 21, "Humid:  --.- %%");
    }
    printRow(2, line);
  } else {
    printRow(2, "");
  }

  // Row 3: IP
  snprintf(line, 21, "IP   : ");
  lcd.setCursor(0, 3);
  lcd.print(line);
  lcd.print(Ethernet.localIP());
  lcd.print("  ");
}

void setup() {

  Serial.begin(9600);
  lcd.begin(20, 4);
  lcd.createChar(0, degreeChar);
  lcd.createChar(1, dropChar);
  pinMode(HUMID_PIN,  INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP); // pin 2: polled in loop, not interrupt
  
  
  // Block until we get a valid humidity signal, then pre-fill the buffer
  // so the first displayed value is already averaged (not zero).
  Serial.print("waiting signal...");
  unsigned long hi, lo; 
  do {
    hi = pulseIn(HUMID_PIN, HIGH, 50000);
    lo = pulseIn(HUMID_PIN, LOW,  50000);
  } while (hi == 0 || lo == 0);

  float initRH = humidityFromFreq(1000000.0f / (hi + lo));
  for (int i = 0; i < NUM_READINGS; i++) {
    readings[i] = initRH;
  }
  bufTotal         = initRH * NUM_READINGS;
  samplesCollected = NUM_READINGS;

 
  lcd.print("Stabilizing...");
  delay(800);

  connectEthernet();

  // Subtract SEND_INTERVAL so the first MQTT send happens immediately
  lastSendTime = millis() - SEND_INTERVAL;
}

void loop() {

  checkButton();

  float temp     = readTemperature();
  float humidity = readHumidity();

  updateDisplay(temp, humidity);

  if (millis() - lastSendTime >= SEND_INTERVAL) {
    lastSendTime = millis();
    sendMQTT(temp, humidity);
  }

  mqtt.loop();
}
