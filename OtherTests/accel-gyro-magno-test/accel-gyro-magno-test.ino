
#include <Wire.h>
#include <LSM6.h>
#include <LIS3MDL.h>

LSM6 imu;
LIS3MDL mag;

char report[120];

#define WINDOW_SIZE 10
int INDEX = 0;
int VALUE = 0;
int SUM = 0;
int READINGS[WINDOW_SIZE];
int AVERAGED = 0;



struct AccelData {
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

void setup(){
  Wire.begin();

  if (!mag.init())
  {
    // Failed to detect the LIS3MDL.

    while (1)
    {
      Serial.println(F("Failed to detect the LIS3MDL."));
      delay(100);
    }
  }

  mag.enableDefault();

  if (!imu.init())
  {

    while (1)
    {
      Serial.println(F("Failed to detect the LSM6."));
      delay(100);
    }
  }

  imu.enableDefault();

  // Set the gyro full scale to 1000 dps because the default
  // value is too low, and leave the other settings the same.
  imu.writeReg(LSM6::CTRL2_G, 0b10001000);

  // Set the accelerometer full scale to 16 g because the default
  // value is too low, and leave the other settings the same.
  imu.writeReg(LSM6::CTRL1_XL, 0b10000100);

  Serial.println("Starting");
}

void loop()
{
  imu.read();
  mag.read();

  /*snprintf_P(report, sizeof(report),
    PSTR("A: %6d %6d %6d    M: %6d %6d %6d    G: %6d %6d %6d"),
    imu.a.x, imu.a.y, imu.a.z,
    mag.m.x, mag.m.y, mag.m.z,
    imu.g.x, imu.g.y, imu.g.z);
    Serial.println(report);*/

  /*//  Serial.print("Raw:");
    Serial.print(imu.a.x);
    Serial.print(",");
    Serial.print(imu.a.y);
    Serial.print(",");
    Serial.print(imu.a.z);
    Serial.print(",");

    Serial.print(imu.g.x);
    Serial.print(",");
    Serial.print(imu.g.y);
    Serial.print(",");
    Serial.print(imu.g.z);
    Serial.print(",");

    Serial.print(mag.m.x/10);
    Serial.print(",");
    Serial.print(mag.m.y/10);
    Serial.print(",");
    Serial.print(mag.m.z/10);

    Serial.println();*/


  SUM = SUM - READINGS[INDEX];       // Remove the oldest entry from the sum
  VALUE = imu.a.x;        // Read the next sensor value
  READINGS[INDEX] = VALUE;           // Add the newest reading to the window
  SUM = SUM + VALUE;                 // Add the newest reading to the sum
  INDEX = (INDEX + 1) % WINDOW_SIZE; // Increment the index, and wrap to 0 if it exceeds the window size

  AVERAGED = SUM / WINDOW_SIZE;      // Divide the sum of the window by the window size for the result


  AccelData data;
  data = calibreAccel(imu.a.x, imu.a.y, imu.a.z);

  Serial.print("Value:");
  Serial.print(VALUE);
  Serial.print(",");
  Serial.print("Calibre:");
  Serial.print(data.x);
  Serial.print(",");
  Serial.println(AVERAGED);

  delay(25);


}



AccelData calibreAccel(int aX, int aY, int aZ) {

  AccelData result;

  // X axis
  sumValue[0] = sumValue[0] - readingValuesX[iterator];
  readingValuesX[iterator] = aX;
  sumValue[0] = sumValue[0] + aX;
  iterator = (iterator + 1) % RESOLUTION;
  result.x = sumValue[0] / RESOLUTION;

 /*// Y axis
  sumValue[1] = sumValue[1] - readingValuesY[iterator];
  readingValuesY[iterator] = aX;
  sumValue[1] = sumValue[1] + aX;
//  iterator = (iterator + 1) % resolution;
  result.y = sumValue[1] / resolution;

  // Z axis
  sumValue[2] = sumValue[2] - readingValuesZ[iterator];
  readingValuesZ[iterator] = aX;
  sumValue[2] = sumValue[2] + aX;
  iterator = (iterator + 1) % resolution;
  result.z = sumValue[2] / resolution;*/

  return result;
}
