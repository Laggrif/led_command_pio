#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <heltec.h>
#include <ArduinoJson.h>
#include <vector>

#include "backgrounds\rgb.h"

using namespace std;

struct Button {
    const uint8_t PIN;
    bool pressed;
    bool waiting;
    long time;
};

struct Analog {
    const uint8_t PIN;
    int value;
    int old_value;
    long time;
};

struct wifiConnect {
    bool connected;
    long time;
};

Analog analog = {A0, 1023, 1023, static_cast<long>(millis())};
Button button1 = {D3, false, false, static_cast<long>(millis())};
bool lightOn = false;
wifiConnect connected = {false, static_cast<long>(millis())};

long timeSinceAction = millis();
bool disableScreen = false;

const char* ssid = "UPC1864541";
const char* password = "gn7ZrfgkRxyx";

const char* serverName = "http://192.168.0.163:5000/api/lights/color";

WiFiClient client;
HTTPClient http;


String send();
void drawString(vector<String> str);
void ensureWifi();
void setupWiFi();
void setupWiFi(bool forceDisplay);

String send() {
    ensureWifi();

    int httpResponseCode;

    StaticJsonDocument<128> doc;
    JsonObject color = doc.createNestedObject("color");
    JsonArray values = color.createNestedArray("all");
    values.add((lightOn) ? 0 : 5);
    values.add(0);
    values.add((lightOn) ? 0 : 40);
    values.add((lightOn) ? 0 : 255);

    String request;
    serializeJson(doc, request);

    httpResponseCode = http.PUT(request);

    return (String) httpResponseCode;
}


void drawString(vector<String> str) {
    Heltec.display->clear();
    int i = 0;
    for (String &name: str){
        Heltec.display->drawString(0, 9 * i, name);
        i++;
    }
    Heltec.display->display();
    timeSinceAction = millis();
}


void ensureWifi() {
    if (WiFi.status() != WL_IDLE_STATUS && WiFi.status() != 3) {
        Serial.println(WiFi.status());
        Serial.println("WiFi Disconnected");
        setupWiFi();
    }
}


void setupWiFi() {
    setupWiFi(false);
}

void setupWiFi(bool forceDisplay) {
    Heltec.display->clear();
    WiFi.reconnect();

    String connectingStr = "Connecting";
    while(WiFi.status() != WL_CONNECTED)
    {
        delay(300);
        if (!disableScreen || forceDisplay) {
            Heltec.display->drawString(0, 0, connectingStr);
            Heltec.display->display();
        }
        // Display a dot for each attempt.
        connectingStr += ".";
    }
    connected.connected = true;

    http.begin(client, serverName);
    http.addHeader("Content-Type", "application/json");

    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());
    if (!disableScreen || forceDisplay) {
        drawString({WiFi.localIP().toString(), (lightOn) ? "On at " + String((int) (100.0 - analog.value/1023.0 * 100.0 + 0.5)) + "% " : "Off"});
    }
}


void setup() {
    Heltec.begin(true, true);
    drawString({"initial setup"});
    pinMode(button1.PIN, INPUT_PULLUP);

    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
    WiFi.begin(ssid, password);

    analog.value = analogRead(analog.PIN);

    //setupWiFi(true);
    timeSinceAction = millis();

    Heltec.display->setBrightness(0);
}


void loop() {
    int i = 0;
    while (true)
    {
        drawHome(0, 1, 50, 99, 100, i, true);
        i = (i + 1) % 5;
        delay(500);
    }
    

    analog.value = analogRead(analog.PIN);

    if (millis() - timeSinceAction <= 5000 && disableScreen) {
        disableScreen = false;
    } else if (millis() - timeSinceAction > 5000 && !disableScreen) {
        disableScreen = true;
        Heltec.display->clear();
        Heltec.display->display();
    }

    //Reset state of button
    if (button1.pressed && millis() - button1.time > 200 && digitalRead(button1.PIN) == HIGH){
        button1.pressed = false;
    } else if (!button1.pressed && digitalRead(button1.PIN) == LOW && millis() - button1.time > 100)
    {
        button1.pressed = true;
        button1.waiting = true;
        button1.time = millis();
    }

    ensureWifi();
    
    if (button1.pressed && button1.waiting){
        ensureWifi();

        String httpResponseCode = send();

        if (httpResponseCode == "200") {
            lightOn = !lightOn;
        }

        String str = "On at " + String((int) (100 - analog.value/1023.0 * 100.0 + 0.5)) + "% ";
        str = (lightOn) ? str : "Off";
        drawString({WiFi.localIP().toString(), str});
        button1.waiting = false;
    }

    if (analog.value <= analog.old_value - 10 || analog.value >= analog.old_value + 10) {
        ensureWifi();

        int alpha = (int) ((1.0 - analog.value/1023.0) * 255.0 + 0.5);

        String httpResponseCode;
        StaticJsonDocument<32> doc;
        doc["alpha"] = alpha;

        String request;
        serializeJson(doc, request);

        httpResponseCode = String(http.PUT(request));

        if (httpResponseCode == "200"){
            drawString({WiFi.localIP().toString(), (lightOn) ? "On at " + String((int) (alpha/255.0 * 100.0)) + "% " : "Off"});
            analog.old_value = analog.value;
        }
    }
}