#ifndef __SHELL_H__
#define __SHELL_H__

struct commandRecord 
{
	const char * name;
	int (*callProcedure) (char *, unsigned int , 
		const char **);
};

void initShell();
void shell();

int cmdProcedure_help(char *, unsigned int,	const char ** );
int cmdProcedure_echo(char *, unsigned int,	const char ** );
int cmdProcedure_read(char *, unsigned int,	const char ** );
int cmdProcedure_write(char *, unsigned int,	const char ** );
int cmdProcedure_reboot(char *, unsigned int,	const char ** );
int cmdProcedure_arglist(char *, unsigned int,	const char ** );
int cmdProcedure_set(char *, unsigned int,	const char ** );
int cmdProcedure_reg(char *, unsigned int,	const char ** );
#endif
