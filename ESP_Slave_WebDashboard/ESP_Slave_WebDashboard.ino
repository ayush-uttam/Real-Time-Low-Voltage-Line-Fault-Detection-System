// #include <ESP8266WiFi.h>
// #include <ESP8266WebServer.h>

// // ---------- WiFi Credentials ----------
// const char* ssid = "AYUSH-PC0335";
// const char* password = "P!693n57";

// // ---------- Web Server ----------
// ESP8266WebServer server(80);

// // ---------- HTML Page Stored in PROGMEM ----------
// const char index_html[] PROGMEM = R"rawliteral(
// <!DOCTYPE html>
// <html>
// <head>
//   <meta charset="UTF-8">
//   <title>ESP8266 D0 Monitor</title>
//   <style>
//     body {
//       font-family: Arial, sans-serif;
//       text-align: center;
//       background: #0a0a0a;
//       color: #e0e0e0;
//     }
//     .card {
//       display: inline-block;
//       background: #1c1c1c;
//       padding: 20px;
//       margin-top: 40px;
//       border-radius: 12px;
//       box-shadow: 0 0 15px #333;
//       width: 300px;
//     }
//     h1 { color: #00ffff; margin-bottom: 10px; }
//     .status {
//       font-size: 1.5em;
//       margin-top: 10px;
//     }
//     .led {
//       width: 60px;
//       height: 60px;
//       margin: 20px auto;
//       border-radius: 50%;
//       background-color: gray;
//       box-shadow: 0 0 20px #000;
//       transition: background-color 0.3s, box-shadow 0.3s;
//     }
//     .high {
//       background-color: #00ff00;
//       box-shadow: 0 0 25px #00ff00;
//     }
//     .low {
//       background-color: #ff0000;
//       box-shadow: 0 0 25px #ff0000;
//     }
//   </style>
// </head>
// <body>
//   <h1>ESP8266 Web Dashboard</h1>
//   <div class="card">
//     <h2>D0 Pin Status:</h2>
//     <div class="led" id="led"></div>
//     <div class="status" id="pinState">Loading...</div>
//   </div>

//   <script>
//     async function updateStatus() {
//       try {
//         const res = await fetch('/status');
//         const data = await res.text();
//         const led = document.getElementById('led');
//         const text = document.getElementById('pinState');
//         text.textContent = data;

//         if (data.trim() === "HIGH") {
//           led.classList.add("high");
//           led.classList.remove("low");
//         } else {
//           led.classList.add("low");
//           led.classList.remove("high");
//         }
//       } catch (err) {
//         document.getElementById('pinState').textContent = "Error";
//       }
//     }

//     setInterval(updateStatus, 1000);
//     updateStatus();
//   </script>
// </body>
// </html>
// )rawliteral";

// // ---------- Function to Handle Root Page ----------
// void handleRoot() {
//   server.send_P(200, "text/html", index_html);
// }

// // ---------- Function to Return Pin Status ----------
// void handleStatus() {
//   int state = digitalRead(D0);
//   server.send(200, "text/plain", (state == HIGH) ? "HIGH" : "LOW");
// }

// void setup() {
//   Serial.begin(115200);
//   delay(100);

//   pinMode(D0, INPUT);
  
//   Serial.println();
//   Serial.println("Connecting to WiFi...");
//   WiFi.begin(ssid, password);

//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
  
//   Serial.println("\nWiFi Connected!");
//   Serial.print("IP Address: ");
//   Serial.println(WiFi.localIP());

//   // Define Routes
//   server.on("/", handleRoot);
//   server.on("/status", handleStatus);

//   // Start Server
//   server.begin();
//   Serial.println("HTTP Server started");
// }

// void loop() {
//   server.handleClient();
// }































#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <espnow.h>

// ---------- WiFi Credentials ----------
const char* ssid = "AYUSH-PC0335";
const char* password = "P!693n57";

// ---------- Web Server ----------
ESP8266WebServer server(80);

typedef struct struct_message {
  int id;
  float x;
  float y;
}struct_message;

#define relayPin D2

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
  //Serial.printf("Board ID %u: %u bytes\n", myData.id, len);
  // Update the structures with the new incoming data
  boardsStruct[myData.id-1].x = myData.x;
  boardsStruct[myData.id-1].y = myData.y;
  //Serial.printf("x value: %d \n", boardsStruct[myData.id-1].x);
  //Serial.printf("y value: %d \n", boardsStruct[myData.id-1].y);
  //Serial.println();
}
 

// ---------- HTML Page Stored in PROGMEM ----------
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>ESP8266 D0 Monitor</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      text-align: center;
      background: #0a0a0a;
      color: #e0e0e0;
    }
    .card {
      display: inline-block;
      background: #1c1c1c;
      padding: 20px;
      margin-top: 40px;
      border-radius: 12px;
      box-shadow: 0 0 15px #333;
      width: 300px;
    }
    h1 { color: #00ffff; margin-bottom: 10px; }
    .status {
      font-size: 1.5em;
      margin-top: 10px;
    }
    .led {
      width: 60px;
      height: 60px;
      margin: 20px auto;
      border-radius: 50%;
      background-color: gray;
      box-shadow: 0 0 20px #000;
      transition: background-color 0.3s, box-shadow 0.3s;
    }
    .high {
      background-color: #00ff00;
      box-shadow: 0 0 25px #00ff00;
    }
    .low {
      background-color: #ff0000;
      box-shadow: 0 0 25px #ff0000;
    }
  </style>
</head>
<body>
  <h1>ESP8266 Web Dashboard</h1>
  <div class="card">
    <h2>D0 Pin Status:</h2>
    <div class="led" id="led"></div>
    <div class="status" id="pinState">Loading...</div>
  </div>

  <script>
    async function updateStatus() {
      try {
        const res = await fetch('/status');
        const data = await res.text();
        const led = document.getElementById('led');
        const text = document.getElementById('pinState');
        text.textContent = data;

        if (data.trim() === "HIGH") {
          led.classList.add("high");
          led.classList.remove("low");
        } else {
          led.classList.add("low");
          led.classList.remove("high");
        }
      } catch (err) {
        document.getElementById('pinState').textContent = "Error";
      }
    }

    setInterval(updateStatus, 1000);
    updateStatus();
  </script>
</body>
</html>
)rawliteral";

// ---------- Function to Handle Root Page ----------
void handleRoot() {
  server.send_P(200, "text/html", index_html);
}

// ---------- Function to Return Pin Status ----------
void handleStatus() {
  int state = digitalRead(D0);
  server.send(200, "text/plain", (state == HIGH) ? "HIGH" : "LOW");
}

void setup() {
  Serial.begin(115200);
  delay(100);

  pinMode(D0, INPUT);

  pinMode(relayPin,OUTPUT);
  digitalWrite(relayPin,LOW);
  
  Serial.println();
  Serial.println("Connecting to WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  //Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

  // Define Routes
  server.on("/", handleRoot);
  server.on("/status", handleStatus);

  // Start Server
  server.begin();
  Serial.println("HTTP Server started");
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
    //float node3Curr = boardsStruct[2].x;//Node 3 current sensor
    //float node3Flex = boardsStruct[2].y;//Node 3 flex sensor
    Serial.println(node1Curr);
    Serial.println(node1Flex);
    Serial.println("........");
    Serial.println(node2Curr);
    Serial.println(node2Flex);
    //Serial.println("........");
    //Serial.println(node3Curr);
    //Serial.println(node3Flex);
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
    lastTime = millis();
    server.handleClient();
}
}
