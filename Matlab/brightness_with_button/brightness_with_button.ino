#include <math.h>
#include <EEPROM.h>

const int led1 = 11;
const int buttonPin = 4;
const int sensorPin = A0;

const int min_PWM = 0;
const int max_PWM = 255;

// Ref brightness to be used in testing (should be made auto of some sort)
float ref_brightness = 50;
float sim_brightness = 150;

// Set values from components
const float R1 = 10000;
float m;
float b;
const float C = pow(10,-6);
const int Vcc = 5;

int counter = 0;
int counter2 = 0;
volatile int flag = true;
volatile int flag2 = 0;
float g0;
int occup = 0;

// List all functions avaliable to use (per file)
// PID
void PID(int ref_brightness, int k);

// read_vars
float read_ldr(int r_sensorPin);
float illuminance_fun(float v);
void readVoltage(int k);

// print_vars
void PrintStep();

// interruptions
void setupint_1();

//gain
float gain();

//commands
void Read_serial();

// increasing the frequency of PWM signal
// mask bits that are not prescale
const byte mask = B11111000;
//fastest possible
int prescale = 1;

void setup() {
  
  EEPROM.get(1,m);
  EEPROM.get(5,b);
  
  TCCR2B = (TCCR2B & mask) | prescale; // Changing frequency of timer2

  Serial.begin(9600);
  Serial.println("<Arduino 1 is ready>");
  pinMode(buttonPin, INPUT);
  digitalWrite(buttonPin, HIGH);

  g0 = gain();
  
  analogWrite(led1, 0);
  delay(300);

  //setupint_1();
}

volatile bool flag_teste = true;

void loop() {

  if (flag2) {
    Read_serial();
    flag2 = 0;
  }
}
