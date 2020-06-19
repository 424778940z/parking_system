#include <Arduino.h>
#include <Wire.h>

#include <LiquidCrystal.h>
#include <HardwareSerial.h>
#include <Servo.h>
#include <string.h>

#define pin_RS A0
#define pin_EN A1
#define pin_d4 A2
#define pin_d5 A3
#define pin_d6 A4
#define pin_d7 A5
LiquidCrystal lcd(pin_RS,  pin_EN,  pin_d4,  pin_d5,  pin_d6,  pin_d7);

#define pin_sensor_in PA0
#warning TODO: change to a working button
#define pin_sensor_out PC13

#define pin_servo 3
Servo servo;

#define LP_LEN 6 //license plate text length without chinese.

#define CMD_GATE_OPEN       0xC0
#define CMD_GATE_CLOSE      0xC1
#define CMD_GATE_IGNORE     0xC2
#define CMD_DISPLAY_LP      0xC3

#define SIG_SENSOR_IN_PRESSED  0xD0
#define SIG_SENSOR_IN_RELEASED 0xD1
#define SIG_SENSOR_OUT_PRESSED  0xD2
#define SIG_SENSOR_OUT_RELEASED 0xD3

volatile uint8_t sensor_in_status; //1=closed 0=open;
volatile uint8_t sensor_out_status; //1=closed 0=open;

void gate_control(uint8_t cmd)
{
  switch (cmd)
  {
  case CMD_GATE_OPEN:
    for(int ang=0; ang<=90; ang++)
		{
			delay(4);
			servo.write(ang);
		}
    break;
  case CMD_GATE_CLOSE:
    for(int ang=90; ang>=0; ang--)
		{
			delay(4);
			servo.write(ang);
		}
    break;
  case CMD_GATE_IGNORE:
    //do nothing
    break;
  default:
    break;
  }
}

void lcd_wrapper(String msg, bool clr=false, uint8_t col=0, uint8_t row=0)
{
  if(clr)
    lcd.clear();
  lcd.setCursor(col,row);
  lcd.print(msg);
}

void setup()
{
  pinMode(LED_GREEN,OUTPUT);
  pinMode(LED_BLUE,OUTPUT);
  pinMode(LED_RED,OUTPUT);

  Serial.begin(115200);

  pinMode(pin_servo,OUTPUT);
  servo.attach(pin_servo);

  pinMode(pin_sensor_in,INPUT_PULLUP);
  pinMode(pin_sensor_out,INPUT_PULLUP);
  
  lcd.begin(16,2);

  lcd_wrapper("Initializing...",true);
  servo.write(90);
  delay(500);
  servo.write(0);
  uint8_t cache_sensor_in_status = digitalRead(pin_sensor_in);
  uint8_t cache_sensor_out_status = digitalRead(pin_sensor_out);
  lcd_wrapper("Parking Control",true);

  uint8_t serial_cmd;
  char lp_text[LP_LEN+1];

  //main loop
  while(true)
  {
    delay(50);
    digitalToggle(LED_RED);

    sensor_in_status = digitalRead(pin_sensor_in);
    sensor_out_status = digitalRead(pin_sensor_out);
    
    //check if sensor status changed, if so send signal.
    if(cache_sensor_in_status != sensor_in_status)
    {
      cache_sensor_in_status = sensor_in_status;
      digitalWrite(LED_GREEN, !sensor_in_status);
      if (sensor_in_status == HIGH)
      {
        Serial.write(SIG_SENSOR_IN_RELEASED);
        Serial.flush();
      }
      if (sensor_in_status == LOW)
      {
        Serial.write(SIG_SENSOR_IN_PRESSED);
        Serial.flush();
      }
    }
    if(cache_sensor_out_status != sensor_out_status)
    {
      cache_sensor_out_status = sensor_out_status;
      digitalWrite(LED_BLUE, !sensor_out_status);
      if (sensor_out_status == HIGH)
      {
        Serial.write(SIG_SENSOR_OUT_RELEASED);
        Serial.flush();
      }
      if (sensor_out_status == LOW)
      {
        Serial.write(SIG_SENSOR_OUT_PRESSED);
        Serial.flush();
      }
    }

    //wait for CMD.
    if(Serial.available() > 0)
    {
      serial_cmd = Serial.read();
      switch (serial_cmd)
      {
      case CMD_DISPLAY_LP:
        if(Serial.available() >= LP_LEN)
        {
          Serial.readBytes(lp_text, LP_LEN);
          lp_text[LP_LEN+0] = '\0'; // LP_LEN+0 means last element in the array
          lcd_wrapper("Parking Control",true);
          lcd_wrapper(String(lp_text),false,0,1);
        }
        break;
      case CMD_GATE_OPEN:
        gate_control(serial_cmd);
        lcd_wrapper("Parking Control",true);
        lcd_wrapper("Drive Ahead",false,0,1);
        break;
      case CMD_GATE_CLOSE:
        lcd_wrapper("Parking Control",true);
        lcd_wrapper("Stop Here",false,0,1);
        gate_control(serial_cmd);
        break;
      case CMD_GATE_IGNORE:
        lcd_wrapper("Parking Control",true);
        lcd_wrapper("Error...",false,0,1);
        gate_control(serial_cmd);
        break;
      default:
        break;
      }
    }
    
  }
}

void loop(){};