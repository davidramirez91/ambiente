// #include <Arduino.h>
#include <WiFi.h>
#include "DHT.h"
#include <Wire.h>
#include <HTTPClient.h>

/* 1. Define WiFi credentials */
#define WIFI_SSID "SPEEDY-RAMIREZ"
#define WIFI_PASSWORD "R@mirez2022"
const String url = "https://ambiente-n3bl.onrender.com/datos";
// const String url = "http://192.168.88.109:3000/datos";

/* 2. Define Sensores */
#define DHTPIN 27     // Pin digital conectado al sensor DHT
#define DHTTYPE DHT11 // Detalle tipo de sensor
DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor.

bool estado = true;

////  PINES ////
// Entradas
const int UV_SENSOR = 13; // Sensor UV
const int SHS = 32;   // Sensor de humedad de suelo
const int INICIO = 5;   // INICIO
// Salidas
const int rele = 25;

void setup() {
  Serial.begin(115200);
  pinMode(INICIO, INPUT);
  pinMode(rele, OUTPUT);
  dht.begin();

  // Wifi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectando a Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Conectado con IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}

void loop() {  

  delay(10000); // Espere unos segundos entre mediciones.

  if (digitalRead(INICIO) == HIGH) {
    if (WiFi.status() == WL_CONNECTED) {
      int uvLevel = analogRead(UV_SENSOR);  // lectura del Sensor de Luz Uv Ultravioleta 
      float voltage = uvLevel * (3.3 / 4095.0); // Convertir el valor analógico a un valor de voltaje, 3.3V y resolución de 12 bits
      float uv = mapfloat(voltage, 0.0, 2.8, 0.0, 15.0); // Los valores pueden necesitar calibración
      int hs = map(analogRead(SHS), 0, 4095, 100, 0); // lectura del Sensor de Humedad de Suelo
      float h = dht.readHumidity(); // Leer la temperatura o la humedad tarda unos 250 milisegundos  
      float t = dht.readTemperature(); // Leer la temperatura como Celsius (el valor predeterminado)
      float hic = dht.computeHeatIndex(t, h, false); // Calcular el índice de calor en grados Celsius (isFahreheit = false)

      // Verifique si alguna lectura falló (to try again).
      if (!isnan(h) && !isnan(t)) {
        sendSensorDataToServer(h, t, hs, uv);
        digitalWrite(rele, HIGH);
        delay(2000);
        digitalWrite(rele, LOW);
      } else {
        Serial.println("Error al leer desde el sensor DHT");
      }

      impresion_datos(h, t, hs, uv);
      
      // Actividad de lectura exitosa
      digitalWrite(rele, HIGH);
      delay(2000);
      digitalWrite(rele, LOW);
      delay(2000);      
      
      estado = true;
    }
  } else {
    if (estado) {
      Serial.println("Esperando INICIO");
      estado = false;
    }
  }
}

void sendSensorDataToServer(float humidity, float temperature, int soilMoisture, float uvIntensity) {
  // INICIA CONEXCION DE DATOS
  HTTPClient http;
  http.begin(url); // tu servidor
  http.addHeader("Content-Type", "application/json"); // cabecera de la pagina

  String request = "{\"humedad_a\":" + String(humidity) + ",\"temperatura\":" + String(temperature) + ",\"humedad_s\":" + String(soilMoisture) + ",\"intensidad\":" + String(uvIntensity) + "}";
  int req_body= http.POST(request); // envia datos metodo POST (request)
  
  if (req_body > 0) { // Verifica que la conexión fue exitosa
    String resp = http.getString();
    Serial.println("Envio de datos exitoso");
  } else {
    Serial.print("Error en la solicitud: ");
    Serial.println(req_body);
  }
  http.end();
}



void impresion_datos(float humidity, float temperature, int soilMoisture, float uvIntensity) {
  Serial.print(F("Humedad: "));
  Serial.print(humidity);
  Serial.print(F("%  Temperatura: "));
  Serial.print(temperature);
  Serial.print(F("°C  \t humedad de suelo: "));
  Serial.print(soilMoisture);
  Serial.print(F("%  \t Intensidad UV:"));
  Serial.print(uvIntensity);
  Serial.println(F("mW/cm^2"));
}

// Función para asignar un valor flotante
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
