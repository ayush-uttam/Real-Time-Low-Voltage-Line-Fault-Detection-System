#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Arduino.h>
#include <SoftwareSerial.h>

#define relayPin D2

typedef struct struct_message {
  int id;
  float x;
  float y;
}struct_message;

struct forUART{
  float node1;
  float node2;
} forUART;

// Create a struct_message called myData
struct_message myData;

// Create a structure to hold the readings from each board
struct_message board1;
struct_message board2;
struct_message board3;

unsigned long lastTime = 0;
unsigned long timerDelay = 1000;

// Create an array with all the structures
struct_message boardsStruct[3] = {board1, board2, board3};

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&myData, incomingData, sizeof(myData));
  boardsStruct[myData.id-1].x = myData.x;
  boardsStruct[myData.id-1].y = myData.y;
}
 
void setup() {
  //Initialize Serial Monitor
  Serial.begin(9600);
  //mySerial.begin(9600);
  pinMode(relayPin,OUTPUT);
  digitalWrite(relayPin,LOW);
  
  //Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  //Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}

bool node1Status=true;
bool node2Status=true;

float node1Threshold=0.15;
float node2Threshold=0.15;
float node1node2Threshold=0.25;

int currCheck(float node1Curr, float node2Curr){
  if(node1Curr>=node1node2Threshold&&node2Curr>=node2Threshold)
    return 0;//Both loads connected
  else if(node1Curr>=node1Threshold&&node2Curr<=node2Threshold)
    return 1;//Node 1 working
  else if(node1Curr>=node1Threshold&&node1Curr<=node1node2Threshold&&node2Curr>=node2Threshold)
    return 2;//Node 2 working
  else if(node1Curr<=node1Threshold&&node2Curr<=node2Threshold)
    return 3;//Both loads are not connected
  return 999;
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    // Set values to send
    float node1Curr = boardsStruct[0].x;//Node 1 current sensor
    float node1Flex = boardsStruct[0].y;//Node 1 flex sensor
    float node2Curr = boardsStruct[1].x;//Node 2 current sensor
    float node2Flex = boardsStruct[1].y;//Node 2 flex sensor
    forUART.node1 = node1Curr;
    forUART.node2 = node2Curr;
    //float node3Curr = boardsStruct[2].x;//Node 3 current sensor
    //float node3Flex = boardsStruct[2].y;//Node 3 flex sensor
    Serial.println(node1Curr);
    //Serial.println(node1Flex);
    Serial.println("........");
    Serial.println(node2Curr);
    int currStatus = currCheck(node1Curr, node2Curr);
    if(currStatus==0){
      Serial.println("Load at Both Nodes");
      digitalWrite(relayPin,LOW);
    }
    else if(currStatus==1){
      Serial.println("Break after Node 2");
      digitalWrite(relayPin,HIGH);
    }
    else if(currStatus==2){
      Serial.println("Break between Node 1 and Node 2");
      digitalWrite(relayPin,HIGH);
    }
    else if(currStatus==3){
      Serial.println("Break before Node 1");
      digitalWrite(relayPin,HIGH);
    }
    else if(currStatus==999){
      Serial.println("Error");
    }
    Serial.println();
    Serial.write('`');
    Serial.write((byte*)&forUART,sizeof(forUART));
    Serial.write('~');
    lastTime = millis();

  }  
}