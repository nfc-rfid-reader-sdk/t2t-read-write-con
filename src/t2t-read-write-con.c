/*
 ============================================================================
 Name        : t2t-read-write-con.c
 Author      : www.d-logic.net
 Version     : 1.1
 Description : t2t-read-write console example in C
 Dependency  : uFCoder library, min. ver. 3.7.3 for NTAG203, UL, UL C
 Dependency  : uFCoder library, min. ver. 3.8.5 for NTAG21x, UL EV1, MIK640D
 Dependency  : uFR firmware from ver. 3.9.10 for PWD/PACK T2T authentication.
 Dependency  : uFCoder library, min. ver. 4.0.3 for NTAG & UL EV1 Counters
 Dependency  : uFR firmware from ver. 3.9.11 for NTAG & UL EV1 Counters
 ============================================================================
 */

#define STR_APP_VERSION "1.2"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#if __WIN32 || __WIN64
#	include <conio.h>
#elif linux || __linux__ || __APPLE__
#	include "conio_gnu.h"
#endif
#include "uFCoder.h"
#include "t2t_read_write.h"
//------------------------------------------------------------------------------
#define DEFAULT_LINE_LEN	57
//------------------------------------------------------------------------------
void usage(void);
void SetAuthMode(void);
void print_ln_len(char symbol, uint8_t cnt);
void print_ln(char symbol);
void print_hex(const uint8_t *data, uint32_t len, const char *delimiter);
void print_hex_ln(const uint8_t *data, uint32_t len, const char *delimiter);
char *GetDlTypeName(uint8_t dl_type_code);
//------------------------------------------------------------------------------
int main(void) {
#define CONTENT_BUFF_LEN	2048
#define STR_BUFF_LEN(x)		STR_BUFF_LEN1(x)
#define STR_BUFF_LEN1(x)	#x
	UFR_STATUS status;
	int loop = 1;
	int input_val, nitems, page, len;
	uint16_t actual;
	uint32_t bytes_4;
	uint8_t page_content[CONTENT_BUFF_LEN];
	uint8_t pwd_pack_key[6];
	uint8_t sak;
	uint8_t uid_len;
	uint8_t dl_card_type;
	uint8_t uid[MAX_UID_LEN];
	uint32_t counter_val;

	printf(
			"\nConsole application example t2t-read-write-con v"
			STR_APP_VERSION "\n"
			"Demonstration of uFR devices support for NFC T2T tags (i.e. NXP ULTRALIGHT and NTAG).\n");
	printf("FCoder Library version: %s\n"
			"Please wait while trying connect to the uFR device.\n",
			GetDllVersionStr());

	status = ReaderOpen();
	if (status != UFR_OK) {
		printf("Can't connect to the device. Is uFR connected?\n");
		return EXIT_FAILURE;
	}

	t2t_read_write_init();
	usage();

	while (loop) {
		switch (getch()) {
		case 'I': // Get reader description:
		case 'i':
			printf("%s\n", GetReaderDescription());
			break;
			//..................................................................
		case 'O': // Get software versions:
		case 'o':
			print_ln('=');
			printf(
					"t2t-read-write console example version: " STR_APP_VERSION "\n");
			printf("FCoder Library version: %s\n", GetDllVersionStr());
			print_ln('=');
			break;
			//..................................................................
		case 'P': // Get ID of the tag in reader field:
		case 'p':
			print_ln('=');
			status = GetCardIdEx(&sak, uid, &uid_len);
			if (status != UFR_OK) {
				printf("Error, status is: 0x%08X\n", status);
				print_ln('=');
				break;
			}
			status = GetDlogicCardType(&dl_card_type);
			if (status != UFR_OK) {
				printf("Error, status is: 0x%08X\n", status);
				print_ln('=');
				break;
			}
			printf("Card type: %s, uid[%d] = ", GetDlTypeName(dl_card_type),
					uid_len);
			print_hex_ln(uid, uid_len, ":");
			if ((dl_card_type < DL_MIFARE_ULTRALIGHT)
					|| (dl_card_type > NFC_T2T_GENERIC))
				printf("UNSUPPORTED Tag by this application.\n");
			else
				printf("Tag is supported by this application.\n");
			print_ln('=');
			break;
			//..................................................................
		case 'A': // Set authentication mode:
		case 'a':
			print_ln('=');
			SetAuthMode();
			print_ln('=');
			usage();
			break;
			//..................................................................
		case 'S': // Show authentication parameters:
		case 's':
			print_ln('=');
			switch (getAuthMode()) {
			case T2T_NO_PWD_AUTH:
				printf("T2T_NO_PWD_AUTH (without PWD authentication)\n");
				break;
			case T2T_RKA_PWD_AUTH:
				printf(
						"T2T_RKA_PWD_AUTH (PWD authentication using PWD/PACK stored in reader)\n");
				printf("    PWD/PACK stored in reader index = %d\n",
						getPwdPackReaderIdx());
				break;
			case T2T_PK_PWD_AUTH:
				printf(
						"T2T_PK_PWD_AUTH (PWD authentication with provided PWD/PACK)\n");
				printf("     PWD = ");
				print_hex_ln(getPwd(), 4, ":");
				printf("     PACK = ");
				print_hex_ln(getPack(), 2, ":");
				break;
			default:
				printf("Unknown authentication mode.\n");
				break;
			}
			print_ln('=');
			break;
			//..................................................................
		case 'R': // Page Read:
		case 'r':
			printf("    Enter page address: ");
			nitems = scanf("%d", &page);
			fflush(stdin);
			if ((nitems == EOF) || (nitems == 0)) {
				printf("\nInput error.\n");
				print_ln('=');
				break;
			}
			if ((page < 0) || (page > 0xFF)) {
				printf("\nPage out of range.\n");
				print_ln('=');
				break;
			}

			switch (getAuthMode()) {
			case T2T_NO_PWD_AUTH:
				status = BlockRead(page_content, page, T2T_WITHOUT_PWD_AUTH, 0);
				break;
			case T2T_RKA_PWD_AUTH:
				status = BlockRead(page_content, page, T2T_WITH_PWD_AUTH,
						getPwdPackReaderIdx());
				break;
			case T2T_PK_PWD_AUTH:
				memcpy(pwd_pack_key, getPwd(), 4);
				memcpy(&pwd_pack_key[4], getPack(), 2);
				status = BlockRead_PK(page_content, page, T2T_WITH_PWD_AUTH,
						pwd_pack_key);
				break;
			}

			if (status == UFR_OK) {
				printf("Page Read operation successful\nPage content: ");
				print_hex_ln(page_content, 4, ":");
			} else {
				printf("Error, status is: 0x%08X\n", status);
			}
			print_ln('=');
			break;
			//..................................................................
		case 'W': // Page Write:
		case 'w':
			printf("    Enter page address: ");
			nitems = scanf("%d", &page);
			fflush(stdin);
			if ((nitems == EOF) || (nitems == 0)) {
				printf("\nInput error.\n");
				print_ln('=');
				break;
			}
			if ((page < 0) || (page > 0xFF)) {
				printf("\nPage out of range.\n");
				print_ln('=');
				break;
			}

			printf("    Enter page content: ");
			nitems = scanf("%8X", &bytes_4);
			fflush(stdin);
			if ((nitems == EOF) || (nitems == 0)) {
				printf("\nInput error.\n");
				print_ln('=');
				break;
			}
			for (int i = 0; i < 4; i++) {
				page_content[3 - i] = (uint8_t) (bytes_4 >> (i * 8));
			}

			switch (getAuthMode()) {
			case T2T_NO_PWD_AUTH:
				status = BlockWrite(page_content, page, T2T_WITHOUT_PWD_AUTH,
						0);
				break;
			case T2T_RKA_PWD_AUTH:
				status = BlockWrite(page_content, page, T2T_WITH_PWD_AUTH,
						getPwdPackReaderIdx());
				break;
			case T2T_PK_PWD_AUTH:
				memcpy(pwd_pack_key, getPwd(), 4);
				memcpy(&pwd_pack_key[4], getPack(), 2);
				status = BlockWrite_PK(page_content, page, T2T_WITH_PWD_AUTH,
						pwd_pack_key);
				break;
			}

			if (status == UFR_OK) {
				printf("Page Write operation successful.\n");
			} else {
				printf("Error, status is: 0x%08X\n", status);
				print_ln('=');
			}
			print_ln('=');
			break;
			//..................................................................
		case 'K': // Linear Read
		case 'k':
			printf("    Enter start byte address: ");
			nitems = scanf("%d", &page);
			fflush(stdin);
			if ((nitems == EOF) || (nitems == 0)) {
				printf("\nInput error.\n");
				print_ln('=');
				break;
			}
			if ((page < 0) || (page > 0xFFFF)) {
				printf("\nAddress out of range.\n");
				print_ln('=');
				break;
			}
			printf("    Enter read length (in bytes): ");
			nitems = scanf("%d", &len);
			fflush(stdin);
			if ((nitems == EOF) || (nitems == 0)) {
				printf("\nInput error.\n");
				print_ln('=');
				break;
			}
			if ((len < 0) || (len > sizeof(page_content))) {
				printf("\nLength out of range.\n");
				print_ln('=');
				break;
			}

			switch (getAuthMode()) {
			case T2T_NO_PWD_AUTH:
				status = LinearRead(page_content, (uint16_t) page,
						(uint16_t) len, &actual, T2T_WITHOUT_PWD_AUTH, 0);
				break;
			case T2T_RKA_PWD_AUTH:
				status = LinearRead(page_content, (uint16_t) page,
						(uint16_t) len, &actual, T2T_WITH_PWD_AUTH,
						getPwdPackReaderIdx());
				break;
			case T2T_PK_PWD_AUTH:
				memcpy(pwd_pack_key, getPwd(), 4);
				memcpy(&pwd_pack_key[4], getPack(), 2);
				status = LinearRead_PK(page_content, (uint16_t) page,
						(uint16_t) len, &actual, T2T_WITH_PWD_AUTH,
						pwd_pack_key);
				break;
			}

			if (status == UFR_OK) {
				printf("Linear Read operation successful\nContent: ");
				for (int i = 0; i < actual; i++) {
					printf("%c", (char) page_content[i]);
				}
				printf("\n");
			} else {
				printf("Error, status is: 0x%08X\n", status);
			}
			print_ln('=');
			break;
			//..................................................................
		case 'L': // Linear Write
		case 'l':
			printf("    Enter start byte address: ");
			nitems = scanf("%d", &page);
			fflush(stdin);
			if ((nitems == EOF) || (nitems == 0)) {
				printf("\nInput error.\n");
				print_ln('=');
				break;
			}
			if ((page < 0) || (page > 0xFFFF)) {
				printf("\nPage out of range.\n");
				print_ln('=');
				break;
			}

			printf("    Enter content (length will be set automatically): ");
			nitems = scanf("%"STR_BUFF_LEN(CONTENT_BUFF_LEN)"s", page_content);
			fflush(stdin);
			if ((nitems == EOF) || (nitems == 0)) {
				printf("\nInput error.\n");
				print_ln('=');
				break;
			}
			len = strlen((char *) page_content);

			switch (getAuthMode()) {
			case T2T_NO_PWD_AUTH:
				status = LinearWrite(page_content, (uint16_t) page,
						(uint16_t) len, &actual, T2T_WITHOUT_PWD_AUTH, 0);
				break;
			case T2T_RKA_PWD_AUTH:
				status = LinearWrite(page_content, (uint16_t) page,
						(uint16_t) len, &actual, T2T_WITH_PWD_AUTH,
						getPwdPackReaderIdx());
				break;
			case T2T_PK_PWD_AUTH:
				memcpy(pwd_pack_key, getPwd(), 4);
				memcpy(&pwd_pack_key[4], getPack(), 2);
				status = LinearWrite_PK(page_content, (uint16_t) page,
						(uint16_t) len, &actual, T2T_WITH_PWD_AUTH,
						pwd_pack_key);
				break;
			}

			if (status == UFR_OK) {
				printf("Page Write operation successful.\n");
			} else {
				printf("Error, status is: 0x%08X\n", status);
				print_ln('=');
			}
			print_ln('=');
			break;
			//..................................................................
		case 'B': // Show counter capabilities
		case 'b':
			status = GetDlogicCardType(&dl_card_type);
			if (status != UFR_OK) {
				printf("Error, status is: 0x%08X\n", status);
				print_ln('=');
				break;
			}
			printf("Card type: %s\n", GetDlTypeName(dl_card_type));

			switch (dl_card_type) {
			case DL_MIFARE_ULTRALIGHT_C:
			case DL_NTAG_203:
				printf("Card features 16-bit one-way counter, located at first two bytes of page 41 (0x29).\n"
						"In  its  delivery  state,  counter  value  is  set  to 0.  The  first valid  Write\n"
						"the address 41 can be  performed with any value  in the range  between 1 and 65535\n"
						"(0xFFFF) and  corresponds to  initial counter value.  Every consequent valid WRITE\n"
						"command, which represents the increment, can contain values between 1 and 15.\n");
				break;
			case DL_MIFARE_ULTRALIGHT_EV1_11:
			case DL_MIFARE_ULTRALIGHT_EV1_21:
				printf("Card features three independent 24-bit one-way counters.\n"
						"This kind of counters are supported in firmware from version 3.9.11\n"
						"and in uFCoder library from version 4.0.3.\n");
				break;
			case DL_NTAG_213:
			case DL_NTAG_215:
			case DL_NTAG_216:
				printf("Tag features a NFC counter function. This function enables tag to\n"
						"automatically increase the 24 bit counter value, triggered by the\n"
						"first valid READ or FAST-READ command.\n"
						"This kind of counter is supported in firmware from version 3.9.11\n"
						"and in uFCoder library from version 4.0.3.\n");
				break;
			case TAG_UNKNOWN:
				break;
			default:
				printf("Card does not support any kind of the T2T counters.\n");
				break;
			}
			print_ln('=');
			break;
			//..................................................................
		case 'C': // Get counter value
		case 'c':
			status = GetDlogicCardType(&dl_card_type);
			if (status != UFR_OK) {
				printf("Error, status is: 0x%08X\n", status);
				break;
			}
			switch (dl_card_type) {
			case DL_MIFARE_ULTRALIGHT_C:
			case DL_NTAG_203:
				status = BlockRead((void *)&counter_val, 41, T2T_WITHOUT_PWD_AUTH, 0);
				if (status != UFR_OK) {
					printf("Error, status is: 0x%08X\n", status);
					break;
				}
				counter_val &= 0xFFFF;
				printf("Counter page read operation successful\nCounter value is: %d\n", counter_val);
				break;
			case DL_MIFARE_ULTRALIGHT_EV1_11:
			case DL_MIFARE_ULTRALIGHT_EV1_21:
				printf("    Enter counter address (0, 1 or 2): ");
				nitems = scanf("%d", &page);
				fflush(stdin);
				if ((nitems == EOF) || (nitems == 0)) {
					printf("\nInput error.\n");
					break;
				}
				if ((page < 0) || (page > 2)) {
					printf("\nAddress out of range.\n");
					break;
				}

				status = ReadCounter(page, (void *)&counter_val);
				if (status != UFR_OK) {
					printf("Error, status is: 0x%08X\n", status);
					break;
				}
				printf("Read Counter operation successful\nCounter value is: %d\n", counter_val);
				break;
			case DL_NTAG_213:
			case DL_NTAG_215:
			case DL_NTAG_216:
				switch (getAuthMode()) {
				case T2T_NO_PWD_AUTH:
					status = ReadNFCCounter((void *)&counter_val);
					break;
				case T2T_RKA_PWD_AUTH:
					status = ReadNFCCounterPwdAuth_RK((void *)&counter_val, getPwdPackReaderIdx());
					break;
				case T2T_PK_PWD_AUTH:
					memcpy(pwd_pack_key, getPwd(), 4);
					memcpy(&pwd_pack_key[4], getPack(), 2);
					status = ReadNFCCounterPwdAuth_PK((void *)&counter_val, pwd_pack_key);
					break;
				}

				if (status == UFR_OK) {
					printf("Read Counter operation successful\nCounter value is: %d\n", counter_val);
				} else {
					printf("Error, status is: 0x%08X\n", status);
				}
				break;
			default:
				printf("Card in field not supported for counter operations.\n");
				break;
			}
			print_ln('=');
			break;
			//..................................................................
		case 'V': // Increase counter
		case 'v':
			status = GetDlogicCardType(&dl_card_type);
			if (status != UFR_OK) {
				printf("Error, status is: 0x%08X\n", status);
				break;
			}
			switch (dl_card_type) {
			case DL_MIFARE_ULTRALIGHT_C:
			case DL_NTAG_203:
				printf("If current counter is 0, any 16-bit value takes into account\n"
						"and considered as an initial value. After the initial write,\n"
						"only the lower nibble of the first data byte is used for the\n"
						"increment value and the remaining part of the data is ignored.\n"
						"    Enter increment value: ");
				nitems = scanf("%d", &page);
				fflush(stdin);
				if ((nitems == EOF) || (nitems == 0)) {
					printf("\nInput error.\n");
					break;
				}
				if ((page < 0) || (page > 0xFFFF)) {
					printf("\nPage out of range.\n");
					break;
				}
				counter_val = (uint32_t) page;
				status = BlockWrite((void *)&counter_val, 41, T2T_WITHOUT_PWD_AUTH, 0);
				if (status == UFR_OK) {
					printf("Increase Counter operation successful.\n");
				} else {
					printf("Error, status is: 0x%08X\n", status);
				}
				break;
			case DL_MIFARE_ULTRALIGHT_EV1_11:
			case DL_MIFARE_ULTRALIGHT_EV1_21:
				printf("    Enter counter address (0, 1 or 2): ");
				nitems = scanf("%d", &page);
				fflush(stdin);
				if ((nitems == EOF) || (nitems == 0)) {
					printf("\nInput error.\n");
					break;
				}
				if ((page < 0) || (page > 2)) {
					printf("\nAddress out of range.\n");
					break;
				}

				printf("    Enter increment value: ");
				nitems = scanf("%d", &input_val);
				fflush(stdin);
				if ((nitems == EOF) || (nitems == 0)) {
					printf("\nInput error.\n");
					break;
				}
				if ((input_val < 0) || (input_val > 0xFFFFFF)) {
					printf("\nIncrement value out of range.\n");
					break;
				}
				counter_val = (uint32_t) input_val;

				status = IncrementCounter(page, counter_val);
				if (status == UFR_OK) {
					printf("Increase Counter operation successful.\n");
				} else {
					printf("Error, status is: 0x%08X\n", status);
				}
				break;
			case DL_NTAG_213:
			case DL_NTAG_215:
			case DL_NTAG_216:
				printf("Card in the field have NFC counter which is incremented with each\n"
						"valid read operation. NFC counter functionality have to be enabled\n"
						"by setting NFC_CNT_EN bit in the ACCESS configuration byte.\n");
				break;
			default:
				printf("Card in field not supported for counter increment operation.\n");
				break;
			}
			print_ln('=');
			break;
			//..................................................................
		case '\x1b':
			status = ReaderClose();
			if (status != UFR_OK) {
				printf("Error while closing device.\n");
				return EXIT_FAILURE;
			}
			loop = 0;
			break;

		case 'H':
		case 'h':
		case 'U':
		case 'u':
			usage();
			break;
		}
	}

	return EXIT_SUCCESS;
}
//------------------------------------------------------------------------------
void usage(void) {
	print_ln('=');
	printf("\nCommand keys (case insensitive):\n");
	print_ln('-');

	printf("  [I] Get reader description.\n"
			"  [O] Get software versions.\n"
			"  [P] Get ID of the tag in reader field.\n"

			"  [A] Set authentication mode.\n"
			"  [S] Show authentication parameters.\n"

			"  [R] Page Read.\n"
			"  [W] Page Write.\n"

			"  [K] Linear Read.\n"
			"  [L] Linear Write.\n"

			"  [B] Show counter capabilities of the card in field.\n"
			"  [C] Get counter value.\n"
			"  [V] Increase counter.\n"

			"[esc] Close device and exit program.\n"
			"[H,U] This help screen.\n");
	print_ln('=');
}
//------------------------------------------------------------------------------
char *GetDlTypeName(uint8_t dl_type_code) {
	static char s[50];

	switch (dl_type_code) {
	case DL_MIFARE_ULTRALIGHT:
		strcpy(s, "DL_MIFARE_ULTRALIGHT");
		break;
	case DL_MIFARE_ULTRALIGHT_EV1_11:
		strcpy(s, "DL_MIFARE_ULTRALIGHT_EV1_11");
		break;
	case DL_MIFARE_ULTRALIGHT_EV1_21:
		strcpy(s, "DL_MIFARE_ULTRALIGHT_EV1_21");
		break;
	case DL_MIFARE_ULTRALIGHT_C:
		strcpy(s, "DL_MIFARE_ULTRALIGHT_C");
		break;
	case DL_NTAG_203:
		strcpy(s, "DL_NTAG_203");
		break;
	case DL_NTAG_210:
		strcpy(s, "DL_NTAG_210");
		break;
	case DL_NTAG_212:
		strcpy(s, "DL_NTAG_212");
		break;
	case DL_NTAG_213:
		strcpy(s, "DL_NTAG_213");
		break;
	case DL_NTAG_215:
		strcpy(s, "DL_NTAG_215");
		break;
	case DL_NTAG_216:
		strcpy(s, "DL_NTAG_216");
		break;
	case DL_MIKRON_MIK640D:
		strcpy(s, "DL_MIKRON_MIK640D");
		break;
	case DL_MIFARE_MINI:
		strcpy(s, "DL_MIFARE_MINI");
		break;
	case DL_MIFARE_CLASSIC_1K:
		strcpy(s, "DL_MIFARE_CLASSIC_1K");
		break;
	case DL_MIFARE_CLASSIC_4K:
		strcpy(s, "DL_MIFARE_CLASSIC_4K");
		break;
	case DL_MIFARE_PLUS_S_2K:
		strcpy(s, "DL_MIFARE_PLUS_S_2K");
		break;
	case DL_MIFARE_PLUS_S_4K:
		strcpy(s, "DL_MIFARE_PLUS_S_4K");
		break;
	case DL_MIFARE_PLUS_X_2K:
		strcpy(s, "DL_MIFARE_PLUS_X_2K");
		break;
	case DL_MIFARE_PLUS_X_4K:
		strcpy(s, "DL_MIFARE_PLUS_X_4K");
		break;
	case DL_MIFARE_DESFIRE:
		strcpy(s, "DL_MIFARE_DESFIRE");
		break;
	case DL_MIFARE_DESFIRE_EV1_2K:
		strcpy(s, "DL_MIFARE_DESFIRE_EV1_2K");
		break;
	case DL_MIFARE_DESFIRE_EV1_4K:
		strcpy(s, "DL_MIFARE_DESFIRE_EV1_4K");
		break;
	case DL_MIFARE_DESFIRE_EV1_8K:
		strcpy(s, "DL_MIFARE_DESFIRE_EV1_8K");
		break;
	case DL_IMEI_UID:
		strcpy(s, "DL_IMEI_UID");
		break;
	default:
		strcpy(s, "UNSUPPORTED CARD");
	}

	return s;
}
//------------------------------------------------------------------------------
void SetAuthModePrintout(void) {

	printf("Set T2T Authorisation mode:\n"
			"     [0] for T2T_NO_PWD_AUTH (without PWD authentication)\n");
	if (is_reader_support_pwd()) {
		printf(
				"     [1] for T2T_RKA_PWD_AUTH (PWD authentication using PWD/PACK stored in reader)\n");
	} else {
		printf(
				"     [1] <DISABLED> this reader type does not support PWD authentication using stored PWD/PACK\n");
	}
	printf(
			"     [2] for T2T_PK_PWD_AUTH (PWD authentication with provided PWD/PACK)\n"
					"   [esc] for cancel\n");
	print_ln('-');
	printf("Enter your choice:\n");

}
//------------------------------------------------------------------------------
void SetAuthMode(void) {
	int loop = 1;
	int nitems;
	uint8_t local_mode;

	SetAuthModePrintout();
	do {
		switch (getch()) {
		case '0':
			local_mode = T2T_NO_PWD_AUTH;
			loop = 0;
			break;

		case '1':
			if (is_reader_support_pwd()) {
				local_mode = T2T_RKA_PWD_AUTH;
				loop = 0;
			} else {
				SetAuthModePrintout();
			}
			break;
		case '2':
			local_mode = T2T_PK_PWD_AUTH;
			loop = 0;
			break;
		case '\x1b':
			print_ln('-');
			printf("Authentication mode setup canceled off.\n");
			return;

		default:
			print_ln('-');
			printf("Wrong choice, try again...\n");
			print_ln('-');
			SetAuthModePrintout();
			break;
		}
	} while (loop);

	switch (local_mode) {

	case T2T_NO_PWD_AUTH:

		printf("You choose T2T_NO_PWD_AUTH (without PWD authentication)\n");
		break;

	case T2T_RKA_PWD_AUTH:

		printf(
				"You choose T2T_RKA_PWD_AUTH (PWD authentication using PWD/PACK stored in reader)\n");
		printf("    PWD/PACK stored in reader index = %d\n",
				getPwdPackReaderIdx());
		printf("    Enter new reader index: ");
		int idx;

		nitems = scanf("%d", &idx);
		if ((nitems == EOF) || (nitems == 0)) {
			printf("\nInput error.\n");
			break;
		}
		if ((idx > -1) && (idx < 32)) {
			setPwdPackReaderIdx((uint8_t) idx);
		} else {
			printf("\nIndex out of range (0 .. 31).\n");
		}
		break;

	case T2T_PK_PWD_AUTH: {
		uint32_t bytes_4;
		uint8_t bytes_arr[4];

		printf("     PWD = ");
		print_hex_ln(getPwd(), 4, ":");
		printf("     PACK = ");
		print_hex_ln(getPack(), 2, ":");

		printf("    Enter new PWD (hex): ");
		nitems = scanf("%8X", &bytes_4);
		if ((nitems == EOF) || (nitems == 0)) {
			printf("\nInput error.\n");
			break;
		}
		for (int i = 0; i < 4; i++) {
			bytes_arr[3 - i] = (uint8_t) (bytes_4 >> (i * 8));
		}
		setPwd(bytes_arr);

		fflush(stdin);
		printf("    Enter new PACK (hex): ");
		nitems = scanf("%4X", &bytes_4);
		if ((nitems == EOF) || (nitems == 0)) {
			printf("\nInput error.\n");
			break;
		}
		for (int i = 0; i < 2; i++) {
			bytes_arr[1 - i] = (uint8_t) (bytes_4 >> (i * 8));
		}
		setPack(bytes_arr);

		break;
	}

	default:
		printf("Unknown authentication mode.\n");
		setAuthMode(T2T_NO_PWD_AUTH);
		return;
	}

	fflush(stdin);
	setAuthMode(local_mode);
}
//------------------------------------------------------------------------------
void print_ln_len(char symbol, uint8_t cnt) {

	for (int i = 0; i < cnt; i++)
		printf("%c", symbol);

	printf("\n");
}
//------------------------------------------------------------------------------
void print_ln(char symbol) {

	print_ln_len(symbol, DEFAULT_LINE_LEN);
}
//------------------------------------------------------------------------------
void print_hex(const uint8_t *data, uint32_t len, const char *delimiter) {

	for (int i = 0; i < len; i++) {
		printf("%02X", data[i]);
		if ((delimiter != NULL) && (i < (len - 1)))
			printf("%c", *delimiter);
	}
}
//------------------------------------------------------------------------------
void print_hex_ln(const uint8_t *data, uint32_t len, const char *delimiter) {

	print_hex(data, len, delimiter);
	printf("\n");
}
//==============================================================================
