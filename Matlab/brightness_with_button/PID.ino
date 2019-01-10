const float k1 = 0.01575;
const float k2 = 0.00945;
const float kwdp = 0.034;

float wdp; // anti-windup term
float i_ant = 0, e_ant = 0, y_ant = 0, u = 0, usat = 0;

void PID(int des_brightness, int k) {

  float y = illuminance_fun(read_ldr(sensorPin));
  float e = des_brightness - y;
  float p = k1 * e;
  float i = i_ant + k2 * (e + e_ant);

  if (abs(e) < 0.1)
    p = 0;

  u = (p + i) / g0;

  if (usat > 255)
    wdp = 255 - u;
  else if (usat < 0)
    wdp = -usat;
  else
    wdp = 0;

  u += wdp * kwdp / g0;  
  u += des_brightness / g0;

  usat = u;
  u = constrain(u, 0, 255);
  
  analogWrite(led1, u);

  i_ant = i;
  e_ant = e;
  y_ant = y;
}
