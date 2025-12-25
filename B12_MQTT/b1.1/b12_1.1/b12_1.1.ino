#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* device_name = "ESP8266";
const char* ssid = "REDMI Turbo 4 Pro";
const char* password = "quandznhattg";

// IP Home Assistant
const char* mqtt_server = "10.153.38.110";
const int mqtt_port = 1883;

const char* mqtt_user = "quan";
const char* mqtt_pass = "1234";

const char* student_id = "10123272";
const char* mqtt_topic = "iot/lab1/10123272/sensor";
/* ==================== */

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void reconnect() {
  while (!client.connected()) {
    String clientId = device_name;
    clientId += "_";
    clientId += student_id;

    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println("MQTT connected");
    } else {
      Serial.print("MQTT failed, rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}


void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  StaticJsonDocument<200> doc;
  doc["temp"] = random(250, 350) / 10.0;
  doc["hum"]  = random(400, 800) / 10.0;

  char payload[128];
  serializeJson(doc, payload);

  client.publish(mqtt_topic, payload);

  delay(5000);
}

