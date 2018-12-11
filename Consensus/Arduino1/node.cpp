#include "node.h"

typedef struct NodeInfo{
  float m;
  float b;
  float k1;
  float k2;
  float L;
  float d1;
  float d2;
} NodeInfo;

Node::Node(){
    
    d[0] = 0; // Duty-cycle set by the local controller
    d[1] = 0;
};

NodeInfo* Node::getNodeInfo(){
  
  NodeInfo* n;
  n->m = m;
  n->b = b;
  n->L = L_ref;
  
  n->k1 = k[0];
  n->k2 = k[1];
  n->d1 = d[0];
  n->d2 = d[1];

  return n;
}


