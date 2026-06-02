#include <ESP8266WiFi.h>
#include <espnow.h>

#define CURRENT_SENSOR_PIN A0

const float calibrationfactor = 0.00192;  // Amps per unit
const int sampleCount = 1000;             // Number of samples for averaging
float voltageOffset;

// REPLACE WITH RECEIVER MAC Address
//uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t broadcastAddress[] = {0xA4,0xCF,0x12,0xC5,0xB5,0x99};

// Set your Board ID (ESP32 Sender #1 = BOARD_ID 1, ESP32 Sender #2 = BOARD_ID 2, etc)
#define BOARD_ID 2

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
    int id;
    float x;
    float y;
} struct_message;

// Create a struct_message called test to store variables to be sent
struct_message myData;

unsigned long lastTime = 0;
unsigned long timerDelay = 1000;

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("\r\nLast Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  voltageOffset = calibrate_voltage_Offset();
  Serial.println("Offset Value :" +String(voltageOffset));
  delay(1000);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  } 
  // Set ESP-NOW role
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);

  // Once ESPNow is successfully init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

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
  return rmsVoltage * calibrationfactor > 0.004 ? rmsVoltage * calibrationfactor : 0;   // convert to current
  //return rmsVoltage;
}

int a=0;
 
void loop() {
  if ((millis() - lastTime) > timerDelay) {
    // Set values to send
    float rmsCurrent = measureCurrent();
    myData.id = BOARD_ID;
    myData.x = rmsCurrent;
    myData.y = 0.00;
    Serial.println(rmsCurrent,4);
    // Send message via ESP-NOW
    esp_now_send(0, (uint8_t *) &myData, sizeof(myData));
    lastTime = millis();
  }
}