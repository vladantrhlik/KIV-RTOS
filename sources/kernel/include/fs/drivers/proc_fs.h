#pragma once

#include <drivers/gpio.h>
#include <hal/peripherals.h>
#include <memory/kernel_heap.h>
#include <fs/filesystem.h>
#include <stdstring.h>
#include <process/process_manager.h>

// virtualni soubor pro proces
class CProc_PID_File final : public IFile
{
    private:
        int _pid;
    public:
        CProc_PID_File(int pid) : IFile(NFile_Type_Major::Character), _pid(pid)
        {
        }

        ~CProc_PID_File()
        {
            Close();
        }

        virtual uint32_t Read(char* buffer, uint32_t num) override
        {
            memset(buffer, 0, num);
            strncpy(buffer, "Ahoj z PROCFS: ", 15);
            itoa(_pid, buffer + 15, 10);
            return 1;
        }
};

// driver for proc filesystem 
class CProc_FS_Driver : public IFilesystem_Driver
{
	public:
		virtual void On_Register() override
        {
            //
        }

        virtual IFile* Open_File(const char* path, NFile_Open_Mode mode) override
        {
            // check if all nums -> PID
            bool is_pid = true;
            char *s = const_cast<char*>(path);
            while (*s) {
                if (*s < '0' || *s > '9') is_pid = false;
                s++;
            }
            
            if (is_pid) {
                uint32_t pid = atoi(path);
                // check if process exists
                if (sProcessMgr.Get_Process_By_PID(pid)) {
                    return new CProc_PID_File(pid);
                } else {
                    return nullptr;
                }
            }

            return nullptr;
        }
};

CProc_FS_Driver fsProc_FS_Driver;
