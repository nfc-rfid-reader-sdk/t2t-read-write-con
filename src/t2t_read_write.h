/*
 * t2t_read_write.h
 *
 *  Created on: 31.05.2016.
 *      Author: www.d-logic.net
 */

#ifndef T2T_READ_WRITE_H_
#define T2T_READ_WRITE_H_

// T2T authentication constants:
#define T2T_NO_PWD_AUTH		0
#define T2T_RKA_PWD_AUTH	1
#define T2T_PK_PWD_AUTH		3
#define T2T_WITHOUT_PWD_AUTH	0x60
#define T2T_WITH_PWD_AUTH		0x61

void t2t_read_write_init(void);
void setAuthMode(uint8_t auth_mode_);
uint8_t getAuthMode(void);
void setPwdPackReaderIdx(uint8_t pwd_pack_idx_);
uint8_t getPwdPackReaderIdx(void);
void setPwd(const uint8_t *pwd_);
uint8_t *getPwd(void);
void setPack(const uint8_t *pack_);
uint8_t *getPack(void);
int is_reader_support_pwd(void);

#endif /* T2T_READ_WRITE_H_ */
