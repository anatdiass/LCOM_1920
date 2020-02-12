#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

//global variables
int global_counter = 0;
static int hook_ID;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {

  uint8_t st;
  uint8_t lsb, msb;
  uint32_t frequency = TIMER_FREQ / freq; //frequency
  

  if(timer!=0 && timer !=1 && timer!=2)
    return 1;
  
  //2¹⁶ = 65536
  if(frequency>=65536)
    return 1;

  //gets timer configuration
  if(timer_get_conf(timer, &st) ==1)
    return 1;

  //keep 4 lsb
  st &= 0x0F;

  //get lsb and msb
  util_get_LSB(frequency, &lsb);
  util_get_MSB(frequency, &msb);

  switch(timer){
    case 0:
      st = st | TIMER_LSB_MSB | TIMER_SEL0;
      if(sys_outb(TIMER_CTRL, st)!=OK || sys_outb(TIMER_0, lsb)!=OK || sys_outb(TIMER_0, msb)!=OK)
        return 1;
      break;
    case 1:
      st = st | TIMER_LSB_MSB | TIMER_SEL1;
      if(sys_outb(TIMER_CTRL, st)!=OK || sys_outb(TIMER_1, lsb)!=OK || sys_outb(TIMER_1, msb)!=OK)
        return 1;
      break;
    case 2:
      st = st | TIMER_LSB_MSB | TIMER_SEL2;
      if(sys_outb(TIMER_CTRL, st)!=OK || sys_outb(TIMER_2, lsb)!=OK || sys_outb(TIMER_2, msb)!=OK)
        return 1;
      break;
    default:
      return 1;
      break; 
  }
  return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {

  hook_ID = *bit_no;
  if(sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_ID)!=OK)
    return 1;
  return 0;
}

int (timer_unsubscribe_int)() {

  if(sys_irqrmpolicy(&hook_ID)!=OK)
    return 1;
  return 0;
}

void (timer_int_handler)() {
  global_counter++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {

  if(timer!=0 && timer != 1 && timer != 2)
    return 1;
  
  uint8_t control_word; 
  control_word = TIMER_RB_CMD | TIMER_RB_SEL(timer) | TIMER_RB_COUNT_;

  //writes to the control register
  sys_outb(TIMER_CTRL, control_word);
  //reads selected timer configuration
  util_sys_inb(TIMER_0 + timer, st);
  
  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {

  //union to store information to display
  union timer_status_field_val val;
  
  uint8_t mode_cnt = (st&(BIT(1) | BIT(2) | BIT(3)));

  switch(field){
    case tsf_all:
      val.byte = st;
      break;
    
    case tsf_initial:
      if((st & TIMER_LSB_MSB) == TIMER_LSB)
        val.in_mode = LSB_only;
      else if((st& TIMER_LSB_MSB) == TIMER_MSB)
        val.in_mode = MSB_only;
      else if((st&TIMER_LSB_MSB) == TIMER_LSB_MSB)
        val.in_mode = MSB_after_LSB;
      else
        val.in_mode = INVAL_val;
      break;

    case tsf_mode:
      if(mode_cnt == 0)
        val.count_mode = 0;
      else if(mode_cnt == BIT(1))
        val.count_mode = 1;
      else if(mode_cnt == BIT(2) || mode_cnt == (BIT(2)|BIT(3)))
        val.count_mode = 2;
      else if(mode_cnt == (BIT(1)|BIT(2)) || mode_cnt == (BIT(1) | BIT(2) | BIT(3)))
        val.count_mode = 3;
      else if(mode_cnt == BIT(3))
        val.count_mode = 4;
      else if(mode_cnt == (BIT(1)|BIT(3)))
        val.count_mode = 5;
      break;
    case tsf_base:
      val.bcd = st & (TIMER_BCD|TIMER_BIN);
      break;

    default:
      break;
  }

  timer_print_config(timer, field, val);
  return 0;
}
