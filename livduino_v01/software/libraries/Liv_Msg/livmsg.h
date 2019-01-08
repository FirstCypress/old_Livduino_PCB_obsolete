/* alfredc333
 * 2017
 * MIT License
 */

//structure for passing LiVduino messages over serial between Arduino Nano and NodeMCU

#ifndef LIVMSG_h
#define LIVMSG_h

const byte SONOFF_ON  = 200;
const byte SONOFF_OFF = 201;
const byte TS_REPORT  = 202;

typedef struct __attribute__ ((packed)) livmsg {
  byte op;
  float t;
  float h;
  float l;
} livmsg;

class emptyClass
{
  public:
    emptyClass(int a);
  private:
    int _a;
};


#endif
