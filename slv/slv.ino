#include <WiFi.h>
#include <DHT.h>

// --- 1. KONFIGURASI WIFI (SUDAH DIUPDATE) ---
const char* ssid = "MR-FK-01";
const char* password = "adzanmasjid";

// --- 2. KONFIGURASI PIN SENSOR (Sesuai Rangkaian Responsi) ---
#define DHTPIN 4          // Pin DATA DHT22 terhubung ke D4 (GPIO 4)
#define DHTTYPE DHT22     // Menggunakan tipe sensor DHT22
#define TRIG_PIN 5        // Pin TRIG HC-SR04 terhubung ke D5 (GPIO 5)
#define ECHO_PIN 18       // Pin ECHO HC-SR04 terhubung ke D18 (GPIO 18)

// Inisialisasi objek sensor DHT22
DHT dht(DHTPIN, DHTTYPE);

// Fungsi untuk menghubungkan ESP32 ke jaringan WiFi
void connectToWiFi() {
  Serial.print("Menghubungkan ke WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nBerhasil terhubung ke WiFi!");
  Serial.print("Alamat IP ESP32: ");
  Serial.println(WiFi.localIP());
}

// Fungsi untuk membaca jarak dari sensor Ultrasonik HC-SR04
float readUltrasonicDistance() {
  // Memicu sinyal ultrasonik dari pin TRIG
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Membaca durasi pantulan gelombang pada pin ECHO (dalam mikrodetik)
  long duration = pulseIn(ECHO_PIN, HIGH);
  
  // Menghitung jarak dalam satuan cm (kecepatan suara = 0.034 cm/mikrodetik)
  float distance = duration * 0.034 / 2;
  return distance;
}

void setup() {
  // Memulai komunikasi Serial Monitor pada baud rate 115200
  Serial.begin(115200);
  
  // Mengatur mode pin untuk sensor HC-SR04
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Memulai sensor DHT22
  dht.begin();

  // Memanggil fungsi koneksi WiFi
  connectToWiFi();
  Serial.println("Sistem IoT Siap Bekerja!\n");
}

void loop() {
  // Mengecek apakah koneksi WiFi masih terhubung
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Koneksi WiFi terputus! Menghubungkan ulang...");
    connectToWiFi();
  }

  // --- Membaca Data dari Kedua Sensor ---
  float humidity = dht.readHumidity();        // Membaca kelembapan (%)
  float temperature = dht.readTemperature();  // Membaca suhu (°C)
  float distance = readUltrasonicDistance();  // Membaca jarak objek (cm)

  // Validasi pembacaan sensor DHT22 agar tidak error runtime
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Error: Gagal membaca data dari sensor DHT22! Periksa kabel.");
    delay(2000);
    return;
  }

  // --- Menampilkan Data ke Serial Monitor ---
  Serial.println("======= DATA SENSOR IOT =======");
  Serial.print("Suhu Udara       : ");
  Serial.print(temperature);
  Serial.println(" °C");
  
  Serial.print("Kelembapan Udara : ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Jarak Objek      : ");
  Serial.print(distance);
  Serial.println(" cm");

  // --- Pemrosesan Logika Sistem ---
  Serial.print("Status Sistem    : ");
  if (temperature > 32.0 && distance < 20.0) {
    Serial.println("BAHAYA! Suhu panas dan ada objek terlalu dekat.");
  } else if (temperature > 32.0) {
    Serial.println("PERINGATAN! Suhu ruangan terlalu panas.");
  } else if (distance < 20.0) {
    Serial.println("PERINGATAN! Objek terdeteksi dalam jarak dekat.");
  } else {
    Serial.println("NORMAL (Kondisi lingkungan aman).");
  }
  Serial.println("===============================\n");

  // Jeda 2 detik sebelum pembacaan berikutnya agar sistem berjalan stabil
  delay(2000);
}