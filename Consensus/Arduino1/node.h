#ifndef _NODE_H
#define _NODE_H

#include <math.h>

using namespace std;

class Node{

    private:
        const int R1 = 10000;
        const int Vcc = 5;

    protected:

        float m;
        float b;
        float k[2];
        float o;
        float L_ref;
        float d[2];

    public:
        typedef struct NodeInfo NodeInfo;
        Node();
        NodeInfo* getNodeInfo();
};

#endif
