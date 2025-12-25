#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "REDMI Turbo 4 Pro";
const char* password = "quandznhattg";

// IP Home Assistant
const char* mqtt_server = "10.153.38.110";
const int mqtt_port = 1883;

const char* mqtt_user = "quan";
const char* mqtt_pass = "1234";
const char* student_id = "10123272";

/* ===== MQTT CLIENT ===== */
WiFiClient espClient;
PubSubClient client(espClient);

/* ===== MQTT TOPICS ===== */
String topic_cmd;
String topic_state;

/* ===== LED ===== */
const int LED_PIN = LED_BUILTIN;   // ESP8266 LED onboard
bool ledState = false;

/* ===== LED CONTROL ===== */
void applyLed(bool on) {
  ledState = on;
  digitalWrite(LED_PIN, on ? LOW : HIGH); // ESP8266 LED active LOW
}

/* ===== PUBLISH STATE ===== */
void publishState() {
  client.publish(topic_state.c_str(), ledState ? "ON" : "OFF", true);
}

/* ===== WIFI ===== */
void setup_wifi() {
  Serial.print("Connecting WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.println(WiFi.localIP());
}

/* ===== MQTT CALLBACK ===== */
void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  msg.trim();

  Serial.print("MQTT [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println(msg);

  if (String(topic) == topic_cmd) {
    if (msg == "ON") applyLed(true);
    else if (msg == "OFF") applyLed(false);

    publishState();   // gửi trạng thái về HA
  }
}

/* ===== MQTT RECONNECT ===== */
void reconnect() {
  while (!client.connected()) {
    String clientId = "ESP8266-Lab2-";
    clientId += student_id;

    Serial.print("Connecting MQTT... ");
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      client.subscribe(topic_cmd.c_str());
      Serial.print("Subscribed ");
      Serial.println(topic_cmd);

      publishState();
    } else {
      Serial.print("failed, rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

/* ===== SETUP ===== */
void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  applyLed(false);

  /* Build topics */
  topic_cmd   = "iot/lab3/" + String(student_id) + "/cmd";
  topic_state = "iot/lab3/" + String(student_id) + "/state";

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

/* ===== LOOP ===== */
void loop() {
  if (!client.connected()) reconnect();
  client.loop();
}
