#include "node.h"
#include "math.h"

volatile bool Node::consensusCheck = false;
int Node::iter_consensus = 1;
int Node::iter = 1;

/*Node::Node(){
    d[0] = 0; // Duty-cycle set by the local controller
    d[1] = 0;
  };*/


Node::Node(const float _m, const float _b, int _addr, float _c, float _rho) {

  addr = _addr;
  m = _m;
  b = _b;
  c = _c;
  Lcon = L;
  rho = _rho;
  o = 0;
  iter = 1;
  L_desk = 0;
  L_ref = 0;
  d_avg[0] = 0; d_avg[1] = 0;
  d_best[0] = 0; d_avg[1] = 0;
  d_out[0] = 0; d_out[1] = 0;
  //d[0] = 0; d[1] = 0;
}


typedef struct NodeInfo {
  float m;
  float b;
  float k1;
  float k2;
  float L;
  float d1;
  float d2;
} NodeInfo;

float Node::readIlluminance() {

  float v = readVoltage();
  /*Serial.println(v);
    Serial.println(m);
    Serial.println(b);*/
  float R2 = R1 * (Vcc - v) / v;
  float illuminance = pow(10, (log10(R2) - b) / m);
  return illuminance;
}

float Node::readVoltage() {

  return 5 * analogRead(sensorPin) / 1023.0;
}

bool Node::setPWM(int PWM) {

  if (PWM < 0 || PWM > 255)
    return false;

  analogWrite(ledPin, PWM);
  return true;
}


float Node::extIlluminance() {

  float a = readIlluminance() - k[0] * d_best[0] - k[1] * d_out[1];
  if (a < 0) return 0;
  
  return a;
}

NodeInfo* Node::getNodeInfo() {

  /*NodeInfo* n;
  n->m = m;
  n->b = b;
  n->L = L_ref;

  n->k1 = k[0];
  n->k2 = k[1];
  n->d1 = d[0];
  n->d2 = d[1];

  return n;*/
}

void Node::setLux(float _L) {
  L = _L;
  //Serial.println(_L);
}

void Node::NodeSetup(){

  k = new float[ndev];
  d_best = new float[ndev];
  y = new float[ndev];
  d_avg = new float[ndev];
  d_best = new float[ndev];
  d_out = new float[ndev];
}

bool Node::calib() {

  if(iter == 1) NodeSetup();
  
  setPWM(0);
  float oj, lj;
  calib_flag = false;

  if (iter > ndev) return true;

  if (iter == addr) {

    o = readIlluminance();
    delay(500);

    setPWM(255);
    delay(500);
    float x_max = readIlluminance();
    k[addr-1] = (x_max - o) / 100;
    Serial.println(k[addr-1]);

    setPWM(0);
    //msgSync();
    delay(500);

    for(int j = 1; j <= ndev; j++){

      if(j == addr) continue;

      oj = readIlluminance();
      msgSync(j);
      delay(500);
      lj = readIlluminance();
      msgSync(j);

      k[j-1] = (lj - oj) / 100;
      Serial.println(k[j-1]);
    }

    msgBroadcast(2,"");
    delay(500);
    ++iter;
    calib();
  }
  else {

    msgSync(iter);
    setPWM(255);

    msgSync(iter);
    setPWM(0);

    while(!calib_flag);
    delay(500);
    ++iter;
    calib();
  }
}

const char* Node::getConsensusData() {

  return consensus_data.c_str();
}

int Node::msgConsensus(char id, int src_addr, String data_str) {

  /*switch (id){
    case 1:
      Serial.println("Consensus Flag -> T");
      consensus_flag = true;
      consensus_data = data_str;
      break;

    case 'L':
      d_best[src_addr-1] = atof(data_str.c_str());
      consensus_init = false;
      break;

    default:
      return -1;
      break;

    }*/
}


bool Node::checkFeasibility(float d1, float d2) {

  float tol = 0.001;

  if (d1 < 0 - tol || k[0]*d1 + k[1]*d2 < L - o - tol) {
    max_act = false;
    return false;
  }

  if (d1 > 100 + tol) {
    max_act = true;
    return false;
  }

  return true;
}

float Node::getCost(float d1, float d2) {

  return c * d1 + y[0] * (d1 - d_avg[0]) + y[1] * (d2 - d_avg[1]) + rho / 2 * (pow(d1 - d_avg[0], 2) + pow(d2 - d_avg[1], 2));
}

void Node::checkSolution(float d1_test, float d2_test) {

  if (checkFeasibility(d1_test, d2_test)) {

    float cost = getCost(d1_test, d2_test);
    if (cost < cost_best) {

      cost_best = cost;
      d_best[0] = d1_test;
      d_best[1] = d2_test;
    }
  }
}

void Node::getCopy() {

  char d21_str[8];
  char d22_str[8];

  //Serial.print("Received: ");
  //Serial.println(consensus_data.c_str());

  String aux_str = consensus_data;
  char* token = strtok((char*)aux_str.c_str(), "/");

  if (token != NULL) strcpy(d21_str, token);
  token = strtok(NULL, "/");
  if (token != NULL) strcpy(d22_str, token);

  float d_aux[2];
  d_aux[0] = atof(d21_str);
  d_aux[1] = atof(d22_str);

  /*Serial.println(d_aux[0]);
    Serial.println(d_aux[1]);*/

  d_out[0] = d_aux[0];
  d_out[1] = d_aux[1];

  consensus_data = "";

}

void Node::sendCopy(float d1, float d2) {

  String str = floatToString(d2) + "/" + floatToString(d1);
  msgSend(1, OTHER_ADDR, str);

  //Serial.print("Sent: ");
  //Serial.println(str.c_str());
}

void Node::initConsensus() {

  //k[0] = 2; k[1] = 0.5;
  consensusCheck = false;
  Lcon = L; // update lux reference
  d1_m = pow(k[0], 2) + pow(k[1], 2);
  d1_n = d1_m - pow(k[0], 2);
  o = extIlluminance(); // Update external illuminance estimate
  //Serial.println(o);
  y[0] = 0; // Lagrange Multipliers
  y[1] = 0;
  d_best[0] = 0;
  d_best[1] = 0;

  /*if (consensus_flag) {
    delay(200);
    d_out[1] = atof(consensus_data.c_str()); // get ref value from the other node
    msgSend(2,OTHER_ADDR,floatToString(Lcon/k[0]));
    } else {
    msgSend(2,OTHER_ADDR,floatToString(Lcon/k[0]));
    delay(200);
    d_out[1] = atof(consensus_data.c_str()); // get ref value from the other node
    }*/

  consensus_data = "";

  //d[0] = Lcon/k[0];
  //Serial.println(d[0]);
  //Serial.println(d[1]);
  d_avg[0] = (d_best[0] + d_out[0]) / 2;
  d_avg[1] = (d_best[1] + d_out[1]) / 2;

  //msgSync();
}

void Node::consensusAlgorithm() {

  L_desk = k[0] * d_best[0] + k[1] * d_out[1] + o; // Value to be sent to the local controller

  /*Serial.println(L_desk);
  Serial.println(readIlluminance());
  Serial.println(o);*/

  /*if(abs(o - extIlluminance()) > 10 && consensusCheck){
    msgSend(2,OTHER_ADDR,"");
    delay(5);
    consensusCheck = false;
    initConsensus();
    iter_consensus = 1;
  }*/

  /*float a = abs(extIlluminance() - o);
    Serial.println(a);*/

  if (iter_consensus > 20) {
    o = extIlluminance();
    consensusCheck = true;
    return;
  }

  //unsigned long init = micros();
  float rho_inv = 1.0 / rho;
  float d1, d2;
  cost_best = COST_BEST;
  d_best[0] = -1;
  d_best[1] = -1;

  float z1 = rho * d_avg[0] - c - y[0];
  float z2 = rho * d_avg[1] - y[1];

  // Unconstrained minimum
  d1 = rho_inv * z1;
  d2 = rho_inv * z2;
  //checkSolution(d1,d2);

  // Solution in the DLB (Dimming lower bound)
  d1 = 0;
  d2 = rho_inv * z2;
  checkSolution(d1, d2);

  // Solution in the DUB (Dimming Upper Bound)
  d1 = 100;
  d2 = rho_inv * z2;
  checkSolution(d1, d2);

  // Solution in the ILB (Illuminance Lower Bound)
  d1 = rho_inv * z1 - k[0] * (o - Lcon + rho_inv * (k[0] * z1 + k[1] * z2)) / d1_m;
  d2 = rho_inv * z2 - k[1] * (o - Lcon + rho_inv * (k[0] * z1 + k[1] * z2)) / d1_m;
  checkSolution(d1, d2);

  // Solution in the ILB & DLB
  d1 = 0;
  d2 = rho_inv * z2 - (k[1] * (o - Lcon) + rho_inv * k[1] * k[1] * z2) / d1_n;
  checkSolution(d1, d2);

  // Solution in the ILB & DUB
  d1 = 100;
  d2 = rho_inv * z2 - (k[1] * (o - Lcon) + 100 * k[1] * k[0] + rho_inv * k[1] * k[1] * z2) / d1_n;
  checkSolution(d1, d2);

  if (max_act) { // Request Illuminance value above LED actuation, power everthing at max
    d_best[0] = 100;
    d_best[1] = 100;
  }

  //msgSync();
  sendCopy(d_best[0], d_best[1]);
  delay(5);
  getCopy();

  // Average solutions from all nodes
  d_avg[0] = (d_best[0] + d_out[0]) / 2;
  d_avg[1] = (d_best[1] + d_out[1]) / 2;

  // Dual update -> Update the Lagrangian Multipliers
  y[0] += rho * (d_best[0] - d_avg[0]);
  y[1] += rho * (d_best[1] - d_avg[1]);

  ++iter_consensus;
  L_ref = k[0] * d_best[0];

  /*unsigned long finish = micros() - init;
    Serial.println(finish);*/

  /*Serial.println(d_avg[0]);
    Serial.println(d_avg[1]);*/

  /*if(iter_consensus == 20){
    Serial.println(L_desk);
    Serial.println(d_best[0]);
    Serial.println(d_best[1]);
    }*/

}

float Node::Windup(float u) {

  int wdp = 0;

  if (u > 100)
    wdp = 100 - u;
  else if (u < 0)
    wdp = -u;
  else
    wdp = 0;

  return wdp;
}

void Node::PID() {

  float y = readIlluminance();
  //Serial.print(y);
  //Serial.print("\t");
  //Serial.println(des_brightness);
  //Serial.print("\t");
  float e = L_ref - y;           // error in LUX
  //Serial.print(e);
  //Serial.print("\t");
  float p = k1 * e;                       // porpotional term
  float i = i_ant + k2 * (e + e_ant) + kwdp * Windup(u);    // integal term
  //float i = i_ant + (e    + e_ant);     // integal term
  if (abs(e) < 0.5)
    p = 0;

  float u = p + i + des_brightness / k[0];     // add feed-forward term

  u = constrain(u, 0, 100);

  setPWM(map(u, 0, 100, 0, 255));
  //Serial.println(u);
  i_ant = i;
  e_ant = e;
  y_ant = y;
}

void Node::init_PID(float ku, float T) {

  k1 = ku * 0.45;
  ki = 1.2 / T;
  k2 = k1 * ki * T / 2;

  i_ant = 0, e_ant = 0, y_ant = 0;//, usat = 0;
}

void Node::set_Brightness() {
  if (consensusCheck) {
    des_brightness = L_ref;
    //Serial.print("VALOR VINDO DO CONSENSUS = ");
  }
  else {
    des_brightness = L;
    //Serial.print("VALOR VINDO DO USER = ");
  }
  //Serial.println(des_brightness);
}

/*void Node::button() {
  if (digitalRead(buttonPin) == HIGH)
    return;

  for (int k = 0; k < 50; k++) {

    delay(1);
    if (digitalRead(buttonPin) == HIGH)
      return;
  }

  while (digitalRead(buttonPin) == LOW);
  Occu = !Occu;
  Serial.println(Occu);
  }*/

void Node::Read_serial(char v_read) {
  if (v_read == 'o') {
    Occu = !Occu;
    //iter_consensus = 1;
  }

  if(v_read == 'd'){
    delete[] k;
    delete[] d_best;
    delete[] y;
    delete[] d_avg;
    delete[] d_best;
    delete[] d_out;

    Serial.println("Bye");

    while(1);
  }
}

void Node::set_occupancy() {
  //button();
  if (Occu == 1) {
    setLux(UPPB); //Set desired illuminance to 150 lux
    //Serial.println("Lux set to Upper Bound");
  }
  else {
    setLux(LOWB); //Set desired illuminance to 50 lux
    //Serial.println("Lux set to Lower Bound");
  }
}

void Node::SendInfo(int counter) {
  if (counter % 10 == 0) {
    msgBroadcast('l', floatToString(L)); //Send Illuminance
    Serial.println(L);
    msgBroadcast('p', floatToString(map(u, 0, 100, 0, 255))); //Send PWM
    Serial.println(map(u, 0, 100, 0, 255));
    msgBroadcast('e', floatToString(o)); //Send External Illuminance
    Serial.println(o);
    msgBroadcast('o', floatToString(Occu)); //Send Occupancy
    Serial.println(Occu);
    msgBroadcast('b', floatToString(LOWB)); //Send Lower Bound
    Serial.println(LOWB);
    msgBroadcast('r', floatToString(L_ref)); //Send L
    Serial.println(L_ref);
  }
}

void Node::setupint_1() {

  const int freq_des = 100; //   set freq = 100Hz

  cli();  //stop interrupts
  TCCR1A = 0; // set entire TCCR1A register to 0
  TCCR1B = 0; // same for TCCR1B
  TCNT1 = 0; //initialize counter value to 0

  OCR1A = 2499;  // (must be <65536) (16*10^6) / (freq_des*64) - 1
  TCCR1B |= (1 << WGM12); // turn on CTC mode
  TCCR1B |= (1 << CS11) | (1 << CS10); // Set prescaler
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt

  sei(); //allow interrupts*/
}
