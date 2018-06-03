#include <gfxfont.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <PubSubClient.h>
#include <ESP8266WiFi.h>

WiFiClient espClient;
PubSubClient psClient(espClient);
//
const char* ssid = "xxx";
const char* password = "xxx";

const char* clientId = "exp-001";

const char* mqtt_server = "xxx";
const char* uname = "xxx";
const char* pass = "xxx";

// OLED
#define OLED_RESET D8
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

// PIN
#define TRIGGER D4
#define ECHO D3
#define PUSHA D5
#define PUSHB D0
#define SCL D5
#define SDA D6

void drawText(String string) {

  display.clearDisplay();
  
  display.setCursor(0, 0);
  display.println(string);
  display.display();
}

void drawSecondary(String string) {

  display.clearDisplay();
  
  display.setCursor(20, 50);
  display.println(string);
  display.display();
}

void reconnect() {
  while(!psClient.connected()) {

//    Serial.println("Connecting " + clientId);
    
    // Connect
    if(psClient.connect(clientId, uname, pass)) {
      Serial.println("MQTT Connected");
    }
  }
}

long checkUS() {

  long duration, distance;
  digitalWrite(TRIGGER, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIGGER, LOW);
  duration = pulseIn(ECHO, HIGH);
  distance = (duration/2) / 29.1;

  Serial.print(distance);
  Serial.println(" cm");

  return distance;
}

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  Serial.println("Connecting to .. ");

  drawText("Booting up..");
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  // Clear the buffer.
  display.clearDisplay();
  // text display tests
  display.setTextSize(1);
  display.setTextColor(WHITE);
  
  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");  
    drawText("Ready to try..");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  psClient.setServer(mqtt_server, 1883);

  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(PUSHA, INPUT);
}

void loop() {

  if(!psClient.connected()) {
    reconnect();
  }
  
  psClient.loop();

  long distance = checkUS();
  
  String payloadUS = "";
  String payloadA = "";
  String payloadB = "";
  
  if(distance < 20) {
    payloadUS = "{ \"value\": 1}";
    int return_us = psClient.publish("ultrasonic", payloadUS.c_str());
    Serial.println("Ultrasonic: " + String(return_us));
    drawSecondary("Ultrasonic: " + String(return_us));
  }

  int pushA = digitalRead(PUSHA);
  if(pushA == HIGH) {
    payloadA = "{ \"value\": 1}";
    int return_a = psClient.publish("pushA", payloadA.c_str());
    Serial.println("Push Button A: " + String(return_a));
    drawSecondary("Push Button A: " + String(return_a));
  }

  int pushB = digitalRead(PUSHB);
  if(pushB == HIGH) {
    payloadB = "{ \"value\": 1}";
    int return_b = psClient.publish("pushB", payloadB.c_str());
    Serial.println("Push Button B: " + String(return_b));
    drawSecondary("Push Button B: " + String(return_b));
  }
  
  delay(1000);
}
