#include "DHT.h"

#define DHTPIN 2     // Chân DATA nối vào chân số 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  delay(2000); // DHT11 cần 2 giây để ổn định dữ liệu
  
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Kiểm tra nếu cảm biến lỗi (tránh gửi nan làm sập server)
  if (isnan(h) || isnan(t)) return;

  // Gửi định dạng JSON chuẩn qua Serial
  Serial.print("{\"temp\":");
  Serial.print(t);
  Serial.print(",\"hum\":");
  Serial.print(h);
  Serial.println("}");
}