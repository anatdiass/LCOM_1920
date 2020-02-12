#ifndef _LCOM_I8042_H_

#define _LCOM_I8042_H_

#include <lcom/lcf.h>

#define DELAY_US        20000

#define KBD_IRQ             1     /**< @brief Keyboard IRQ line */
#define MOUSE_IRQ           12    /**< @brief Mouse IRQ line */

/*-----------KEYBOARD-------------*/

/* I/O port addresses */
#define STAT_REG          0x64    /**< @brief Status Register */
#define KBC_CMD_REG       0x64
#define OUT_BUF           0x60    /**< @brief Output Buffer */
#define IN_BUF_COM        0x64    /**< @brief Input Buffer Commands to the KBC */
#define IN_BUF_ARG        0x60    /**< @brief Input Buffer Arguments of KBC Commands */

#define IBF               BIT(1)

#define OBF               BIT(0)   /* Output buffer */
#define PAR_ERR           BIT(7)   /* Parity error */
#define TO_ERR            BIT(6)   /* Timeout error */
#define AUX               BIT(5)  /*Mouse data*/

/* Measuring Time */
#define DELAY_US          20000

/* Scancodes */
#define ESC_KEY           0x81
#define TW0_BYTE_SCANCODE 0xE0

/*KBC Commands */
#define READ_COMD_BYTE    0x20
#define WRITE_COMD_BYTE   0x60


/*-----------MOUSE-------------*/
#define MOUSE_DISABLE     0xF5
#define MOUSE_ENABLE      0xF4
#define MOUSE_STREAM_MODE 0xEA
#define MOUSE_REMOTE      0xF0
#define MOUSE_READ_DATA   0xEB
#define ACK               0xFA
#define MOUSE_WRITE_BYTE  0XD4
#define CTRL_REG          0x64
#define IN_BUF            0x60

#define MOUSE_LB            BIT(0)
#define MOUSE_RB            BIT(1)
#define MOUSE_MB            BIT(2)
#define MOUSE_X             BIT(4)
#define MOUSE_Y             BIT(5)
#define MOUSE_X_OVERFLOW    BIT(6)
#define MOUSE_Y_OVERFLOW    BIT(7)


/*---------------VIDEO CARD--------------*/


#endif /* _LCOM_I8042_H */
