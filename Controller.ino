
#include <RH_ASK.h> //Library was changed so Rx pin is 9
#include <SPI.h>

RH_ASK driver;

void setup() {
  Serial.begin(9600) ;
  driver.init();
  pinMode(2, INPUT_PULLUP);
}

void loop() {
  // put your main code here, to run repeatedly:
  /*  Controller Values:
   *   *Note that these X and Y values can be off*
   *  LEFT:   X=0     Y=510 
   *  RIGHT:  X=1023  Y=510
   *  UP:     X=503   Y=0
   *  DOWN:   X=503   Y=1023
   *  STILL:  X=504   Y=510
   */
  uint16_t xValue = analogRead(A0);
  uint16_t yValue = analogRead(A1);
  uint8_t button = digitalRead(2);
  if(!button){
    const char *data = "B";
    Serial.println("BUTTON");
    driver.send((uint8_t *)data,1);
    delay(500);
  }
  else if(xValue < 100){
    const char *data = "L";
    Serial.println("LEFT");
    driver.send((uint8_t *)data,1);
    delay(500);
  }
  else if(yValue < 100){
    const char *data = "U";
    Serial.println("UP");
    driver.send((uint8_t *)data,1);
    delay(500);
  }
  else if(xValue > 900){
    const char *data = "R";
    Serial.println("RIGHT");
    driver.send((uint8_t *)data,1);
    delay(500);
  }
  else if(yValue > 900){
    const char *data = "D";
    Serial.println("DOWN");
    driver.send((uint8_t *)data,1);
    delay(500);
  }
}
