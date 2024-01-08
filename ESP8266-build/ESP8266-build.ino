#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <LittleFS.h>
#include <SoftwareSerial.h>

const char* ssid = "NV_TECH";     
const char* password = "7777@123";
const int relayPin = 0;


ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

void setup() {
  
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);   
  WiFi.begin(ssid, password);
  if (!LittleFS.begin()) {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

  server.on("/", HTTP_GET, []() {
    File file = LittleFS.open("/index.html", "r");
    server.streamFile(file, "text/html");
    file.close();
  });

  server.on("/styles.css", HTTP_GET, []() {
    File file = LittleFS.open("/styles.css", "r");
    server.streamFile(file, "text/css");
    file.close();
  });

  server.on("/script.js", HTTP_GET, []() {
    File file = LittleFS.open("/script.js", "r");
    server.streamFile(file, "application/javascript");
    file.close();
  });

  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  pinMode(relayPin, OUTPUT);
}

void loop() {
  server.handleClient();
  webSocket.loop();
  if(WiFi.status() == WL_CONNECTED){
    Serial.println(WiFi.localIP().toString());
  }
  
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');
    webSocket.broadcastTXT(data); 
  }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

  if (type == WStype_TEXT) {
    if (payload[0] == '1') {
      digitalWrite(relayPin, HIGH); 
    } else if (payload[0] == '0') {
      digitalWrite(relayPin, LOW);
    }
  }

  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {
      IPAddress ip = webSocket.remoteIP(num);
      Serial.printf("[%u] Connection from %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);
      break;
    }
    case WStype_TEXT:
      Serial.printf("[%u] Text: %s\n", num, payload);
      // Here you can send a message back to the client
      break;
    case WStype_BIN:
      Serial.printf("[%u] Binary length: %u\n", num, length);
      break;
  }
}

