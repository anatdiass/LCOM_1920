// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you

#include "i8042.h"
#include "mouse.h"
#include "timer.h"

extern uint8_t byte;
extern unsigned int global_counter;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int (mouse_test_packet)(uint32_t cnt) {

  int ipc_status, r;
  message msg;

  struct packet pp;

  uint8_t mouse_hookID = 0;
  uint32_t irq_set = BIT(mouse_hookID);

  if (mouse_subscribe_int(&mouse_hookID) == 1) {
    printf("Mouse subscription failed!\n");
    return 1;
  }

  enableDataReporting();

  uint32_t printed_packets=0;
  unsigned int read_bytes=0;

  while (printed_packets<cnt) {
    /*Get a request message.*/
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /*received notification*/
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /*hardware interrupt notification*/
          if (msg.m_notify.interrupts & irq_set) { /*subscribed interrupt*/

            mouse_ih();

            if((read_bytes==0) && ((byte&BIT(3))==0)){
              continue;
            }
            else{
              if(read_bytes==0 || read_bytes==1 || read_bytes==2){
                pp.bytes[read_bytes]=byte;
                read_bytes++;
              }
              if(read_bytes==3){
                mouse_process_packets(&pp);
                mouse_print_packet(&pp);
                printed_packets++;
                read_bytes=0;
              }
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

  disableDataReporting();

  if(mouse_unsubscribe_int()==1){
    printf("Mouse unsubscription failed!\n");
    return 1;
  }

  return 0;
}


int (mouse_test_remote)(uint16_t period, uint8_t cnt) {

    uint32_t printed_packets=0;
    struct packet pp;

    while (printed_packets < cnt){
      uint8_t st;
      write_command(MOUSE_READ_DATA);

      if(util_sys_inb(STAT_REG, &st) != OK)
        return 1;

      if ((st & PAR_ERR) || (st & TO_ERR) || !(st & AUX))
			     return 1;

      if (st & BIT(0)){
        int i= 0;

        while(i < 3){
          if (util_sys_inb(OUT_BUF, &byte) != OK) return 1;

          switch (i) {
            case 0:
              if(byte & BIT(3)) pp.bytes[0] = byte;
            case 1:
              pp.bytes[1] = byte;
            case 2:
              pp.bytes[2] = byte;

          }
          i++;
        }
      }
      mouse_process_packets(&pp);
      mouse_print_packet(&pp);
      printed_packets++;
      tickdelay(micros_to_ticks(period * 1000));

    }

    if(write_command(MOUSE_STREAM_MODE))
      return 1;
    if(write_command(MOUSE_DISABLE))
      return 1;

    sys_outb(CTRL_REG,WRITE_COMD_BYTE);
    sys_outb(IN_BUF, minix_get_dflt_kbc_cmd_byte());

    return 0;
}

int (mouse_test_async)(uint8_t idle_time) {
  int ipc_status, r;
  message msg;

  struct packet pp;

  uint8_t timer_hookID = 0;
  uint32_t irq_set_timer = BIT(timer_hookID);
  uint32_t timer_frequency=sys_hz();

  uint8_t _mouse_hookID = 12;
  uint32_t irq_set = BIT(_mouse_hookID);

  if (timer_subscribe_int(&timer_hookID) == 1) {
    printf("Timer subscription failed!\n");
    return 1;
  }

  if (mouse_subscribe_int(&_mouse_hookID) == 1) {
    printf("Mouse subscription failed!\n");
    return 1;
  }

  enableDataReporting();

  unsigned int read_bytes=0;

  while (global_counter < (idle_time*timer_frequency)) {
    /*Get a request message.*/
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /*received notification*/
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /*hardware interrupt notification*/
          if (msg.m_notify.interrupts & irq_set) { /*subscribed interrupt*/

            global_counter=0;
            mouse_ih();

            if((read_bytes==0) && ((byte& BIT(3))==0)){
              continue;
            }
            else{
              if(read_bytes==0 || read_bytes==1 || read_bytes==2){
                pp.bytes[read_bytes]=byte;
                read_bytes++;
              }
              if(read_bytes==3){
                mouse_process_packets(&pp);
                mouse_print_packet(&pp);
                read_bytes=0;
              }
            }
        }
        if(msg.m_notify.interrupts & irq_set_timer){
          timer_int_handler();
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

  disableDataReporting();

  if(mouse_unsubscribe_int()==1){
    printf("Mouse unsubscription failed!\n");
    return 1;
  }

  if(timer_unsubscribe_int()==1){
    printf("Timer unsubscription failed!\n");
    return 1;
  }

  return 0;
}

//bool used to detect end of state machine
bool finalState = false;

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {

  int ipc_status, r;
  message msg;

  struct packet pp;

  struct mouse_ev *event;

  uint8_t mouse_hookID = 0;
  uint32_t irq_set = BIT(mouse_hookID);

  if (mouse_subscribe_int(&mouse_hookID) == 1) {
    printf("Mouse subscription failed!\n");
    return 1;
  }

  enableDataReporting();

  unsigned int read_bytes=0;

  while (!finalState) {
    /*Get a request message.*/
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /*received notification*/
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /*hardware interrupt notification*/
          if (msg.m_notify.interrupts & irq_set) { /*subscribed interrupt*/
            mouse_ih();
            if((read_bytes==0) && ((byte&BIT(3))==0)){
              continue;
            }
            else{
              if(read_bytes==0 || read_bytes==1 || read_bytes==2){
                pp.bytes[read_bytes]=byte;
                read_bytes++;
              }
              if(read_bytes==3){
                mouse_process_packets(&pp);
                mouse_print_packet(&pp);
                event = detectMouseEvent(&pp);
                mouse_gesture_handler(event,x_len,tolerance);
                read_bytes=0;
              }
          }
          break;
        }
        default:
          break; /*no other notifications expected: do nothing*/
      }
    }

    else{ /*received a standard message, not a notification*/
      /*no standard messages expected: do nothing*/
    }
  }

  disableDataReporting();

  if(mouse_unsubscribe_int()==1){
    printf("Mouse unsubscription failed!\n");
    return 1;
  }

  return 0;
}
