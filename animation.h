/******************************************** 
 *  animation.h
 *  
 *  Library for pomodoro timer arduino project
 *  Created by Matthew Grogan 2017
 ********************************************/

#ifndef animation_h
#define animation_h

#define SEGMENT_LENGTH 4

#define SEG_A 0x01
#define SEG_B 0x02
#define SEG_C 0x04
#define SEG_D 0x08
#define SEG_E 0x10
#define SEG_F 0x20
#define SEG_G 0x40
#define SEG_P 0x80

#define RIGHT SEG_B | SEG_C
#define MID   SEG_A | SEG_G | SEG_D
#define LEFT  SEG_F | SEG_E

class Pattern {
  private:
    uint8_t * _steps;
    uint8_t _n_steps;
    uint8_t _mask = 0x00;
    uint8_t _index = 0;
  public:
    void set_pattern(uint8_t *steps, uint8_t n_steps);
    void first();
    void next();
    bool is_done();
    uint8_t mask();
    uint8_t mask(uint8_t bitmap);
    uint8_t current_item();
};

class MatrixPattern {
  private:
    Pattern _digits[SEGMENT_LENGTH];
    int _index = 3;
    
  public:
    void set_pattern(uint8_t *steps, uint8_t n_steps);
    void first();
    void next();
    bool is_done();
    uint8_t mask(uint8_t digit, uint8_t bitmap);
    uint8_t digit();
};

#endif
