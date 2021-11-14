#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SimpleTimer.h>
#include <ArduinoJson.h>
SimpleTimer timer;
/* DHT22*/
#include "DHT.h"
// Wifi
#define ssid "QC Truong Thanh"
#define password "0978353969QC"
// mqtt
WiFiClient espClient;
PubSubClient client(espClient);
// DHT22
#define DHTPIN 0
#define DHTTYPE DHT22
// Soik
#define SOILPIN A0
#define ONE_WIRE_BUS 2
// config MQTT
#define mqtt_user "duynguyen123"
#define mqtt_pass "4DDF77D157AD4F47"
#define mqtt_broker "ngoinhaiot.com"
const int mqtt_port = 1111;

DHT dht(DHTPIN, DHTTYPE);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
//
const int redButton = 13;
const int yellowButton = 12;
const int greenButton = 14;
const int pumpPin = 5;
const int lampPin = 4;
//
int pumpStatus = 0;
int lampStatus = 0;
float hum = 0;
float temp = 0;
float soilTemp = 0;
int soilMoister = 0;

// Get DaTA tu cam bien
void getDhtData(void) {
  float tempIni = temp;
  float humIni = hum;
  temp = dht.readTemperature();
  hum = dht.readHumidity();
  if (isnan(hum) || isnan(temp))   // Check if any reads failed and exit early (to try again).
  {
    Serial.println("Failed to read from DHT sensor!");
    temp = tempIni;
    hum = humIni;
    return;
  }
}
void getSoilMoisterData(void) {
  soilMoister = 0;
  delay (500);
  int N = 3;
  for (int i = 0; i < N; i++) // read sensor "N" times and get the average
  {
    soilMoister += analogRead(SOILPIN);
    delay(150);
  }
  soilMoister = soilMoister / N;

  soilMoister = 100 - map(soilMoister, 0, 1023, 0, 100);
}

void getSoilTemp(void) {
  soilTemp = sensors.getTempCByIndex(0);
  if (!(soilTemp != DEVICE_DISCONNECTED_C))
  {
    Serial.println("Error: Could not read temperature data");
  }
}

void displayData(void) {
  Serial.print(" Temperature: "); // ham nay chay ok
  Serial.print(temp);
  Serial.print("oC   Humidity: ");
  Serial.print(hum);
  Serial.println("%");
  Serial.print("Soil Temperature: ");
  Serial.println(soilTemp);
  Serial.print("Soil Humidity: ");
  Serial.println(soilMoister);
}

boolean debounce(int pin) {
  boolean state;
  boolean previousState;
  const int debounceDelay = 30;

  previousState = digitalRead(pin);
  for (int counter = 0; counter < debounceDelay; counter++)
  {
    delay(1);
    state = digitalRead(pin);
    if (state != previousState)
    {
      counter = 0;
      previousState = state;
    }
  }
  return state;
}
void startTimers(void) {
  timer.setInterval(1000, readButtonCmd); //
  timer.setInterval(2000, getSoilTemp); // ok
  timer.setInterval(2000, getDhtData); // ok
  timer.setInterval(10000, getSoilMoisterData); // ok
  timer.setInterval(1000, autoControl); // ok
}
void autoControl() {
  if (soilMoister < 50) {
    pumpControl();
  }
  if (temp < 20) {
    lampControl();
  }
}
void pumpControl() {
  turnOnPump();
}
void turnOnPump(){
  pumpStatus = 1;
  control();
  timer.setTimeout(15 * 1000, turnOffPump);
}
void turnOffPump(){
  pumpStatus = 0;
  control();
}
void lampControl() {
  turnOnLamp();
}
void turnOnLamp(){
  lampStatus = 1;
  control();
  timer.setTimeout(5*60000, turnOffLamp);
}
void turnOffLamp(){
  lampStatus = 0;
  control();
}
void control() {
  if (pumpStatus == 1) {
    digitalWrite(pumpPin, HIGH);
  } else {
    digitalWrite(pumpPin, LOW);
  }
  if (lampStatus == 1) {
    digitalWrite(lampPin, HIGH);
  } else {
    digitalWrite(lampPin, LOW);
  }
}
void setup() {
  Serial.begin(9600);

  pinMode(SOILPIN, OUTPUT);

  pinMode(yellowButton, INPUT_PULLUP);
  pinMode(greenButton, INPUT_PULLUP);
  pinMode(redButton, INPUT_PULLUP);

  pinMode(pumpPin, OUTPUT);
  pinMode(lampPin, OUTPUT);
  digitalWrite(pumpPin, LOW);
  digitalWrite(lampPin, LOW);
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  setup_wifi();
  delay(10);
  dht.begin();
  sensors.begin();
  startTimers();
}
void readButtonCmd() {
  boolean buttonStatus = debounce(greenButton);
  if (buttonStatus == 0) {
    pumpStatus = !pumpStatus;
    control();
  }
  buttonStatus = debounce(redButton);
  if (buttonStatus == 0) {
    lampStatus = !lampStatus;
    control();
  }
  buttonStatus = debounce(yellowButton);
  if (buttonStatus == 0) {
    displayData();
  }
}

void setup_wifi() {
  delay(10);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
unsigned long t;
void setup_mqtt() {
  if (!client.connected())// Kiểm tra kết nối
    reconnect();
  client.loop();
}
void reconnect()
{
  while (!client.connected()) // Chờ tới khi kết nối
  {
    if (client.connect("ESP8266_id1", mqtt_user, mqtt_pass)) //kết nối vào broker
    {
      Serial.println("Đã kết nối:");
      client.subscribe("duynguyen123/plant");
    }
    else
    {
      Serial.print("Lỗi:, rc=");
      Serial.print(client.state());
    }
  }
}
void callback(char* topic, byte* payload, unsigned int length) {
  char buffer[256];
  String message;
  StaticJsonDocument<200> doc;
  for (int i = 0; i < length; i++) {
   message += (char)payload[i];
  }
  Serial.println(message);
  if(message == "PumpON"){
    pumpStatus = !pumpStatus;
    control();
  }
  if(message == "PumpOnAuto"){
    pumpControl();
  }
  if(message == "LampON") {
    lampStatus = !lampStatus;
    control();
  }
  if(message == "UPDATE") {
    doc["hum"] = hum;
    doc["temp"] = temp;
    doc["soilTemp"] = soilTemp;
    doc["soilMoister"] = soilMoister;
    doc["lamp"] = lampStatus;
    doc["pump"] = pumpStatus;
    serializeJson(doc, buffer);
    client.publish("duynguyen123/update", buffer); 
  }
}
void sendData() {
  
}
void loop() {
  setup_mqtt();
  timer.run();
}
