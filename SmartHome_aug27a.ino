#include "thingProperties.h"
#include <DHTesp.h>
#define DHTPIN 18
#define DHTTYPE DHT11
DHTesp dht;
const int btn[4] = {13, 12, 14};
const int out[4] = {26, 25, 33};
#define BUZZER_PIN 4      
#define SWITCH_PIN 27     
unsigned long timeDelay = millis();
unsigned long timeRead = millis();
bool update = 0;
bool buzzerState = false; 

void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  delay(1500); 

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

bool switchPressed = false;

void loop() {
  ArduinoCloud.update();

  if (millis() - timeRead > 1000) {
    float t = dht.getTemperature();
    temperature = t;
    timeRead = millis();

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
    }
  }

  if (digitalRead(SWITCH_PIN) == LOW) {  // Nếu nhấn nút
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