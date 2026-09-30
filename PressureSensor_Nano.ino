// 0-100 bar pressure sensor (0.5-4.5 V output) read by an Arduino Nano.
// Wiring: sensor signal -> A0, sensor +V -> 5V, sensor GND -> GND.
// Streams CSV lines over serial: millis,raw_adc,voltage_V,pressure_bar

const uint8_t  SENSOR_PIN      = A0;
const float    VREF            = 4.78;    // Nano ADC reference (measure your 5V pin and put the real value here)
const float    V_MIN           = 0.5;    // sensor output at 0 bar
const float    V_MAX           = 4.5;    // sensor output at full scale
const float    P_MAX_BAR       = 100.0;  // full-scale pressure
const uint8_t  NUM_SAMPLES     = 16;     // readings averaged per output line
const uint16_t SAMPLE_PERIOD_MS = 100;   // 10 Hz output

unsigned long lastSample = 0;

void setup() {
  Serial.begin(115200);
  analogReference(DEFAULT);
  Serial.println("millis,raw_adc,voltage_V,pressure_bar");
}

void loop() {
  unsigned long now = millis();
  if (now - lastSample < SAMPLE_PERIOD_MS) return;
  lastSample = now;

  uint32_t sum = 0;
  for (uint8_t i = 0; i < NUM_SAMPLES; i++) {
    sum += analogRead(SENSOR_PIN);
  }
  float raw = (float)sum / NUM_SAMPLES;

  float voltage  = raw * VREF / 1023.0;
  float pressure = (voltage - V_MIN) * P_MAX_BAR / (V_MAX - V_MIN);
  if (pressure < 0) pressure = 0;  // small negative values are just noise/offset near 0 bar

  Serial.print(now);
  Serial.print(',');
  Serial.print(raw, 1);
  Serial.print(',');
  Serial.print(voltage, 3);
  Serial.print(',');
  Serial.println(pressure, 2);
}
