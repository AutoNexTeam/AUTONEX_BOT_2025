/*
     _   _   _ _____ ___  _   _ _______  __          ____  _    _   __        ______   ___    _____ _____ 
    / \ | | | |_   _/ _ \| \ | | ____\ \/ /         |  _ \/ |  | |  \ \      / /  _ \ / _ \  |  ___| ____|
   / _ \| | | | | || | | |  \| |  _|  \  /   _____  | |_) | |  | |   \ \ /\ / /| |_) | | | | | |_  |  _|  
  / ___ \ |_| | | || |_| | |\  | |___ /  \  |_____| |  _ <| |  | |    \ V  V / |  _ <| |_| | |  _| | |___ 
 /_/   \_\___/  |_| \___/|_| \_|_____/_/\_\         |_| \_\_|  | |     \_/\_/  |_| \_\\___/  |_|   |_____|


Reg.no-1425 

Team members:
SANJAI ARUNPRASHANA D
Vijayarayar.S
V.E.LOKESH

Coach:
KARTHIKEYAN P

*/

#include <Wire.h>
#include <NewPing.h>
#include <FastLED.h>
#include <ESP32Servo.h>
#include <Adafruit_TCS34725.h>
// ########### Declerations ############################################################################################################ //

// RGB Led
#define LED_PIN 15
#define NUM_LEDS 1
CRGB leds[NUM_LEDS];

// Colour Sensor
#define TCS3414CS_ADDRESS 0x29 //ColorSensor address 0x29

// INTEGRATIONTIME - 2.4 - 614 ms | GAIN - 1x, 4x, 16x, 64x.
Adafruit_TCS34725 tcs1 = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_4X); // Initializing ColorSensor 

int col_out;

// DC Motor
const int motorPin1 = 32; 
const int motorPin2 = 33; 

const int dc_chan1 = 2;
const int dc_chan2 = 3;
const int nslp = 13; 
const int frequency = 5000;


// Servo Motor
#define SERVO_PIN 27
Servo servo;

// Ultrasonic Sensors

#define FRONT_TRIGGER 12 
#define FRONT_ECHO  4  

#define FRONT1_TRIGGER 16
#define FRONT1_ECHO 14

#define FRONT2_TRIGGER 25 
#define FRONT2_ECHO  26  

#define BACK_TRIGGER 17
#define BACK_ECHO 19

#define LEFT_TRIGGER  2
#define LEFT_ECHO  23

#define RIGHT_TRIGGER 5
#define RIGHT_ECHO  18

#define MAX_DISTANCE 400

NewPing sonar1(FRONT_TRIGGER, FRONT_ECHO, MAX_DISTANCE); 
NewPing sonar5(FRONT1_TRIGGER, FRONT1_ECHO, MAX_DISTANCE); 
NewPing sonar6(FRONT2_TRIGGER, FRONT2_ECHO, MAX_DISTANCE); 

NewPing sonar2(BACK_TRIGGER, BACK_ECHO, MAX_DISTANCE); 
NewPing sonar3(LEFT_TRIGGER, LEFT_ECHO, MAX_DISTANCE);
NewPing sonar4(RIGHT_TRIGGER, RIGHT_ECHO, MAX_DISTANCE); 

// ########### Loop ############################################################################################################ //
int line_chk_count = 12;  // 3 Laps - 12 count, 1 Lap - 4 count.
int line_count = 0; // 12
//#---Bot Speeds---######################################################################
int normal_speed = 200;//pwm
int turn_speed = 220;//pwm
int turn_delay = 2000;//ms
//#######################################################################################
int fus_slow_speed = 200;//pwm
int fus_slow_dist = 130;//cm
//#---Servo Angles---####################################################################
int servo_center = 100;//deg
int left_turn_angle = servo_center - 20; //80 deg
int right_turn_angle = servo_center + 20;//120 deg
//####################################################################
const int blue_line_thold = 8000; //Blue Color Temp value
const int orange_line_thold = 3000; //Orange Color Temp value
//####################################################################
bool COLOR_LOCK = 1;
bool lt_st_count = 0;
bool rt_st_count = 0;

bool left_right_arc_turn = 1;
bool left_right_r_turn = 0;
//#########################################################################################//
#define DPDT_Push_Button_Pin 34

int col_val, start_col, colour_line;  // colour_line = 1 ->> Orange // colour_line = 3 ->> Blue
int f_us, f1_us, f2_us, b_us, l_us, r_us, fusa, far;

bool LOGIC_LOCK = 1; //1 True state.
bool DPDT_STATE = 0; //0 False state.
//#########################################################################################//

void loop() {
  
  DPDT_STATE = digitalRead(DPDT_Push_Button_Pin);
  //Serial.println("DPDT Button State : "+String(DPDT_STATE));

  US_Values(f_us, f1_us, f2_us, b_us, l_us, r_us);
  // Serial.println("F_US : " + String(f_us) + " | F1_US : " + String(f1_us) + " | F2_US : " + String(f2_us) + 
  //             " | B_US : " + String(b_us) + 
  //             " | L_US : " + String(l_us) + " | R_US : " + String(r_us));

  col_val = front_colour_sensor();
  //Serial.println("Color Value : " + String(col_val));

  color_line_fun(); // color line pickup

  
  if (DPDT_STATE) 
  { 
    if (LOGIC_LOCK) 
    { 
      if (col_val != 0)
      {
        color_logic_fun();
      }
      else if (col_val == 0)
      {
        side_us_logic_fun();
      }   
    }     
  }
  else
  {
    bot_shutdown();
  }

  if (line_count == line_chk_count) 
  {
    end_stop();
    bot_shutdown();
    LOGIC_LOCK = 0;
    line_count = 0; 
  }   

}

void color_line_fun()
{
  if(COLOR_LOCK == 1)
  {
    if(col_val == 1)
    {
      start_col = 1;
      colour_line = 1;
      COLOR_LOCK = 0;
    }
    else if(col_val == 3)
    {
      start_col = 3;
      colour_line = 3;
      COLOR_LOCK = 0;
    }
  }
}

void color_logic_fun()
{

   //# Orange Line & Right Turn Condition: #/////////////////////////////////////////////////////////////////
      if ((col_val == colour_line) && (col_val != 3))
      { 
        line_count++;
        Serial.println("Orange Line Count : " + String(line_count));

        if(left_right_arc_turn)
        {
          rgb_led(0, 0, 0);  
          delay(1);
          motor_forward(turn_speed);
          rgb_led(150, 0, 150);
          moveServoTo(right_turn_angle);
          delay(turn_delay);
          moveServoTo(servo_center);
          delay(1);
          rgb_led(0, 0, 0);
        }

        if(left_right_r_turn)
        {
          rgb_led(0, 0, 0);
          motor_forward(turn_speed);
          delay(1); 
          rgb_led(150, 0, 150);

          moveServoTo(right_turn_angle);
          delay(1500); 
          motor_backward(210);
          delay(1);      
          moveServoTo(left_turn_angle);
          delay(1500);
          moveServoTo(servo_center);
          delay(1);  
          motor_forward(200);
          delay(1); 
          rgb_led(0, 0, 0);
        }

      } 
      
      //# Blue Line & Left Turn Condition: #/////////////////////////////////////////////////////////////////
      if ((col_val == colour_line) && (col_val != 1))
      { 
        line_count++;
        Serial.println("Blue Line Count : " + String(line_count));

        if(left_right_arc_turn)
        {
          rgb_led(0, 0, 0);
          delay(1);
          motor_forward(turn_speed);
          rgb_led(150, 0, 150);
          moveServoTo(left_turn_angle);
          delay(turn_delay);
          moveServoTo(servo_center);
          rgb_led(0, 0, 0);
        }
        
        if(left_right_r_turn)
        {
          rgb_led(0, 0, 0);
          motor_forward(turn_speed);
          delay(1); 
          rgb_led(150, 0, 150);

          moveServoTo(left_turn_angle);
          delay(1500); 
          motor_backward(turn_speed);
          delay(1);      
          moveServoTo(right_turn_angle);
          delay(1500);
          moveServoTo(servo_center);
          delay(1);  
          motor_forward(200);
          delay(1); 
          rgb_led(0, 0, 0);
        }
        

        
      } 

}

void side_us_logic_fun()
{              
    if(f_us < fus_slow_dist){motor_forward(fus_slow_speed);}
    else{motor_forward(normal_speed);}

    if ((l_us < 30) && (l_us != 0))
    { 
        rgb_led(0, 0, 0);
        rgb_led(255, 0, 50); 
        moveServoTo(servo_center + 10);

        if(rt_st_count == 0)
        {
          if(lt_st_count == 0)
          {
            lt_st_count = 1;
          }
        }

    }
    if ((l_us > 30) && (r_us > 30) && (l_us != 0) && (r_us != 0)) 
    { 
        rgb_led(0, 0, 0);
        rgb_led(255, 255, 0);
        moveServoTo(servo_center); //100
    } 

    if ((r_us < 30) && (r_us != 0))
    {
        rgb_led(0, 0, 0);
        rgb_led(255, 0, 50); 
        moveServoTo(servo_center - 10);//90

        if(lt_st_count == 0)
        {
          if(rt_st_count == 0)
          {
            rt_st_count = 1;
          }
        }

    }
}

void bot_shutdown()
{
  motor_stop();
  moveServoTo(servo_center);
  rgb_led(0, 0, 0);
}

void left_stop()
{
  rgb_led(0, 0, 0);
  delay(1);
  rgb_led(255, 255, 255);
  
  if(left_right_arc_turn)
  {
    motor_forward(210);
    delay(1000);
  }
  moveServoTo(left_turn_angle);
  delay(1500);
  moveServoTo(right_turn_angle);
  delay(1500);
  moveServoTo(servo_center);
  delay(1000);
  
  rgb_led(0, 0, 0);
}

void right_stop()
{
  rgb_led(0, 0, 0);
  delay(1);
  rgb_led(255, 255, 255);

  if(left_right_arc_turn)
  {
    motor_forward(210);
    delay(1000);
  }
  moveServoTo(right_turn_angle);
  delay(1500);
  moveServoTo(left_turn_angle);
  delay(1500);
  moveServoTo(servo_center);
  delay(1000);

  rgb_led(0, 0, 0);

}

void end_stop()
{

  if(start_col == 3)
  {
      // Anti-clock wise:
      if(colour_line == 3)
      {
        if(lt_st_count == 1)
        {
          left_stop();
        }
        else if(rt_st_count == 1)
        {
          right_stop();
        }
      }

      if(colour_line == 1)
      {
        if(lt_st_count == 1)
        {
          right_stop();
        }
        else if(rt_st_count == 1)
        {
          left_stop();
        }
      }
  }
  
 ///////////////////////////////////////////////////////////////////
  if(start_col == 1)
  {
      // clock wise:
      if(colour_line == 3)
      {
        if(lt_st_count == 1)
        {
          right_stop();
        }
        else if(rt_st_count == 1)
        {
          left_stop();
        }
      }

      if(colour_line == 1)
      {
        if(lt_st_count == 1)
        {
          left_stop();
        }
        else if(rt_st_count == 1)
        {
          right_stop();
        }
      }
  }

}


// ########### Functions ############################################################################################################ //
// RGB Led Function
void rgb_led(int r, int g, int b)
{
  leds[0] = CRGB(r, g, b);
  FastLED.show();
}

// ColorSensor Function
int front_colour_sensor() {
  //TCA9548A(0);
  uint16_t r, g, b, c;
  tcs1.getRawData(&r, &g, &b, &c);
  uint16_t colorTemp = tcs1.calculateColorTemperature(r, g, b);
  //Serial.println("CS-1 Color Temp: "+ String(colorTemp));
  //uint16_t lux = tcs1.calculateLux(r, g, b);
  //Serial.println("Red: "+ String(r)+" Green: "+String(g)+" Blue: "+String(b)+" Clear: "+String(c));
  

  // Color Temp Condition
  if((0 < colorTemp) && (colorTemp < orange_line_thold) && (colorTemp != 0)) // For Orange Line
  {col_out = 1; } 

  else if((colorTemp > blue_line_thold) && (colorTemp != 0)) // For Blue Line 
  {col_out = 3;}
  
  else
  {col_out = 0;}

  return col_out;
}

// DC Motor Functions
void motor_forward(int speed) {
  ledcWrite(dc_chan1, speed);
  ledcWrite(dc_chan2, 0);
  //Serial.println("motor_forward");
}

void motor_backward(int speed) {
  ledcWrite(dc_chan1, 0);
  ledcWrite(dc_chan2, speed);
  //Serial.println("motor_backward");
}

void motor_stop() {
  ledcWrite(dc_chan1, 0);
  ledcWrite(dc_chan2, 0);
  //Serial.println("motor_stop");
}

// Servo Functions
void moveServoTo(int angle) {
  // Constrain the angle between 0 and 180 degrees
  angle = constrain(angle, 75, 125);
  servo.write(angle);
  //delay(15);
  //Serial.println("Servo Angle : "+String(angle));
}

// UltraSonic Function
void US_Values(int &f, int &f1, int &f2, int &b, int &l, int &r)
{
  unsigned int front_us = sonar1.ping_cm();
  unsigned int front1_us = sonar5.ping_cm();
  unsigned int front2_us = sonar6.ping_cm();
  unsigned int back_us = sonar2.ping_cm(); 
  unsigned int left_us = sonar3.ping_cm(); 
  unsigned int right_us = sonar4.ping_cm(); 

  f = front_us;
  f1 = front1_us;
  f2 = front2_us;
  b = back_us;
  l = left_us;
  r = right_us;
}

// ########### Setup ############################################################################################################ //
void setup() 
{
  Serial.begin(115200);

  //######### DPDT Setup #########//
  pinMode(DPDT_Push_Button_Pin, INPUT);

  //######### RGB Led Setup #########//
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  FastLED.clear();
  FastLED.show();

  //######### Colour Sensor Setup #########//
  analogReadResolution(12);       
  analogSetAttenuation(ADC_0db);
  
  //######### DC Motor Setup ###########//
  ledcSetup(dc_chan1, frequency, 8);
  ledcSetup(dc_chan2, frequency, 8);
  ledcAttachPin(motorPin1, dc_chan1);
  ledcAttachPin(motorPin2, dc_chan2);
  pinMode(nslp, OUTPUT);
  digitalWrite(nslp, HIGH);

  //######### Servo Motor Setup ###########//

  servo.attach(SERVO_PIN, 500, 2400);
  //initial servo angle
  moveServoTo(servo_center);
  delay(500);
  }
