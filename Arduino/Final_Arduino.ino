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
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// variables orientation/motion
Quaternion q;           // [w, x, y, z] quaternion container

String DatosMPU;

// ================================================================
// ===               VARIABLES DE LOS SENSORES                  ===
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

    //Serial.begin(115200);
    Serial.begin(250000);
    //while (!Serial); // wait for Leonardo enumeration, others continue immediately

    
    
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
        TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    // inicializo dispositivos
    mpu.initialize();
    devStatus = mpu.dmpInitialize();

    // mis offset
    mpu.setXGyroOffset(-2023);
    mpu.setYGyroOffset(-30);
    mpu.setZGyroOffset(-13);
    mpu.setZAccelOffset(1470);

    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
        // turn on the DMP, now that it's ready
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        attachInterrupt(2, dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        Serial.print(F("DMP Initialization failed (code "));
        Serial.print(devStatus);
        Serial.println(F(")"));
    }

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
    if (!dmpReady) return; // if programming failed, don't try to do anything
    
    // wait for MPU interrupt or extra packet(s) available
    while (!mpuInterrupt && fifoCount < packetSize) { 
    }

    // reset interrupt flag and get INT_STATUS byte
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();

    // get current FIFO count
    fifoCount = mpu.getFIFOCount();

    // check for overflow (this should never happen unless our code is too inefficient)
    if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
        mpu.resetFIFO();
    } else if (mpuIntStatus & 0x02) {
        while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
        mpu.getFIFOBytes(fifoBuffer, packetSize);
        fifoCount -= packetSize;

        // display quaternion values in easy matrix form: w,x,y,z
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        
        DatosMPU = String(q.w)+","+String(q.x)+","+String(q.y)+","+String(q.z);
    }

    flexposition0 = analogRead(flexpin0);
    flexposition1 = analogRead(flexpin1);
    flexposition2 = analogRead(flexpin2);
    flexposition3 = analogRead(flexpin3); 
    flexposition4 = analogRead(flexpin4);
  
    flexposition0 = map(flexposition0,min0,max0,0,100);
    flexposition1 = map(flexposition1,min1,max1,0,100);
    flexposition2 = map(flexposition2,min2,max2,0,100);
    flexposition3 = map(flexposition3,min3,max3,0,100);
    flexposition4 = map(flexposition4,min4,max4,0,100);

    DatosSENSORES = String(flexposition0)+","+String(flexposition1)+","+String(flexposition2)+","+String(flexposition3)+","+String(flexposition4);

    Serial.println(DatosSENSORES+","+DatosMPU);
    delay(20);//20
}
