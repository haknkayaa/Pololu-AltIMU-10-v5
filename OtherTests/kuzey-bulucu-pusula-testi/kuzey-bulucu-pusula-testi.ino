
#include <Wire.h>
#include <LSM6.h>
#include <LIS3MDL.h>

LSM6 imu;
LIS3MDL mag;

char report[120];

#define CALIBRATION false

float xMax, yMax, xMin, yMin = 0.0;

void setup()
{
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
  /*
    Serial.print("Raw:");
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

    Serial.print(mag.m.x);
    Serial.print(",");
    Serial.print(mag.m.y);
    Serial.print(",");
    Serial.print(mag.m.z);

    Serial.println();
  */



  if (xMax == 0.0) {
    xMax = mag.m.x;
  }

  if (yMax == 0.0) {
    yMax = mag.m.y;
  }

  if (CALIBRATION) {
    xMax = max(xMax, mag.m.x);
    yMax = max(yMax, mag.m.y);
    xMin = min(xMin, mag.m.x);
    yMin = min(yMin, mag.m.y);

    Serial.print(xMax);
    Serial.print(",");
    Serial.print(xMin);
    Serial.print(",");
    Serial.print(yMax);
    Serial.print(",");
    Serial.print(yMin);
    Serial.print(",");
    Serial.println();

  }

  // 4665.00,-5418.00,2424.00,-10541.00,


  
  float heading = atan2((mag.m.y - ((2424.00 -10541.00) / 2.0)), (mag.m.x - ((4665.00 -5418.00) / 2.0)));
  float declinationAngle = 0.087; // Cairns magnetic declination. 6°41’ east.
  heading += declinationAngle;

  // Correct for when signs are reversed.
  if (heading < 0) {
    heading += 2 * PI;
  }

  // Check for wrap due to addition of declination.
  if (heading > 2 * PI) {
    heading -= 2 * PI;
  }

  float headingDegrees = heading * 180 / M_PI; // Convert radians to degrees.

  Serial.println(headingDegrees);
  
  delay(10);
}
