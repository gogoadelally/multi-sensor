#include <Adafruit_MLX90393.h>

#include <Wire.h>
#include "Adafruit_MLX90393.h"

Adafruit_MLX90393 sensor = Adafruit_MLX90393();

void setup(void)
{
  Serial.begin(9600);

  /* Wait for serial on USB platforms. */
  while(!Serial) {
      delay(10);
  }
  // search for I2C devices ===========================================
  byte count = 0;
  Serial.println ("I2C scanner. Scanning ...");
  Wire.begin();
  for (byte i = 8; i < 120; i++)
  {
    Wire.beginTransmission (i);
    if (Wire.endTransmission () == 0)
      {
      Serial.print ("Found address: ");
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

  count = 0;
  Serial.println("Searching for Adafruit MLX90393 Sensors");
  // adress pins A0 and A1 at bottomside of sensorprint
  // 12: A1 = GND, A0 = GND, default adress
  // 13: A1 = GND, A0 = +V
  // 14: A1 = +v , A0 = GND
  // 15: A1 = +V , A0 = +V
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
    while (1); 
  }
  else
  {
    Serial.print("ID");
    Serial.print("\t");  // "\t" = tab
    Serial.print("X");
    Serial.print("\t");
    Serial.print("Y");
    Serial.print("\t"); 
    Serial.println("Z");
  }
}


// continuous loop ==================================================
void loop(void)
{
    float x, y, z;
    if (sensor.begin(12))
    {
      if(sensor.readData(&x, &y, &z)) {
          Serial.print("12");
          Serial.print("\t");
          Serial.print(x, 4);
          Serial.print("\t");
          Serial.print(y, 4);
          Serial.print("\t"); 
          Serial.println(z, 4);
      }
    }

      if (sensor.begin(13))
    {
      if(sensor.readData(&x, &y, &z)) {
          Serial.print("13");
          Serial.print("\t");
          Serial.print(x, 4);
          Serial.print("\t");
          Serial.print(y, 4);
          Serial.print("\t"); 
          Serial.println(z, 4);
      }
    }
    if (sensor.begin(14))
    {
      if(sensor.readData(&x, &y, &z)) {
          Serial.print("14");
          Serial.print("\t");
          Serial.print(x, 4);
          Serial.print("\t");
          Serial.print(y, 4);
          Serial.print("\t"); 
          Serial.println(z, 4);    
      }
    }
    if (sensor.begin(15))
    {
      if(sensor.readData(&x, &y, &z)) {
          Serial.print("15");
          Serial.print("\t");
          Serial.print(x, 4);
          Serial.print("\t");
          Serial.print(y, 4);
          Serial.print("\t"); 
          Serial.println(z, 4);
      }
    }      

    //delay(500);
   delay(1500);
}
