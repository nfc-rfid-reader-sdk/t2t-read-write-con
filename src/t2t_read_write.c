/*
 * t2t_read_write.c
 *
 *  Created on: 31.05.2016.
 *      Author: Zborac
 */

#include <string.h>
#include "uFCoder.h"
#include "t2t_read_write.h"

uint8_t pwd_default[4] = {0xFF, 0xFF, 0xFF, 0xFF};
uint8_t pack_default[2] = {0, 0};
uint8_t pwd[4];
uint8_t pack[2];
uint8_t pwd_pack_idx = 0;
uint8_t auth_mode = T2T_NO_PWD_AUTH;
int reader_support_pwd = 0;
//------------------------------------------------------------------------------
void t2t_read_write_init(void) {
	UFR_STATUS status;
	uint32_t reader_type;

	memcpy(pwd, pwd_default, 4);
	memcpy(pack, pack_default, 2);

	status = GetReaderType(&reader_type);
	if (status == UFR_OK) {
		if ((reader_type & 0xFF) != 0x21) {
			reader_support_pwd = 1;
		}
	}
}
//------------------------------------------------------------------------------
void setAuthMode(uint8_t auth_mode_) {

	auth_mode = auth_mode_;
}
//------------------------------------------------------------------------------
uint8_t getAuthMode(void) {
	return auth_mode;
}
//------------------------------------------------------------------------------
void setPwdPackReaderIdx(uint8_t pwd_pack_idx_) {

	pwd_pack_idx = pwd_pack_idx_;
}
//------------------------------------------------------------------------------
uint8_t getPwdPackReaderIdx(void) {
	return pwd_pack_idx;
}
//------------------------------------------------------------------------------
void setPwd(const uint8_t *pwd_) {

	memcpy(pwd, pwd_, 4);
}
//------------------------------------------------------------------------------
uint8_t *getPwd(void) {

	return pwd;
}
//------------------------------------------------------------------------------
void setPack(const uint8_t *pack_) {
	memcpy(pack, pack_, 2);
}
//------------------------------------------------------------------------------
uint8_t *getPack(void) {
	return pack;
}
//------------------------------------------------------------------------------
int is_reader_support_pwd(void) {

	return reader_support_pwd;
}
//------------------------------------------------------------------------------
