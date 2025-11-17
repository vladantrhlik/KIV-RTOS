#include <interrupt_controller.h>

#include <drivers/timer.h>
#include <process/process_manager.h>
#include <fs/filesystem.h>

#include <drivers/uart.h>

extern "C" void Timer_Callback()
{
	sProcessMgr.Schedule();
}

extern "C" unsigned char __init_task_elf[];
extern "C" unsigned int __init_task_elf_len;

extern "C" unsigned char __logger_task_elf[];
extern "C" unsigned int __logger_task_elf_len;

extern "C" unsigned char __t1_patient_elf[];
extern "C" unsigned int __t1_patient_elf_len;

extern "C" unsigned char __t2_sensor_elf[];
extern "C" unsigned int __t2_sensor_elf_len;

extern "C" unsigned char __t3_insulin_elf[];
extern "C" unsigned int __t3_insulin_elf_len;

extern "C" unsigned char __t4_display_elf[];
extern "C" unsigned int __t4_display_elf_len;

extern "C" int _kernel_main(void)
{
	// inicializace souboroveho systemu
	sFilesystem.Initialize();

	// vytvoreni hlavniho systemoveho (idle) procesu
	sProcessMgr.Create_Process(__init_task_elf, __init_task_elf_len, true);

	// vytvoreni vsech tasku
	/*
	sProcessMgr.Create_Process(__counter_task_elf, __counter_task_elf_len, false);
	sProcessMgr.Create_Process(__logger_task_elf, __logger_task_elf_len, false);
	sProcessMgr.Create_Process(__procfs_test_task_elf, __procfs_test_task_elf_len, false);
	*/

	sProcessMgr.Create_Process(__t1_patient_elf, __t1_patient_elf_len, false);
	sProcessMgr.Create_Process(__t2_sensor_elf, __t2_sensor_elf_len, false);
	sProcessMgr.Create_Process(__t3_insulin_elf, __t3_insulin_elf_len, false);
	sProcessMgr.Create_Process(__t4_display_elf, __t4_display_elf_len, false);
	sProcessMgr.Create_Process(__logger_task_elf, __logger_task_elf_len, false);

	// zatim zakazeme IRQ casovace
	sInterruptCtl.Disable_Basic_IRQ(hal::IRQ_Basic_Source::Timer);

	// nastavime casovac - v callbacku se provadi planovani procesu
	sTimer.Enable(Timer_Callback, 0x80, NTimer_Prescaler::Prescaler_1);

	// povolime IRQ casovace
	sInterruptCtl.Enable_Basic_IRQ(hal::IRQ_Basic_Source::Timer);

	// povolime IRQ (nebudeme je maskovat) a od tohoto momentu je vse v rukou planovace
	sInterruptCtl.Set_Mask_IRQ(false);

	// vynutime prvni spusteni planovace
	sProcessMgr.Schedule();

	// tohle uz se mockrat nespusti - dalsi IRQ preplanuje procesor na nejaky z tasku (bud systemovy nebo uzivatelsky)
	while (true)
		;
	
	return 0;
}
