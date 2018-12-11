#ifndef LED_CONTROLLER
#define LED_CONTROLLER
#include "node.h"

class led_controller : public Node{

    private:

        //float k1, ki, k2;
		float k1 = 0.021; //aten��o, depende da frequ�ncia de amostragem
    float k2 = 0.021;
    float ki = 17.5439;
		float T = 35.0877;
        float i_ant, e_ant, y_ant, usat;
        float des_brightness;
        void setupint_1();

    public:

        led_controller();

        void  PID();

        void init_PID(float ku, float T);

        void set_Brigthness();
};

#endif
