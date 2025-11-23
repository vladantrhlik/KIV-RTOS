#include <stdfile.h>
#include <stdstring.h>
#include <stdfile.h>

#include "patient/patient.cpp" // TODO: include .h, prasárna fuj

int main(int argc, char** argv)
{

	CVirtual_Patient patient;
	bool fast = false;

	uint32_t log = pipe("log", 32);
	uint32_t pipe_to2 = pipe("1to2", sizeof(float));

	uint32_t pipe_from3 = pipe("3to1", sizeof(float));
	uint32_t sem_31 = open("SYS:sem/sem3to1", NFile_Open_Mode::Read_Write);

	uint32_t switch1_file = open("DEV:gpio/4", NFile_Open_Mode::Read_Only);

	while (true)
	{
		patient.Step();
		float g = patient.Get_Current_Glucose();

		// send g to task 2
		write(pipe_to2, (char*)(&g), sizeof(float));

		write(log, "1", 2);
		
		wait(sem_31, 1, 0x100);
		float ins;
		if (read(pipe_from3, (char*)(&ins), sizeof(float)) > 0) {
			patient.Dose_Insulin(ins);
		}

		// 1s / 50ms sleep
		char tmp = '0';
		read(switch1_file, &tmp, 1);
		fast = (tmp == '1');
		sleep(fast ? 0x100 : 0x500);
	}
    return 0;
}
