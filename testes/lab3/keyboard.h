#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <stdint.h>

#include "i8042.h"

int kbd_subscribe_int(uint8_t *bit_no);
int kbd_unsubscribe_int();
void process_scancodes(uint8_t scode);
void enableInterrupts();

#endif
