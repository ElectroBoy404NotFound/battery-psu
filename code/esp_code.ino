#define DEBUG false
#define USE_WIFI false

#if USE_WIFI
  #include <ESP8266WiFi.h>
  #include <ESP8266WebServer.h>
#endif

#define PS_ON D1
#define PS_OK D5
#define BATTERY_ENABLE D7

#define PS_OK_FALSE LOW
#define BATTERY_ENABLE_FALSE HIGH

#define PS_OK_TRUE HIGH
#define BATTERY_ENABLE_TRUE LOW

bool startup = false;
bool powerdown = false;
bool running = false;

#if USE_WIFI
  const char* ssid = "YOUR_WIFI_SSID";
  const char* password = "YOUR_WIFI_PASSWORD";

  ESP8266WebServer server(80);

  void handleWebRoot() {
    String html = "<html><body style='font-family:sans-serif;'>";
    html += "<h1>Smart PSU Controller</h1>";
    html += "<p>Status: " + String(running ? "Running" : "Stopped") + "</p>";
    html += "</body></html>";
    server.send(200, "text/html", html);
  }
#endif


void IRAM_ATTR ps_on_change() {
  if (digitalRead(PS_ON) == LOW) {
    startup = true;
  } else {
    powerdown = true;
  }
}

void setup() {
  #if DEBUG
    Serial.begin(115200);
  #endif

  pinMode(PS_ON, INPUT_PULLUP);
  pinMode(PS_OK, OUTPUT);
  pinMode(BATTERY_ENABLE, OUTPUT);

  digitalWrite(PS_OK, PS_OK_FALSE);
  digitalWrite(BATTERY_ENABLE, BATTERY_ENABLE_FALSE);

  attachInterrupt(digitalPinToInterrupt(PS_ON), ps_on_change, CHANGE);

  #if USE_WIFI
    WiFi.begin(ssid, password);
    #if DEBUG
      Serial.print("Connecting to Wi-Fi");
    #endif
    unsigned long startAttempt = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 10000) {
      delay(50);
      #if DEBUG
        Serial.print(".");
      #endif
    }
    if (WiFi.status() != WL_CONNECTED) {
      #if DEBUG
        Serial.println("\nWi-Fi failed, running in offline mode.");
      #endif
    } else {
      #if DEBUG
        Serial.println("\nConnected!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
      #endif

      // Web server routes
      server.on("/", handleWebRoot);

      server.begin();
      #if DEBUG
        Serial.println("HTTP server started");
      #endif
    }
  #endif

  #if DEBUG
    Serial.println("READY");
  #endif
}

void loop() {
  if (startup && !running) {
    #if DEBUG
      Serial.println("Start up triggered");
    #endif

    digitalWrite(PS_OK, PS_OK_FALSE);
    digitalWrite(BATTERY_ENABLE, BATTERY_ENABLE_TRUE);
    delay(300);
    digitalWrite(PS_OK, PS_OK_TRUE);

    startup = false;
    running = true;
  }

  if (powerdown && running) {
    #if DEBUG
      Serial.println("Power down triggered");
    #endif

    digitalWrite(BATTERY_ENABLE, BATTERY_ENABLE_FALSE);
    digitalWrite(PS_OK, PS_OK_FALSE);

    powerdown = false;
    running = false;
  }

  #if USE_WIFI
    if (WiFi.status() == WL_CONNECTED) {
      server.handleClient();
    }
  #endif
}
