#ifndef __MOUSE_H
#define __MOUSE_H

#include "i8042.h"

int mouse_subscribe_int (uint8_t *bit_no);
int mouse_unsubscribe_int();
int util_sys_inb (int port, uint8_t *value);
void mouse_process_packets(struct packet *pp);
int write_command(uint8_t command);
int enableDataReporting();
int disableDataReporting();
void mouse_gesture_handler(struct mouse_ev *evt, uint8_t x_len, uint8_t tolerance);
struct mouse_ev* detectMouseEvent(struct packet *pp);

//State machine
enum state_t{
    INIT, 
    DRAW,   //Start drawing first line
    FIRSTLINE, //First line done
    DRAWSECOND,  //Start drawing second line 
    //COMP,        //Inverted V complete
};

#endif /* __MOUSE_H */
