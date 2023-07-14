#ifndef __TERMINAL_H__
#define __TERMINAL_H__

	#define ASCII_CR 	0x0D
	#define ASCII_LF 	0x0A
	#define ASCII_ESC 	0x1B
	#define ASCII_BS 	0x08
	#define ASCII_DEL 	0x7F
	#define ASCII_ETX 	0x03

	void printTerminal(const char * );
	void lfTerminal();
	void termSGR_reset();
	void termSGR_fGreen();

#endif

