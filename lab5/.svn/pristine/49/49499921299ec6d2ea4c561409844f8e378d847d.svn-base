#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include "i8042.h"

int(kbc_subscribe_int)(uint8_t *bit_no);

int(kbc_unsubscribe_int)();

int(util_sys_inb)(int port, uint8_t *value);

void processKeyScancode(uint8_t scancode);

int readCommandByte(uint8_t command_byte);

int writeCommandByte(uint8_t command_byte);

int processCommandByte(uint8_t command_byte);

void enableInterrupts();

#endif
/* __KEYBOARD_H */
