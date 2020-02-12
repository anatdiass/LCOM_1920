#include <lcom/lcf.h>

#include <lcom/lab3.h>
#include <lcom/timer.h>

#include <stdbool.h>
#include <stdint.h>

#include "keyboard.h"
#include "i8042.h"
#include "i8254.h"

extern uint8_t scancode;
extern int counter;
extern int global_counter;

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
  uint8_t kbd_hookID=0;
  uint32_t irq_set = BIT(kbd_hookID);

  if(kbd_subscribe_int(&kbd_hookID)!=OK)
    return 1;


  while(scancode!=ESC_KEY) { /* You may want to use a different condition */
      /* Get a request message. */
      if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
          printf("driver_receive failed with: %d", r);
        continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
        switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE: /* hardware interrupt notification */				
                if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */
                  kbc_ih();
                  process_scancodes(scancode);
                }
                break;
            default:
                break; /* no other notifications expected: do nothing */	
        }
    } else { /* received a standard message, not a notification */
        /* no standard messages expected: do nothing */
    }
  }

  if(kbd_unsubscribe_int()!=OK)
    return 1;

  if(kbd_print_no_sysinb(counter)!=0)
    return 1;

  return 0;
}

int(kbd_test_poll)() {
  uint8_t status;

  while(scancode!=ESC_KEY){
    //Reads from status register
    util_sys_inb(STAT_REG, &status);
    if(status&OBF && !(status&AUX)){ //obf set and aux cleared
      util_sys_inb(OUT_BUF, &scancode); //reads from output buffer
      if((status&(PAR_ERR|TIME_ERR))==0)
        process_scancodes(scancode); //processes scancodes
    }
    tickdelay(micros_to_ticks(DELAY_US));
  }

  if(kbd_print_no_sysinb(counter)!=0)
    return 1;

  counter = 0;

  enableInterrupts();

  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {
  
  int ipc_status, r;
  message msg;
  uint8_t hookID = 2;
  uint32_t irq_timer = BIT(hookID);
  uint8_t kbd_hookID = 5;
  uint32_t irq_set = BIT(kbd_hookID);


  if(kbd_subscribe_int(&kbd_hookID)!=OK)
    return 1;

  if(timer_subscribe_int(&hookID)!=OK)
    return 1;

  while(scancode!=ESC_KEY && (global_counter/60)<n) { /* You may want to use a different condition */
      /* Get a request message. */
      if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
          printf("driver_receive failed with: %d", r);
        continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
        switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE: /* hardware interrupt notification */
                if(msg.m_notify.interrupts & irq_timer){
                  timer_int_handler();
                }				
                if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */
                  kbc_ih();
                  process_scancodes(scancode);
                  global_counter=0;
                }
                break;
            default:
                break; /* no other notifications expected: do nothing */	
        }
    } else { /* received a standard message, not a notification */
        /* no standard messages expected: do nothing */
    }
  }

  if(kbd_unsubscribe_int()!=OK)
    return 1;

  if(timer_unsubscribe_int()!=OK)
    return 1;

  if(kbd_print_no_sysinb(counter)!=0)
    return 1;

  counter=0;
  return 0;
}
