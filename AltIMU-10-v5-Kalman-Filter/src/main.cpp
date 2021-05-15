#include <Arduino.h>

// Libraries
#include <Wire.h>
#include <LSM6.h>
#include <LIS3MDL.h>

#include "../lib/Kalman.h"

LSM6 imu;
LIS3MDL mag;

/// example.x, example.y, example.z
/// seklinde veri saklamak icin yaratılan
/// ozel degisken turu
struct AxisData {
    int x;
    int y;
    int z;
};

LIS3MDL::vector<int16_t> running_min = {32767, 32767, 32767}, running_max = {-32768, -32768, -32768};

#define RESTRICT_PITCH // Comment out to restrict roll to ±90deg instead - please read: http://www.freescale.com/files/sensors/doc/app_note/AN3461.pdf

Kalman kalmanX, kalmanY, kalmanZ; // Create the Kalman instances

/** IMU Data */
double accX, accY, accZ;
double gyroX, gyroY, gyroZ;
double magX, magY, magZ;
int16_t tempRaw;

double roll, pitch, yaw; // Roll and pitch are calculated using the accelerometer while yaw is calculated using the magnetometer

double gyroXangle, gyroYangle, gyroZangle; // Angle calculate using the gyro only
double compAngleX, compAngleY, compAngleZ; // Calculated angle using a complementary filter
double kalAngleX, kalAngleY, kalAngleZ; // Calculated angle using a Kalman filter

uint32_t timer;

// min: { -3347,  -5192,  -4355}   max: { +3467,  +2301,  +3515}
#define MAG0MAX 3467
#define MAG0MIN -3347

#define MAG1MAX 2301
#define MAG1MIN -5192

#define MAG2MAX 3515
#define MAG2MIN -4355

float magOffset[3] = {(MAG0MAX + MAG0MIN) / 2, (MAG1MAX + MAG1MIN) / 2, (MAG2MAX + MAG2MIN) / 2};
double magGain[3];

// Function Prototypes
void calibrateLIS3MDL();

void updateLSM6();

void updateLIS3MDL();

void updatePitchRoll();

void updateYaw();

void setup() {
    Serial.begin(115200);
    Wire.begin();

    while (!imu.init()) {
        Serial.println(F("Failed to detect the LSM6."));
        delay(100);
    }
    imu.enableDefault();

    // Set the gyro full scale to 1000 dps because the default
    // value is too low, and leave the other settings the same.
//    imu.writeReg(LSM6::CTRL2_G, 0b10001000);
//
//    // Set the accelerometer full scale to 16 g because the default
//    // value is too low, and leave the other settings the same.
//    imu.writeReg(LSM6::CTRL1_XL, 0b10000100);

    while (!mag.init()) {
        // Failed to detect the LIS3MDL.
        Serial.println(F("Failed to detect the LIS3MDL."));
        delay(100);
    }
    mag.enableDefault();

    Serial.println("Starting...");

    for (uint16_t i = 0; i < 327; ++i) {
        calibrateLIS3MDL();
    }

}

void loop() {
    /* Update all the IMU values */
    updateLSM6();
    updateLIS3MDL();

    double dt = (double) (micros() - timer) / 1000000; // Calculate delta time
    timer = micros();

    /* Roll and pitch estimation */
    updatePitchRoll();
    double gyroXrate = gyroX / 131.0; // Convert to deg/s
    double gyroYrate = gyroY / 131.0; // Convert to deg/s

#ifdef RESTRICT_PITCH
    // This fixes the transition problem when the accelerometer angle jumps between -180 and 180 degrees
    if ((roll < -90 && kalAngleX > 90) || (roll > 90 && kalAngleX < -90)) {
        kalmanX.setAngle(roll);
        compAngleX = roll;
        kalAngleX = roll;
        gyroXangle = roll;
    } else
        kalAngleX = kalmanX.getAngle(roll, gyroXrate, dt); // Calculate the angle using a Kalman filter

    if (abs(kalAngleX) > 90)
        gyroYrate = -gyroYrate; // Invert rate, so it fits the restricted accelerometer reading
    kalAngleY = kalmanY.getAngle(pitch, gyroYrate, dt);
#else
    // This fixes the transition problem when the accelerometer angle jumps between -180 and 180 degrees
  if ((pitch < -90 && kalAngleY > 90) || (pitch > 90 && kalAngleY < -90)) {
    kalmanY.setAngle(pitch);
    compAngleY = pitch;
    kalAngleY = pitch;
    gyroYangle = pitch;
  } else
    kalAngleY = kalmanY.getAngle(pitch, gyroYrate, dt); // Calculate the angle using a Kalman filter

  if (abs(kalAngleY) > 90)
    gyroXrate = -gyroXrate; // Invert rate, so it fits the restricted accelerometer reading
  kalAngleX = kalmanX.getAngle(roll, gyroXrate, dt); // Calculate the angle using a Kalman filter
#endif


    /* Yaw estimation */
    updateYaw();
    double gyroZrate = gyroZ / 131.0; // Convert to deg/s
    // This fixes the transition problem when the yaw angle jumps between -180 and 180 degrees
    if ((yaw < -90 && kalAngleZ > 90) || (yaw > 90 && kalAngleZ < -90)) {
        kalmanZ.setAngle(yaw);
        compAngleZ = yaw;
        kalAngleZ = yaw;
        gyroZangle = yaw;
    } else
        kalAngleZ = kalmanZ.getAngle(yaw, gyroZrate, dt); // Calculate the angle using a Kalman filter


    /* Estimate angles using gyro only */
    gyroXangle += gyroXrate * dt; // Calculate gyro angle without any filter
    gyroYangle += gyroYrate * dt;
    gyroZangle += gyroZrate * dt;
    //gyroXangle += kalmanX.getRate() * dt; // Calculate gyro angle using the unbiased rate from the Kalman filter
    //gyroYangle += kalmanY.getRate() * dt;
    //gyroZangle += kalmanZ.getRate() * dt;

    /* Estimate angles using complimentary filter */
    compAngleX = 0.93 * (compAngleX + gyroXrate * dt) + 0.07 * roll; // Calculate the angle using a Complimentary filter
    compAngleY = 0.93 * (compAngleY + gyroYrate * dt) + 0.07 * pitch;
    compAngleZ = 0.93 * (compAngleZ + gyroZrate * dt) + 0.07 * yaw;

    // Reset the gyro angles when they has drifted too much
    if (gyroXangle < -180 || gyroXangle > 180)
        gyroXangle = kalAngleX;
    if (gyroYangle < -180 || gyroYangle > 180)
        gyroYangle = kalAngleY;
    if (gyroZangle < -180 || gyroZangle > 180)
        gyroZangle = kalAngleZ;


    /* Print Data */
#if 1
    Serial.print(roll);
    Serial.print("\t");
    Serial.print(gyroXangle);
    Serial.print("\t");
    Serial.print(compAngleX);
    Serial.print("\t");
    Serial.print(kalAngleX);
    Serial.print("\t");

//    Serial.print("\t");

    Serial.print(pitch);
    Serial.print("\t");
    Serial.print(gyroYangle);
    Serial.print("\t");
    Serial.print(compAngleY);
    Serial.print("\t");
    Serial.print(kalAngleY);
    Serial.print("\t");

//    Serial.print("\t");

    Serial.print(yaw);
    Serial.print("\t");
    Serial.print(gyroZangle);
    Serial.print("\t");
    Serial.print(compAngleZ);
    Serial.print("\t");
    Serial.print(kalAngleZ);
    Serial.print("\t");
#endif

#if 0 // Set to 1 to print the IMU data
    Serial.print(accX / 16384.0); Serial.print("\t"); // Converted into g's
  Serial.print(accY / 16384.0); Serial.print("\t");
  Serial.print(accZ / 16384.0); Serial.print("\t");

  Serial.print(gyroXrate); Serial.print("\t"); // Converted into degress per second
  Serial.print(gyroYrate); Serial.print("\t");
  Serial.print(gyroZrate); Serial.print("\t");

  Serial.print(magX); Serial.print("\t"); // After gain and offset compensation
  Serial.print(magY); Serial.print("\t");
  Serial.print(magZ); Serial.print("\t");
#endif

#if 0 // Set to 1 to print the temperature
    Serial.print("\t");

  double temperature = (double)tempRaw / 340.0 + 36.53;
  Serial.print(temperature); Serial.print("\t");
#endif

    Serial.println();

    delay(10);
}

// Calibrate Magnetometer
void calibrateLIS3MDL() {

    mag.read();
    delay(10);

    running_min.x = min(running_min.x, mag.m.x);
    running_min.y = min(running_min.y, mag.m.y);
    running_min.z = min(running_min.z, mag.m.z);

    running_max.x = max(running_max.x, mag.m.x);
    running_max.y = max(running_max.y, mag.m.y);
    running_max.z = max(running_max.z, mag.m.z);

    int16_t magPosOff[3] = {running_max.x, running_max.y, running_max.z};
    int16_t magNegOff[3] = {running_min.x, running_min.y, running_min.z};

    magGain[0] = -2500 / float(magNegOff[0] - magPosOff[0]);
    magGain[1] = -2500 / float(magNegOff[1] - magPosOff[1]);
    magGain[2] = -2500 / float(magNegOff[2] - magPosOff[2]);


#if 1
    Serial.print("Mag cal: ");
    Serial.print(magNegOff[0] - magPosOff[0]);
    Serial.print(",");
    Serial.print(magNegOff[1] - magPosOff[1]);
    Serial.print(",");
    Serial.println(magNegOff[2] - magPosOff[2]);

    Serial.print("Gain: ");
    Serial.print(magGain[0]);
    Serial.print(",");
    Serial.print(magGain[1]);
    Serial.print(",");
    Serial.println(magGain[2]);
#endif
}

// Like MPU6050 Gyro and Accelerometer
void updateLSM6() {
    imu.read();

    accX = imu.a.x;
    accY = -imu.a.y;
    accZ = imu.a.z;

//    tempRaw = 0; //!todo check

    gyroX = -imu.g.x;
    gyroY = imu.g.y;
    gyroZ = -imu.g.z;
}

// Like HMC5883L magnetometer
void updateLIS3MDL() {
    mag.read();

    magX = mag.m.x;
    magZ = mag.m.y;
    magY = mag.m.z;
}

void updatePitchRoll() {
    // Source: http://www.freescale.com/files/sensors/doc/app_note/AN3461.pdf eq. 25 and eq. 26
    // atan2 outputs the value of -π to π (radians) - see http://en.wikipedia.org/wiki/Atan2
    // It is then converted from radians to degrees
#ifdef RESTRICT_PITCH // Eq. 25 and 26
    roll = atan2(accY, accZ) * RAD_TO_DEG;
    pitch = atan(-accX / sqrt(accY * accY + accZ * accZ)) * RAD_TO_DEG;
#else // Eq. 28 and 29
    roll = atan(accY / sqrt(accX * accX + accZ * accZ)) * RAD_TO_DEG;
    pitch = atan2(-accX, accZ) * RAD_TO_DEG;
#endif
}

void updateYaw() {
    magX *= -1; // Invert axis - this it done here, as it should be done after the calibration
    magZ *= -1;

    magX *= magGain[0];
    magY *= magGain[1];
    magZ *= magGain[2];

    magX -= magOffset[0];
    magY -= magOffset[1];
    magZ -= magOffset[2];

    double rollAngle = kalAngleX * DEG_TO_RAD;
    double pitchAngle = kalAngleY * DEG_TO_RAD;

    double Bfy = magZ * sin(rollAngle) - magY * cos(rollAngle);
    double Bfx =
            magX * cos(pitchAngle) + magY * sin(pitchAngle) * sin(rollAngle) + magZ * sin(pitchAngle) * cos(rollAngle);
    yaw = atan2(-Bfy, Bfx) * RAD_TO_DEG;

    yaw *= -1;
}

