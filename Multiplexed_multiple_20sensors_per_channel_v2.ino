#include <Wire.h>
#include "Adafruit_MLX90393.h"
#define MUX_Address 0x70 // default TCA9548A Encoders address

Adafruit_MLX90393 sensor = Adafruit_MLX90393();
int Chan = 0;


// motor related parameters ===============================
#define A4998_dirpin 10 //direction  
#define A4998_steppin 9 //step
#define A4998_enablepin 11 //enable if low
bool Motor_stephigh = false;
bool Motor_forward = false;
bool Motor_reverse = false;
bool Motor_openclose = false;
long Motor_steps = 0;
int S_Minpin = 0;   //D0
boolean S_Min = false;
boolean S_Min_1 = false;
boolean S_Minpos = false;
// timing related parameters ===============================
int dt = 1;
int dtmicro = 4000;
int dtMotor = 0;  // integrated dt.  Every loop dtMotor = dtMotor + 1. if (dtMotor = dt) update motor, dtMotor = 0 
int dtSwitch = 0; // integrated dt.  Every loop dtSwitch = dtSwitch + 1. if (dtSwitch = 50) read switch status, dtSwitch = 0 

byte incomingByte;

void setup(void)
{
  Serial.begin(38400);

  /* Wait for serial on USB platforms. */
  while(!Serial) {
      delay(100);
  }
  //set pin modes
  pinMode(A4998_enablepin, OUTPUT);
  digitalWrite(A4998_enablepin, HIGH); //deactivate driver (LOW active)
  pinMode(A4998_dirpin, OUTPUT);
  digitalWrite(A4998_dirpin, HIGH); //deactivate driver
  pinMode(A4998_steppin, OUTPUT);
  digitalWrite(A4998_steppin, LOW);
  pinMode(S_Minpin, INPUT);
  
  // search for I2C devices ===========================================
  byte count = 0;
  Serial.println ("I2C scanner. Scanning ...");
  Wire.begin();
  for (byte i = 8; i < 120; i++)
  {
    Wire.beginTransmission (i);
    if (Wire.endTransmission () == 0)
      {
      Serial.print ("MUX Found at address: ");
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);
      Serial.println (")");
      count++;
      delay (1); 
      } // end of good response
  } // end of for loop search devices
  if (count == 0) {
    Serial.println("No I2C devices found");
    while (1);
  }
  
  Serial.println("Searching for Adafruit MLX90393 Sensors");
  // adress pins A0 and A1 at bottomside of sensorprint
  // 12: A1 = GND, A0 = GND, default adress
  // 13: A1 = GND, A0 = +V
  // 14: A1 = +v , A0 = iGND
  // 15: A1 = +V , A0 = +V


  for (int Channel = 0;  Channel <5; Channel++) {
    tcaselect(Channel);
    Serial.print("scanning MUX channel:");
    Serial.print("\t");
    Serial.println(Channel);
    
    if (sensor.begin(12))
    {
      Serial.println("Found a MLX90393 sensor at adress 12 (0xC)");
      count++;
    }
    if (sensor.begin(13))
    {
      Serial.println("Found a MLX90393 sensor at adress 13 (0xD)");
      count++;
    }
    if (sensor.begin(14))
    {
      Serial.println("Found a MLX90393 sensor at adress 14 (0xE)");
      count++;
    }
    if (sensor.begin(15))
    {
      Serial.println("Found a MLX90393 sensor at adress 15 (0xF)");
      count++;
    }
    if (count == 0)
    {
      Serial.println("No MLX90393 sensors found");
      // while (1); 
    }
    
 } //end for 
}

// Initialize I2C buses using TCA9548A I2C Multiplexer
void tcaselect(uint8_t i2c_bus) {
    if (i2c_bus > 7) return;
    Wire.beginTransmission(MUX_Address);
    Wire.write(1 << i2c_bus);
    Wire.endTransmission(); 
}


// continuous loop ==================================================
void loop(void){
  if (Serial.available() > 0) {
    incomingByte = Serial.read();
    if (incomingByte == 32) { // spacebar = stop
      digitalWrite(A4998_enablepin, HIGH); //deactivate driver (LOW active)
      Motor_forward = false;
      Motor_reverse = false;
      Motor_openclose = false;
    }
    
    if ((incomingByte == 43)||(incomingByte == 61))  { // +/=  increase speed
      //if (dt > 1) {
      //  dt = dt - 1;
      //} 
      dtmicro = dtmicro - 100; 
    }

    if ((incomingByte == 45)||(incomingByte == 95))  { // -/_  decrease speed
      //if (dt < 16) {
      //  dt = dt + 1;
      //}  
      dtmicro = dtmicro + 100; 
    }

    if ((incomingByte == 79)||(incomingByte == 111))  { // O/o move to min (close) position
      //if (!S_Min) {
        digitalWrite(A4998_dirpin, HIGH); //LOW or HIGH
        digitalWrite(A4998_enablepin, LOW); //activate driver
        Motor_reverse = true;
        Motor_forward = false;
        Motor_openclose = false;
      //} 
    }

    if ((incomingByte == 82)||(incomingByte == 114))  { // R/r if position = 0 make 4000 steps and return to 0
      //if (!S_Min) {
        digitalWrite(A4998_dirpin, HIGH); //LOW or HIGH
        digitalWrite(A4998_enablepin, LOW); //activate driver
        Motor_reverse = true;
        Motor_forward = false;
        Motor_openclose = true;
      //} 
    }

    if ((incomingByte == 67)||(incomingByte == 99))  { // C/c move to min (close) position
      if (!S_Min) {
        digitalWrite(A4998_dirpin, LOW); //LOW or HIGH
        digitalWrite(A4998_enablepin, LOW); //activate driver
        Motor_reverse = false;
        Motor_forward = true;
        Motor_openclose = false;
      } 
    }
    
  } // end if serial.available  
  
  dtMotor++;
  dtSwitch++;
  if(dtMotor >= dt) {
    dtMotor = 0;
    if (Motor_reverse or Motor_forward) { 
      if (Motor_stephigh){
        digitalWrite(A4998_steppin, HIGH);
      } else {  
        digitalWrite(A4998_steppin, LOW);
      } //end if;
      Motor_stephigh = !Motor_stephigh;
    }
    if (Motor_forward) {
      Motor_steps--;
    }
    if (Motor_reverse) {
      Motor_steps++;
    }

    if (Motor_openclose){
      if (Motor_steps >= 100) {
        digitalWrite(A4998_dirpin, LOW); //LOW or HIGH
        Motor_forward = true;
        Motor_reverse = false;   
      }
    }
  } //end if >=dt
  S_Min_1= S_Min;
  S_Min = !digitalRead(S_Minpin);   // D8 min switch
  if (S_Min && !S_Min_1){
    if (Motor_forward){
      digitalWrite(A4998_enablepin, HIGH); //deactivate driver (LOW active)
      Motor_forward = false;
      Motor_reverse = false;
      Motor_steps = 0;
      Motor_openclose = false;
    }
  }
  
  tcaselect(Chan);
    
    float x, y, z;
    if (sensor.begin(12))
    {
      if(sensor.readData(&x, &y, &z)) {
          Serial.print(10 * Chan);
          Serial.print("\t");
          Serial.print(x, 1);
          Serial.print("\t");
          Serial.print(y, 1);
          Serial.print("\t"); 
          Serial.print(z, 1);
          Serial.print("\t"); 
          Serial.println(Motor_steps);
      }
    }

      if (sensor.begin(13))
    {
      if(sensor.readData(&x, &y, &z)) {
          Serial.print((10 * Chan)+ 1);
          Serial.print("\t");
          Serial.print(x, 1);
          Serial.print("\t");
          Serial.print(y, 1);
          Serial.print("\t"); 
          Serial.print(z, 1);
          Serial.print("\t"); 
          Serial.println(Motor_steps);
      }
    }
    if (sensor.begin(14))
    {
      if(sensor.readData(&x, &y, &z)) {
          Serial.print((10 * Chan)+ 2);
          Serial.print("\t");
          Serial.print(x, 1);
          Serial.print("\t");
          Serial.print(y, 1);
          Serial.print("\t"); 
          Serial.print(z, 1); 
          Serial.print("\t"); 
          Serial.println(Motor_steps);   
      }
    }
    if (sensor.begin(15))
    {
      if(sensor.readData(&x, &y, &z)) {
          Serial.print((10 * Chan)+ 3);
          Serial.print("\t");
          Serial.print(x, 1);
          Serial.print("\t");
          Serial.print(y, 1);
          Serial.print("\t"); 
          Serial.print(z, 1);
          Serial.print("\t"); 
          Serial.println(Motor_steps);
      }
    }      
    Chan = Chan + 1;
    if (Chan > 4) {
      Chan = 0;
    }
    delay(20);
}
