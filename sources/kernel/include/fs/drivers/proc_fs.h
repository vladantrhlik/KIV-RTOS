#pragma once

#include <drivers/gpio.h>
#include <hal/peripherals.h>
#include <memory/kernel_heap.h>
#include <fs/filesystem.h>
#include <stdstring.h>
#include <process/process_manager.h>

// virtualni soubor pro proces
class CProc_File final : public IFile
{
    private:
        char name[32];
    public:
        CProc_File(const char *path) : IFile(NFile_Type_Major::Character)
        {
            strncpy(name, path, strlen(path) + 1);
        }

        ~CProc_File()
        {
            Close();
        }

        virtual uint32_t Read(char* buffer, uint32_t num) override
        {
            memset(buffer, 0, num);
            strncpy(buffer, "Ahoj z PROCFS: ", 15);
            strncpy(buffer + 15, name, strlen(name) + 1);
            return 1;
        }
};

// driver pro proc filesystem 
class CProc_FS_Driver : public IFilesystem_Driver
{
	public:
		virtual void On_Register() override
        {
            //
        }

        virtual IFile* Open_File(const char* path, NFile_Open_Mode mode) override
        {
            CProc_File* f = new CProc_File(path);
            return f;
        }
};

CProc_FS_Driver fsProc_FS_Driver;
