// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

#include "keyboard.h"
#include "video_gr.h"

extern uint8_t key_scancode;

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
  if (vg_init(mode) == NULL)
    return 1;

  sleep(delay);

  if (vg_exit() != 0)
    return 1;

  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,uint16_t width, uint16_t height, uint32_t color) {

  int ipc_status, r;
  message msg;
  uint8_t keyboard_hookID = 0;
  uint32_t irq_set = BIT(keyboard_hookID);

  if(kbc_subscribe_int(&keyboard_hookID) == 1){
    printf("Keyboard subscription failed!\n");
    return 1;
  }

  if(vg_init(mode) == NULL)
    return 1;

  vg_draw_rectangle(x,y,width,height,color);

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
  if(vg_exit() != OK){
      return 1;
  }
  
  if(kbc_unsubscribe_int()==1){
    printf("Timer unsubscription failed!\n");
    return 1;
  }

  return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  int ipc_status, r;
  message msg;
  bool make;
  uint8_t size;
  bool has2Bytes = false;
  uint8_t keyboard_hookID = 0;
  uint32_t irq_set = BIT(keyboard_hookID);

  if(kbc_subscribe_int(&keyboard_hookID) == 1){
    printf("Keyboard subscription failed!\n");
    return 1;
  }

  if(vg_init(mode) == NULL)
    return 1;

  /*if (vg_draw_pattern( no_rectangles, first, step)!= OK){
    return 1;
  }*/

  while (key_scancode != ESC_KEY) {
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

            if (key_scancode == 0xE0) { //checks if scancode has 2 bytes
              has2Bytes = true;
            }

            else {
              if ((key_scancode & BIT(7)) == 0)
                make = true;
              else
                make = false;

              if (has2Bytes) {

                size = 2;
                uint8_t bytes[size];

                bytes[0] = 0xE0;
                bytes[1] = key_scancode;
              }

              else {

                size = 1;
                uint8_t bytes[size];

                bytes[0] = key_scancode;

              }
              has2Bytes = false;
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

  if(vg_exit() != OK){
      return 1;
  }

  if(kbc_unsubscribe_int()==1){
    printf("Timer unsubscription failed!\n");
    return 1;
  }

  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {

  int ipc_status, r;
  message msg;
  uint8_t keyboard_hookID = 0;
  uint32_t irq_set = BIT(keyboard_hookID);

  //Keyboard subscription
  if(kbc_subscribe_int(&keyboard_hookID) == 1){
    printf("Keyboard subscription failed!\n");
    return 1;
  }

  //Initialize mode 105
  if(vg_init(0x105)==NULL){
    printf("Couldn't initialize mode 105!\n");
    return 1;
  }

  xpm_image_t img;
  uint8_t *map;// get the pixmap from the XPM
  map = xpm_load(xpm, XPM_INDEXED, &img);

  //Draws xpm
  vg_draw_img(img,x,y);

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
  if(vg_exit() != OK){
      return 1;
  }
  
  if(kbc_unsubscribe_int()==1){
    printf("Timer unsubscription failed!\n");
    return 1;
  }

  return 0;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {
  /* To be completed */
  printf("%s(%8p, %u, %u, %u, %u, %d, %u): under construction\n",
         __func__, xpm, xi, yi, xf, yf, speed, fr_rate);

  return 1;
}

int(video_test_controller)() {
  /* To be completed */
  printf("%s(): under construction\n", __func__);

  return 1;
}
