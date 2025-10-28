#include <stdstring.h>
#include <stdfile.h>
#include <stdmutex.h>

#include <drivers/gpio.h>
#include <process/process_manager.h>

/**
 * Counter task
 * 
 * Na 7-segmentovem displeji zobrazuje cisla od 0 do 9 (resp. obracene), a to:
 *  - vzestupne pokud je prepinac 1 v poloze "zapnuto", jinak sestupne
 *  - rychle pokud je prepinac 2 v poloze "zapnuto", jinak pomalu
 **/

int main(int argc, char** argv)
{
	uint32_t display_file = open("DEV:segd", NFile_Open_Mode::Write_Only);
	uint32_t switch1_file = open("DEV:gpio/4", NFile_Open_Mode::Read_Only);
	uint32_t switch2_file = open("DEV:gpio/17", NFile_Open_Mode::Read_Only);

	unsigned int counter = 0;
	bool fast = false;
	bool ascending = true;

	set_task_deadline(fast ? 0x1000 : 0x2800);

	while (true)
	{
		char tmp = '0';

		read(switch1_file, &tmp, 1);
		ascending = (tmp == '1');

		read(switch2_file, &tmp, 1);
		fast = (tmp == '1');

		if (ascending)
			counter++;
		else
			counter--;

		tmp = '0' + (counter % 2);
		write(display_file, &tmp, 1);

		sleep(fast ? 0x400 : 0x600, fast ? 0x1000 : 0x2800);
	}

	close(display_file);
	close(switch1_file);
	close(switch2_file);

    return 0;
}
