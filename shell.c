#include <shell.h>
#include <terminal.h>
#include <configs.h>
#include <uart.h>

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


struct commandRecord commandsRecords[] = {
	{"echo", 	cmdProcedure_echo},
	{"reg", 	cmdProcedure_reg},
	{"reboot", 	cmdProcedure_reboot},
	{"help",	cmdProcedure_help},
	{"arglist", cmdProcedure_arglist},
	{"set",		cmdProcedure_set}
};

bool isNormalChar(char value)
{
	if(value >= 0x20 && value <= 0x7E)
		return true;

	return false; 
};

void echoCmdLinePrompt()
{
	char buffer[256];

	size_t offset = 0;
	size_t lineSize = 0;

	lineSize = strlen(machineName);
	memcpy(buffer, machineName, lineSize);
	offset += lineSize;

	memcpy(buffer + offset, "# ", 2);
	offset += 2;	 
	
	termSGR_fGreen();
	UARTSend(buffer, offset);
	termSGR_reset();	
};

void initShell()
{
	printTerminal("\r\nWelcome to IOSYS v1.0.0\r\n");
	echoCmdLinePrompt();
};

void processCmdLine(char * cmdLine, size_t cmdLineLength)
{
	char *argv[20];
	unsigned int argc;
	size_t charsRemain;
	char * iterator;
	char * curArg;

	argc = 0;
	charsRemain = cmdLineLength;
	curArg = NULL;

	for(iterator = cmdLine; charsRemain--; iterator++)
	{
		if( *iterator == ' ' || *iterator == ASCII_ESC)
		{
			*iterator = '\0';	
			if(curArg) curArg = NULL;

		}
		else
		if(curArg == NULL)
		{
			curArg = iterator;
			if( argc >= 20) break;
			argv[argc++] = iterator;
		}


	}
	*iterator = '\0';

	if(argc > 0)
	{
		unsigned int i;
		bool found;

		found = false;
		for( i = 0; 
			i < sizeof(commandsRecords)/sizeof(struct commandRecord); i++)
			if(strcmp(commandsRecords[i].name, argv[0]) == 0)
			{
				found = true;
				commandsRecords[i].callProcedure(argv[0], argc, argv);
				break;
			}

		if(!found)
		{
			printTerminal("undefine command \"");
			printTerminal(argv[0]);
			printTerminal("\"\r\n");
		}
	}

};

void shell()
{
	static char cmdLine[256];
	static size_t cmdLineLength = 0;

	char buffer[256];
	size_t dataRecv;
	size_t dataSend;
	unsigned int i;

	dataRecv = 0;
	dataSend = 0;
	dataRecv = UARTRecv(buffer, 256);

	for(i = 0; i < dataRecv; i++)
	{

		#define _curChar buffer[i] 

		if(isNormalChar(_curChar))
		{
			cmdLine[cmdLineLength++] = _curChar;
			UARTEcho(_curChar);
		}
		else
		{
			if(_curChar == ASCII_CR)
			{
				printTerminal("\r\n");
				processCmdLine(cmdLine, cmdLineLength);
				cmdLineLength = 0;
				echoCmdLinePrompt();
			}
			else 
			if(_curChar == ASCII_BS || _curChar == ASCII_DEL)
			{
				if(cmdLineLength)
				{
					printTerminal("\b \b");
					cmdLineLength--;
				}
			}
			else
			if(_curChar == ASCII_ETX)
			{
				printTerminal("\r\nbreak command\r\n");		
				cmdLineLength = 0;
				echoCmdLinePrompt();
			};
		};

		#undef _curChar
	};
};


