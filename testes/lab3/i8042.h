#include <lcom/lcf.h>

#define KBD_IRQ     1

#define STAT_REG    0x64
#define KBC_CMD_REG 0x64

#define OUT_BUF     0x60
#define IN_BUF_ARG  0x60
#define IN_BUF_COM  0x64

#define OBF         BIT(0)
#define PAR_ERR     BIT(7)
#define TIME_ERR    BIT(6)
#define AUX         BIT(5)
#define IBF         BIT(1)

#define ESC_KEY     0x81
#define TWO_BYTES   0xE0

#define MSB         BIT(7)

#define READ_COMD_BYTE  0x20
#define WRITE_COMD_BYTE 0x60

#define DELAY_US    20000
