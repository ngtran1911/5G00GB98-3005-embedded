#include <LiquidCrystal.h>
#include <Ethernet.h>
#include <PubSubClient.h>

#define OUT_TOPIC   "ICT4_out_2020"
#define CLIENT_ID   "shortcircuit"
#define HUMID_PIN   3
#define BUTTON_PIN  2

static uint8_t mymac[]  = { 0x44, 0x76, 0x58, 0x10, 0x00, 0x73 };
static byte    mqttIP[] = { 10, 6, 0, 23 };

const unsigned long SEND_INTERVAL    = 5000UL;
const unsigned long DISPLAY_INTERVAL = 500UL;
const unsigned long DEBOUNCE_MS      = 200UL;
const int           NUM_READINGS     = 10;

LiquidCrystal  lcd(4, 5, 6, 7, 8, 9);
EthernetClient ethClient;
PubSubClient   mqtt(mqttIP, 1883, ethClient);

byte degreeChar[8] = { 0x06, 0x09, 0x09, 0x06, 0x00, 0x00, 0x00, 0x00 };
byte dropChar[8]   = { 0x04, 0x04, 0x0A, 0x0A, 0x11, 0x11, 0x11, 0x0E };

uint8_t sendMode = 0;
const char* modeLabels[] = { "TEMP", "HUMID", "BOTH" };

float readings[NUM_READINGS];
int   readIndex        = 0;
float bufTotal         = 0;
int   samplesCollected = 0;

unsigned long lastSendTime    = 0;
unsigned long lastDisplayTime = 0;
uint8_t       lastDisplayMode = 255;

// LM35: 10mV/°C, ADC range 0-1023 maps to 0-5V
// °C = ADC * (5V / 1024) / 0.01V = ADC * 5 / 10.24
float readTemperature() {
  return analogRead(A0) * (5.0f / 10.24f);
}

// Datasheet calibration: RH% = -0.06 * frequency(Hz) + 514
float humidityFromFreq(float freq) {
  return constrain(-0.06f * freq + 514.0f, 0.0f, 100.0f);
}

float updateAverage(float newVal) {
  bufTotal -= readings[readIndex];
  readings[readIndex] = newVal;
  bufTotal += newVal;
  readIndex = (readIndex + 1) % NUM_READINGS;
  if (samplesCollected < NUM_READINGS) samplesCollected++;
  return bufTotal / samplesCollected;
}


float readHumidity() {
  unsigned long hi = pulseIn(HUMID_PIN, HIGH, 50000);
  unsigned long lo = pulseIn(HUMID_PIN, LOW,  50000);
  if (hi == 0 || lo == 0) return -1.0f;
  // freq = 1s / period(µs), period = high pulse + low pulse
  return updateAverage(humidityFromFreq(1000000.0f / (hi + lo)));

}

void prefillHumidityBuffer() {
  Serial.print(F("Waiting for humidity sensor..."));
  unsigned long hi, lo;
  do {
    hi = pulseIn  (HUMID_PIN, HIGH, 50000);
    lo = pulseIn(HUMID_PIN, LOW,  50000);
  } while (hi == 0 || lo == 0);

  float initRH = humidityFromFreq(1000000.0f / (hi + lo));
  for (int i = 0; i < NUM_READINGS; i++) readings[i] = initRH;
  bufTotal         = initRH * NUM_READINGS;
  samplesCollected = NUM_READINGS;
}

void checkButton() {
  static int lastState = HIGH;
  static unsigned long lastPress = 0;

  int cur = digitalRead(BUTTON_PIN);
  if (lastState == HIGH && cur == LOW) {
    unsigned long now = millis();
    if (now - lastPress > DEBOUNCE_MS) {
      sendMode = (sendMode + 1) % 3;
      lastPress    = now;
      lastSendTime = now;
      Serial.println(modeLabels[sendMode]);
    }
  }
  lastState = cur;
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
  if (mqtt.connected()) return;
  Serial.print(F("Connecting MQTT... "));
  if (mqtt.connect(CLIENT_ID)) {
    Serial.println(F("OK"));
  } else {
    Serial.println(F("FAILED"));
  }
}

char* formatPayload(float temp, float humidity) {
  static char result[200];
  char hStr[20], tStr[20];
  dtostrf(humidity, 6, 2, hStr);
  dtostrf(temp, 6, 2, tStr);

  if (sendMode == 0) {
    snprintf(result, sizeof(result),
      "{\"shortcircuit\":\"1\",\"data\":{\"temperature\":\"%s\"}}", tStr);
  } else if (sendMode == 1) {
    snprintf(result, sizeof(result),
      "{\"shortcircuit\":\"1\",\"data\":{\"humidity_in\":\"%s\"}}", hStr);
  } else {
    snprintf(result, sizeof(result),
      "{\"device_id\":\"1\",\"data\":{\"temperature\":\"%s\",\"humidity_in\":\"%s\"}}", tStr, hStr);
  }
  return result;
}

void sendMQTT(float temp, float humidity) {
  if (!mqtt.connected()) connectMQTT();
  if (!mqtt.connected()) {
    Serial.println(F("MQTT send failed"));
    return;
  }
  char* msg = formatPayload(temp, humidity);
  mqtt.publish(OUT_TOPIC, msg);
  Serial.println(msg);
}

void printRow(int row, const char* text) {
  char buf[21];
  snprintf(buf, sizeof(buf), "%-20s", text);
  lcd.setCursor(0, row);
  lcd.print(buf);
}

void printTempRow(float temp, int row) {
  char val[8];
  dtostrf(temp, 5, 1, val);
  lcd.setCursor(0, row);
  lcd.print("Temp : ");
  lcd.print(val);
  lcd.write(byte(0));
  lcd.print("C       ");
}

void printHumidRow(float humidity, int row) {
  char line[21];
  if (humidity >= 0) {
    char val[8];
    dtostrf(humidity, 5, 1, val);
    snprintf(line, 21, "Humid: %s %%", val);
  } else {
    snprintf(line, 21, "Humid:  --.- %%");
  }
  printRow(row, line);
}

void updateDisplay(float temp, float humidity) {
  char line[21];

  unsigned long elapsed = millis() - lastSendTime;
  int secs = (elapsed >= SEND_INTERVAL) ? 0 : (SEND_INTERVAL - elapsed) / 1000;
  snprintf(line, 21, "Mode : %-5s  [%2ds]", modeLabels[sendMode], secs);
  printRow(0, line);

  if (sendMode == 0 || sendMode == 2) {
    printTempRow(temp, 1);
  } else {
    printHumidRow(humidity, 1);
  }

  if (sendMode == 2) {
    printHumidRow(humidity, 2);
  } else {
    printRow(2, "");
  }

  lcd.setCursor(0, 3);
  lcd.print("IP   : ");
  lcd.print(Ethernet.localIP());
  lcd.print("  ");
}

void setup() {
  Serial.begin(9600);
  lcd.begin(20, 4);
  lcd.createChar(0, degreeChar);
  lcd.createChar(1, dropChar);
  pinMode(HUMID_PIN,  INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  prefillHumidityBuffer();

  lcd.print("Stabilizing...");
  delay(800);

  connectEthernet();
  lastSendTime = millis() - SEND_INTERVAL;
}

void loop() {
  checkButton();

  float temp     = readTemperature();
  float humidity = readHumidity();

  bool modeChanged = (sendMode != lastDisplayMode);
  if (modeChanged || millis() - lastDisplayTime >= DISPLAY_INTERVAL) {
    if (modeChanged) lcd.clear();
    lastDisplayMode = sendMode;
    lastDisplayTime = millis();
    updateDisplay(temp, humidity);
  }

  if (millis() - lastSendTime >= SEND_INTERVAL) {
    lastSendTime = millis();
    Serial.print("SEND TIME: ");       // ADD THIS
    Serial.println(lastSendTime); 
    sendMQTT(temp, humidity);
  }

  mqtt.loop();
}
