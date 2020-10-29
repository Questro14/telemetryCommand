#ifndef buttonz_h
#define buttonz_h

class Buttonz {

  private:
    unsigned char pin;
    unsigned long press_time;
    unsigned long time_stamp;

  public:
    unsigned char state;
    Buttonz(unsigned char sw_pin, unsigned long sw_delay);
    void init(void);
    void CheckStatus(void);

};

#endif
