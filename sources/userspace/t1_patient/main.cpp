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
	uint32_t switch1_file = open("DEV:gpio/4", NFile_Open_Mode::Read_Only);

	while (true)
	{
		patient.Step();
		float g = patient.Get_Current_Glucose();

		// send g to task 2
		write(pipe_to2, (char*)(&g), sizeof(float));

		// TODO: read from 3

		// 1s / 50ms sleep
		char tmp = '0';
		read(switch1_file, &tmp, 1);
		fast = (tmp == '1');
		sleep(fast ? 0x100 : 0x600);
	}
    return 0;
}
