#include <shell.h>
#include <terminal.h>
#include <configs.h>
#include <format.h>

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>


const char sNotImpl[] = "not implemented\r\n";

extern const size_t registersNumber;
extern uint16_t registers[];

int cmdProcedure_reg(char * command, unsigned int argc,
	const char ** argv)
{
	struct registerAlias
	{
		const char * name;
		uint16_t * pRegister;
	};

	static const struct registerAlias aliases[] = 
	{
		{"temp", 	registers + 0},
		{"testVal",	registers + 1}
	};

	const char * findAlias(void * pReg)
	{
		unsigned int j;
		for( j = 0; j < sizeof(aliases)/sizeof(struct registerAlias); j++)
			if(pReg == aliases[j].pRegister)
				return aliases[j].name;

		return NULL;
	};

	char textBuffer[128];
	char valueBuffer[9];
	const char * alias;
	size_t iReg;
	size_t wide;

	static const char sRegListHeader[] = 
	"reg#     value     alias\r\n"
    "--------------------------\r\n";
	if(argc == 1)
	{
		printTerminal(sRegListHeader);
		for( iReg = 0; iReg < registersNumber; iReg++)
		{
			wide = pHEXUInt16_t(valueBuffer, 8, registers[iReg]);
			valueBuffer[wide] = '\0';
			sprintf(textBuffer,"%3d      %8s  ", iReg, valueBuffer);
			printTerminal(textBuffer);

			alias = findAlias(registers + iReg);
			if(alias)
				printTerminal(alias);

			lfTerminal();
		};
	}
	else
	{
		unsigned int iArg;
		const char * arg;
		for( iArg = 1; iArg < argc; iArg++)
		{
			arg = argv[iArg];
			if(arg[0] == '-')
			{

			}
		}
	}

	return 0;
};

int cmdProcedure_set(char * command, unsigned int argc,
	const char ** argv)
{
	printTerminal(sNotImpl);
	return 0;
};


int cmdProcedure_reboot(char * command, unsigned int argc,
	const char ** argv)
{
	printTerminal(sNotImpl);
	return 0;
};

int cmdProcedure_echo(char * command, unsigned int argc,
	const char ** argv)
{
	char textBuff[80];
	unsigned int i;

	for( i = 1; i < argc; i++)
	{
		printTerminal(argv[i]);
		if(i != argc - 1)
			printTerminal(" ");
	}
	lfTerminal();

	return 0;
};

int cmdProcedure_arglist(char * command, unsigned int argc,
	const char ** argv)
{
	char textBuff[80];
	if( argc == 0 )
	{
		//printTerminal("not enoth arguments\r\n");
		return 1;
	}

	unsigned int i;
	for( i = 1; i < argc; i++)
	{
		sprintf(textBuff, "arg[%d]: \"%s\"\r\n", i, argv[i]);
		printTerminal(textBuff);
	}
		

	return 0;
};

int cmdProcedure_help(char * command, unsigned int argc,
	const char ** argv)
{
	/*
	printTerminal(
		"help      - current message\r\n"
		"echo      - echo message back\r\n"
		"arglist   - print list of present arguments\r\n"
		"reg	   - register read/write utilit\r\n"
		"reboot	   - reboot host\r\n"
		"set	   - set config variable\r\n"
	);
	*/

	return 0;
};

