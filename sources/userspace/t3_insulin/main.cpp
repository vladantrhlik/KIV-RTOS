#include <stdfile.h>
#include <stdstring.h>
#include <stdfile.h>

int main(int argc, char** argv)
{
	uint32_t pipe_to4 = pipe("3to4", sizeof(float));

	uint32_t sem_31 = open("SYS:sem/sem3to1", NFile_Open_Mode::Read_Write);
	uint32_t pipe_to1 = pipe("3to1", sizeof(float));

	uint32_t sem_23 = open("SYS:sem/sem2to3", NFile_Open_Mode::Read_Write);
	uint32_t pipe_from2 = pipe("2to3", sizeof(float));
	

	uint32_t log = pipe("log", 32);
	while(true)
	{
		wait(sem_23);
		float avg;
		uint32_t v = read(pipe_from2, (char*)(&avg), sizeof(float));

		if (v > 0) {
			// TODO pid regulátor pro výpočet insulinu
			float D = -.5;
			float P = .1;
			float ins = D + P * avg;
			if (ins < 0) ins = 0;

			write(pipe_to4, (char*)(&ins), sizeof(float));
			write(pipe_to1, (char*)(&ins), sizeof(float));
			notify(sem_31);
		}
	}
    return 0;
}
