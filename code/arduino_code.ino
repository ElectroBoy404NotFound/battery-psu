#define DEBUG false

#define PS_ON 2
#define PS_OK 4
#define BATTERY_ENABLE 5

#define PS_OK_FALSE LOW
#define BATTERY_ENABLE_FALSE HIGH

#define PS_OK_TRUE HIGH
#define BATTERY_ENABLE_TRUE LOW

bool startup = false;
bool powerdown = false;
bool running = false;

void ps_on_change() {
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
}
