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

	uint32_t proc_file = open("PROC:ahoj", NFile_Open_Mode::Read_Only);
	char buffer[32];
	volatile int tim;

	while (true)
	{
		bzero(buffer, 32);
		if (proc_file > 0) {
			read(proc_file, buffer, 32);
			fputs(log, buffer);
			fputs(log, "file found!");
		} else {
			fputs(log, "no file found");
		}


		sleep(0x100);
	}

    return 0;
}