#include <stdstring.h>
#include <stdfile.h>
#include <stdmutex.h>

#include <oled.h>

#include <drivers/bridges/uart_defs.h>
#include <drivers/gpio.h>

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

int main(int argc, char** argv)
{
	COLED_Display disp("DEV:oled");
	disp.Clear(false);
	disp.Put_String(10, 10, "PROC FS init...");
	disp.Flip();

	sleep(0x800, 0x800);

	uint32_t proc_file = open("PROC:1/stats", NFile_Open_Mode::Read_Only);
	char buffer[32];

	while (true)
	{
		read(proc_file, buffer, 32);

		disp.Clear(false);
		disp.Put_String(0, 0, buffer);
		disp.Flip();

		sleep(0x4000, 0x800); // TODO: z tohohle bude casem cekani na podminkove promenne (na eventu) s timeoutem
	}

    return 0;
}