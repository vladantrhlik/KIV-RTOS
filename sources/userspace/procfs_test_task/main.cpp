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

	const char *files[] = {
		"PROC:self/pid","PROC:self/fd", "PROC:self/fd_n", "PROC:self/status","PROC:self/state","PROC:self/page",
		"PROC:sched","PROC:ticks" 
	};

	int file_count = sizeof(files) / sizeof(char*);
	uint32_t fds[file_count];

	for (int i = 0; i < file_count; i++) fds[i] = open(files[i], NFile_Open_Mode::Read_Only);

	char buffer[64];
	volatile int tim;

	while (true)
	{
		for (int i = 0; i < file_count; i++) {
			bzero(buffer, 64);
			read(fds[i], buffer, 64);

			fputs(log, buffer);
		}
		fputs(log, "--------");

		sleep(0x500);
	}

    return 0;
}