#include <lcom/lcf.h>

#include "keyboard.h"

static int kbd_hookID;
uint8_t scancode=0;

#ifdef LAB3
int counter = 0;
#endif

int kbd_subscribe_int(uint8_t *bit_no){
    kbd_hookID = *bit_no;
    if(sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE|IRQ_EXCLUSIVE, &kbd_hookID)!=OK)
        return 1;
    return 0;
}

int kbd_unsubscribe_int(){
    if(sys_irqrmpolicy(&kbd_hookID)!=OK)
        return 1;
    return 0;
}

int util_sys_inb(int port, uint8_t *value){
    uint32_t val;
    sys_inb(port, &val);
    *value = (uint8_t) val;
    #ifdef LAB3
      counter++;
    #endif

    return 0;
}

void (kbc_ih)(){
    uint8_t status;
    
    //Reads the status register
    util_sys_inb(STAT_REG, &status);

    if(status&OBF){ //OBF -> data available for reading
        util_sys_inb(OUT_BUF, &scancode); //reads from the output buffer
        if((status&(PAR_ERR|TIME_ERR))==0)  //checks bits 6 and 7
            return;
    }
}

void process_scancodes(uint8_t scode){
    bool makecode;
    uint8_t size;

  if((scode&MSB)==0)
    makecode=true;
  else
    makecode = false;

  if(scode == TWO_BYTES){
    size=2;
    uint8_t bytes[size];
    bytes[0] = TWO_BYTES;
    bytes[1] = scode;
    kbd_print_scancode(makecode, size, bytes);
  }
  else{
    size=1;
    uint8_t bytes[size];
    
    bytes[0] = scode;
    kbd_print_scancode(makecode, size, bytes);
  }
}



int readCommandByte(uint8_t command_byte){
  //Reads from the output buffer 0x60
  if(util_sys_inb(OUT_BUF, &command_byte)==OK)
    return 0;
  else{
    printf("Failed reading controller command byte\n");
    return 1;
  }
}

int writeCommandByte(uint8_t command_byte){
  //Writes to the output buffer 0x60
  if(sys_outb(OUT_BUF, command_byte)==OK){
    return 0;
  }
  else{
    printf("Failed writing controller command byte\n");
    return 1;
  }
}

int processCommandByte(uint8_t command_byte){
  uint8_t status=0;
  //Checks status register
  util_sys_inb(STAT_REG, &status);

  if((status&BIT(1))==0){ //Checks if inputBuffer is full
    sys_outb(KBC_CMD_REG, command_byte);
    return 0;
  }
  return 1;
}

void enableInterrupts(){

  //Checks if can read command byte
  processCommandByte(READ_COMD_BYTE);

  //Reads controller's command byte
  readCommandByte(READ_COMD_BYTE);

  //Set value
  processCommandByte(WRITE_COMD_BYTE);

  //Writes controller's command byte
  writeCommandByte(WRITE_COMD_BYTE| 0x01);
}

