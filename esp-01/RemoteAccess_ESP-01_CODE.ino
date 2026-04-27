// ESP-01 Code
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiManager.h>

// TCP Server settings
WiFiServer server(23); //port number

void setup() {
    Serial.begin(9600);  //serial communication with UNO
    
    WiFiManager wifiManager;                         
    wifiManager.autoConnect("ESP8266_AP");                //to configure ssid and password settings remotely through temporary webpage
    
    Serial.println("Connected to WiFi");                  //send ip and port number to UNO for app config
    Serial.println(WiFi.localIP());
    Serial.println("Port=23");
    server.begin();
    Serial.println("TCP Server started");
}

void loop() {
    WiFiClient client = server.available();
    if (client) {
        Serial.println("Client connected");
        while (client.connected()) {                                        
            if (client.available()) {                                                      //read client messages
                String message = client.readStringUntil('\n');
                Serial.println(message); // Forward message to Arduino
            }
            if (Serial.available()) {                                                      //read Uno messages
                String response = Serial.readStringUntil('\n');
                client.println(response); // Send Arduino response back to client
            }
        }
        client.stop();
        Serial.println("Client disconnected");
    }
}
