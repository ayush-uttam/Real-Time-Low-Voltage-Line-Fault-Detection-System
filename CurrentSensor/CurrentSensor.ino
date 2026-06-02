#define CURRENT_SENSOR_PIN A0
const float calibrationfactor = 0.00192;  // Amps per unit
const int sampleCount = 1000;             // Number of samples for averaging
float voltageOffset;
void setup() {
  Serial.begin(115200);
  voltageOffset = calibrate_voltage_Offset();
  Serial.println("Offset Value :" +String(voltageOffset));
  delay(1000);
}

void loop() {
  float rmsCurrent = measureCurrent();
  Serial.print("RMS Current: ");
  Serial.print(rmsCurrent, 4);    // 3 decimal places
  Serial.println(" A");
  delay(1000);
}

float calibrate_voltage_Offset() {
  long total = 0;
  for (int i=0;i<500;i++){
    total += analogRead(CURRENT_SENSOR_PIN);
  }
  return total / (float)500;    // average value of offset
}

float measureCurrent() {
  long total = 0;
  voltageOffset = calibrate_voltage_Offset();
  for(int i = 0; i< sampleCount; i++) {
    float voltage = analogRead(CURRENT_SENSOR_PIN) - voltageOffset;
    total += voltage*voltage; // sum of squares
  }
  float rmsVoltage = sqrt(total / (float)sampleCount);                                  // Root Mean square Voltage
  Serial.println(rmsVoltage, 4);
  return rmsVoltage * calibrationfactor > 0.004 ? rmsVoltage * calibrationfactor : 0;   // convert to current
}