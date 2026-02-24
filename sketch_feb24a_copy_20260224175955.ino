#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "NASA";
const char* password = "W-gv_12444456;%";

WebServer server(80);

// Pins
#define PIR_PIN 13
#define LIGHT_RELAY 26
#define FAN_RELAY 27

unsigned long lastMotionTime = 0;
unsigned long delayTime = 30000; // 30 seconds

bool lightStatus = false;
bool fanStatus = false;

void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Smart Room</title>
<style>
body {
  font-family: Arial;
  background: #111;
  color: white;
  text-align: center;
}
.card {
  background: #1e1e1e;
  padding: 20px;
  margin: 15px;
  border-radius: 15px;
  box-shadow: 0 0 15px rgba(0,255,150,0.2);
}
.status {
  font-size: 22px;
  margin: 10px;
}
.switch {
  position: relative;
  display: inline-block;
  width: 60px;
  height: 34px;
}
.switch input {display:none;}
.slider {
  position: absolute;
  cursor: pointer;
  background-color: #ccc;
  border-radius: 34px;
  top: 0; left: 0; right: 0; bottom: 0;
  transition: .4s;
}
.slider:before {
  position: absolute;
  content: "";
  height: 26px; width: 26px;
  left: 4px; bottom: 4px;
  background-color: white;
  border-radius: 50%;
  transition: .4s;
}
input:checked + .slider {
  background-color: #00ff99;
}
input:checked + .slider:before {
  transform: translateX(26px);
}
</style>
</head>
<body>

<h1>🏠 SMART ROOM</h1>

<div class="card">
  <div class="status">PIR: <span id="pir">Loading...</span></div>
</div>

<div class="card">
  <div class="status">Light</div>
  <label class="switch">
    <input type="checkbox" id="light" onchange="toggleLight(this)">
    <span class="slider"></span>
  </label>
</div>

<div class="card">
  <div class="status">Fan</div>
  <label class="switch">
    <input type="checkbox" id="fan" onchange="toggleFan(this)">
    <span class="slider"></span>
  </label>
</div>

<script>
function toggleLight(el){
  fetch(el.checked ? "/lightOn" : "/lightOff");
}
function toggleFan(el){
  fetch(el.checked ? "/fanOn" : "/fanOff");
}

setInterval(()=>{
  fetch("/status")
  .then(res => res.json())
  .then(data => {
    document.getElementById("pir").innerHTML = data.pir;
    document.getElementById("light").checked = data.light;
    document.getElementById("fan").checked = data.fan;
  });
},1000);
</script>

</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);

  pinMode(PIR_PIN, INPUT);
  pinMode(LIGHT_RELAY, OUTPUT);
  pinMode(FAN_RELAY, OUTPUT);

  digitalWrite(LIGHT_RELAY, HIGH); // Relay OFF (Active LOW)
  digitalWrite(FAN_RELAY, HIGH);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);

  server.on("/status", [](){
    String json = "{";
    json += "\"pir\":\"" + String(digitalRead(PIR_PIN) ? "MOTION" : "NO MOTION") + "\",";
    json += "\"light\":" + String(lightStatus ? "true" : "false") + ",";
    json += "\"fan\":" + String(fanStatus ? "true" : "false");
    json += "}";
    server.send(200, "application/json", json);
  });

  server.on("/lightOn", [](){
    digitalWrite(LIGHT_RELAY, LOW);
    lightStatus = true;
    server.send(200, "text/plain", "OK");
  });

  server.on("/lightOff", [](){
    digitalWrite(LIGHT_RELAY, HIGH);
    lightStatus = false;
    server.send(200, "text/plain", "OK");
  });

  server.on("/fanOn", [](){
    digitalWrite(FAN_RELAY, LOW);
    fanStatus = true;
    server.send(200, "text/plain", "OK");
  });

  server.on("/fanOff", [](){
    digitalWrite(FAN_RELAY, HIGH);
    fanStatus = false;
    server.send(200, "text/plain", "OK");
  });

  server.begin();
}

void loop() {
  server.handleClient();

  int motion = digitalRead(PIR_PIN);

  if (motion == HIGH) {
    digitalWrite(LIGHT_RELAY, LOW);
    digitalWrite(FAN_RELAY, LOW);
    lightStatus = true;
    fanStatus = true;
    lastMotionTime = millis();
  }

  if (lightStatus && (millis() - lastMotionTime > delayTime)) {
    digitalWrite(LIGHT_RELAY, HIGH);
    digitalWrite(FAN_RELAY, HIGH);
    lightStatus = false;
    fanStatus = false;
  }
}