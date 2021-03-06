#ifndef _LCOM_I8042_H_

#define _LCOM_I8042_H_

#include <lcom/lcf.h>

/** @defgroup i8042 i8042

 * @{

 *

 * Constants for programming the i8042 KeyBoard.

 */

#define KBD_IRQ             1     /**< @brief Keyboard IRQ line */

/* I/O port addresses */

#define STAT_REG          0x64    /**< @brief Status Register */
#define KBC_CMD_REG       0x64
#define OUT_BUF           0x60    /**< @brief Output Buffer */
#define IN_BUF_COM        0x64    /**< @brief Input Buffer Commands to the KBC */
#define IN_BUF_ARG        0x60    /**< @brief Input Buffer Arguments of KBC Commands */


#define OBF               BIT(0)   /* Output buffer */
#define PAR_ERR           BIT(7)   /* Parity error */
#define TO_ERR            BIT(6)   /* Timeout error */
#define AUX               BIT(5)  /*Mouse data*/

/* Measuring Time */
#define DELAY_US          20000


#define ESC_KEY           0x81
#define TW0_BYTE_SCANCODE 0xE0

/*KBC COMMANDS */
#define READ_COMD_BYTE    0x20
#define WRITE_COMD_BYTE   0x60

#define TIMER0_IRQ 0 /**< @brief Timer 0 IRQ line */

/**@}*/

#endif /* _LCOM_I8042_H */
