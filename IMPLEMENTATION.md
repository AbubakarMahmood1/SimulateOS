# MiniOS Implementation Documentation

## Project Overview

MiniOS is a comprehensive operating system simulator that demonstrates fundamental OS concepts including process management, memory allocation, multilevel scheduling, inter-process communication, and deadlock avoidance.

## Architecture

### System Components

1. **Kernel Layer**
   - `boot.c`: System initialization and boot sequence
   - `memory.c`: RAM and HDD memory management
   - `process.c`: Process creation, termination, and lifecycle management
   - `scheduler.c`: Multilevel queue scheduler with RR, Priority, and FCFS
   - `ipc.c`: Inter-process communication (message queues, shared memory, pipes)
   - `sync.c`: Synchronization primitives (mutexes, semaphores, condition variables)
   - `deadlock.c`: Banker's Algorithm for deadlock avoidance

2. **Library Layer**
   - `utils.c`: Common utility functions (display, input, time)
   - `app_framework.c`: Application launcher and manager

3. **Application Layer**
   - 15 separate application programs demonstrating process isolation

## Key Features Implemented

### 1. Process Management
- Process Control Block (PCB) with full state tracking
- Process states: NEW, READY, RUNNING, WAITING, TERMINATED
- Dynamic PID allocation starting from 1000
- Process creation and termination with resource cleanup
- Process table with configurable maximum (64 processes)

### 2. Memory Management
- RAM simulation (2GB default, configurable)
- HDD simulation (256GB default, configurable)
- Thread-safe allocation and deallocation
- Resource tracking and validation
- Mutex-protected critical sections

### 3. Multilevel Queue Scheduler
Three-level priority queue implementation:
- **Level 0 (High Priority)**: Round Robin with 100ms time quantum
  - Used for: Interactive tasks (games, calculator, notepad)
- **Level 1 (Medium Priority)**: Priority Scheduling
  - Used for: Background tasks (music player, file operations)
- **Level 2 (Low Priority)**: First-Come-First-Served (FCFS)
  - Used for: System tasks (clock, auto-save)

### 4. Deadlock Avoidance
- Banker's Algorithm implementation
- Safe state checking before resource allocation
- Resource request validation
- Automatic rollback on unsafe state detection
- Tracks: RAM, HDD, and CPU cores

### 5. Inter-Process Communication
Three IPC mechanisms implemented:
- **Message Queues**: For process coordination and messaging
- **Shared Memory**: For large data transfers between processes
- **Pipes**: For unidirectional command communication

### 6. Synchronization Primitives
- **Mutexes**: For critical section protection
- **Semaphores**: For resource counting and access control
- **Condition Variables**: For thread coordination and signaling

### 7. Applications (15 Total)

#### System Applications (5)
1. **System Monitor**: Real-time resource usage display
2. **Process Manager**: View and kill running processes
3. **File Explorer**: Navigate and manage files
4. **Terminal Emulator**: Command-line interface
5. **Settings Panel**: System configuration viewer

#### Productivity Applications (4)
6. **Notepad+**: Text editor with simulated auto-save
7. **Calculator**: Scientific calculator with arithmetic operations
8. **Calendar**: Date and event management
9. **Task Scheduler**: Schedule process execution

#### Utility Applications (3)
10. **File Operations Tool**: Copy, move, delete with progress tracking
11. **System Info**: Hardware and resource information display
12. **Search Tool**: File and content search utility

#### Entertainment Applications (3)
13. **Minesweeper**: Classic game implementation
14. **Music Player**: Background audio with playlist simulation
15. **Clock & Timer**: Multiple timezone display with alarms

## Building and Running

### Prerequisites
- GCC compiler
- POSIX-compliant system (Linux/Unix)
- pthread library
- Make build system

### Build Instructions
```bash
# Build the entire system
make

# Clean build files
make clean

# Build and run
make run
```

### Running MiniOS
```bash
./bin/minios
```

## System Configuration

Configuration file: `config/system.conf`

```
[HARDWARE]
RAM_SIZE_MB=2048
HDD_SIZE_GB=256
CPU_CORES=8

[SCHEDULING]
RR_TIME_QUANTUM_MS=100
MAX_PROCESSES=64

[SYSTEM]
OS_NAME=MiniOS
OS_VERSION=2.0
```

## Design Decisions

### 1. Process Simulation
While applications are compiled as separate executables, the current implementation creates process entries in the kernel's process table without actually forking. This simplifies the demonstration while maintaining all kernel-level process management logic.

**Future Enhancement**: Full fork/exec implementation for true process isolation.

### 2. Resource Management
Resources (RAM, HDD, cores) are tracked symbolically rather than actually allocating physical memory. This allows simulation of large memory spaces without consuming actual system resources.

### 3. Scheduler Implementation
The scheduler implements queue management and algorithm logic. In a full implementation, this would interface with a dispatcher for actual context switching and time-slicing.

### 4. Thread-Safe Design
All critical sections are protected with mutexes to ensure thread-safety, preparing the system for future multi-threaded enhancements.

## Testing

### Functional Tests
- ✅ Boot sequence completes successfully
- ✅ All 15 applications compile and can be launched
- ✅ Resource allocation prevents over-allocation
- ✅ Banker's Algorithm prevents unsafe states
- ✅ Process creation and termination work correctly
- ✅ Menu system and user interaction functional

### Stress Testing Scenarios
1. Launch maximum processes (64) and verify limit enforcement
2. Request resources exceeding available - verify denial
3. Fill RAM to capacity - verify allocation failure handling
4. Multiple simultaneous resource requests - verify thread-safety

## Code Quality

### Documentation
- Every function has a descriptive comment block
- Clear variable naming throughout
- Proper indentation and formatting
- Header files with comprehensive documentation

### Error Handling
- Null pointer checks
- Resource validation before allocation
- Graceful failure with error messages
- Safe cleanup on shutdown

### Modularity
- Clean separation of concerns
- Well-defined interfaces
- Minimal coupling between components
- Easy to extend with new applications

## Future Enhancements

1. **True Process Isolation**: Implement fork/exec for applications
2. **GUI Interface**: Add graphical desktop environment (GTK/SDL)
3. **Virtual Memory**: Implement paging and swapping
4. **File System**: Create virtual file system with persistence
5. **Network Stack**: Add socket-based IPC and networking
6. **Device Drivers**: Simulate hardware device management
7. **Shell Scripting**: Add scripting support in terminal
8. **Multi-User**: Implement user authentication and permissions

## Known Limitations

1. Applications don't actually run as separate processes (simplified for demonstration)
2. No actual file I/O - file system is simulated
3. Scheduler runs in simulation mode rather than real-time
4. IPC mechanisms are created but not fully utilized by applications
5. No persistent storage - all state is lost on shutdown

## Conclusion

MiniOS successfully demonstrates all required OS concepts:
- ✅ Process creation and management
- ✅ Memory allocation and tracking
- ✅ Multilevel queue scheduling (RR, Priority, FCFS)
- ✅ IPC mechanisms (shared memory, message queues, pipes)
- ✅ Synchronization primitives (mutex, semaphores, condition variables)
- ✅ Deadlock avoidance (Banker's Algorithm)
- ✅ 15+ applications across 4 categories
- ✅ User/kernel mode separation concept
- ✅ Clean, documented, maintainable code

The implementation provides a solid foundation for understanding operating system internals and can be extended for more advanced features.
