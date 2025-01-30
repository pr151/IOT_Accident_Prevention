#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <Wire.h>
#include <MPU6050.h>

#define WIFI_SSID "Prashant"
#define WIFI_PASSWORD "12345"
#define FIREBASE_HOST "your-firebase-database.firebaseio.com"
#define FIREBASE_AUTH "your-firebase-secret-key"

#define TRIG_PIN D1
#define ECHO_PIN D2
#define BUZZER_PIN D3

MPU6050 mpu;
FirebaseData firebaseData;

void setup() {
    Serial.begin(115200);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting...");
    }
    Serial.println("Connected to WiFi");

    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    Firebase.reconnectWiFi(true);

    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    Wire.begin();
    mpu.initialize();
}

void loop() {
    long duration;
    int distance;
    
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    duration = pulseIn(ECHO_PIN, HIGH);
    distance = duration * 0.034 / 2;

    int ax, ay, az;
    mpu.getAcceleration(&ax, &ay, &az);

    if (distance < 20) {
        digitalWrite(BUZZER_PIN, HIGH);
        Firebase.setInt(firebaseData, "/AccidentData/distance", distance);
        Firebase.setString(firebaseData, "/AccidentData/status", "Obstacle Detected!");
    } else {
        digitalWrite(BUZZER_PIN, LOW);
    }

    if (ax > 16000 || ay > 16000 || az > 16000) {
        Firebase.setString(firebaseData, "/AccidentData/status", "Sudden Impact Detected!");
    }

    delay(2000);
}
