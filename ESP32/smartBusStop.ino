#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>

// ------------------------ WIFI ------------------------
const char* ssid = "Shravyaa";
const char* password = "shravyaa";

// -------------------- THINGSPEAK ----------------------
String writeAPI = "5K0YL8TDF4PIK60O";
String channelID = "3167997";

// ---------------------- SENSORS -----------------------
#define TRIG 5
#define ECHO 18
#define IR_PIN 19
#define LDR_DO 21

// ---------------------- OUTPUTS ------------------------
#define LED_BUS 14
#define LED_PASSENGER 4
#define LED_LIGHT 2
#define BUZZER 25

// ---------------------- SERVER -------------------------
WebServer server(80);

// ---------------------- VARIABLES -----------------------
int busIncoming = 0;
int passengerWaiting = 0;
int lightIntensity = 0;

unsigned long lastTS = 0;   // FIXED VARIABLE NAME

// ===================== SETUP ===========================
void setup() {
  Serial.begin(115200);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  pinMode(IR_PIN, INPUT);
  pinMode(LDR_DO, INPUT);

  pinMode(LED_BUS, OUTPUT);
  pinMode(LED_PASSENGER, OUTPUT);
  pinMode(LED_LIGHT, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  digitalWrite(LED_BUS, LOW);
  digitalWrite(LED_PASSENGER, LOW);
  digitalWrite(LED_LIGHT, LOW);
  digitalWrite(BUZZER, LOW);

  // ---------------- WIFI ----------------
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println("\nConnected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// ===================== MEASURE DISTANCE ============================
long getDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(5);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH, 25000);

  if (duration == 0) return -1;

  long distance = (duration * 0.0343) / 2;
  return distance;
}

// ===================== THINGSPEAK ============================
void sendToThingSpeak(int d, int p, int l, int bus) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    String url = "https://api.thingspeak.com/update?api_key=" + writeAPI +
                 "&field1=" + String(d) +
                 "&field2=" + String(p) +
                 "&field3=" + String(l) +
                 "&field4=" + String(bus);

    http.begin(url);
    http.GET();
    http.end();
  }
}

// ===================== LOOP ============================
void loop() {

  long distance = getDistance();
  int irVal = digitalRead(IR_PIN);
  int ldrRaw = digitalRead(LDR_DO);

  // LDR CORRECTED LOGIC
  // If LDR_DO is LOW → DARK (0)
  // If LDR_DO is HIGH → BRIGHT (1)
  int isDark = (ldrRaw == HIGH) ? 1 : 0;   // FIXED as per your requirement

  // Logic
  busIncoming = (distance > 0 && distance < 25) ? 1 : 0;
  passengerWaiting = (irVal == LOW);
  lightIntensity = isDark ? 100 : 500;

  // Outputs
  digitalWrite(LED_BUS, busIncoming);
  digitalWrite(BUZZER, busIncoming);

  digitalWrite(LED_PASSENGER, passengerWaiting);

  digitalWrite(LED_LIGHT, isDark);

  // Debug
  Serial.printf("DIST: %ld cm | IR: %d | DARK: %d | BUS: %d | PASS: %d\n",
                distance, irVal, isDark, busIncoming, passengerWaiting);

  // ThingSpeak every 15 sec (FIXED)
  if (millis() - lastTS > 15000) {
    sendToThingSpeak(distance, passengerWaiting, lightIntensity, busIncoming);
    lastTS = millis();   // FIXED
  }

  delay(50);
}
