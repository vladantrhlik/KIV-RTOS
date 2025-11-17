#include <stdfile.h>
#include <stdstring.h>
#include <stdfile.h>

int main(int argc, char** argv)
{
	const int WINDOW = 5;

	uint32_t log = pipe("log", 32);
	uint32_t pipe_from1 = pipe("1to2", sizeof(float));
	uint32_t pipe_to4 = pipe("2to4", sizeof(float));

	float g = 0;
	int count = 0;
	float sum;

	while(true)
	{
		// read data from task 1
		uint32_t v = read(pipe_from1, (char*)(&g), sizeof(float));
		if (v > 0)
		{
			sum += g;
			count++;
			if (count == WINDOW) {
				float avg = sum / WINDOW;

				// send data to display
				write(pipe_to4, (char*)(&avg), sizeof(float));

				// TODO: send avg to 3

				count = 0;
				sum = 0;
			}
		}
	}
    return 0;
}
