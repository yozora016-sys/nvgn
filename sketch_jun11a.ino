#include <Arduino.h>
#include <DHT.h>

#define DHTPIN 4      
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define RXp2 16
#define TXp2 17

void taskDocCamBien(void *pvParameters);

void setup() {
  Serial.begin(115200);   
  Serial2.begin(9600, SERIAL_8N1, RXp2, TXp2); 
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  dht.begin();
  Serial.println("Hệ thống ESP32 đang khởi động...");

  xTaskCreatePinnedToCore(
    taskDocCamBien,
    "Task_DHT11",
    2048,
    NULL,
    1,
    NULL,
    1
  );
}

void loop() {
  while (Serial2.available() > 0) {
    char c = Serial2.read(); 
    Serial.println(c);
    if (c == '1') {
      digitalWrite(2, HIGH);
    } 
    else if (c == '0') {
      digitalWrite(2, LOW); 
    }
  } 
  vTaskDelay(50 / portTICK_PERIOD_MS);
}

void taskDocCamBien(void *pvParameters) {
  for (;;) {
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (isnan(h) || isnan(t)) {
      Serial.println("Lỗi: Không thể đọc dữ liệu từ DHT11!");
    } else {
      String packet = "$DATA,TEMP:" + String(t, 1) + ",HUM:" + String((int)h) + ",LIGHT:120#";
      Serial.println("Đang truyền UART: " + packet);
      Serial2.println(packet); 
    }
    
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}
