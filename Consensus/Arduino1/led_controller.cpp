#include "led_controller.h"
#include "math.h"

led_controller::led_controller(){

}

void led_controller::PID(){

    float y = readIlluminance();

    float e = des_brightness - y;           // error in LUX
    float p = k1 * e;                       // porpotional term
    //float i = i_ant + k2 * (e + e_ant);     // integal term
    float i = i_ant + (e + e_ant);     // integal term
    if (abs(e) < 0.5)
        p = 0;

    float u = (p + i) / k[0];
    float wdp = (u - usat) * 0.1;           // anti-windup term

    u = u + wdp + des_brightness / k[0];       // add feed-forward term
    usat = u;       

    u = constrain(u, 0, 100);

    analogWrite(ledPin, map(u, 0, 100, 0, 255));

    i_ant = i;
    e_ant = e;
    y_ant = y;
}

void led_controller::init_PID(float ku, float T){

    k1 = ku * 0.6;
    ki = 2 / T;
    k2 = k1 * ki * T / 2;

    i_ant = 0, e_ant = 0, y_ant = 0, usat = 0;
}

void led_controller::set_Brigthness(){
	if (consensusCheck)
		des_brightness = L_ref;
	else
		des_brightness = L;
}


void led_controller::setupint_1() {

  const int freq_des = 100; // set freq = 100Hz

  cli();  //stop interrupts
  TCCR1A = 0; // set entire TCCR1A register to 0
  TCCR1B = 0; // same for TCCR1B
  TCNT1 = 0; //initialize counter value to 0

  OCR1A = 19999;  // (16*10^6) / (freq_des*8) - 1
  TCCR1B |= (1 << WGM12); // turn on CTC mode
  TCCR1B |= (1 << CS11); // Set prescaler
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt

  sei(); //allow interrupts
}
