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

	uint32_t proc_pid = open("PROC:self/pid", NFile_Open_Mode::Read_Only);
	uint32_t proc_fd = open("PROC:self/fd", NFile_Open_Mode::Read_Only);
	uint32_t proc_status = open("PROC:self/status", NFile_Open_Mode::Read_Only);
	uint32_t proc_state = open("PROC:self/state", NFile_Open_Mode::Read_Only);

	uint32_t proc1_pid = open("PROC:1/pid", NFile_Open_Mode::Read_Only);
	uint32_t proc1_fd = open("PROC:1/fd", NFile_Open_Mode::Read_Only);
	uint32_t proc1_status = open("PROC:1/status", NFile_Open_Mode::Read_Only);
	uint32_t proc1_state = open("PROC:1/state", NFile_Open_Mode::Read_Only);

	uint32_t procs[] = {proc_pid, proc_fd, proc_state, proc_status, proc1_pid, proc1_fd, proc1_state, proc1_status};
	uint32_t proc_count = sizeof(procs) / sizeof(uint32_t);

	char buffer[64];
	volatile int tim;

	while (true)
	{
		for (int i = 0; i < proc_count; i++) {
			bzero(buffer, 64);
			read(procs[i], buffer, 64);
			fputs(log, buffer);
		}

		sleep(0x500);
	}

    return 0;
}