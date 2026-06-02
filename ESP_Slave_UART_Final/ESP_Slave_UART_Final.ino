// #include <ESP8266WiFi.h>
// #include <WiFiUdp.h>
// #include <NTPClient.h>               
// #include <TimeLib.h>  
// #include <SPI.h>
// #include <SD.h>     
// #include <String.h>   
// #include <Arduino.h>
// #include <WiFiClientSecure.h>
// #include <UniversalTelegramBot.h>
// #include <ArduinoJson.h>

// struct __attribute__((packed)) forUART {
//   float node1;
//   float node2;
//   int stateNode1;
//   int stateNode2;
// } receivedData;

// bool inFrame = false;
// int ind = 0;
// const int structSize = sizeof(forUART);
// byte buffer[sizeof(forUART)];

// unsigned long lastPrint = 0;

// #define relayPin D2

// const int CS = 5;
// File myFile;
// int SerialNumber = 1; 
// const char* ssid = "AYUSH-PC0335";
// const char* password = "P!693n57";

// WiFiUDP ntpUDP;
// NTPClient timeClient(ntpUDP, "asia.pool.ntp.org", 19800, 60000);
 
// char Time[ ] = "TIME:00:00:00";
// char Date[ ] = "DATE:00/00/2000";
// char status[32];
// byte last_second, second_, minute_, hour_, day_, month_;
// int year_;

// #define BOT_TOKEN "7552218776:AAEHpNVsA9V9Tj1qo-yxoJlw_16EGfugB3g"
// #define CHAT_ID1 7085380590
// #define CHAT_ID2 6189968826

// X509List cert(TELEGRAM_CERTIFICATE_ROOT);
// WiFiClientSecure client;
// UniversalTelegramBot bot(BOT_TOKEN, client);

// void setup() {
//   client.setTrustAnchors(&cert);
//   Serial.begin(9600);  // lower = more stable
//   Serial.println("Receiver started...");
//   pinMode(relayPin,OUTPUT);
//   digitalWrite(relayPin,LOW);
//   WiFi.mode(WIFI_STA);
//   WiFi.begin(ssid, password);
//   Serial.print("Connecting.");
//   while ( WiFi.status() != WL_CONNECTED ) {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("connected");
//   timeClient.begin();

//    while (!SD.begin(CS)) {
//     Serial.println("SD card initialization failed!");
//     delay(1000);
//   }
//   delay(1000);
//   Serial.println("SD card initialization done.");
//   SD.remove("NewFile.txt");
// }

// int node1Status;
// int node2Status;

// float node1Threshold=0.15;
// float node2Threshold=0.15;
// float node1node2Threshold=0.25;

// int currCheck(float node1Curr, float node2Curr){
//   if(node1Curr>=node1node2Threshold&&node2Curr>=node2Threshold)
//     return 0;//Both loads connected
//   else if(node1Curr>=node1Threshold&&node2Curr<=node2Threshold)
//     return 1;//Node 1 working
//   else if(node1Curr>=node1Threshold&&node1Curr<=node1node2Threshold&&node2Curr>=node2Threshold)
//     return 2;//Node 2 working
//   else if(node1Curr<=node1Threshold&&node2Curr<=node2Threshold)
//     return 3;//Both loads are not connected
//   return 999;
// }

// bool isStat1Broken = false;
// bool isStat2Broken = false;
// bool isStat3Broken = false;

// void loop() {
//   timeClient.update();
//   unsigned long unix_epoch = timeClient.getEpochTime();
//   second_ = second(unix_epoch);
//   if (last_second != second_) {
//     minute_ = minute(unix_epoch);
//     hour_   = hour(unix_epoch);
//     day_    = day(unix_epoch);
//     month_  = month(unix_epoch);
//     year_   = year(unix_epoch);
 
//     Time[12] = second_ % 10 + 48;
//     Time[11] = second_ / 10 + 48;
//     Time[9]  = minute_ % 10 + 48;
//     Time[8]  = minute_ / 10 + 48;
//     Time[6]  = hour_   % 10 + 48;
//     Time[5]  = hour_   / 10 + 48;
 
//     Date[5]  = day_   / 10 + 48;
//     Date[6]  = day_   % 10 + 48;
//     Date[8]  = month_  / 10 + 48;
//     Date[9]  = month_  % 10 + 48;
//     Date[13] = (year_   / 10) % 10 + 48;
//     Date[14] = year_   % 10 % 10 + 48;
 
//     Serial.println(Time);
//     Serial.println(Date);
 
//     last_second = second_;
//   }
//   char formattedData[50];
//   while (Serial.available() > 0) {
//     byte b = Serial.read();

//     if (b == '`') {
//       inFrame = true;
//       ind = 0;
//     } 
//     else if (b == '~' && inFrame) {
//       inFrame = false;

//       if (ind == structSize) {
//         memcpy(&receivedData, buffer, structSize);
//         // node1Status=receivedData.stateNode1;
//         // Serial.print("Node1 Status: ");
//         // Serial.print(node1Status);
//         // node2Status=receivedData.stateNode2;
//         // Serial.print("\tNode2 Status: ");
//         // Serial.print(node2Status);
//         Serial.print("\nNode1: ");
//         Serial.print(receivedData.node1, 2);
//         Serial.print("\tNode2: ");
//         Serial.println(receivedData.node2, 2);
//         int currStatus = currCheck(receivedData.node1, receivedData.node2);
//         if(currStatus==0){
//           Serial.println("Load at Both Nodes");
//           digitalWrite(relayPin,LOW);
//         }
//         else if(currStatus==1){
//           Serial.println("Break after Node 2");
//           sprintf(formattedData, "%lu - %s - %s - %s", SerialNumber, Date, Time, "Break after Node 2");
//           //bot.sendMessage(String(CHAT_ID2), "Break before Node 1", ""); 
//           digitalWrite(relayPin,HIGH);
//         }
//         else if(currStatus==2){
//           Serial.println("Break between Node 1 and Node 2");
//           sprintf(formattedData, "%lu - %s - %s - %s", SerialNumber, Date, Time, "Break between Node 1 and Node 2");
//           //bot.sendMessage(String(CHAT_ID1), "Break between Node 1 and Node 2", "");
//           //bot.sendMessage(String(CHAT_ID2), "Break between Node 1 and Node 2", "");
//           digitalWrite(relayPin,HIGH);
//         }
//         else if(currStatus==3){
//           Serial.println("Break before Node 1");
//           sprintf(formattedData, "%lu - %s - %s - %s", SerialNumber, Date, Time, "Break before Node 1");
//           //bot.sendMessage(String(CHAT_ID1), "Break before Node 1", "");
//           digitalWrite(relayPin,HIGH);
//         }
//         else if(currStatus==999){
//           sprintf(formattedData, "%lu - %s - %s - %s", SerialNumber, Date, Time, "Invalid");
//           Serial.println("Error");
//         }
//         WriteFile("/NewFile.txt", formattedData);
//         SerialNumber++;
//         Serial.println();
//       } else {
//         Serial.print("⚠️ Packet size mismatch: ");
//         Serial.println(ind);
//       }
//     } 
//     else if (inFrame) {
//       if (ind < structSize)
//         buffer[ind++] = b;
//       else {
//         // Overflow guard
//         inFrame = false;
//         ind = 0;
//       }
//     }
//   }
// }

// long recentMessageTime=0;

// void WriteFile(const char *path, const char *message) {
//   myFile = SD.open(path, FILE_WRITE);
//   if (myFile) {
//     myFile.println(message);
//     myFile.close();
//   } else {
//     Serial.println("error opening file");
//   }
// }


#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>               
#include <TimeLib.h>  
#include <SPI.h>
#include <SD.h>     
#include <String.h>   
#include <Arduino.h>
#include <ESP8266WebServer.h> 

// ----------------- STRUCT & VARIABLES -----------------
struct _attribute_((packed)) forUART {
  float node1;
  float node2;
  int stateNode1;
  int stateNode2;
} receivedData;

bool inFrame = false;
int ind = 0;
const int structSize = sizeof(forUART);
byte buffer[sizeof(forUART)];

unsigned long lastPrint = 0;
#define relayPin D2

const int CS = 5;
File myFile;
int SerialNumber = 1; 

const char* ssid = "AYUSH-PC0335";
const char* password = "P!693n57";

// ----------------- NTP & TIME -----------------
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "asia.pool.ntp.org", 19800, 60000);
char Time[ ] = "TIME:00:00:00";
char Date[ ] = "DATE:00/00/2000";
byte last_second, second_, minute_, hour_, day_, month_;
int year_;

// ----------------- STATUS VARIABLES -----------------
int node1Status;
int node2Status;
float node1Threshold=0.15;
float node2Threshold=0.15;
float node1node2Threshold=0.25;

String currentStatus = "Initializing...";
float lastNode1 = 0.0, lastNode2 = 0.0;

// ----------------- WEB SERVER -----------------
ESP8266WebServer server(80);

// Dashboard HTML
const char DASHBOARD[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<link rel="icon" href="https://ayush-uttam.github.io/Zinnovatio3dot0/icon.png" type="image/x-icon">
<title>Line Breakage Monitor</title>
<style>
  body {
    margin: 0;
    font-family: "Segoe UI", Arial, sans-serif;
    background: radial-gradient(circle at top, #0f0f0f, #000);
    color: #f1f1f1;
    text-align: center;
    min-height: 100vh;
  }
  h1 {
    font-size: 1.8rem;
    color: #FFD700;
    margin-top: 25px;
    text-shadow: 0 0 10px #FFD700;
  }
  .container {
    display: flex;
    justify-content: center;
    flex-wrap: wrap;
    gap: 25px;
    margin-top: 40px;
  }
  .card {
    background: linear-gradient(145deg, #1a1a1a, #101010);
    border: 2px solid #FFD700;
    border-radius: 20px;
    box-shadow: 0 0 20px rgba(255,215,0,0.4);
    width: 270px;
    padding: 25px;
    transition: transform 0.3s ease, box-shadow 0.3s ease;
  }
  .card:hover {
    transform: translateY(-5px);
    box-shadow: 0 0 30px rgba(255,215,0,0.7);
  }
  .label {
    font-size: 1.1rem;
    color: #aaa;
    margin-bottom: 10px;
  }
  .value {
    font-size: 2.2rem;
    font-weight: bold;
  }
  .ok {
    color: #00ff88;
    text-shadow: 0 0 10px #00ff88;
  }
  .break {
    color: #ff2f2f;
    text-shadow: 0 0 15px #ff2f2f;
  }
  button {
    margin-top: 40px;
    background: #FFD700;
    color: #000;
    border: none;
    border-radius: 10px;
    font-size: 1.1rem;
    padding: 12px 25px;
    cursor: pointer;
    font-weight: bold;
    box-shadow: 0 0 10px rgba(255,215,0,0.6);
    transition: all 0.2s ease;
  }
  button:hover {
    transform: scale(1.05);
    box-shadow: 0 0 20px rgba(255,215,0,1);
  }
  #logPopup {
    display: none;
    position: fixed;
    top: 0; left: 0;
    width: 100%; height: 100%;
    background: rgba(0,0,0,0.8);
    color: #FFD700;
    overflow: auto;
    z-index: 10;
    padding: 20px;
  }
  #logPopup pre {
    background: #111;
    padding: 20px;
    border-radius: 10px;
    text-align: left;
    color: #f1f1f1;
    font-size: 0.95rem;
    box-shadow: 0 0 15px rgba(255,215,0,0.3);
  }
  #closeBtn {
    position: fixed;
    top: 15px; right: 20px;
    background: #ff2f2f;
    color: white;
    border: none;
    border-radius: 8px;
    font-size: 1rem;
    padding: 8px 14px;
    cursor: pointer;
    z-index: 20;
  }
  footer {
    margin-top: 50px;
    font-size: 0.9rem;
    color: #555;
  }
</style>
</head>
<body>
  <h1>⚡ Low Voltage Line Breakage Dashboard</h1>
  <div class="container">
    <div class="card">
      <div class="label">Node 1 Current</div>
      <div id="node1" class="value">--</div>
    </div>
    <div class="card">
      <div class="label">Node 2 Current</div>
      <div id="node2" class="value">--</div>
    </div>
    <div class="card">
      <div class="label">System Status</div>
      <div id="status" class="value ok">Loading...</div>
    </div>
  </div>

  <button onclick="showLog()">📜 View SD Log</button>

  <div id="logPopup">
    <button id="closeBtn" onclick="closeLog()">✖ Close</button>
    <pre id="logContent">Loading log...</pre>
  </div>

  <footer>
    Developed by <strong>Kasukabe Defence Group</strong> ⚙
  </footer>

  <script>
    async function updateData() {
      const res = await fetch('/data');
      const data = await res.json();
      document.getElementById('node1').innerText = data.node1.toFixed(2) + " A";
      document.getElementById('node2').innerText = data.node2.toFixed(2) + " A";
      const st = document.getElementById('status');
      st.innerText = data.status;
      st.className = data.status.includes("Break") ? "value break" : "value ok";
    }
    async function showLog(){
      document.getElementById('logPopup').style.display = 'block';
      const res = await fetch('/log');
      const text = await res.text();
      document.getElementById('logContent').innerText = text;
    }
    function closeLog(){
      document.getElementById('logPopup').style.display = 'none';
    }
    setInterval(updateData, 2000);
    updateData();
  </script>
</body>
</html>
)rawliteral";

// ----------------- FUNCTIONS -----------------
int currCheck(float node1Curr, float node2Curr){
  if(node1Curr>=node1node2Threshold && node2Curr>=node2Threshold)
    return 0; // Both loads connected
  else if(node1Curr>=node1Threshold && node2Curr<=node2Threshold)
    return 1; // Break after Node 2
  else if(node1Curr>=node1Threshold && node1Curr<=node1node2Threshold && node2Curr>=node2Threshold)
    return 2; // Break between Node 1 and Node 2
  else if(node1Curr<=node1Threshold && node2Curr<=node2Threshold)
    return 3; // Break before Node 1
  return 999;
}

void WriteFile(const char *path, const char *message) {
  myFile = SD.open(path, FILE_WRITE);
  if (myFile) {
    myFile.println(message);
    myFile.close();
  } else {
    Serial.println("error opening file");
  }
}

void handleLog() {
  if (!SD.exists("/NewFile.txt")) {
    server.send(200, "text/plain", "No log file found.");
    return;
  }

  File logFile = SD.open("/NewFile.txt", FILE_READ);
  if (!logFile) {
    server.send(500, "text/plain", "Error opening log file.");
    return;
  }

  String logData;
  while (logFile.available()) {
    logData += char(logFile.read());
    if (logData.length() > 6000) break;
  }
  logFile.close();

  server.send(200, "text/plain", logData);
}

// ----------------- WEB HANDLERS -----------------
void handleRoot() {
  server.send_P(200, "text/html", DASHBOARD);
}

void handleData() {
  String json = "{\"node1\":" + String(lastNode1,2) +
                ",\"node2\":" + String(lastNode2,2) +
                ",\"status\":\"" + currentStatus + "\"}";
  server.send(200, "application/json", json);
}

// ----------------- SETUP -----------------
void setup() {
  Serial.begin(9600);
  Serial.println("Receiver started...");
  pinMode(relayPin,OUTPUT);
  digitalWrite(relayPin,LOW);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting.");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  timeClient.begin();

  while (!SD.begin(CS)) {
    Serial.println("SD card initialization failed!");
    delay(1000);
  }
  delay(1000);
  Serial.println("SD card initialization done.");
  SD.remove("NewFile.txt");

  // Start web server
  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.on("/log", handleLog);
  server.begin();
  Serial.println("Web server started!");
}

// ----------------- MAIN LOOP -----------------
void loop() {
  server.handleClient(); 

  timeClient.update();
  unsigned long unix_epoch = timeClient.getEpochTime();
  second_ = second(unix_epoch);
  if (last_second != second_) {
    minute_ = minute(unix_epoch);
    hour_   = hour(unix_epoch);
    day_    = day(unix_epoch);
    month_  = month(unix_epoch);
    year_   = year(unix_epoch);
    Time[12] = second_ % 10 + 48;
    Time[11] = second_ / 10 + 48;
    Time[9]  = minute_ % 10 + 48;
    Time[8]  = minute_ / 10 + 48;
    Time[6]  = hour_   % 10 + 48;
    Time[5]  = hour_   / 10 + 48;
    Date[5]  = day_   / 10 + 48;
    Date[6]  = day_   % 10 + 48;
    Date[8]  = month_  / 10 + 48;
    Date[9]  = month_  % 10 + 48;
    Date[13] = (year_ / 10) % 10 + 48;
    Date[14] = year_ % 10 % 10 + 48;
    last_second = second_;
  }

  char formattedData[80];
  while (Serial.available() > 0) {
    byte b = Serial.read();

    if (b == '`') {
      inFrame = true;
      ind = 0;
    } 
    else if (b == '~' && inFrame) {
      inFrame = false;

      if (ind == structSize) {
        memcpy(&receivedData, buffer, structSize);
        lastNode1 = receivedData.node1;
        lastNode2 = receivedData.node2;

        int currStatus = currCheck(receivedData.node1, receivedData.node2);
        if(currStatus==0){
          currentStatus = "Load at Both Nodes";
          digitalWrite(relayPin,LOW);
        }
        else if(currStatus==1){
          currentStatus = "Break after Node 2";
          sprintf(formattedData, "%lu - %s - %s - %s", SerialNumber, Date, Time, "Break after Node 2");
          digitalWrite(relayPin,HIGH);
        }
        else if(currStatus==2){
          currentStatus = "Break between Node 1 and Node 2";
          sprintf(formattedData, "%lu - %s - %s - %s", SerialNumber, Date, Time, "Break between Node 1 and Node 2");
          digitalWrite(relayPin,HIGH);
        }
        else if(currStatus==3){
          currentStatus = "Break before Node 1";
          sprintf(formattedData, "%lu - %s - %s - %s", SerialNumber, Date, Time, "Break before Node 1");
          digitalWrite(relayPin,HIGH);
        }
        else if(currStatus==999){
          currentStatus = "Invalid Reading";
          sprintf(formattedData, "%lu - %s - %s - %s", SerialNumber, Date, Time, "Invalid");
        }

        Serial.println(currentStatus);
        WriteFile("/NewFile.txt", formattedData);
        SerialNumber++;
      }
    } 
    else if (inFrame) {
      if (ind < structSize)
        buffer[ind++] = b;
      else {
        inFrame = false;
        ind = 0;
      }
    }
  }
}