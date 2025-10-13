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
    public:
        enum Type {
            PID,
            STATUS,
            STATE,
            FD
        };

        CProc_PID_File(int pid, Type type) : IFile(NFile_Type_Major::Character), _pid(pid), _type(type) { }

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
            if (_type == STATE || _type == STATUS) {
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
            }

            // count opened files
            uint8_t f = 0;
            if (_type == STATUS || _type == FD) {
                for (int i = 0; i < Max_Process_Opened_Files; i++) {
                    if (task->opened_files[i] != nullptr) f++;
                }
            }

            char buf[64];
            bzero(buf, 64);

            switch (_type)
            {
                case PID:
                    itoa(_pid, buf, 10);
                    break;
                case STATUS:
                    strcat(buf, "PID: ");
                    itoa(_pid, buf + strlen(buf), 10);
                    strcat(buf, "\nstate: ");
                    strcat(buf, state_str);
                    strcat(buf, "\nopended files: ");
                    itoa(f, buf + strlen(buf), 10);
                    break;
                case STATE:
                    strcat(buf, state_str);
                    break;
                case FD:
                    itoa(f, buf, 10);
                    break;
            }

            int l = strlen(buf);
            bzero(buffer, num);
            strncpy(buffer, buf, l < num ? l : num);

            return 1;
        }

    private:
        int _pid;
        Type _type;
};

class CProc_Status_File final : public IFile {
    public:
        enum Type {
            SCHED = 0,
            TASKS,
            TICKS
        };

        CProc_Status_File(Type type) : IFile(NFile_Type_Major::Character), _type(type) { }

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
                    strcat(buf, "runnable: ");
                    itoa(info.running, buf + strlen(buf), 10);
                    strcat(buf, "\nblocked: ");
                    itoa(info.blocked, buf + strlen(buf), 10);
                    strcat(buf, "\nzombie: ");
                    itoa(info.zombie, buf + strlen(buf), 10);
                    break;
                case TASKS:
                    itoa(info.total, buf, 10);
                    break;
                case TICKS:
                    uint32_t ticks;
                    sProcessMgr.Get_Scheduler_Info(NGet_Sched_Info_Type::Tick_Count, &ticks);
                    itoa(ticks, buf, 10);
                    break;
            };

            int l = strlen(buf);
            strncpy(buffer, buf, l < num ? l : num);

            return 1;
        }
    private:
        Type _type;
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
            while (*s && (*s) != '/') {
                if (*s < '0' || *s > '9') is_pid = false;
                s++;
            }
            
            bool self = strncmp(path, "self", 4) == 0;
            if (is_pid || self ) {
                *s++ = '\0'; // end pid string (replace '/') for atoi

                // resolve pid
                uint32_t pid = 0;
                TTask_Struct *task = nullptr;
                if (self) {
                    task = sProcessMgr.Get_Current_Process();
                    if (!task) return nullptr;
                    pid = task->pid;
                } else {
                    pid = atoi(path);
                    // test if valid pid
                    task = sProcessMgr.Get_Process_By_PID(pid);
                    if (!task) return nullptr;
                }

                if (strncmp(s, "pid", 3) == 0) return new CProc_PID_File(pid, CProc_PID_File::Type::PID);
                if (strncmp(s, "fd", 2) == 0) return new CProc_PID_File(pid, CProc_PID_File::Type::FD);
                if (strncmp(s, "status", 6) == 0) return new CProc_PID_File(pid, CProc_PID_File::Type::STATUS);
                if (strncmp(s, "state", 5) == 0) return new CProc_PID_File(pid, CProc_PID_File::Type::STATE);

                return nullptr;
            } else {
                if (strncmp(path, "sched", 5) == 0) return new CProc_Status_File(CProc_Status_File::Type::SCHED);
                if (strncmp(path, "tasks", 5) == 0) return new CProc_Status_File(CProc_Status_File::Type::TASKS);
                if (strncmp(path, "ticks", 5) == 0) return new CProc_Status_File(CProc_Status_File::Type::TICKS);
            }

            return nullptr;
        }
};

CProc_FS_Driver fsProc_FS_Driver;
