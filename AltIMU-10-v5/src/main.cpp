// Arduino Framework
#include <Arduino.h>

// Libraries
#include <Wire.h>
#include <LSM6.h>
#include <LIS3MDL.h>

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

#define RESOLUTION 10
int iterator = 0;
int sumValue[3] = {0, 0, 0};
int readingValuesX[RESOLUTION];
int readingValuesY[RESOLUTION];
int readingValuesZ[RESOLUTION];

#define MAG_RESOLUTION 10
int mag_iterator = 0;
int mag_sumValue[3] = {0, 0, 0};
int mag_readingValuesX[MAG_RESOLUTION];
int mag_readingValuesY[MAG_RESOLUTION];
int mag_readingValuesZ[MAG_RESOLUTION];

// function prototype
AxisData calibreAccel(int aX, int aY, int aZ);

AxisData calculateDegree(AxisData data);

AxisData calibreMag(int magX, int magY, int magZ);

void printSensorData();


/// ====================================================================================================================
/// Setup Fonksiyonu
/// Buradaki kodlar sadece 1 defa calısacaktır.
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
    imu.writeReg(LSM6::CTRL2_G, 0b10001000);

    // Set the accelerometer full scale to 16 g because the default
    // value is too low, and leave the other settings the same.
    imu.writeReg(LSM6::CTRL1_XL, 0b10000100);

    while (!mag.init()) {
        // Failed to detect the LIS3MDL.

        Serial.println(F("Failed to detect the LIS3MDL."));
        delay(100);

    }
    mag.enableDefault();


    Serial.println("Starting...");
}

/// ===================================================================================================================
/// Loop döngüsü
void loop() {
    imu.read();
    mag.read();

    printSensorData();

    delay(25);
}

void printSensorData() {
    //    char report[120];
//
//    snprintf_P(report, sizeof(report),
//               PSTR("A: %6d %6d %6d    M: %6d %6d %6d    G: %6d %6d %6d"),
//               imu.a.x, imu.a.y, imu.a.z,
//               mag.m.x, mag.m.y, mag.m.z,
//               imu.g.x, imu.g.y, imu.g.z);
//    Serial.println(report);


    AxisData accelData = calibreAccel(imu.a.x, imu.a.y, imu.a.z);
    AxisData axisDegree = calculateDegree(accelData);
    AxisData magData = calibreMag(mag.m.x, mag.m.y, mag.m.z);

    Serial.print("CalibreX:");
    Serial.print(accelData.x);
    Serial.print(",");
    Serial.print("CalibreY:");
    Serial.print(accelData.y);
    Serial.print(",");
    Serial.print("CalibreZ:");
    Serial.print(accelData.z);
    Serial.print(",");

    Serial.print("DegreeX:");
    Serial.print(axisDegree.x);
    Serial.print(",");
    Serial.print("DegreeY:");
    Serial.print(axisDegree.y);
    Serial.print(",");
    Serial.print("DegreeZ:");
    Serial.print(axisDegree.z);
    Serial.print(",");

    Serial.print("MagX:");
    Serial.print(magData.x);
    Serial.print(",");
    Serial.print("MagY:");
    Serial.print(magData.y);
    Serial.print(",");
    Serial.print("MagZ:");
    Serial.print(magData.z);
    Serial.print(",");

    Serial.println();
}

/// Eksenlere gore accelerometerı kalibrasyon yapıp
/// stabil sonuc dondurur.
/// Parametre olarak ham sensörden okundan veriler verilir
/// \param aX
/// \param aY
/// \param aZ
/// \return AxisData _.x, _,y, _,z formatında
AxisData calibreAccel(int aX, int aY, int aZ) {

    AxisData result{};

    // X axis
    sumValue[0] = sumValue[0] - readingValuesX[iterator];
    readingValuesX[iterator] = aX;
    sumValue[0] = sumValue[0] + aX;

    result.x = sumValue[0] / RESOLUTION;

    // Y axis
    sumValue[1] = sumValue[1] - readingValuesY[iterator];
    readingValuesY[iterator] = aY;
    sumValue[1] = sumValue[1] + aY;

    result.y = sumValue[1] / RESOLUTION;

    // Z axis
    sumValue[2] = sumValue[2] - readingValuesZ[iterator];
    readingValuesZ[iterator] = aZ;
    sumValue[2] = sumValue[2] + aZ;

    result.z = sumValue[2] / RESOLUTION;

    iterator = (iterator + 1) % RESOLUTION;

    return result;
}

///  Accelerometerdan okunan sensör verilerini
///  dereceye dönüştürür.
/// \param data : raw olarak verilen sensör bilgileri
/// \return _.x, _.y, _.z şeklinde derece döndürür
AxisData calculateDegree(AxisData data) {
    int degreeResolution = 22;

    AxisData degree;
    degree.x = data.x / degreeResolution;
    degree.y = data.y / degreeResolution;
    degree.z = data.z / degreeResolution;
    return degree;
}

/// Eksenelere göre magnetometerı kalibre edip
/// stabil sonuc dondurur
/// \param magX
/// \param magY
/// \param magZ
/// \return
AxisData calibreMag(int magX, int magY, int magZ) {

    AxisData result{};

    // X axis
    mag_sumValue[0] = mag_sumValue[0] - mag_readingValuesX[mag_iterator];
    mag_readingValuesX[mag_iterator] = magX;
    mag_sumValue[0] = mag_sumValue[0] + magX;

    result.x = mag_sumValue[0] / RESOLUTION;

    // Y axis
    mag_sumValue[1] = mag_sumValue[1] - mag_readingValuesY[mag_iterator];
    mag_readingValuesY[mag_iterator] = magY;
    mag_sumValue[1] = mag_sumValue[1] + magY;

    result.y = mag_sumValue[1] / RESOLUTION;

    // Z axis
    mag_sumValue[2] = mag_sumValue[2] - mag_readingValuesZ[mag_iterator];
    mag_readingValuesZ[mag_iterator] = magZ;
    mag_sumValue[2] = mag_sumValue[2] + magZ;

    result.z = mag_sumValue[2] / RESOLUTION;

    iterator = (iterator + 1) % RESOLUTION;

    return result;
}