import processing.serial.*;
import java.awt.event.KeyEvent;
import java.io.IOException;


String comPort = "COM8";
//final int VIEW_SIZE_X = 1024, VIEW_SIZE_Y = 768;
final int VIEW_SIZE_X = 800, VIEW_SIZE_Y = 600;

PImage topside, downside, frontside, rightside;

Serial myPort;
String data="";
float roll, pitch,yaw;

void setup() {
  size(VIEW_SIZE_X, VIEW_SIZE_Y, P3D);
  textureMode(NORMAL);
  fill(255);
  stroke(color(44,48,32));
  
  println("-->Serial COM Ports:");
  println(Serial.list());
  
  println("-->Connecting ", comPort);
  
  myPort = new Serial(this, comPort, 9600); // starts the serial communication
  myPort.bufferUntil('\n');
  
  
  topside = loadImage("altimu-top.png");//Top Side
  downside = loadImage("altimu-bottom.png");//Botm side
  frontside = loadImage("altimu-edge.png"); //Wide side
  rightside = loadImage("altimu-edge.png");// Narrow side
}

void draw() {
  background(#000000);
  
  drawCube();
}

// Read data from the Serial Port
void serialEvent (Serial myPort) { 
  // reads the data from the Serial Port up to the character '.' and puts it into the String variable "data".
  data = myPort.readStringUntil('\n');
  // if you got any bytes other than the linefeed:
  if (data != null) {
    data = trim(data);
    // split the string at "/"
    String items[] = split(data, '/');
    if (items.length > 1) {
      //--- Roll,Pitch in degrees
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
  vertex(-20, -1, -15, 0, 0);
  vertex( 20, -1, -15, 1, 0);
  vertex( 20, -1,  15, 1, 1);
  vertex(-20, -1,  15, 0, 1);

  endShape();
}

void botomboard(PImage imag) {
  beginShape(QUADS);
  texture(imag);

  // +Y "bottom" face
  vertex(-20,  1,  15, 0, 0);
  vertex( 20,  1,  15, 1, 0);
  vertex( 20,  1, -15, 1, 1);
  vertex(-20,  1, -15, 0, 1);
    
  endShape();
}


void sideboarda(PImage imag) {
  beginShape(QUADS);
  texture(imag);

  // +Z "front" face
  vertex(-20, -1,  15, 0, 0);
  vertex( 20, -1,  15, 1, 0);
  vertex( 20,  1,  15, 1, 1);
  vertex(-20,  1,  15, 0, 1);

  // -Z "back" face
  vertex( 20, -1, -15, 0, 0);
  vertex(-20, -1, -15, 1, 0);
  vertex(-20,  1, -15, 1, 1);
  vertex( 20,  1, -15, 0, 1);


  endShape();
}

void sideboardb(PImage imag) {
  beginShape(QUADS);
  texture(imag);

   // +X "right" face
  vertex( 20, -1,  15, 0, 0);
  vertex( 20, -1, -15, 1, 0);
  vertex( 20,  1, -15, 1, 1);
  vertex( 20,  1,  15, 0, 1);

  // -X "left" face
  vertex(-20, -1, -15, 0, 0);
  vertex(-20, -1,  15, 1, 0);
  vertex(-20,  1,  15, 1, 1);
  vertex(-20,  1, -15, 0, 1);

  endShape();
}




void drawCube() {  
  pushMatrix();
    translate(VIEW_SIZE_X/2, VIEW_SIZE_Y/2 + 50, 0);
    //scale(5,5,5);
    scale(10);
    
    // a demonstration of the following is at 
    // http://www.varesano.net/blog/fabio/ahrs-sensor-fusion-orientation-filter-3d-graphical-rotating-cube
    rotateZ(-pitch);
    rotateX(roll);
    rotateY(yaw);
    
  
    topboard(topside);
    botomboard(downside);
    sideboarda(frontside);
    sideboardb(rightside);
    
    
  popMatrix();
}

