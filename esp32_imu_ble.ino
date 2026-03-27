#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// ============================================================================
// BNO055 IMU SENSOR
// ============================================================================
// 9-axis sensor with on-chip sensor fusion
// I2C Address: 0x29 (default) or 0x28 (if ALT jumper set)
// Wire connections: SDA=GPIO4, SCL=GPIO6
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x29);

// ============================================================================
// BLE CONFIGURATION
// ============================================================================
// Standard Nordic UART Service UUIDs
#define SERVICE_UUID        "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;

// ============================================================================
// TIMING CONSTANTS
// ============================================================================
// Send data at 20 Hz (1000 ms / 50 ms = 20 packets per second)
#define SEND_INTERVAL_MS 50
unsigned long lastSendTime = 0;

// ============================================================================
// BNO055 VECTOR TYPE SHORTCUTS
// ============================================================================
// Each vector represents a different processed output from the BNO055
#define VEC(x) ((Adafruit_BNO055::adafruit_vector_type_t)(x))
#define VEC_LINEAR_ACCEL  VEC(0x28)      // Acceleration without gravity
#define VEC_GRAVITY       VEC(0x2E)      // Gravity component only
#define VEC_GYROSCOPE     VEC(0x14)      // Angular velocity (rad/s)
#define VEC_MAGNETOMETER  VEC(0x0E)      // Magnetic field (µT)
#define VEC_EULER         VEC(0x1A)      // Euler angles (degrees)
#define VEC_ACCELEROMETER VEC(0x08)      // Raw acceleration (includes gravity)

// ============================================================================
// BLE SERVER CALLBACKS
// ============================================================================
class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) override {
    deviceConnected = true;
    Serial.println("BLE Connected");
  }
  
  void onDisconnect(BLEServer* pServer) override {
    deviceConnected = false;
    Serial.println("BLE Disconnected - restarting advertising...");
    BLEDevice::startAdvertising();
  }
};

// ============================================================================
// BINARY PACKING FUNCTION
// ============================================================================
// Converts float array to byte buffer for efficient BLE transmission
// Input: 19 floats (76 bytes)
// Output: 76-byte binary buffer suitable for BLE characteristic
void packBinary(float* vals, int count, uint8_t* buf) {
  for (int i = 0; i < count; i++) {
    memcpy(buf + i * 4, &vals[i], 4);  // Copy each float (4 bytes) to buffer
  }
}

// ============================================================================
// SETUP FUNCTION
// ============================================================================
void setup() {
  // ---- Serial Communication ----
  Serial.begin(115200);
  Serial.println("\n\nESP32 BNO055 IMU BLE Sensor Streaming");
  Serial.println("========================================");
  
  // ---- I2C Bus Initialization ----
  Wire.begin(4, 6);  // SDA=GPIO4, SCL=GPIO6
  Serial.println("I2C initialized: SDA=GPIO4, SCL=GPIO6");

  // ---- BNO055 Initialization ----
  if (!bno.begin()) {
    Serial.println("FATAL: BNO055 not detected! Check wiring and I2C address.");
    Serial.println("  Expected address: 0x29 (or 0x28 if ALT jumper set)");
    Serial.println("  Check connections: SDA=GPIO4, SCL=GPIO6, GND, 3.3V");
    while (1) delay(1000);  // Halt and wait
  }
  
  Serial.println("BNO055 detected successfully!");
  
  // Allow sensor to stabilize
  delay(1000);
  
  // Use external crystal for better timing accuracy
  bno.setExtCrystalUse(true);
  Serial.println("External crystal enabled (32.768 kHz)");
  Serial.println("Waiting for sensor calibration...");
  Serial.println("  Tip: Move device in figure-8 pattern for faster magnetometer calibration");

  // ---- BLE Device Initialization ----
  BLEDevice::init("ESP32_IMU");
  BLEDevice::setMTU(185);                    // Max 185-byte packets
  BLEDevice::setPower(ESP_PWR_LVL_P9);       // Maximum TX power (+20 dBm)

  // Create BLE Server and Service
  BLEServer  *pServer  = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pServer->setCallbacks(new MyServerCallbacks());

  // Create BLE Characteristic for sensor data
  // Properties: NOTIFY (server pushes data) + READ (client can request)
  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ
  );
  
  // Add CCCD descriptor (required for NOTIFY)
  pCharacteristic->addDescriptor(new BLE2902());

  // Start service and advertising
  pService->start();
  pServer->getAdvertising()->start();
  
  Serial.println("\nBLE Configuration:");
  Serial.println("  Device Name: ESP32_IMU");
  Serial.println("  Service UUID: 6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
  Serial.println("  Characteristic UUID: 6E400003-B5A3-F393-E0A9-E50E24DCCA9E");
  Serial.println("  MTU: 185 bytes");
  Serial.println("  TX Power: P9 (+20 dBm)");
  Serial.println("\nBLE advertising started.");
  Serial.println("Look for 'ESP32_IMU' in BLE scanner apps.");
  Serial.println("\n========================================");
  Serial.println("Sensor Data Output (20 Hz):");
  Serial.println("========================================\n");
}

// ============================================================================
// MAIN LOOP
// ============================================================================
void loop() {
  // Non-blocking timer: only process every SEND_INTERVAL_MS
  unsigned long now = millis();
  if (now - lastSendTime < SEND_INTERVAL_MS) return;
  lastSendTime = now;

  // ========================================
  // READ ALL SENSOR VECTORS
  // ========================================
  
  // Sensor data structures
  sensors_event_t linAccel, gravity, gyro, mag, euler;
  
  // Read each vector from BNO055
  bno.getEvent(&linAccel, VEC_LINEAR_ACCEL);  // Gravity-compensated acceleration
  bno.getEvent(&gravity,  VEC_GRAVITY);       // Isolated gravity vector
  bno.getEvent(&gyro,     VEC_GYROSCOPE);     // Angular velocity
  bno.getEvent(&mag,      VEC_MAGNETOMETER);  // Magnetic field
  bno.getEvent(&euler,    VEC_EULER);         // Euler angles (roll, pitch, yaw)

  // Get quaternion (separate function)
  imu::Quaternion quat = bno.getQuat();

  // Get calibration status (0-3 for each axis; 3 = fully calibrated)
  uint8_t sys, g, a, m;
  bno.getCalibration(&sys, &g, &a, &m);

  // ========================================
  // SERIAL DEBUG OUTPUT
  // ========================================
  Serial.printf(
    "CAL[sys:%d gyro:%d accel:%d mag:%d] | "
    "LinAcc: %.2f %.2f %.2f | "
    "Grav: %.2f %.2f %.2f | "
    "Gyro: %.2f %.2f %.2f | "
    "Mag: %.2f %.2f %.2f | "
    "Euler: %.2f %.2f %.2f | "
    "Quat: %.3f %.3f %.3f %.3f\n",
    sys, g, a, m,
    linAccel.acceleration.x, linAccel.acceleration.y, linAccel.acceleration.z,
    gravity.acceleration.x,  gravity.acceleration.y,  gravity.acceleration.z,
    gyro.gyro.x,             gyro.gyro.y,             gyro.gyro.z,
    mag.magnetic.x,          mag.magnetic.y,          mag.magnetic.z,
    euler.orientation.x,     euler.orientation.y,     euler.orientation.z,
    quat.w(), quat.x(), quat.y(), quat.z()
  );

  // ========================================
  // BLE TRANSMISSION
  // ========================================
  if (deviceConnected) {
    // Pack sensor data into float array
    // Order: LinAccel(3) | Gravity(3) | Gyro(3) | Mag(3) | Euler(3) | Quat(4)
    // Total: 19 floats = 76 bytes
    float vals[19] = {
      // Linear Acceleration (x, y, z in m/s²)
      linAccel.acceleration.x, linAccel.acceleration.y, linAccel.acceleration.z,
      // Gravity (x, y, z in m/s²)
      gravity.acceleration.x,  gravity.acceleration.y,  gravity.acceleration.z,
      // Gyroscope (x, y, z in rad/s)
      gyro.gyro.x,             gyro.gyro.y,             gyro.gyro.z,
      // Magnetometer (x, y, z in µT)
      mag.magnetic.x,          mag.magnetic.y,          mag.magnetic.z,
      // Euler Angles (roll, pitch, yaw in degrees)
      euler.orientation.x,     euler.orientation.y,     euler.orientation.z,
      // Quaternion (w, x, y, z - unit norm)
      (float)quat.w(),         (float)quat.x(),         (float)quat.y(),
      (float)quat.z()
    };

    // Convert to binary (76 bytes)
    uint8_t buf[sizeof(vals)];
    packBinary(vals, 19, buf);

    // Send via BLE NOTIFY
    pCharacteristic->setValue(buf, sizeof(buf));
    pCharacteristic->notify();
  }
}

/*
 * ============================================================================
 * RECEIVER SETUP (for reference)
 * ============================================================================
 * 
 * Python (using bleak library):
 * 
 *   import asyncio
 *   import struct
 *   from bleak import BleakClient
 *   
 *   SERVICE_UUID = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
 *   CHAR_UUID = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
 *   
 *   def notify_callback(sender, data):
 *       values = struct.unpack('<19f', data)
 *       lin_accel = values[0:3]
 *       gravity = values[3:6]
 *       gyro = values[6:9]
 *       mag = values[9:12]
 *       euler = values[12:15]
 *       quat = values[15:19]
 *       print(f"Accel: {lin_accel}, Gyro: {gyro}, Quat: {quat}")
 *   
 *   async def main():
 *       async with BleakClient("ESP32_IMU") as client:
 *           await client.start_notify(CHAR_UUID, notify_callback)
 *           await asyncio.sleep(60)
 *   
 *   asyncio.run(main())
 *
 * ============================================================================
 */
