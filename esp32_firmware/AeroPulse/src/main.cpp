#include <WiFi.h>
#include <FirebaseESP32.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_INA219.h>
#include <LiquidCrystal_I2C.h>

// --- KONFIGURASI KREDENSIAL ---
#define WIFI_SSID "Test"
#define WIFI_PASSWORD "Test12345"

#define FIREBASE_HOST "aeropulse-industrial-fb1f8-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "5fib2zd7D4ct187bjLQWBWBejNnkFQtfMeuiHjpN"

// --- DEFINISI PIN HARDWARE ---
#define DHTPIN 4          
#define DHTTYPE DHT22     
#define MQ135_PIN 34      
#define RELAY_FAN1 26     
#define RELAY_FAN2 27     

// Inisialisasi Objek Library
DHT dht(DHTPIN, DHTTYPE);
Adafruit_INA219 ina219;
LiquidCrystal_I2C lcd(0x27, 16, 2); // Alamat I2C umum 0x27 atau 0x3F

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// --- AMBANG BATAS AUTOMATION (EDGE LOGIC) ---
const float TEMP_THRESHOLD = 30.0; // °C
const int GAS_THRESHOLD = 400;     // Nilai ADC mentah MQ-135

void setup() {
  Serial.begin(115200);
  
  // Konfigurasi Pin Aktuator
  pinMode(RELAY_FAN1, OUTPUT);
  pinMode(RELAY_FAN2, OUTPUT);
  digitalWrite(RELAY_FAN1, LOW);
  digitalWrite(RELAY_FAN2, LOW);

  // Inisialisasi Sensor & LCD
  dht.begin();
  Wire.begin(21, 22); // SDA, SCL
  lcd.init();
  lcd.backlight();
  
  lcd.setCursor(0, 0);
  lcd.print("AeroPulse Init...");

  if (!ina219.begin()) {
    Serial.println("Gagal mendeteksi modul INA219!");
  }

  // Koneksi WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Terhubung!");

  // Konfigurasi Firebase
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&config, &auth);
  Firebase.setWiFiReconnect(true);
}

void loop() {
  // 1. Membaca Sensor
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  int gasRaw = analogRead(MQ135_PIN);
  float loadvoltage = ina219.getBusVoltage_V();
  float current_mA = ina219.getCurrent_mA();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Gagal membaca sensor DHT!");
    return;
  }

  // 2. Logika Edge Automation (Otonom Kipas)
  int fan1State = 0;
  int fan2State = 0;

  // Kontrol Kipas 1 (Gas / MQ-135)
  if (gasRaw > GAS_THRESHOLD) {
    digitalWrite(RELAY_FAN1, HIGH);
    fan1State = 1;
  } else {
    digitalWrite(RELAY_FAN1, LOW);
    fan1State = 0;
  }

  // Kontrol Kipas 2 (Suhu / DHT22)
  if (temperature > TEMP_THRESHOLD) {
    digitalWrite(RELAY_FAN2, HIGH);
    fan2State = 1;
  } else {
    digitalWrite(RELAY_FAN2, LOW);
    fan2State = 0;
  }

  // 3. Tampilkan ke LCD Lokal 16x2
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature, 1);
  lcd.print("C H:");
  lcd.print(humidity, 0);
  lcd.print("%");
  
  lcd.setCursor(0, 1);
  lcd.print("G:");
  lcd.print(gasRaw);
  lcd.print(" F1:");
  lcd.print(fan1State);
  lcd.print(" F2:");
  lcd.print(fan2State);

  // 4. Kirim Data ke Firebase Realtime Database
  Firebase.setFloat(fbdo, "/sensors/temperature", temperature);
  Firebase.setFloat(fbdo, "/sensors/humidity", humidity);
  Firebase.setInt(fbdo, "/sensors/gas_ppm", gasRaw);
  Firebase.setFloat(fbdo, "/sensors/voltage", loadvoltage);
  Firebase.setFloat(fbdo, "/sensors/current", current_mA);
  
  Firebase.setInt(fbdo, "/actuators/fan1_status", fan1State);
  Firebase.setInt(fbdo, "/actuators/fan2_status", fan2State);

  delay(2000); // Kirim data setiap 2 detik
}