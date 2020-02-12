// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
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
  uint8_t mouse_hookID=0;
  uint32_t irq_set = BIT(mouse_hookID);

  int ipc_status, r;
  message msg;

  struct packet pp;

  int read_bytes = 0;
  uint32_t printed_packets = 0;

  if(mouse_subscribe_int(&mouse_hookID)!=OK)
    return 1;

  enableDataReporting();

  while(printed_packets<cnt) {
    /*Get a request message.*/     
    if( (r =driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;     
    }     
    if (is_ipc_notify(ipc_status)) { /*received notification*/         
      switch (_ENDPOINT_P(msg.m_source)) {         
        case HARDWARE: /*hardware interrupt notification*/             
          if (msg.m_notify.interrupts & irq_set) { /*subscribed interrupt*/
            mouse_ih();
            if((read_bytes==0)&&((byte&BIT(3))==0)){
              continue;
            }
            else{
              if(read_bytes==0 || read_bytes ==1 || read_bytes == 2){
                pp.bytes[read_bytes] = byte;
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
    else { /*received a standard message, not a notification*/         
      /*no standard messages expected: do nothing*/
    }
  }

  disableDataReporting();

  if(mouse_unsubscribe_int()!=OK)
    return 1;

  return 0;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
  uint32_t printed_packets=0;
  //struct packet pp;
  
  while(printed_packets<cnt){
    uint8_t status;
    
    //reading from status register
    if(util_sys_inb(STAT_REG, &status)!=OK)
      return 1;
  }

  return 0;
}

int (mouse_test_async)(uint8_t idle_time) {
  uint8_t mouse_hookID=15;
  uint32_t irq_set = BIT(mouse_hookID);

  uint8_t timer_hookID=12;
  uint32_t timer_irq = BIT(timer_hookID);

  int ipc_status, r;
  message msg;

  struct packet pp;
  int read_bytes = 0;

  uint32_t timer_frequency=sys_hz();

  if(mouse_subscribe_int(&mouse_hookID)!=OK)
    return 1;

  if(timer_subscribe_int(&timer_hookID)!=OK)
    return 1;

  enableDataReporting();

  while(global_counter<(idle_time*timer_frequency)) {
    /*Get a request message.*/     
    if( (r =driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;     
    }     
    if (is_ipc_notify(ipc_status)) { /*received notification*/         
      switch (_ENDPOINT_P(msg.m_source)) {         
        case HARDWARE: /*hardware interrupt notification*/             
          if (msg.m_notify.interrupts & timer_irq) { /*subscribed interrupt*/
            timer_int_handler();
          }
          if (msg.m_notify.interrupts & irq_set) { /*subscribed interrupt*/
            global_counter=0;
            mouse_ih();
            if((read_bytes==0)&&((byte&BIT(3))==0)){
              continue;
            }
            else{
              if(read_bytes==0 || read_bytes ==1 || read_bytes == 2){
                pp.bytes[read_bytes] = byte;
                read_bytes++;
              }
              if(read_bytes==3){
                mouse_process_packets(&pp);
                mouse_print_packet(&pp);
                read_bytes=0;
              }
            }
          }             
          break;         
        default:             
          break; /*no other notifications expected: do nothing*/         
      } 
    } 
    else { /*received a standard message, not a notification*/         
      /*no standard messages expected: do nothing*/
    }
  }

  disableDataReporting();

  if(mouse_unsubscribe_int()!=OK)
    return 1;

  return 0;
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
  set_cw1();
  reset_cw();

  set_cw2();
  ih2();
  return 0;
}
