// ================================================================
// ===                     LIBRERIAS NECESARIAS                 ===
// ================================================================
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

// ================================================================
// ===                     VARIABLES DEL MPU                    ===
// ================================================================
MPU6050 mpu;

// MPU variables control/status
bool dmpReady = false;  // Setea en true si el DMP inicio correctamete
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // tamaño de paquete esperado (por defecto es 42 bytes)
uint16_t fifoCount;     // conteo de todos los bytes actual en la cola FIFO
uint8_t fifoBuffer[64]; // buffer de almacenamiento FIFO

// variables de orientacion y movimiento
Quaternion q;

String DatosMPU;

// ================================================================
// ===       VARIABLES DE LOS SENSORES DE FLEXIBILIDAD          ===
// ================================================================
const int flexpin0 = 0, flexpin1 = 1, flexpin2 = 2, flexpin3 = 3, flexpin4 = 4;
int min0 = 10000, min1 = 10000, min2 = 10000, min3 = 10000, min4 = 10000;
int max0 = -10000, max1 = -10000, max2 = -10000, max3 = -10000, max4 = -10000;
int flexposition0, flexposition1, flexposition2, flexposition3, flexposition4;

String DatosSENSORES;

// ================================================================
// ===            RUTINA DETECTADORA DE INTERRUPCIONES          ===
// ================================================================
volatile bool mpuInterrupt = false;
void dmpDataReady() {
    mpuInterrupt = true;
}

// ================================================================
// ===                CONFIGURACIÓN INICIAL                     ===
// ================================================================
void setup() {
//  inicializacion del puerto para la lectura de datos
    Serial.begin(115200);
    //Serial.begin(250000);
    
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
        TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

//  inicializo dispositivos
    mpu.initialize();
    devStatus = mpu.dmpInitialize();

//  offset de calibracion del MPU
    mpu.setXGyroOffset(-2023);
    mpu.setYGyroOffset(-30);
    mpu.setZGyroOffset(-13);
    mpu.setZAccelOffset(1470);

//  verifica que funcione (devuelve 0 en caso que funcione correctamente)
    if (devStatus == 0) {
//      encender el DMP
        mpu.setDMPEnabled(true);

//      Activar la deteccion de interrupciones de arduino
        attachInterrupt(2, dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();

//      setea la bandera para indicar que esta listo para usarse
        dmpReady = true;

//      setear el tamaño esperado de paquete para compararlo
        packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
//      EL DMP DEVOLVIO ERROR
//      1 = fallo al cargar la memoria inicial
//      2 = fallo en la actualizacion de configuración
        Serial.print(F("Fallo la inicialización del DMP (codigo "));
        Serial.print(devStatus);
        Serial.println(F(")"));
    }


////////////////////////////////////////////////////////////////////
//              CALIBRACION DE LOS SENSORES FLEX                  //
////////////////////////////////////////////////////////////////////
//  Se leen 100 veces los valores de los 5 sensores estableciendo 
//  los valores maximos y minimos obtenidos
    for(int i=0;i<100;i++){
        flexposition0 = analogRead(flexpin0);
        if (min0 > flexposition0) {
            min0 = flexposition0;
        }
        if (max0 < flexposition0) {
            max0 = flexposition0;
        }
        
        flexposition1 = analogRead(flexpin1);
        if (min1 > flexposition1) {
            min1 = flexposition1;
        }
        if (max1 < flexposition1) {
            max1 = flexposition1;
        }
        
        flexposition2 = analogRead(flexpin2);
        if (min2 > flexposition2) {
            min2 = flexposition2;
        }
        if (max2 < flexposition2) {
            max2 = flexposition2;
        }
        
        flexposition3 = analogRead(flexpin3);
        if (min3 > flexposition3) {
            min3 = flexposition3;
        }
        if (max3 < flexposition3) {
            max3 = flexposition3;
        }
        
        flexposition4 = analogRead(flexpin4);
        if (min4 > flexposition4) {
            min4 = flexposition4;
        }
        if (max4 < flexposition4) {
            max4 = flexposition4;
        }
        delay(50);
    }
}


// ================================================================
// ===             PROGRAMACIÓN PRINCIPAL EN LOOP               ===
// ================================================================
void loop() {
//  si el sensor DMP no esta listo, no hace nada
    if (!dmpReady){
      return;
    }
    
//  espera por una interrupcion del MPU o un paquete disponible
    while (!mpuInterrupt && fifoCount < packetSize) { 
    }
    
//  resetea la badera de interrupcion y obtiene bytes de estado
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();

//  obtiene la cantidad actual de FIFO
    fifoCount = mpu.getFIFOCount();

//  checkea un desbordamiento de datos 
//  (Esto no deberia pasar pero si el codigo es ineficiente sirve para prevenirlo)
    if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
        mpu.resetFIFO();
    } else if (mpuIntStatus & 0x02) {
        while (fifoCount < packetSize){
          fifoCount = mpu.getFIFOCount();
        }
        mpu.getFIFOBytes(fifoBuffer, packetSize);
        fifoCount -= packetSize;

//      obtiene los valores del cuaternion en forma de matriz (W,X,Y,Z)
        mpu.dmpGetQuaternion(&q, fifoBuffer);

//      Guardo los valores obtenidos para ser devueltos       
        DatosMPU = String(q.w)+","+String(q.x)+","+String(q.y)+","+String(q.z);
    }

//  Lectura de todos los sensores de flexibildad
    flexposition0 = analogRead(flexpin0);
    flexposition1 = analogRead(flexpin1);
    flexposition2 = analogRead(flexpin2);
    flexposition3 = analogRead(flexpin3); 
    flexposition4 = analogRead(flexpin4);

//  se realiza un mapeo de los valores obtenidos a un porcentaje entre los 
//  maximos y minimos establecidos en la configuracion incial
    flexposition0 = map(flexposition0,min0,max0,0,100);
    flexposition1 = map(flexposition1,min1,max1,0,100);
    flexposition2 = map(flexposition2,min2,max2,0,100);
    flexposition3 = map(flexposition3,min3,max3,0,100);
    flexposition4 = map(flexposition4,min4,max4,0,100);

//  Guardo los valores obtenidos para ser devueltos
    DatosSENSORES = String(flexposition0)+","+String(flexposition1)+","+String(flexposition2)+","+String(flexposition3)+","+String(flexposition4);

//  Devuelvo todos los valores obtenidos concatenados por "," (comas)
    Serial.println(DatosSENSORES+","+DatosMPU);

//  Espera 20 milisegundos para realizar una nueva lectura
    delay(20);
}
