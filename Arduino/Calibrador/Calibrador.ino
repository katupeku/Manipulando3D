/// Incluye las librerias Wire usada en I2Cdev, I2Cdev usada en MPU6050 y la libreria MPU6050
#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"

/// inicializamos el objeto
MPU6050 accelgyro;

/// Incializamos las variables
int16_t ax, ay, az; // variables acelerometro
int16_t gx, gy, gz; // variables giroscopo

// ================================================================
// ===                         SETUP                            ===
// ================================================================

void setup() {
    Wire.begin(); ///< Inicializamos la comunicacion I2C
    Serial.begin(57600); ///< Inicializamos la comunicacion Serie

    /// Inicializamos el sensor y lo imprimimos
    accelgyro.initialize();
    Serial.println("Inicializando dispositivos I2C...");
   
    /// Confirmamos si la comunicacion es correcta
    Serial.println("Probando conexión con dispositivos...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 conectado correctamente" : "Falló la conexión con MPU6050");
    
    /// Añadimos los valores Offset al sensor
    accelgyro.setXGyroOffset(22);  // gx
    accelgyro.setYGyroOffset(-25); // gy
    accelgyro.setZGyroOffset(6);   // gz
        
    accelgyro.setXAccelOffset(-300); // ax
    accelgyro.setYAccelOffset(-1396);// ay
    accelgyro.setZAccelOffset(2677); // az
}

// ================================================================
// ===                         LOOP                             ===
// ================================================================
void loop() {
    /// Pedimos al sensor los valores raw
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
   
   /// Imprimimos los valores raw
   Serial.print(gx); Serial.print("\t"); // gx
   Serial.print(gy); Serial.print("\t"); // gy
   Serial.print(gz); Serial.print("\t"); // gz

   Serial.print(ax); Serial.print("\t"); // ax
   Serial.print(ay); Serial.print("\t"); // ay
   Serial.println(az);                   // az
}
