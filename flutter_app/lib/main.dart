import 'package:flutter/material.dart';
import 'package:firebase_core/firebase_core.dart';
import 'package:firebase_database/firebase_database.dart';

void main() async {
  WidgetsFlutterBinding.ensureInitialized();
  await Firebase.initializeApp();
  runApp(const AeroPulseApp());
}

class AeroPulseApp extends StatelessWidget {
  const AeroPulseApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'AeroPulse Industrial Monitoring',
      debugShowCheckedModeBanner: false,
      theme: ThemeData(
        brightness: Brightness.dark,
        scaffoldBackgroundColor: const Color(0xFF1E1E1E),
        primarySwatch: Colors.blue,
      ),
      home: const DashboardScreen(),
    );
  }
}

class DashboardScreen extends StatefulWidget {
  const DashboardScreen({super.key});

  @override
  State<DashboardScreen> createState() => _DashboardScreenState();
}

class _DashboardScreenState extends State<DashboardScreen> {
  final DatabaseReference dbRef = FirebaseDatabase.instance.ref();

  String temperature = "-- °C";
  String humidity = "-- %";
  String gasPpm = "-- PPM";
  String voltage = "-- V";
  String current = "-- mA";
  String fan1Status = "MATI";
  String fan2Status = "MATI";

  @override
  void initState() {
    super.initState();
    _fetchRealtimeData();
  }

  void _fetchRealtimeData() {
    dbRef.child('sensors').onValue.listen((event) {
      final data = event.snapshot.value as Map<dynamic, dynamic>?;
      if (data != null && mounted) {
        setState(() {
          temperature = "${data['temperature']} °C";
          humidity = "${data['humidity']} %";
          gasPpm = "${data['gas_ppm']} PPM";
          voltage = "${data['voltage']} V";
          current = "${data['current']} mA";
        });
      }
    });

    dbRef.child('actuators').onValue.listen((event) {
      final data = event.snapshot.value as Map<dynamic, dynamic>?;
      if (data != null && mounted) {
        setState(() {
          fan1Status = data['fan1_status'].toString() == "1" ? "AKTIF" : "MATI";
          fan2Status = data['fan2_status'].toString() == "1" ? "AKTIF" : "MATI";
        });
      }
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text(
          'AeroPulse Industrial Monitoring',
          style: TextStyle(fontSize: 18, fontWeight: FontWeight.bold),
        ),
        backgroundColor: const Color(0xFF2C3E50),
        elevation: 0,
      ),
      body: Padding(
        padding: const EdgeInsets.all(16.0),
        child: ListView(
          children: [
            const Text(
              'Parameter Lingkungan & Daya',
              style: TextStyle(fontSize: 16, fontWeight: FontWeight.bold, color: Colors.white),
            ),
            const SizedBox(height: 12),
            _buildDataCard("Suhu Ruangan (DHT22)", temperature, Colors.white),
            _buildDataCard("Kelembaban (DHT22)", humidity, Colors.white),
            _buildDataCard("Kadar Gas / Amonia (MQ-135)", gasPpm, Colors.white),
            _buildDataCard("Tegangan Sistem (INA219)", voltage, Colors.white),
            _buildDataCard("Arus Beban (INA219)", current, Colors.white),
            const SizedBox(height: 24),
            const Text(
              'Status Aktuator (Kipas DC)',
              style: TextStyle(fontSize: 16, fontWeight: FontWeight.bold, color: Colors.white),
            ),
            const SizedBox(height: 12),
            _buildDataCard(
              "Kipas 1 (Exhaust Gas)",
              fan1Status,
              fan1Status == "AKTIF" ? Colors.red : Colors.grey,
            ),
            _buildDataCard(
              "Kipas 2 (Pendingin Suhu)",
              fan2Status,
              fan2Status == "AKTIF" ? Colors.red : Colors.grey,
            ),
          ],
        ),
      ),
    );
  }

  Widget _buildDataCard(String label, String value, Color valueColor) {
    return Container(
      margin: const EdgeInsets.only(bottom: 10.0),
      padding: const EdgeInsets.symmetric(horizontal: 16.0, vertical: 14.0),
      decoration: BoxDecoration(
        color: const Color(0xFFE0E0E0),
        borderRadius: BorderRadius.circular(8.0),
      ),
      child: Row(
        mainAxisAlignment: MainAxisAlignment.spaceBetween,
        children: [
          Text(
            label,
            style: const TextStyle(fontSize: 15, color: Colors.black87, fontWeight: FontWeight.w500),
          ),
          Text(
            value,
            style: TextStyle(fontSize: 15, fontWeight: FontWeight.bold, color: valueColor),
          ),
        ],
      ),
    );
  }
}