#include <stdfile.h>
#include <stdstring.h>
#include <stdfile.h>
#include <oled.h>
#include <drivers/bridges/uart_defs.h>

void print_float(uint32_t fd, float value, int spaces) {
	char buff[10];

	// int part
	bzero(buff, 10);
	itoa((int)value, buff, 10);
	write(fd, buff, 10);

	// ...,...
	write(fd, ".", 2);

	// float part
	value -= (int) value;
	for (int i = 0; i < spaces; i++) {
		value *= 10;
	}
	bzero(buff, 10);
	itoa((int)value, buff, 10);
	write(fd, buff, 10);
}

void ftoa(char *buffer, int len, float value, int spaces = 2) {
	bzero(buffer, len);

	// int part
	if (value < 0) {
		buffer[0] = '-';
		value *= -1;
		itoa((int)value, buffer + 1, 10);
	} else {
		itoa((int)value, buffer, 10);
	}
	// ...,...
	strcat(buffer, ".");
	// float part
	value -= (int) value;
	for (int i = 0; i < spaces; i++) {
		value *= 10;
	}

	char tmp[spaces + 1];
	bzero(tmp, spaces + 1);
	itoa((int)value, tmp, 10);
	strcat(buffer, tmp);
}

int main(int argc, char** argv)
{
	// setup uart
	/*
	uint32_t uart_file = open("DEV:uart/0", NFile_Open_Mode::Write_Only);

	TUART_IOCtl_Params params;
	params.baud_rate = NUART_Baud_Rate::BR_115200;
	params.char_length = NUART_Char_Length::Char_8;
	ioctl(uart_file, NIOCtl_Operation::Set_Params, &params);
	*/
	// oled
	COLED_Display disp("DEV:oled");
	disp.Clear(false);
	disp.Put_String(0, 0, "KIV-RTOS sem. 2");
	disp.Put_String(0, 16, "Vladan Trhlik");
	disp.Flip();

	// IPC
	uint32_t log = pipe("log", 32);
	uint32_t pipe_from2 = pipe("2to4", sizeof(float));
	uint32_t pipe_from3 = pipe("3to4", sizeof(float));

	float glu_avg = 0, ins = 0;

	char buffer[64], float_buffer[64];
	bzero(buffer, 64);

	while(true)
	{
		uint32_t v1 = read(pipe_from2, (char*)(&glu_avg), sizeof(float));
		uint32_t v2 = read(pipe_from3, (char*)(&ins), sizeof(float));


		// TODO print to display and uart
		if (v1 > 0 || v2 > 0) {
			bzero(buffer, 64);
			strcat(buffer, "GLU: ");
			ftoa(float_buffer, 64, glu_avg, 3);
			strcat(buffer, float_buffer);

			int new_line_pos = strlen(buffer);

			strcat(buffer, "\nINS: ");
			ftoa(float_buffer, 64, ins, 3);
			strcat(buffer, float_buffer);
			strcat(buffer, "\n");

			write(log, buffer, strlen(buffer) + 1);

			buffer[new_line_pos] = '\0';
			// update oled
			disp.Clear(false);
			disp.Put_String(0, 0, buffer);
			disp.Put_String(0, 16, buffer + new_line_pos + 1);
			disp.Flip();
		}
	}
    return 0;
}
