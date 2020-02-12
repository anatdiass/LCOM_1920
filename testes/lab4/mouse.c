#include <lcom/lcf.h>

#include "mouse.h"

static int mouse_hookID;
uint8_t byte;

int mouse_subscribe_int(uint8_t *bit_no){
    mouse_hookID = *bit_no;
    if(sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE|IRQ_EXCLUSIVE, &mouse_hookID)!=OK)
        return 1;
    return 0;
}

int mouse_unsubscribe_int(){
    if(sys_irqrmpolicy(&mouse_hookID)!=OK)
        return 1;
    return 0;
}

int util_sys_inb(int port, uint8_t *value){
    uint32_t val;
    sys_inb(port, &val);
    *value = (uint8_t) val;
    return 0;
}

void (mouse_ih)(){
    uint8_t status;
    util_sys_inb(STAT_REG, &status);
    if(status&OBF){
        util_sys_inb(OUT_BUF, &byte);
        if((status&(PAR_ERR|TIME_ERR))==0)
            return;
    }
}

void mouse_process_packets(struct packet *pp) {
    //Left button
    if(pp->bytes[0] & LB)
        pp->lb = true;
    else
        pp->lb = false;
    //Right button
    if(pp->bytes[0] & RB)
        pp->rb = true;
    else
        pp->rb = false;
    //Middle button
    if(pp->bytes[0] & MB)
        pp->mb = true;
    else 
        pp->mb = false;
    //X overflow
    if(pp->bytes[0] & X_OV)
        pp->x_ov = true;
    else 
        pp->x_ov = false;
    //Y overflow
    if(pp->bytes[0] & Y_OV)
        pp->y_ov = true;
    else
        pp->y_ov = false;
    //x displacement
    if(pp->bytes[0] & MOV_X)
        pp->delta_x = pp->bytes[1]-256;
    else
        pp->delta_x = pp->bytes[1];
    //y displacement
    if(pp->bytes[0] & MOV_Y)
        pp->delta_y = pp->bytes[2]-256;
    else 
        pp->delta_y = pp->bytes[2];
}

int write_command(uint8_t command) {
  uint8_t status, response;

  if (util_sys_inb(STAT_REG, &status) != OK)
    return 1;
  //only writes to control register if ibf isn't set
  if (!(status & IBF)){
    if (sys_outb(CTRL_REG, MOUSE_WRITE_BYTE)) {
      return 1;
    }
  }

  if (util_sys_inb(STAT_REG, &status) != OK)
    return 1;
  //only writes to input buffer if ibf isn't set
  if (!(status & IBF)) {
    if (sys_outb(IN_BUF, command)) {
      return 1;
    }
  }

  tickdelay(micros_to_ticks(DELAY_US));

  if (util_sys_inb(OUT_BUF, &response) != OK)
    return 1;

  if (response != ACK)
    write_command(command);

  return 0;
}

int enableDataReporting() {

  sys_irqdisable(&mouse_hookID); //disable notifications

  if (write_command(MOUSE_ENABLE) == 1){
    printf("Failed to enable mouse data reporting\n");
    return 1;

  }
  sys_irqenable(&mouse_hookID); //enable notifications
  return 0;
}


int disableDataReporting() {

  sys_irqdisable(&mouse_hookID); //disable notifications

  if (write_command(MOUSE_DISABLE) == 1) {
    printf("Failed to disable mouse data reporting\n");
    return 1;
  }
  return 0;
}


int util_sys_inb2(int port, uint8_t *value){
  uint32_t val;
  sys_inb(port, &val);
  *value=(uint8_t) val;
  return 0;
}

int mouse_subscribe(uint8_t *bit_no){
  mouse_hookID = *bit_no;
  if(sys_irqsetpolicy(MOUSE_IRQ, IRQ_EXCLUSIVE|IRQ_REENABLE, &mouse_hookID)!=OK) 
    return 1;
  return 0;
}

int mouse_unsubscribe(){
  if(sys_irqrmpolicy(&mouse_hookID)!=OK) 
    return 1;
  return 0;
}

void ih(){
  uint8_t status;
  util_sys_inb(STAT_REG, &status);
  if(status&OBF)
    util_sys_inb(OUT_BUF, &byte);
    if((status&(PAR_ERR|TIME_ERR))==0)
      return;
}

void process_packett(struct packet *pp){
  if(pp->bytes[0] & RB)
    pp->rb = true;
  else pp->rb=false;
  if(pp->bytes[0] & MB)
    pp->mb=true;
  else pp->mb=false;
  if(pp->bytes[0]&LB)
    pp->lb=true;
  else pp->lb=false;
  if(pp->bytes[0] & MOV_X)
    pp->delta_x = pp->bytes[1]-256;
  else pp->delta_x=pp->bytes[1];
  if(pp->bytes[0] & MOV_Y)
    pp->delta_y = pp->bytes[2]-256;
  else pp->delta_y=pp->bytes[2];
  if(pp->bytes[0]&X_OV)
    pp->x_ov=true;
  else pp->x_ov=false;
  if(pp->bytes[0]&Y_OV)
    pp->y_ov=true;
  else pp->y_ov=false;
}


int subscribe(uint8_t *bit_no){
  *bit_no = mouse_hookID;
  if(sys_irqsetpolicy(MOUSE_IRQ, IRQ_EXCLUSIVE|IRQ_REENABLE, &mouse_hookID)!=OK)
    return 1;
  return 0;
}

int unsubscribe(){
  if(sys_irqrmpolicy(&mouse_hookID)!=OK)
    return 1;
  return 0;
}

static struct packet pp;

struct packet get_packet(){
  return pp;
}

int set_cw1(){
  uint8_t status;
  if(util_sys_inb(STAT_REG, &status)!=OK)
    return 1;
  status = status|BIT(0)|BIT(3)|BIT(5);
  if(sys_outb(STAT_REG, status)!=OK)
    return 1;
  return 0;
}

int ih1(){
  uint8_t byte;
  for(int i=0;i<4;i++){
    util_sys_inb(OUT_BUF, &byte);
    pp.bytes[i] = byte;
  }
  return 0;
}

void process1(){
  if(pp.bytes[0] & LB)
    pp.lb=true;
  else pp.lb=false;

  //igual para mb e rb

  if(pp.bytes[0]&MOV_X)
    pp.delta_x=pp.bytes[2]-256;
  else pp.delta_x=pp.bytes[2];
  //if(pp.bytes[0]&MOV_Y)
    //pp.delta_y=pp.bytes[3]-256;
  //else pp.delta_y=pp.bytes[3];
 /* if(pp.bytes[0]&MOV_Z)
    pp.delta_z=pp.bytes[1]-256;
  else pp.delta_z=pp.bytes[1];*/

  if(pp.bytes[0]&X_OV)
    pp.x_ov=true;
  else pp.x_ov=false;

  //igual para y_ov
}

int reset_cw(){
  uint8_t status;
  if(util_sys_inb(STAT_REG, &status)!=OK)
    return 1;
  
  status = status & ~BIT(0) & ~BIT(3) & ~BIT(5);

  if(sys_outb(STAT_REG, status)!=OK) 
    return 1;

  return 0;  
}

/*
SEQUENCIA NO LOOP:
  mouse_ih1();
  process_packet1();
  print_packet(get_packet());
  printed_packets++;
*/


int set_cw2(){
  uint8_t status;
  if(util_sys_inb(STAT_REG, &status)!=OK)
    return 1;
  
  status |= BIT(5);
  status &= ~BIT(0);
  status &= ~BIT(3);

  if(sys_outb(STAT_REG, status)!=OK)
    return 1;
  
  return 0;
}

int ih2(){
  uint8_t byte;
  int cnt=0;
  if(util_sys_inb(OUT_BUF, &byte)!=OK)
    return 1;
  pp.bytes[cnt]=byte;
  cnt++;
  if(cnt==3){
    cnt=0;
    return 0;
  }
  return 1;
}

/*
SEQUENCIA LOOP:
  if(mouse_ih2()==1)
    break;
  process2(); //igual ao lab mas pp.delta_z=0
  print(get_packet());
  printed_packets++;
*/


