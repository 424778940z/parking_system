#include <Arduino.h>
#include <Wire.h>

#include <LiquidCrystal.h>
#include <HardwareSerial.h>
#include <Servo.h>
#include <string.h>

#define pin_RS 8
#define pin_EN 9
#define pin_d4 4
#define pin_d5 5
#define pin_d6 6
#define pin_d7 7
#define pin_BL 10
LiquidCrystal lcd( pin_RS,  pin_EN,  pin_d4,  pin_d5,  pin_d6,  pin_d7);

#define pin_servo 3
#define pin_sensor 2
Servo servo;

#define LP_LEN 5 //license plate text length without chinese.

#define CMD_GATE_OPEN       0xC0
#define CMD_GATE_CLOSE      0xC1
#define CMD_GATE_IGNORE     0xC2
#define CMD_DISPLAY_LP      0xC3

#define SIG_SENSOR_PRESSED  0xD0
#define SIG_SENSOR_RELEASED 0xD1

uint8_t sensor_status; //1=closed 0=open;

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

void int_sensor()
{
  sensor_status = digitalRead(pin_sensor);
  delayMicroseconds(100); //debounce
}

void lcd_wrapper(String msg, bool clr=false, uint8_t col=0, uint8_t row=0)
{
  if(clr)
    lcd.clear();
  lcd.setCursor(col,row);
  lcd.print(msg);
}

int main()
{
  Serial.begin(115200);

  pinMode(pin_servo,OUTPUT);
  servo.attach(pin_servo);

  pinMode(pin_sensor,INPUT);
  attachInterrupt(pin_sensor,int_sensor,CHANGE);

  lcd.begin(16,2);

  lcd_wrapper("Initializing...",true);
  servo.write(90);
  delay(500);
  servo.write(0);
  uint8_t cache_sensor_status = digitalRead(pin_sensor);
  lcd_wrapper("Parking Control",true);

  uint8_t serial_cmd;
  char lp_text[LP_LEN+1];

  //main loop
  while(true)
  {
    //check if sensor status changed, if so send signal.
    if(cache_sensor_status != sensor_status)
    {
      cache_sensor_status = sensor_status;
      if (cache_sensor_status == HIGH)
      {
        Serial.write(SIG_SENSOR_PRESSED);
      }
      if (cache_sensor_status == LOW)
      {
        Serial.write(SIG_SENSOR_RELEASED);
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
          lp_text[LP_LEN+1] = '\0';
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