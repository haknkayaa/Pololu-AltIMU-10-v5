import processing.serial.*;
import java.awt.event.KeyEvent;
import java.io.IOException;

Serial myPort;

String data = "";

float roll, pitch, yaw;
float gyroAngleX, gyroAngleY, gyroAngleZ;
float kalAngleX, kalAngleY, kalAngleZ;
float compAngleX, compAngleY, compAngleZ;

String comPort = "COM8";

void setup() {
  fullScreen(P3D, 2);
  //size (1000, 750, P3D);

  println("-->Serial COM Ports:");
  println(Serial.list());

  println("-->Connecting ", comPort);

  myPort = new Serial(this, comPort, 9600); // starts the serial communication
  myPort.bufferUntil('\n');
}

void draw() {
  //translate(width / 2, height / 2, 0);
  background(0);
  


  
  drawACube((1 * width)/8, (1 * height)/6, 0.5, -roll, yaw, pitch);
  drawACube((3 * width)/8, (1 * height)/6, 0.5, gyroAngleX, gyroAngleY, gyroAngleZ);
  drawACube((5 * width)/8, (1 * height)/6, 0.5, compAngleX, compAngleY, compAngleZ);
  drawACube((7 * width)/8, (1 * height)/6, 0.5, kalAngleX, kalAngleY, kalAngleZ);
}

void drawACube( int x, int y, float size, float angleX, float angleY, float angleZ) {
 
 
  pushMatrix();
  // Center in display window
  translate(x, y);
 
  // Rotate cube
  // noFill();
  fill(0);
  
  fill(0, 76, 153);
  textSize(22);
  text("roll: " + int(angleX) + " pitch: " + int(angleY) + "  yaw:" + int(angleZ), -200, 200);
  
  rotateX(radians(angleX));
  rotateY(radians(angleY));
  rotateZ(radians(angleZ));
  
  stroke(255);
 
  // Transparent cube, just using box() method 
 scale(size);
  textSize(30);
  fill(0, 76, 153);
  box(386, 40, 200); // Draw box
  textSize(25);
  fill(0, 255, 255);
  text("AltIMU-10 v5", 0, 10, 101);
  
  

  popMatrix();
}



// Read data from the Serial Port
void serialEvent(Serial myPort) {
  // reads the data from the Serial Port up to the character '.' and puts it into the String variable "data".
  data = myPort.readStringUntil('\n');
  // if you got any bytes other than the linefeed:
  if (data != null) {
    data = trim(data);
    // split the string at "/"
    String items[] = split(data, '\t');
    if (items.length > 1) {
      //--- Roll,Pitch in degrees

      roll       = float(items[0]); 
      gyroAngleX = float(items[1]);
      compAngleX = float(items[2]);
      kalAngleX  = float(items[3]);
      
      pitch      = float(items[4]); 
      gyroAngleY = float(items[5]);
      compAngleY = float(items[6]);
      kalAngleY  = float(items[7]);
      
      yaw        = float(items[8]); 
      gyroAngleZ = float(items[9]);
      compAngleZ = float(items[10]);
      kalAngleZ  = float(items[11]);

      print(roll);
      print("\t");
      print(pitch);
      print("\t");
      print(yaw);
      print("\t");
      
      print("\t");
      
      print(gyroAngleX);
      print("\t");
      print(gyroAngleY);
      print("\t");
      print(gyroAngleZ);
      print("\t");
      
      print("\t");
      
      print(compAngleX);
      print("\t");
      print(compAngleY);
      print("\t");
      print(compAngleZ);
      print("\t");
      
      print("\t");
      
      print(kalAngleX);
      print("\t");
      print(kalAngleY);
      print("\t");
      print(kalAngleZ);
      println("\t");
    }
  }
}
