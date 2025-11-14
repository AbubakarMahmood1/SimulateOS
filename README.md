Welcome to the MiniOS Project - a comprehensive simulation of a modern operating
system that incorporates all fundamental OS concepts studied in this course. This hobby
project will touch upon process management, memory
allocation, scheduling algorithms, and system resource management.

System Requirements:
 Language: C/C++ (recommended) or Python
 Platform: Linux/Unix environment
 Graphics: Optional (Bonus: Use graphics library like SDL, GTK, or ncurses)
Hardware Simulation Requirements
Your OS should manage:
 RAM: 2GB (configurable)
 Hard Drive: 256GB (configurable)
 CPU Cores: 8 (configurable)

Core OS Concepts Implementation
1. Process Creation &amp; Management

Requirements:
 Each task must run as a separate process (NOT simple function calls)
 Process creation message must include memory requirements
typedef struct {
int pid;
char name[50];
int memory_required;
int hdd_required;
int cpu_cores_needed;
ProcessState state;
} Process;

 Resource availability check before process execution
 Individual terminal windows for each process

2. Inter-Process Communication (IPC)
Implement at least two IPC mechanisms:
 Shared Memory for large data transfers
 Message Queues for process coordination
 Pipes for command communication

3. Thread Creation &amp; Attributes
pthread_t thread_id;
pthread_attr_t attr;
pthread_attr_init(&attr);
pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);


Thread Types to Implement:
 Worker threads for background tasks
 UI threads for interactive applications
 I/O threads for file operations

4. Process Synchronization
Implement all synchronization primitives:
 Mutex for critical sections
 Semaphores for resource counting
 Condition Variables for thread coordination
 Readers-Writers problem solution

5. Scheduling Algorithms
Multilevel Queue Implementation:

Operating Systems Lab Fall 25 Final Term Project

Level 1 (High Priority - Interactive Tasks):
 Round Robin (time quantum: 100ms)
 For: Games, Calculator, Notepad
Level 2 (Medium Priority - Background Tasks):
 Priority Scheduling
 For: Music player, File operations
Level 3 (Low Priority - System Tasks):
 First-Come-First-Served (FCFS)
 For: Clock, Auto-save tasks

6. Deadlock Avoidance
Implement Banker's Algorithm for:
 Memory allocation safety checks
 Resource request validation
 Deadlock detection and recovery

Task Requirements
Minimum 10 Required Applications
System Applications (5)
1. System Monitor - Real-time resource usage display
2. Process Manager - View/kill running processes
3. File Explorer - Navigate and manage files
4. Terminal Emulator - Command-line interface
5. Settings Panel - System configuration
Productivity Applications (4)
6. Notepad+ - Text editor with auto-save (every 30 seconds)
7. Calculator - Scientific calculator with history
8. Calendar - Date and event management
9. Task Scheduler - Schedule process execution

Utility Applications (3)
10. File Operations Tool - Copy, move, delete with progress
11. System Info - Hardware and resource information
12. Search Tool - File and content search
Entertainment Applications (3)
13. Minesweeper Game - Classic game implementation
14. Music Player - Background audio with playlist
15. Clock &amp; Timer - Multiple timezone display with alarms

Additional Bonus Applications
 Web Browser (simplified)
 Paint Application
 Chat Client (local network)

Implementation Guidelines
Boot Sequence:
#snample boot process
echo "MiniOS v2.0 booting..."
sleep 2;
echo "Initializing hardware..."
sleep 1;
echo "Loading kernel modules...";
sleep 1;
echo "Starting syste services..."
sleep 2;
echo "Welcome yo MiniOS";

Process Creation Workflow
1. User selects application

2. System checks resource availability using Banker's Algorithm
3. If resources available → Create new process with unique PID
4. Allocate memory and assign to ready queue
5. Execute based on scheduling algorithm
6. Handle interrupts (close/minimize)
7. Cleanup resources on termination

Resource Management
typeduf struct {
int total_ram;
int available_ram;
int total hhd;
int available_hdd;
int total_cores;
int available-cores;
Process* runnning_processes[MAX_PROCESSES];
} SystemResources;

User vs Kernel Mode Implementation
User Mode:
 Regular application execution
 Limited system resource access
 Standard task operations
Kernel Mode:
 Process termination authority
 Direct memory management
 System configuration changes
 Priority adjustment capabilities
Detailed Implementation Steps
Phase 1: System Foundation

1. Boot Loader &amp; System Initialization
o Display OS name with loading animation
o Initialize resource management structures
o Set up system queues and tables
2. Basic Process Management
o Process control block implementation
o Fork/exec functionality simulation
o Basic scheduling framework

Phase 2: Core OS Features
1. Memory Management
o RAM allocation/deallocation
o Hard drive simulation
o Memory protection mechanisms
2. Scheduling Implementation
o Multilevel queue setup
o Context switching simulation
o Priority management
Phase 3: Applications &amp; UI (Week 5-6)
1. Application Development
o Create 15+ separate application processes
o Implement IPC between applications
o Add interrupt handling
2. User Interface
o Terminal-based menu system
o Process management interface
o Optional: Graphical interface

Code Structure
minios/
---kernel/
------boot.c
------scheduler.c
------memory.c
------process.c
---apps/
------notepad/
------calculator/
------minesweeper/
---lib/
main.c


Testing Requirements
Functional Testing
 Boot sequence completes successfully
 All 15 applications launch as separate processes
 Resource allocation prevents overallocation
 Scheduling algorithms work correctly
 IPC between applications functions
 User/Kernel mode switching works
 Interrupt handling (close/minimize) functions
Stress Testing
 Maximum process limit enforcement
 Memory exhaustion handling
 CPU saturation management
 Deadlock prevention verification


There must be a block of comments at the start of each function; the block
should contain brief description about functionality of code.
Use understandable name of variables.
Proper indentation of code is essential.
