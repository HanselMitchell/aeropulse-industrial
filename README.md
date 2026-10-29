# AeroPulse Industrial 🚀
> **An Industrial Indoor Environmental & Energy Monitoring Node with Dual-Fan Automated Mitigation**

## 📌 Tentang Proyek
**AeroPulse Industrial** adalah perangkat *smart monitoring and mitigation node* berbasis IoT (*Internet of Things*) yang dirancang khusus untuk fasilitas industri *indoor* (seperti area produksi dan gudang farmasi). Sistem ini mengintegrasikan sensor lingkungan presisi, diagnostik daya listrik, serta sistem mitigasi otonom (*Edge Automation*) dengan dua kipas DC, yang dihubungkan secara *real-time* ke *Cloud Dashboard* berbasis Flutter menggunakan Firebase Realtime Database.

## 🛠️ Fitur Utama
* **Edge Computing Automation:** Logika otonom pada ESP32 untuk mengaktifkan kipas *exhaust* (berdasarkan sensor gas/amonia MQ-135) dan kipas pendingin (berdasarkan sensor suhu/kelembaban DHT22) secara mandiri.
* **Power Diagnostics:** Pemantauan tegangan, arus, dan konsumsi daya secara *real-time* menggunakan modul INA219.
* **Local Monitoring:** Dilengkapi layar LCD I2C 16x2 untuk pemantauan langsung di lokasi.
* **Cloud Telemetry & Flutter Dashboard:** Sinkronisasi data instan ke *Firebase Realtime Database* yang divisualisasikan melalui aplikasi seluler/web *clean-UI* berbasis Flutter.

## 📂 Struktur Repositori
* `esp32_firmware/` : Berisi kode sumber C++ (PlatformIO / Arduino IDE) untuk mikrokontroler ESP32.
* `flutter_app/`     : Berisi kode sumber aplikasi *dashboard* Flutter.
