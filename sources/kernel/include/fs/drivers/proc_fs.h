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
        CProc_PID_File(int pid) : IFile(NFile_Type_Major::Character), _pid(pid) { }

        ~CProc_PID_File()
        {
            Close();
        }

        virtual uint32_t Read(char* buffer, uint32_t num) override
        {
            TTask_Struct *task = sProcessMgr.Get_Process_By_PID(_pid);
            if (!task) return 0;

            // state to string
            char *state_str;
            switch (task->state)
            {
                case NTask_State::New: state_str = const_cast<char*>("new"); break;
                case NTask_State::Runnable: 
                case NTask_State::Running: 
                    state_str = const_cast<char*>("runnable"); 
                    break;
                case NTask_State::Interruptable_Sleep: state_str = const_cast<char*>("sleep"); break;
                case NTask_State::Blocked: state_str = const_cast<char*>("blocked"); break;
                case NTask_State::Zombie: state_str = const_cast<char*>("zombie"); break;
                default:
                    break;
            }

            // count opened files
            uint8_t f = 0;
            for (int i = 0; i < Max_Process_Opened_Files; i++) {
                if (task->opened_files[i] != nullptr) f++;
            }

            char buf[64];
            bzero(buf, 64);

            bzero(buf, num);
            strcat(buf, "PID: ");
            itoa(_pid, buf + strlen(buf), 10);
            strcat(buf, "\nstate: ");
            strcat(buf, state_str);
            strcat(buf, "\nopended files: ");
            itoa(f, buf + strlen(buf), 10);

            int l = strlen(buf);
            strncpy(buffer, buf, l < num ? l : num);

            return 1;
        }
};

class CProc_Status_File final : public IFile {
    public:
        enum StatusType {
            SCHED = 0,
            TASKS
        };

        CProc_Status_File(StatusType type) : IFile(NFile_Type_Major::Character), _type(type) { }

        ~CProc_Status_File()
        {
            Close();
        }

        virtual uint32_t Read(char* buffer, uint32_t num) override {
            char buf[64];
            bzero(buf, 64);

            CProcess_Summary_Info info;
            sProcessMgr.Get_Scheduler_Info(NGet_Sched_Info_Type::Process_Summary, &info);

            switch (_type) {
                case SCHED:
                    strcat(buf, "\nrunnable: ");
                    itoa(info.running, buf + strlen(buf), 10);
                    strcat(buf, "\nblocked: ");
                    itoa(info.blocked, buf + strlen(buf), 10);
                    strcat(buf, "\nzombie: ");
                    itoa(info.zombie, buf + strlen(buf), 10);
                    break;
                case TASKS:
                    strcat(buf, "task count: ");
                    itoa(info.total, buf + strlen(buf), 10);
                    break;
            };

            int l = strlen(buf);
            strncpy(buffer, buf, l < num ? l : num);

            return 1;
        }
    private:
        StatusType _type;
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
            } else {
                if (strncmp(path, "sched", 5) == 0) return new CProc_Status_File(CProc_Status_File::StatusType::SCHED);
                if (strncmp(path, "tasks", 5) == 0) return new CProc_Status_File(CProc_Status_File::StatusType::TASKS);
            }

            return nullptr;
        }
};

CProc_FS_Driver fsProc_FS_Driver;
