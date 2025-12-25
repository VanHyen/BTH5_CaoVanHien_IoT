String text;
void setup() {
  pinMode(13, OUTPUT);
  Serial.begin(9600);
}
void loop() {
  while (Serial.available())
  {
    delay(10);
    char c = Serial.read();
    text += c;
  }
  if (text.length() > 0)
  {
    Serial.println(text);
    if (text == "on")
    {
      digitalWrite(13, HIGH);
    }
  
    if (text == "off")
    {
      digitalWrite(13, LOW);
    }
    text = "";
  }

}
