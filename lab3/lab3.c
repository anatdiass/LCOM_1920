#include <lcom/lcf.h>
#include <lcom/lab3.h>
#include <lcom/timer.h>

#include <stdbool.h>
#include <stdint.h>

#include "keyboard.h"
#include "i8042.h"
#include "i8254.h"



extern uint8_t key_scancode;
extern int global_counter;
extern int counter;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {

int ipc_status, r;
  message msg;

  uint8_t kbd_hookID = 0;
  uint8_t irq_set = BIT(kbd_hookID);

  if (kbc_subscribe_int(&kbd_hookID) != OK) {
    printf("Keyboard controler subscription failed!\n");
    return 1;
  }

  while (key_scancode != ESC_KEY) { /*You may want to use a different condition*/
    /*Get a request message.*/
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /*received notification*/
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /*hardware interrupt notification*/
          if (msg.m_notify.interrupts & irq_set) { /*subscribed interrupt*/
            kbc_ih();
            
            if(key_scancode==0){
              printf("Invalid scancode\n");
            }
            else {
              processKeyScancode(key_scancode);
            }
          }
          break;
        default:
          break; /*no other notifications expected: do nothing*/
      }
    }
    else{ /*received a standard message, not a notification*/
      /*no standard messages expected: do nothing*/
    }
  }

  if(kbc_unsubscribe_int()==1){
    printf("kbc subscription failed\n");
    return 1;
  }
  if(kbd_print_no_sysinb(counter) != 0){
    printf("kbd_print_no_sysinb failed\n");
    return 1;
  }
  return 0;
}

int(kbd_test_poll)() {
  uint8_t stat;

  while(key_scancode != ESC_KEY){

    util_sys_inb(STAT_REG, &stat);

    if( (stat & OBF) && !(stat & AUX)){

      util_sys_inb(OUT_BUF, &key_scancode);

      if ( (stat &(PAR_ERR | TO_ERR)) == 0 ){
        processKeyScancode(key_scancode);
    }
  }
    tickdelay(micros_to_ticks(DELAY_US));

  }

  kbd_print_no_sysinb(counter);

  counter = 0;

  enableInterrupts();

  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {

  int ipc_status, r;
  message msg;

  uint8_t hookID = 0;
  uint8_t irq_set = BIT(hookID);

  uint8_t kbd_hookID = 2;
  uint8_t irq_kbd = BIT(kbd_hookID);


  if (kbc_subscribe_int(&kbd_hookID) != OK) {
    printf("Keyboard controler subscription failed!\n");
    return 1;
  }


  if (timer_subscribe_int(&hookID) == 1) {
    printf("subscription of timer failed\n");
    return 1;
  }

  while ((global_counter / 60 < n) && (key_scancode != ESC_KEY)) {
    /*Get a request message.*/
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /*received notification*/
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                             /*hardware interrupt notification*/
          if (msg.m_notify.interrupts & irq_set) { /*subscribed interrupt*/
            timer_int_handler();
          }
          if (msg.m_notify.interrupts & irq_kbd) { /* subscribed interrupt */
            /* process KBD interrupt request */
            kbc_ih();
            if(key_scancode==0){
              printf("Invalid scancode\n");
            }
            else {
              processKeyScancode(key_scancode);
              global_counter=0;
            }
          }
          break;
        default:
          break; /*no other notifications expected: do nothing*/
      }
    }
    else { /*received a standard message, not a notification*/ /*no standard messages expected: do nothing*/
    }
  }

  if(kbc_unsubscribe_int()==1){
    printf("kbc subscription failed\n");
    return 1;
  }

  if (timer_unsubscribe_int() == 1)
      return 1;

  if(kbd_print_no_sysinb(counter) != 0){
    printf("kbd_print_no_sysinb failed\n");
    return 1;
  }

  counter=0;
  return 0;
}
