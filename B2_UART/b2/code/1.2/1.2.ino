const int PIN_BUTTON = 2;
const int PIN_LED = 3;

bool configActive = true;
const long RATES[] = {9600, 115200};
int rateIndex = 0;

unsigned long startTimestamp = 0;
bool activePress = false;
int clickCounter = 0;

unsigned long timerLED = 0;
bool stateLED = false;
int behaviorLED = 0; 

void setup() {
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);
  
  Serial.begin(9600);
  while (!Serial);
  
  Serial.println("=== UART CONFIGURATION ===");
  Serial.println("Button actions:");
  Serial.println("  1 click  -> 9600 bps");
  Serial.println("  2 clicks -> 115200 bps");
  Serial.println("  Hold >3s -> Confirm/Switch");
  Serial.println();
  
  digitalWrite(PIN_LED, LOW);
  behaviorLED = 0;
}

void loop() {
  processInput();
  manageVisuals();
  
  if (!configActive && Serial.available()) {
    String incoming = Serial.readStringUntil('\n');
    Serial.print("Received: ");
    Serial.println(incoming);
  }
}

void processInput() {
  bool currentReading = (digitalRead(PIN_BUTTON) == LOW);
  
  if (currentReading && !activePress) {
    activePress = true;
    startTimestamp = millis();
  }
  
  if (!currentReading && activePress) {
    unsigned long heldDuration = millis() - startTimestamp;
    
    if (heldDuration < 3000 && configActive) {
      clickCounter++;
      behaviorLED = 2;
      timerLED = millis();

      if (clickCounter == 1) {
        rateIndex = 0;
        Serial.println(">> Selected: 9600 bps");
      } else if (clickCounter >= 2) {
        rateIndex = 1;
        Serial.println(">> Selected: 115200 bps");
        clickCounter = 0;
      }
    }
    activePress = false;
  }
  
  if (activePress && (millis() - startTimestamp >= 3000)) {
    activePress = false;
    if (configActive) {
      switchToRun();
    } else {
      switchToConfig();
    }
  }
}

void switchToRun() {
  configActive = false;
  
  Serial.end();
  delay(100);
  Serial.begin(RATES[rateIndex]);
  delay(100);
  
  Serial.println("\n=== OPERATION MODE ===");
  Serial.print("Active Baudrate: ");
  Serial.println(RATES[rateIndex]);
  Serial.println("Hold >3s to reconfigure");
  Serial.println();
  
  digitalWrite(PIN_LED, HIGH);
  behaviorLED = 1;
  clickCounter = 0;
}

void switchToConfig() {
  configActive = true;
  
  Serial.end();
  delay(100);
  Serial.begin(9600);
  delay(100);
  
  Serial.println("\n=== RETURN TO CONFIG ===");
  Serial.println("1 click: 9600 | 2 clicks: 115200 | Hold: Confirm");
  Serial.println();
  
  digitalWrite(PIN_LED, LOW);
  behaviorLED = 0;
  clickCounter = 0;
}

void manageVisuals() {
  if (!configActive) return;
  
  if (behaviorLED == 0) {
    digitalWrite(PIN_LED, LOW);
  } 
  else if (behaviorLED == 1) {
    digitalWrite(PIN_LED, HIGH);
  } 
  else if (behaviorLED == 2) {
    if (millis() - timerLED >= 200) {
      timerLED = millis();
      stateLED = !stateLED;
      digitalWrite(PIN_LED, stateLED);
    }
  }
}