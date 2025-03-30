#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include "readcmd.h"


void trace_cmd(char ***cmd) {
		int i, j;
		/* Display each command of the pipe */
		/* cmd is a table of table of strings, each table ends with a NULL element */
		printf("trace_cmd\n");
		for (i=0; cmd[i]!=0; i++) {
			char **cmdi = cmd[i];
			printf("cmd[%d]: ", i);
			for (j=0; cmdi[j]!=0; j++) {
				printf("(%s) ", cmdi[j]);
			}
			printf("\n");
		}
}


int main()
{
	int spid, status;
	char ***cmd;
	
	while(1) {
		printf("shell> ");

		cmd = readcommand();
		trace_cmd(cmd);
		
		if (! cmd[0]) continue;

		if (!strcasecmp(cmd[0][0], "exit")) {
			return 0;
		}rcasecmp(cmd[0][0], "exit")) {
			return 0;
		}
	}
}
