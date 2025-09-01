/*
     _   _   _ _____ ___  _   _ _______  __          ____  ____     _   __        ______   ___    _____ _____ 
    / \ | | | |_   _/ _ \| \ | | ____\ \/ /         |  _ \|___ \   | |  \ \      / /  _ \ / _ \  |  ___| ____|
   / _ \| | | | | || | | |  \| |  _|  \  /   _____  | |_) | __) |  | |   \ \ /\ / /| |_) | | | | | |_  |  _|  
  / ___ \ |_| | | || |_| | |\  | |___ /  \  |_____| |  _ < / __/   | |    \ V  V / |  _ <| |_| | |  _| | |___ 
 /_/   \_\___/  |_| \___/|_| \_|_____/_/\_\         |_| \_\_____|  | |     \_/\_/  |_| \_\\___/  |_|   |_____|
                                                                   |_|                                        

Reg.no-1425 

Team members:
SANJAI ARUNPRASHANA D
Vijayarayar.S
V.E.LOKESH

Coach:
KARTHIKEYAN P

*/

#include <Wire.h>
#include <Adafruit_TCS34725.h>
#include <ESP32Servo.h>
#include <NewPing.h>
#include <FastLED.h>

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

//#########################################################################################//
int line_chk_count = 12;  // 3 Laps - 12 count, 1 Lap - 4 count.
int line_count = 0;// 12

int normal_speed = 210;//pwm
int turn_speed = 230;//pwm
int turn_delay = 2000;//ms

const int blue_line_thold = 8000; //Blue Color Temp value
const int orange_line_thold = 3000; //Orange Color Temp value

bool left_right_arc_turn = 1;
bool left_right_r_turn = 0;

int fus_box_dist = 30;
//#---Servo Angles---###################################################################
int servo_center = 100;//deg
int left_turn_angle = servo_center - 20; //80 deg
int right_turn_angle = servo_center + 20;//120 deg
//#########################################################################################//
bool lt_st_count = 0;
bool rt_st_count = 0;
int rbox_count = 0;
int gbox_count = 0;

#define DPDT_Push_Button_Pin 34

int col_val, start_col, colour_line; // colour_line = 1 ->> Orange // colour_line = 3 ->> Blue
int f_us, f1_us, f2_us, b_us, l_us, r_us, fusa, far;

bool COLOR_LOCK = 1;//1 Initial True state.
bool LOGIC_LOCK = 1;//1 Initial True state.
bool DPDT_STATE = 0;//0 Initial False state.
//#########################################################################################//
char ab;
int serial_count;
bool bd_lock = 1;

const int bufferSize = 1;  
char abd[bufferSize];  

int abdRead;
char red_value = '1';
char green_value = '2';
//#########################################################################################//

// ########### Functions ############################################################################################################ //
// RGB Led Function
void rgb_led(int r, int g, int b)
{
  leds[0] = CRGB(r, g, b);
  FastLED.show();
}

// ColorSensor Function

int get_CS_Values() {
  //TCA9548A(0);
  uint16_t r, g, b, c;
  tcs1.getRawData(&r, &g, &b, &c);
  uint16_t colorTemp = tcs1.calculateColorTemperature(r, g, b);
  //Serial.println("CS-1 Color Temp: "+ String(colorTemp));

  // Color Temp Condition
  if((0 < colorTemp) && (colorTemp < orange_line_thold) && (colorTemp != 0))
  {col_out = 1; } 

  else if((colorTemp > blue_line_thold) && (colorTemp != 0))
  {col_out = 3;}
  
  else
  {col_out = 0;}

  return col_out;
}


// DC Motor Functions
void motor_forward(int speed) {
  ledcWrite(5, speed);
  ledcWrite(6, 0);
  //Serial.println("motor_forward");
}

void motor_backward(int speed) {
  ledcWrite(5, 0);
  ledcWrite(6, speed);
  //Serial.println("motor_backward");
}

void motor_stop() {
  ledcWrite(5, 0);
  ledcWrite(6, 0);
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

void get_US_Values(int &f, int &f1, int &f2, int &b, int &l, int &r)
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



// ########### Loop ############################################################################################################ //

//#########################################################################################//

void loop() 
{
  
  DPDT_STATE = digitalRead(DPDT_Push_Button_Pin);
  //Serial.println("DPDT Button State : "+String(DPDT_STATE));

  get_US_Values(f_us, f1_us, f2_us, b_us, l_us, r_us);
  //US_Values(f_us, b_us, l_us, r_us);
  // Serial.println("F_US : " + String(f_us) + " | F1_US : " + String(f1_us) + " | F2_US : " + String(f2_us) + 
  //             " | B_US : " + String(b_us) + 
  //             " | L_US : " + String(l_us) + " | R_US : " + String(r_us));

  far = fus_array();

  col_val = get_CS_Values();
  //Serial.println("Color Value : " + String(col_val));

  color_line_fun(); // color line pickup

  
  if (DPDT_STATE == 1) 
  { 
    if (LOGIC_LOCK == 1) 
    { 
      box_detection();

      if (col_val != 0)
      {
        color_logic_fun();
      }
      else if (col_val == 0)
      {
        side_us_logic_fun();
      }   

    }
      
  }else{
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
          rgb_led(150, 0, 150);

          moveServoTo(right_turn_angle);
          delay(1500); 
          motor_backward(turn_speed);
          delay(1);      
          moveServoTo(left_turn_angle);
          delay(1500);
          moveServoTo(servo_center);
          delay(1);  
          motor_forward(210);
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
            rgb_led(150, 0, 150);

            moveServoTo(left_turn_angle);
            delay(1500); 
            motor_backward(turn_speed);
            delay(1);      
            moveServoTo(right_turn_angle);
            delay(1500);
            moveServoTo(servo_center);
            delay(1);  
            motor_forward(210);
            rgb_led(0, 0, 0);

        }
        
      } 

}


void side_us_logic_fun()
{              
    motor_forward(normal_speed);

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

void box_detection()
{
  motor_forward(normal_speed);
   // Check if data available on serial port
    if((far == 1) && (Serial.available() > 0))  
    { 
      ab = Serial.read();
      serial_count++;
    }
    else
    {
      ab = 'n';
      buffer_limit();  //[*]
    }

    if((ab == '1') && (far == 1)) // Red Box
    {
      rgb_led(0, 0, 0); 
      rgb_led(255, 0, 0);
      rbox_count++;

        motor_backward(220);
        delay(1500);
        motor_forward(220);
        moveServoTo(right_turn_angle); // Right Turn
        delay(2000);
        moveServoTo(left_turn_angle);  // Left Turn
        delay(2000);
        moveServoTo(servo_center);
        rgb_led(0, 0, 0);

        //Event - 1
        // moveServoTo(right_turn_angle); 
        // delay(1500);
        // moveServoTo(left_turn_angle); 
        // delay(1500);
        // moveServoTo(servo_center);

     

      buffer_limit(); //[*]
    } 

    if((ab == '2') && (far == 1)) // Green Box
    {
      rgb_led(0, 0, 0); 
      rgb_led(0, 255, 0); 
      gbox_count++;

      motor_backward(220);
      delay(1500);
      motor_forward(220);
      moveServoTo(left_turn_angle);  // Left Turn
      delay(2000);
      moveServoTo(right_turn_angle); // Right Turn
      delay(2000);
      moveServoTo(servo_center);

      // moveServoTo(left_turn_angle); 
      // delay(1500);
      // moveServoTo(right_turn_angle); 
      // delay(1500);
      // moveServoTo(servo_center);

      rgb_led(0, 0, 0); 

      buffer_limit(); //[*]
    }
            
}

int fus_array()
{
  if(((f_us <= fus_box_dist) && (f_us != 0)) || 
    ((f1_us <= fus_box_dist) && (f1_us != 0)) || 
    ((f2_us <= fus_box_dist) && (f2_us != 0)) ) { fusa = 1; }

  else{fusa = 0;}

  return fusa;
}

void buffer_limit()
{
  while(Serial.available() > 0)
  {
    abdRead = Serial.readBytesUntil(red_value, abd, bufferSize);
    abdRead = Serial.readBytesUntil(green_value, abd, bufferSize);
    
    abd[abdRead] = '\0';
  }

}

void left_stop()
{
  rgb_led(0, 0, 0);
  delay(1);
  rgb_led(255, 255, 255);
  
  motor_forward(215);
  delay(1000);
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

  motor_forward(215);
  delay(1000);
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

// ########### Setup ############################################################################################################ //
void setup() {
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
  ledcSetup(5, frequency, 8);
  ledcSetup(6, frequency, 8);
  ledcAttachPin(motorPin1, 5);
  ledcAttachPin(motorPin2, 6);
  pinMode(nslp, OUTPUT);
  digitalWrite(nslp, HIGH);

  //######### Servo Motor Setup ###########//

  servo.attach(SERVO_PIN, 500, 2400);
  //initial servo angle
  moveServoTo(servo_center);
  delay(500);
  }



