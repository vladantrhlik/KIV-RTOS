#include <stdfile.h>
#include <stdstring.h>
#include <stdfile.h>

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

int main(int argc, char** argv)
{
	uint32_t log = pipe("log", 32);
	uint32_t pipe_from2 = pipe("2to4", sizeof(float));
	uint32_t pipe_from3 = pipe("3to4", sizeof(float));
	float glu_avg, ins;

	while(true)
	{
		uint32_t v1 = read(pipe_from2, (char*)(&glu_avg), sizeof(float));
		uint32_t v2 = read(pipe_from3, (char*)(&ins), sizeof(float));

		// TODO print to display
		if (v1 > 0 || v2 > 0) {
			write(log, "GLU: ", 5);
			print_float(log, glu_avg, 2);
			write(log, "INS: ", 5);
			print_float(log, ins, 2);
		}
	}
    return 0;
}
