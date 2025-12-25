#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>

// Cấu hình LCD và RTC
LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS1307 rtc;

// Định nghĩa chân cắm (Pins)
#define PIN_MODE   3
#define PIN_UP     4
#define PIN_DOWN   5
#define PIN_SET    6
#define PIN_BUZZER 2
#define PIN_LED    7

// Các trạng thái của hệ thống
enum SystemState {
  SHOW_TIME,
  EDIT_HOUR,
  EDIT_MINUTE
};

SystemState currentState = SHOW_TIME;

// Biến lưu trữ báo thức
int alarmH = 20;
int alarmM = 30;
bool isAlarmEnabled = true;
bool isRinging = false;

unsigned long lastDebounce = 0;
const int debounceTime = 250;

void setup() {
  lcd.init();
  lcd.backlight();

  if (!rtc.begin()) {
    lcd.print("RTC Error!");
    while (1);
  }

  if (!rtc.isrunning()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  pinMode(PIN_MODE, INPUT_PULLUP);
  pinMode(PIN_UP, INPUT_PULLUP);
  pinMode(PIN_DOWN, INPUT_PULLUP);
  pinMode(PIN_SET, INPUT_PULLUP);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_LED, OUTPUT);
  
  // Trạng thái ban đầu của còi (tắt)
  digitalWrite(PIN_BUZZER, HIGH); 
}

void loop() {
  DateTime now = rtc.now();
  readButtons();

  switch (currentState) {
    case SHOW_TIME:
      displayMainScreen(now);
      checkAlarmLogic(now);
      break;
    case EDIT_HOUR:
      displaySetupScreen(true);
      break;
    case EDIT_MINUTE:
      displaySetupScreen(false);
      break;
  }
  delay(150);
}

// Hàm bổ trợ hiển thị số có 2 chữ số (VD: 09, 10)
void printDigits(int num) {
  if (num < 10) lcd.print('0');
  lcd.print(num);
}

void readButtons() {
  if (millis() - lastDebounce < debounceTime) return;

  // Nút MODE: Chuyển đổi trạng thái hoặc tắt báo thức
  if (digitalRead(PIN_MODE) == LOW) {
    lastDebounce = millis();
    if (isRinging) {
      isRinging = false;
      lcd.clear();
    } else {
      if (currentState == SHOW_TIME) currentState = EDIT_HOUR;
      lcd.clear();
    }
  }

  // Nút SET: Chuyển đổi giữa chỉnh Giờ -> Phút -> Thoát
  if (digitalRead(PIN_SET) == LOW) {
    lastDebounce = millis();
    if (currentState == SHOW_TIME) {
      isAlarmEnabled = !isAlarmEnabled;
    } else if (currentState == EDIT_HOUR) {
      currentState = EDIT_MINUTE;
    } else {
      currentState = SHOW_TIME;
      lcd.clear();
    }
  }

  // Nút UP/DOWN: Tăng giảm giá trị
  if (digitalRead(PIN_UP) == LOW) {
    lastDebounce = millis();
    if (currentState == EDIT_HOUR) alarmH = (alarmH + 1) % 24;
    if (currentState == EDIT_MINUTE) alarmM = (alarmM + 1) % 60;
  }

  if (digitalRead(PIN_DOWN) == LOW) {
    lastDebounce = millis();
    if (currentState == EDIT_HOUR) alarmH = (alarmH <= 0) ? 23 : alarmH - 1;
    if (currentState == EDIT_MINUTE) alarmM = (alarmM <= 0) ? 59 : alarmM - 1;
  }
}

void displayMainScreen(DateTime now) {
  if (isRinging) {
    lcd.setCursor(0, 0);
    lcd.print(" >> WAKE UP! << ");
    lcd.setCursor(0, 1);
    lcd.print("  Bam MODE tat  ");
    // Hiệu ứng còi báo
    digitalWrite(PIN_BUZZER, (millis() % 500 < 250) ? LOW : HIGH);
    return;
  }

  // Dòng 1: Giờ hiện tại
  lcd.setCursor(0, 0);
  printDigits(now.hour());
  lcd.print(':');
  printDigits(now.minute());
  lcd.print(':');
  printDigits(now.second());
  
  lcd.setCursor(12, 0);
  lcd.print(isAlarmEnabled ? "[ON]" : "[-]");

  // Dòng 2: Ngày tháng và giờ báo thức
  lcd.setCursor(0, 1);
  printDigits(now.day());
  lcd.print('/');
  printDigits(now.month());
  
  lcd.setCursor(11, 1);
  if (isAlarmEnabled) {
    printDigits(alarmH);
    lcd.print(':');
    printDigits(alarmM);
  } else {
    lcd.print(" --:-- ");
  }
  
  digitalWrite(PIN_BUZZER, HIGH); // Tắt còi khi không trong trạng thái báo thức
}

void displaySetupScreen(bool isHourActive) {
  lcd.setCursor(0, 0);
  lcd.print(" CAI DAT BAO ");
  lcd.setCursor(0, 1);
  lcd.print("Time: ");
  
  if (isHourActive) lcd.print('>'); else lcd.print(' ');
  printDigits(alarmH);
  lcd.print(':');
  if (!isHourActive) lcd.print('>'); else lcd.print(' ');
  printDigits(alarmM);
}

void checkAlarmLogic(DateTime now) {
  if (!isAlarmEnabled) return;

  // Kích hoạt báo thức khi khớp giờ, phút, giây
  if (now.hour() == alarmH && now.minute() == alarmM && now.second() == 0) {
    if (!isRinging) {
      isRinging = true;
      lcd.clear();
    }
  }
}