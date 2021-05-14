import processing.serial.*;
import java.awt.event.KeyEvent;
import java.io.IOException;

Serial myPort;

String data="";

float aX, aY, aZ;
float roll, pitch, yaw;

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
  translate(width/2, height/2, 0);
  background(0);
  textSize(22);
  text("Roll: " + int(roll) + "     Pitch: " + int(pitch), 0, 265);
  
  // Rotate the object
  rotateX(radians(-pitch));
  rotateZ(radians(roll));
  rotateY(radians(yaw));
  
  // 3D 0bject
  textSize(30);  
  fill(0, 76, 153);
  box (386, 40, 200); // Draw box
  textSize(25);
  fill(0, 255, 255);
  text("AltIMU-10 v5", 0, 10, 101);
  //delay(10);
  //println("ypr:\t" + angleX + "\t" + angleY); // Print the values to check whether we are getting proper values
}

// Read data from the Serial Port
void serialEvent (Serial myPort) { 
  // reads the data from the Serial Port up to the character '.' and puts it into the String variable "data".
  data = myPort.readStringUntil('\n');
  // if you got any bytes other than the linefeed:
  if (data != null) {
    data = trim(data);
    // split the string at "/"
    String items[] = split(data, ',');
    if (items.length > 1) {
      //--- Roll,Pitch in degrees
      aX = float(item[0]);
        aX = float(item[0]);
        aX = float(item[0]);
      roll = float(items[0]);
      pitch = float(items[1]);
      yaw = float(items[2]);
      
      print(roll);
      print("\t");
      print(pitch);
      print("\t");
      print(yaw);
      println("\t");
    }
  }
}
