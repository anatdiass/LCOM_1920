#include <lcom/lcf.h>

#define MOUSE_IRQ   12

#define STAT_REG    0x64
#define OBF         BIT(0)
#define PAR_ERR     BIT(7)
#define TIME_ERR    BIT(6)
#define AUX         BIT(5)
#define IBF         BIT(1)
#define OUT_BUF     0x60

#define LB          BIT(0)
#define RB          BIT(1)
#define MB          BIT(2)
#define MOV_X       BIT(4)
#define MOV_Y       BIT(5)
#define X_OV        BIT(6)
#define Y_OV        BIT(7)


#define MOUSE_DISABLE     0xF5
#define MOUSE_ENABLE      0xF4
#define MOUSE_STREAM_MODE 0xEA
#define MOUSE_REMOTE      0xF0
#define MOUSE_READ_DATA   0xEB

#define ACK               0xFA
#define MOUSE_WRITE_BYTE  0XD4
#define CTRL_REG          0x64
#define IN_BUF            0x60

#define DELAY_US          20000
