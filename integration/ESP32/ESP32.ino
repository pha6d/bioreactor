#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <WebSocketsClient.h>
#include "config.h"

// Define the pins for Serial2
const int rxPin = 18;
const int txPin = 19;

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000); // Update every 60 seconds

// WebSocket client
WebSocketsClient webSocket;

// Variable pour suivre le dernier message reçu
unsigned long lastMessageTime = 0;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  Serial.println("WebSocket event received");
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.println("WebSocket disconnected");
      break;
    case WStype_CONNECTED:
      Serial.println("WebSocket connected");
      break;
    case WStype_TEXT:
      Serial.printf("WebSocket received text: %s\n", payload);
      lastMessageTime = millis();
      handleCommand((char*)payload);
      break;
    case WStype_BIN:
      Serial.println("WebSocket received binary data");
      break;
    case WStype_PING:
      Serial.println("WebSocket received ping");
      break;
    case WStype_PONG:
      Serial.println("WebSocket received pong");
      break;
    default:
      Serial.printf("WebSocket received unhandled event type: %d\n", type);
      break;
  }
}

void handleCommand(const char* payload) {
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, payload);
  if (error) {
    Serial.println("Failed to parse JSON command");
    return;
  }

  String program = doc["program"];
  String command;

  if (program == "mix") {
    int speed = doc["speed"];
    command = "mix " + String(speed);
  } else if (program == "drain") {
    int rate = doc["rate"];
    int duration = doc["duration"];
    command = "drain " + String(rate) + " " + String(duration);
  } else if (program == "fermentation") {
    float temperature = doc["temperature"];
    float pH = doc["pH"];
    float dissolvedOxygen = doc["dissolvedOxygen"];
    float nutrientConcentration = doc["nutrientConcentration"];
    float baseConcentration = doc["baseConcentration"];
    int duration = doc["duration"];
    String experimentName = doc["experimentName"];
    String comment = doc["comment"];
    command = "fermentation " + String(temperature) + " " + String(pH) + " " + 
              String(dissolvedOxygen) + " " + String(nutrientConcentration) + " " + 
              String(baseConcentration) + " " + String(duration) + " " + 
              experimentName + " " + comment;
  } else if (program == "stop") {
    command = "stop";
  } else {
    Serial.println("Unknown program: " + program);
    return;
  }

  Serial2.println(command);
  Serial.println("Sent to Arduino: " + command);
}

void setup() {
  Serial.begin(115200);
  delay(3000);
  Serial.println("ESP32 Ready");
  Serial2.begin(9600, SERIAL_8N1, rxPin, txPin);
  Serial2.setRxBufferSize(256);
  Serial2.setTimeout(500);

  Serial.println("ESP32 ready");
  delay(1000);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Initialize NTP Client
  timeClient.begin();

  // Initialize WebSocket connection
  webSocket.setExtraHeaders("X-Client-Type: ESP32");
  webSocket.begin("192.168.1.25", 8000, "/ws");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
}

void loop() {
  static unsigned long lastCheck = 0;
  
  webSocket.loop();

  // Vérifier périodiquement l'état de la connexion WebSocket
  if (millis() - lastCheck > 5000) {
    lastCheck = millis();
    if (!webSocket.isConnected()) {
      Serial.println("WebSocket disconnected. Attempting to reconnect...");
      webSocket.disconnect();
      webSocket.begin("192.168.1.25", 8000, "/ws");
    } else {
      Serial.println("WebSocket is connected");
    }
    
    // Vérifier si nous avons reçu un message récemment
    if (millis() - lastMessageTime > 60000) {
      Serial.println("No WebSocket message received in the last minute");
    }
  }

  static String receivedData = "";

  while (Serial2.available()) {
    char incomingChar = Serial2.read();
    receivedData += incomingChar;

    if (incomingChar == '\n') {
      Serial.print("Received from Arduino Mega: ");
      Serial.println(receivedData);

      JsonDocument doc;
      DeserializationError error = deserializeJson(doc, receivedData);
      if (!error) {
        Serial.println("Valid JSON received");
        Serial.println(receivedData);

        if (WiFi.status() == WL_CONNECTED) {
          HTTPClient http;
          http.begin("http://192.168.1.25:8000/sensor_data");
          http.addHeader("Content-Type", "application/json");

          // Prepare JSON data
          if (doc.containsKey("ev") && doc["ev"] == "startup") {
            doc["event"] = doc["ev"];
            doc["programType"] = doc["pt"];
            doc["rateOrSpeed"] = doc["rate"];
            doc["duration"] = doc["dur"];
            doc["tempSetpoint"] = doc["tSet"];
            doc["phSetpoint"] = doc["phSet"];
            doc["doSetpoint"] = doc["doSet"];
            doc["nutrientConc"] = doc["nutC"];
            doc["baseConc"] = doc["baseC"];
            doc["experimentName"] = doc["expN"];
            doc["comment"] = doc["comm"];
          } else {
            doc["event"] = "data";
            doc["programType"] = doc["prog"];
            doc["rateOrSpeed"] = 0;
            doc["duration"] = 0;
            doc["tempSetpoint"] = 0.0;
            doc["phSetpoint"] = 0.0;
            doc["doSetpoint"] = 0.0;
            doc["nutrientConc"] = 0.0;
            doc["baseConc"] = 0.0;
            doc["experimentName"] = "";
            doc["comment"] = "";
          }

          doc["currentProgram"] = doc["prog"];
          doc["programStatus"] = doc["stat"];
          doc["airPumpStatus"] = doc["ap"];
          doc["drainPumpStatus"] = doc["dp"];
          doc["nutrientPumpStatus"] = doc["np"];
          doc["basePumpStatus"] = doc["bp"];
          doc["stirringMotorStatus"] = doc["sm"];
          doc["heatingPlateStatus"] = doc["hp"];
          doc["ledGrowLightStatus"] = doc["lg"];
          doc["waterTemp"] = doc["wT"];
          doc["airTemp"] = doc["aT"];
          doc["ph"] = doc["pH"];
          doc["turbidity"] = doc["tb"];
          doc["oxygen"] = doc["ox"];
          doc["airFlow"] = doc["af"];

          String jsonData;
          serializeJson(doc, jsonData);
          jsonData = "{\"sensor_value\": " + jsonData + ", \"timestamp\": \"" + timeClient.getFormattedTime() + "\"}";
          Serial.print("Sending JSON to server: ");
          Serial.println(jsonData);

          int httpResponseCode = http.POST(jsonData);
          if (httpResponseCode > 0) {
            String response = http.getString();
            Serial.println("Server response: " + response);
          } else {
            Serial.print("Error on sending POST: ");
            Serial.println(httpResponseCode);
          }
          http.end();
        }
      } else {
        Serial.println("Invalid JSON format received: " + receivedData);
      }
      receivedData = "";
    }
  }
  
  delay(10); // Petit délai pour éviter de surcharger le CPU

  // Log périodique pour s'assurer que la boucle fonctionne
  static unsigned long lastLog = 0;
  if (millis() - lastLog > 10000) {
    lastLog = millis();
    Serial.println("ESP32 loop is running");
  }
}