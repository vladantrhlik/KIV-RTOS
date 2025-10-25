#import "@preview/touying:0.6.1": *
#import "@preview/treet:1.0.0": *
#import "@preview/numbly:0.1.0": numbly
#import "@preview/codly:1.3.0": *
#import "@preview/codly-languages:0.1.1": *
#show: codly-init.with()

#import themes.simple: *
#show: simple-theme.with(
  aspect-ratio: "16-9",
)
// Reset raw blocks to the same size as normal text,
// but keep inline raw at the reduced size.
#show raw.where(block: true): set text(1em / 0.8)
#set text(size: .8em)


//#title-slide()
//#outline-slide(title: "Obsah")
#title-slide()[
  = Implementace PROC FS
  #v(2em)
  Vladan Trhlík
]

== Cíl
- vytvoření procfs mount point v dosavadním FS KIV-RTOS
- musí obsahovat:
    - aktuálních počet tasků
    - informace plánovače
    - celkový počet otevřených souborů
    - informace ke každému tasku (PID, počet otevřených souborů...)

= Struktura FS
== Přidání mount pointu
#tree-list[
- `DEV`
- `MNT`
- `SYS`
- #highlight[`PROC`]
]

== Složky procesů
#tree-list()[
- `PROC`
    - `1/`
    - $dots$
    - `[pid]`/
        - `state` - stav tasku (runnable, blocked atd)
        - `fd_n` - počet otevřených souborů
        - `fd` - otevřené soubory
        - `pid` - PID tasku
        - `page` - počet alokovaných stránek
        - `status` - shrnutí stavu (human readable)
    - `self/`
    - $dots$
]

== Informace o systému
#tree-list[
- `PROC`
    - $dots$
    - `sched` - počet runnable, blocked tasků
    - `tasks` - aktuální počet tasků
    - `ticks` - počet tiků od startu (obodoba `/proc/uptime`)
]

= Implementace
== Mount point
- `fs/filesystem.h` -- `CFileSystem`:
#codly(
    zebra-fill: none,
    display-name: false,
    offset: 142,
    highlights: ((line: 146, start: 0, end: none, fill: blue),),
)
```cpp
TFS_Tree_Node mRoot_Dev;
TFS_Tree_Node mRoot_Sys;
TFS_Tree_Node mRoot_Mnt;
TFS_Tree_Node mRoot_Proc;
```

== FS Driver
- `fs/drivers/proc_fs.h` -- třída driveru
#codly(number-format: none)
```cpp
class CProc_FS_Driver : public IFilesystem_Driver {
    IFile* Open_File(const char* path, NFile_Open_Mode mode) 
    { ... }
}
```

== Soubor Tasku
#codly(number-format: none)
```cpp
class CProcFS_PID_File final : public IFile {
    CProcFS_PID_File(int pid, NProcFS_PID_Type type) { ... }
    uint32_t Read(char* buffer, uint32_t num) { ... }
}

enum NProcFS_PID_Type {
    PID, STATE, FD_N, FD, STATUS, PAGE
};
```

== Soubor systémové informace
#codly(number-format: none)
```cpp
class CProcFS_Status_File final : public IFile
    CProcFS_Status_File(NProcFS_Status_Type type) { ... }
    uint32_t Read(char* buffer, uint32_t num) { ... }
}

enum NProcFS_Status_Type {
    SCHED, TASKS, TICKS 
};
```
