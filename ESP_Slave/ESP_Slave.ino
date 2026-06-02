// #define BLYNK_TEMPLATE_ID "TMPL3SC0jPqUp"
// #define BLYNK_TEMPLATE_NAME "Overhead Line Monitor"
// #define BLYNK_AUTH_TOKEN "iuuzHN6ZpaS4tfiqktYIOiUF7bM8oOal"

// #define BLYNK_TEMPLATE_ID "TMPL3gH5Vt-HF"
// #define BLYNK_TEMPLATE_NAME "Overhead Line Monitor"
// #define BLYNK_AUTH_TOKEN "YgxYTiW7opNHwB1nvPXUfc4DVI9gHimz"

#define BLYNK_TEMPLATE_ID "TMPL3SC0jPqUp"
#define BLYNK_TEMPLATE_NAME "Overhead Line Monitor"
#define BLYNK_AUTH_TOKEN "WZuonyd3Im_JyL42wiOx5tLB8NEz6qzQ"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <espnow.h>

// Wi-Fi credentials
// char ssid[] = "JioFiber-oMP2O";
// char pass[] = "Iebeequeequuey5a";

char ssid[] = "Nothing_Phone_(2a)";
char pass[] = "pt9mefec2ik68sq";

typedef struct struct_message {
  int id;
  float x;
  float y;
}struct_message;

struct_message myData;

struct_message board1;
struct_message board2;
struct_message board3;

unsigned long lastTime = 0;
unsigned long timerDelay = 1000;

struct_message boardsStruct[3] = {board1, board2, board3};

void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&myData, incomingData, sizeof(myData));
  //Serial.printf("Board ID %u: %u bytes\n", myData.id, len);
  // Update the structures with the new incoming data

  boardsStruct[myData.id-1].x = myData.x;
  //Serial.println(myData.x);
  boardsStruct[myData.id-1].y = myData.y;

  //Serial.printf("x value: %d \n", boardsStruct[myData.id-1].x);
  //Serial.printf("y value: %d \n", boardsStruct[myData.id-1].y);
  //Serial.println();
}

// Store fixed test values
//float currentNode1 = 111; // Healthy (above threshold)
//float currentNode2 = 99;  // Breakage (below threshold)

double currentNode1 = boardsStruct[0].x*1000;
double currentNode2 = boardsStruct[1].x*1000;

// Threshold for break detection
double currentThresholdNode1 = 0.15*1000;
double currentThresholdNode2 = 0.15*1000;
double currentThresholdNode1Node2 = 0.25*1000;

// Function to process individual node status
void processNode(int nodeID, double value1, double value2, int redPin, int greenPin, int dataPin) {
  if(nodeID == 1){
    if (value1 < currentThresholdNode1) {
      Serial.printf("Node %d: ❌ BREAKAGE Detected!\n", nodeID);
      Blynk.virtualWrite(redPin, 255);
      Blynk.virtualWrite(greenPin, 0);
    }else if (value1 > currentThresholdNode1 && value1 < currentThresholdNode1Node2 && value2 > currentThresholdNode2) {
      Serial.printf("Node %d: ❌ BREAKAGE Detected!\n", nodeID);
      Blynk.virtualWrite(redPin, 255);
      Blynk.virtualWrite(greenPin, 0);
    } else {
      Serial.printf("Node %d: ✅ Healthy\n", nodeID);
      Blynk.virtualWrite(redPin, 0);
      Blynk.virtualWrite(greenPin, 255);
    }
    Blynk.virtualWrite(dataPin, value1);
  }
  if(nodeID == 2){
    if (value2 < currentThresholdNode2) {
      Serial.printf("Node %d: ❌ BREAKAGE Detected!\n", nodeID);
      Blynk.virtualWrite(redPin, 255);
      Blynk.virtualWrite(greenPin, 0);
    } else {
      Serial.printf("Node %d: ✅ Healthy\n", nodeID);
      Blynk.virtualWrite(redPin, 0);
      Blynk.virtualWrite(greenPin, 255);
    }
    Blynk.virtualWrite(dataPin, value2);
  }
}

// Function to compare nodes and detect break location
void checkBreakage() {
  String statusMsg;
  Serial.println(currentNode1);
  Serial.println(currentNode2);
  Serial.println(".........");
  if (currentNode1 >= currentThresholdNode1Node2 && currentNode2 >= currentThresholdNode2) {
    statusMsg = "✅ Line Healthy";
  } else if (currentNode1 >= currentThresholdNode1 && currentNode2 < currentThresholdNode2) {
    statusMsg = "❌ Break after Node 2";
  }else if(currentNode1 >= currentThresholdNode1 && currentNode1 < currentThresholdNode1Node2 && currentNode2 >= currentThresholdNode2){
    statusMsg = "❌ Break between Node 1 and Node 2";
  } else if (currentNode1 < currentThresholdNode1 && currentNode2 < currentThresholdNode2) { 
    statusMsg = "⚠ No current in both nodes (possible upstream fault)";
  } else {
    statusMsg = "⚠ Unexpected readings";
  }

  Serial.println(statusMsg);
  Blynk.virtualWrite(V6, statusMsg);  // Send message to Blynk display widget
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  // Connect to Wi-Fi for Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  Serial.println("Gateway ready with fixed values...");

  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

  currentNode1 = boardsStruct[0].x*1000;
  currentNode2 = boardsStruct[1].x*1000;

  // Process both nodes immediately
  processNode(1, currentNode1, currentNode2, V0, V1, V2);
  processNode(2, currentNode1, currentNode2, V3, V4, V5);
  checkBreakage();
}

void loop() {
  Blynk.run();
  // Re-send periodically to keep dashboard updated
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 1000) {
    lastUpdate = millis();
    currentNode1 = boardsStruct[0].x*1000;
    currentNode2 = boardsStruct[1].x*1000;
    processNode(1, currentNode1, currentNode2, V0, V1, V2);
    processNode(2, currentNode1, currentNode2, V3, V4, V5);
    checkBreakage();
  }
}