#include "thingProperties.h"
#include <DHTesp.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Thông tin Wi-Fi
const char* ssid = "";
const char* password = "";

// URL webhook Zapier
const char* webhook_url = "";

// Khai báo DHT
#define DHTPIN 18
#define DHTTYPE DHT11
DHTesp dht;

const int btn[4] = {13, 12, 14};
const int out[4] = {26, 25, 32};

#define BUZZER_PIN 4      
#define SWITCH_PIN 27     

unsigned long timeDelay = millis();
unsigned long timeRead = millis();
bool update = 0;
bool buzzerState = false; 
bool switchPressed = false;
bool emailSent = false; // Biến để theo dõi trạng thái gửi email

void setup() {
  // Khởi tạo serial
  Serial.begin(9600);
  delay(1500); 

  // Kết nối Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Đang kết nối Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nĐã kết nối Wi-Fi");

  initProperties();

  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  dht.setup(DHTPIN, DHTesp::DHTTYPE);

  for (int i = 0; i < 4; i++) {
    pinMode(btn[i], INPUT_PULLUP);
    attachInterrupt(btn[i], handleBtn, FALLING);
    pinMode(out[i], OUTPUT);
  }

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);  
  pinMode(SWITCH_PIN, INPUT_PULLUP);  
}

void loop() {
  ArduinoCloud.update();

  // Đọc nhiệt độ mỗi giây
  if (millis() - timeRead > 1000) {
    float currentTemperature = dht.getTemperature();
    temperature = currentTemperature; // Cập nhật biến từ thingProperties.h
    timeRead = millis();

    Serial.println("Nhiệt độ hiện tại: " + String(temperature));

    // Nếu nhiệt độ vượt ngưỡng và chưa gửi email
    if (temperature > 30.0 && !switchPressed) {
      digitalWrite(BUZZER_PIN, HIGH);  // Bật còi
      buzzerState = true;

      // Tắt tất cả các đèn
      for (int i = 0; i < 4; i++) {
        digitalWrite(out[i], LOW);
      }
      led1 = LOW;
      led2 = LOW;
      led3 = LOW;

      if (!emailSent) {
        sendNotification(temperature); // Gửi thông báo qua webhook
        emailSent = true; // Đánh dấu đã gửi email
      }
    } 

    // Reset trạng thái khi nhiệt độ <= 30°C
    if (temperature <= 30.0) {
      emailSent = false;
      digitalWrite(BUZZER_PIN, LOW); // Tắt còi nếu nhiệt độ trở lại bình thường
      buzzerState = false;
    }
  }

  // Kiểm tra nút nhấn để tắt còi
  if (digitalRead(SWITCH_PIN) == LOW) {
    delay(50); // Chống nhiễu (debounce)
    if (digitalRead(SWITCH_PIN) == LOW) {  // Xác nhận nhấn
      switchPressed = !switchPressed;     // Đổi trạng thái switchPressed
      if (switchPressed) {
        digitalWrite(BUZZER_PIN, LOW);    // Tắt còi
        buzzerState = false;
      }
    }
    while (digitalRead(SWITCH_PIN) == LOW) {
      // Đợi cho đến khi thả tay khỏi nút
    }
  }

  // Cập nhật trạng thái LED nếu cần
  if (update == 1) {
    led1 = digitalRead(out[0]);
    led2 = digitalRead(out[1]);
    led3 = digitalRead(out[2]);
    update = 0;
  }
}

ICACHE_RAM_ATTR void handleBtn() {
  if (millis() - timeDelay > 500) {
    for (int i = 0; i < 4; i++) {
      bool btnState = digitalRead(btn[i]);
      if (btnState == 0) {
        digitalWrite(out[i], !digitalRead(out[i]));
      }
    }
    update = 1;
    timeDelay = millis();
  }
}

// Hàm gửi thông báo qua webhook Zapier
void sendNotification(float temp) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Tạo dữ liệu JSON cần gửi
    String postData = "{\"message\": \"Nhiệt độ cao!\", \"temperature\": " + String(temp) + "}";

    http.begin(webhook_url);
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(postData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Phản hồi từ Zapier: " + response);
    } else {
      Serial.println("Lỗi khi gửi dữ liệu: " + String(httpResponseCode));
    }

    http.end();
  } else {
    Serial.println("Không thể kết nối Wi-Fi để gửi thông báo.");
  }
}

void onLed1Change() {
  digitalWrite(out[0], led1);
}

void onLed2Change() {
  digitalWrite(out[1], led2);
}

void onLed3Change() {
  digitalWrite(out[2], led3);
}

void onTemperatureChange() {
}
