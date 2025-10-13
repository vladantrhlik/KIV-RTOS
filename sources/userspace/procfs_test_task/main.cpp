#include <stdstring.h>
#include <stdfile.h>
#include <stdmutex.h>

#include <oled.h>

#include <drivers/bridges/uart_defs.h>
#include <drivers/gpio.h>
#include <drivers/uart.h>

#include <process/process_manager.h>

/**
 * Displejovy task
 * 
 * Zobrazuje hlasky na OLED displeji, a pokud prijde udalost od jinych tasku, zobrazi neco relevantniho k nim
 **/

const char* messages[] = {
	"Zdarec sranec",
	"gcc -o",
};

static void fputs(uint32_t file, const char* string)
{
	write(file, string, strlen(string));
}

int main(int argc, char** argv)
{
	COLED_Display disp("DEV:oled");
	disp.Clear(false);
	disp.Put_String(10, 10, "PROC FS init...");
	disp.Flip();

	uint32_t log = pipe("log", 32);

	uint32_t proc1 = open("PROC:1", NFile_Open_Mode::Read_Only);
	uint32_t self = open("PROC:self", NFile_Open_Mode::Read_Only);
	uint32_t tasks = open("PROC:tasks", NFile_Open_Mode::Read_Only);
	uint32_t sched = open("PROC:sched", NFile_Open_Mode::Read_Only);

	uint32_t procs[] = {proc1, self, tasks, sched};

	char buffer[64];
	volatile int tim;

	while (true)
	{
		for (int i = 0; i < 4; i++) {
			bzero(buffer, 64);
			read(procs[i], buffer, 64);
			fputs(log, buffer);
		}

		sleep(0x500);
	}

    return 0;
}