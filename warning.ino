#include <Wire.h>
#include <Filters.h>
#include "I2Cdev.h"
#include "MPU6050.h"
FilterOnePole XFHigh(HIGHPASS, 1), YFHigh(HIGHPASS, 1), ZFHigh(HIGHPASS, 1);

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

MPU6050 accelgyro;

 int16_t ax,ay,az;
 
 #define OUTPUT_READABLE_ACCELGYRO
 
void setup() {
   // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    // initialize serial communication
    // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
    // it's really up to you depending on your project)
    Serial.begin(74880);
    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();
    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
//led
  pinMode(12, OUTPUT);
  digitalWrite(12, LOW);
//siren
   pinMode(10, OUTPUT);
  digitalWrite(10, LOW);
//ışıldak
   pinMode(8, OUTPUT);
  digitalWrite(8, LOW);
  
  pinMode(A0, OUTPUT);
  pinMode(A1, INPUT);
  pinMode(A2, OUTPUT);
  digitalWrite(A0, LOW);
  digitalWrite(A2, LOW);
  Serial.print("basarili            1       ");
}

int Pwave_alarm_time, Pwave_alarm_going, Swave_alarm_time, Swave_alarm_going, alarm_counter, alarm_delay = 10;

void loop() {
 accelgyro.getAcceleration(&ax, &ay, &az);
 unsigned short trimpot_val;
  float xy_vector_mag, z_vector_mag, scale_factor;
  digitalWrite(A2, HIGH);
  trimpot_val = analogRead(A1);
  digitalWrite(A2, LOW);
  XFHigh.input(ay / 16384.0);
  YFHigh.input(ax / 16384.0);
  ZFHigh.input(az / 16384.0 - 1.0);
  scale_factor = pow(2, trimpot_val / 100.0);
  xy_vector_mag = sqrt(XFHigh.output() * XFHigh.output() + YFHigh.output() * YFHigh.output()) * scale_factor;
  z_vector_mag = abs(ZFHigh.output() * scale_factor);
  Serial.print("|XY| = "); Serial.print(xy_vector_mag); Serial.print(", |Z| = "); Serial.println(z_vector_mag);
Serial.print("basarili");
  if( alarm_delay ) {
    --alarm_delay;
  } else {
    if( xy_vector_mag >= 1 )
      ++Pwave_alarm_time;
    else if( Pwave_alarm_time > 0 )
      --Pwave_alarm_time;
    if( z_vector_mag >= 1 )
      ++Swave_alarm_time;
    else if( Swave_alarm_time > 0 )
      --Swave_alarm_time;
  }

  if( Pwave_alarm_time > 5 ) {
    if( !Pwave_alarm_going )
      alarm_counter = 0;
    Pwave_alarm_going = 80;
  } else if( Swave_alarm_time > 5 ) {
    Swave_alarm_going = 80;
  }
  if( Pwave_alarm_going || Swave_alarm_going ) {
    digitalWrite(12, alarm_counter < 10 || Swave_alarm_going ? HIGH : LOW);
    digitalWrite(10, alarm_counter < 10 || Swave_alarm_going ? HIGH : LOW);
     digitalWrite(8, HIGH);
    if( ++alarm_counter == 20 )
      alarm_counter = 0;
    if( Pwave_alarm_going )
      Pwave_alarm_going--;
    if( Swave_alarm_going )
      Swave_alarm_going--;
  } else {
    digitalWrite(12, LOW);
    digitalWrite(10, LOW);
    digitalWrite(8, LOW);
  }
   delay(40);
}
