#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>
#include <errno.h>


#include "lib_general.h"
#include "lib_watchdog.h"




int main(int argc, char *argv[])
{
	int timeout = 0;
	
	lib_wdt_init();

	timeout = lib_wdt_get_timeout();
	fprintf(stderr, "Reboot WDT timeout: %d\n", timeout);

	lib_sleep(1);
	
	lib_wdt_system_reboot();

	lib_wdt_release();

	return 0;
}


