#include <lcom/lcf.h>

#include <stdint.h>

#include "i8042.h"

int mouse_subscribe_int(uint8_t *bit_no);
int mouse_unsubscribe_int();
int util_sys_inb(int port, uint8_t *value);
void mouse_process_packets(struct packet *pp);
int enableDataReporting();
int disableDataReporting();
int write_command(uint8_t command);
int set_cw1();
int reset_cw();
int set_cw2();
int ih2();
