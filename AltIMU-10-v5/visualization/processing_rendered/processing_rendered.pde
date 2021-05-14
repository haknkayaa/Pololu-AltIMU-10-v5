import processing.serial.*;
import java.awt.event.KeyEvent;
import java.io.IOException;

Serial myPort;

String comPort = "COM8";

String data = "";

float aX, aY, aZ;
float x, y, z;
float magX, magY, magZ;


PImage topside,downside,frontside,rightside;


void setup() {
    fullScreen(P3D, 2);
    //size (1000, 750, P3D);

    println("-->Serial COM Ports:");
    println(Serial.list());

    println("-->Connecting ", comPort);

    myPort = new Serial(this, comPort, 9600); // starts the serial communication
    myPort.bufferUntil('\n');
    
    textureMode(NORMAL);
    
    topside = loadImage("altimu-top.png");//Top Side
    downside = loadImage("altimu-bottom.png");//Botm side
    frontside = loadImage("altimu-edge.png"); //Wide side
    rightside = loadImage("altimu-edge.png");// Narrow side
  

}

void draw() {
    translate(width / 2, height / 2, 0);
    background(0);
    

    textSize(22);
    text("X: " + int(x) + "  Y: " + int(y) + "  Z:" + int(z), 0, 250);
    
    float roll = atan2(y, z) * 180.0 / PI;
    float pitch = atan2(-x, sqrt(y * y + z * z)) * 180.0 / PI;
    
    
    fill(#00FFFF);
    text("roll: " + int(roll) + " pitch: " + int(pitch) + "  Z:" + int(z), 0, 280);
   
    

   drawCube();

 

}

// Read data from the Serial Port
void serialEvent(Serial myPort) {
    // reads the data from the Serial Port up to the character '.' and puts it into the String variable "data".
    data = myPort.readStringUntil('\n');
    // if you got any bytes other than the linefeed:
    if (data != null) {
        data = trim(data);
        // split the string at "/"
        String items[] = split(data, ',');
        if (items.length > 1) {
            //--- Roll,Pitch in degrees
            
            aX = float(split(items[0], ':')[1]); //float(items[0]);
            aY = float(split(items[1], ':')[1]);
            aZ = float(split(items[2], ':')[1]);
            x = float(split(items[3], ':')[1]);
            y = float(split(items[4], ':')[1]);
            z = float(split(items[5], ':')[1]);
            
            magX = float(split(items[6], ':')[1]);
            magY = float(split(items[7], ':')[1]);
            magZ = float(split(items[8], ':')[1]);
            
            
            
            print(aX);
            print("\t");
            print(aY);
            print("\t");
            print(aZ);
            print("\t");
            
            print(x);
            print("\t");
            print(y);
            print("\t");
            print(z);
            print("\t");
            
            print(magX);
            print("\t");
            print(magY);
            print("\t");
            print(magZ);

            
            println("\t");
        }
    }
}

/*
From
* Texture Cube
* by Dave Bollinger.
I only Added multiple sides textured. AHV Ago 2013
*/

void topboard(PImage imag) {
  beginShape(QUADS);
  texture(imag);
  // -Y "top" face
  vertex(-25, -1, -13, 0, 0);
  vertex( 25, -1, -13, 1, 0);
  vertex( 25, -1,  13, 1, 1);
  vertex(-25, -1,  13, 0, 1);

  endShape();
}

void bottomboard(PImage imag) {
  beginShape(QUADS);
  texture(imag);

  // +Y "bottom" face
  vertex(-25,  1,  13, 0, 0);
  vertex( 25,  1,  13, 1, 0);
  vertex( 25,  1, -13, 1, 1);
  vertex(-25,  1, -13, 0, 1);
    
  endShape();
}


void sideboarda(PImage imag) {
  beginShape(QUADS);
  texture(imag);

  // +Z "front" face
  vertex(-25, -1,  13, 0, 0);
  vertex( 25, -1,  13, 1, 0);
  vertex( 25,  1,  13, 1, 1);
  vertex(-25,  1,  13, 0, 1);

  // -Z "back" face
  vertex( 25, -1, -13, 0, 0);
  vertex(-25, -1, -13, 1, 0);
  vertex(-25,  1, -13, 1, 1);
  vertex( 25,  1, -13, 0, 1);


  endShape();
}

void sideboardb(PImage imag) {
  beginShape(QUADS);
  texture(imag);

   // +X "right" face
  vertex( 25, -1,  13, 0, 0);
  vertex( 25, -1, -13, 1, 0);
  vertex( 25,  1, -13, 1, 1);
  vertex( 25,  1,  13, 0, 1);

  // -X "left" face
  vertex(-25, -1, -13, 0, 0);
  vertex(-25, -1,  13, 1, 0);
  vertex(-25,  1,  13, 1, 1);
  vertex(-25,  1, -13, 0, 1);

  endShape();
}

void drawCube() {  

    scale(10);
    
    float roll = atan2(y, z) * 180.0 / PI;
    float pitch = atan2(-x, sqrt(y * y + z * z)) * 180.0 / PI;
    float yaw = atan2(-magY,magX) * 180.0 / PI; 
  
    rotateX(radians(-roll));
    //rotateY(radians(yaw));
    rotateZ(radians(pitch));
  
    topboard(topside);
    bottomboard(downside);
    sideboarda(frontside);
    sideboardb(rightside);
}
