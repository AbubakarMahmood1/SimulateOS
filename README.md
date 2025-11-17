# 🖥️ MiniOS - Complete Operating System Simulator

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)]()
[![Language](https://img.shields.io/badge/language-C-blue)]()
[![License](https://img.shields.io/badge/license-MIT-green)]()
[![Platform](https://img.shields.io/badge/platform-Linux-lightgrey)]()

A comprehensive operating system simulator that implements all fundamental OS concepts including process management, memory allocation, scheduling algorithms, IPC, synchronization, and deadlock avoidance. Features **15 fully functional applications** running as real processes with **individual terminal window support**.

---

## ✨ Key Features

### 🎯 Core OS Components
- **Process Management**: Full PCB implementation with fork/exec support
- **Memory Management**: RAM/HDD allocation with mutex-protected resource tracking
- **Multilevel Scheduler**: Round Robin, Priority Scheduling, and FCFS
- **Deadlock Avoidance**: Banker's Algorithm with safety checking
- **IPC Mechanisms**: Message queues, shared memory, and pipes
- **Synchronization**: Mutexes, semaphores, and condition variables

### 🪟 Multi-Terminal Window Support
Apps launch in **separate visible windows** using intelligent detection:
- **tmux** (recommended) - Works everywhere, even over SSH
- **xterm** - GUI terminal support
- **gnome-terminal** - Alternative GUI option
- **Background fallback** - Graceful degradation

### 📦 15 Fully Functional Applications

#### System Applications (5)
1. **System Monitor** - Live CPU/RAM monitoring from `/proc`
2. **Process Manager** - View and kill processes
3. **File Explorer** - Real directory navigation
4. **Terminal** - Full shell with command execution
5. **Settings** - System configuration editor

#### Productivity Applications (4)
6. **Notepad+** - Text editor with pthread auto-save (30s intervals)
7. **Calculator** - Full arithmetic with history
8. **Calendar** - Month display with event management
9. **Task Scheduler** - Schedule commands with threaded execution

#### Utility Applications (3)
10. **File Operations** - Copy/move/delete with live progress bar
11. **System Info** - Real hardware information from `/proc`
12. **Search Tool** - Recursive file search

#### Entertainment Applications (3)
13. **Minesweeper** - Full game with flood fill algorithm
14. **Music Player** - Threaded playback simulation with playlist
15. **Clock & Timer** - Live clock with alarms and timers

---

## 🚀 Quick Start

### Prerequisites
```bash
# Install dependencies
sudo apt-get update
sudo apt-get install build-essential tmux  # tmux recommended for best experience
```

### Build
```bash
# Clone the repository
git clone <repository-url>
cd SimulateOS

# Build the project
make

# Run MiniOS
./bin/minios
```

### Recommended Usage (with tmux)
```bash
# Start tmux for separate window support
tmux

# Run MiniOS
./bin/minios

# Each app will launch in a new tmux window
# Switch between windows: Ctrl+B then 0, 1, 2, 3...
```

---

## 📖 Usage Examples

### Launching Applications
```
MiniOS Main Menu
├── System Applications
│   ├── 1. System Monitor
│   ├── 2. Process Manager
│   └── ...
├── Productivity Applications
└── ...

Select option: 1

[App Launcher] Launching: System Monitor
[Terminal] Detected tmux session, creating new window...
[Terminal] Launched in tmux window: System Monitor
[App Launcher] Successfully launched: System Monitor (PID=1, System PID=12345)
```

### Process Management
- **View processes**: System Monitor or Process Manager
- **Kill process**: Process Manager → Select PID → Kill
- **Real OS PIDs tracked**: Each app runs as a real process

### Resource Monitoring
```
╔══════════════════════════════════════════════════════════════╗
║                     SYSTEM RESOURCES                         ║
╠══════════════════════════════════════════════════════════════╣
║  RAM:  1024 / 2048 MB (50% used)                             ║
║  HDD:  128 / 256 GB (50% used)                               ║
║  CPU:  4 / 8 cores (50% allocated)                           ║
║  Processes: 5 running                                        ║
╚══════════════════════════════════════════════════════════════╝
```

---

## 🏗️ Architecture

### Project Structure
```
SimulateOS/
├── kernel/              # Core OS components
│   ├── boot.c          # Boot sequence and initialization
│   ├── memory.c        # RAM/HDD management
│   ├── process.c       # Process creation/termination
│   ├── scheduler.c     # Multilevel queue scheduler
│   ├── deadlock.c      # Banker's Algorithm
│   ├── ipc.c           # Inter-process communication
│   └── sync.c          # Synchronization primitives
├── lib/                # Libraries
│   ├── app_framework.c # Multi-terminal app launcher
│   └── utils.c         # Utility functions
├── apps/               # 15 application implementations
│   ├── calculator.c
│   ├── notepad.c
│   ├── minesweeper.c
│   └── ... (12 more)
├── include/            # Header files
│   ├── kernel.h
│   └── types.h
├── config/             # Configuration
│   └── system.conf
└── bin/                # Compiled binaries (generated)
    ├── minios
    └── apps/
```

### Multi-Terminal Launcher
```
launch_in_terminal()
    ↓
┌─────────────────────────────────────┐
│ Environment Detection:              │
│                                     │
│ 1. TMUX?       → tmux new-window    │
│ 2. DISPLAY?    → xterm              │
│ 3. gnome?      → gnome-terminal     │
│ 4. Fallback    → background         │
└─────────────────────────────────────┘
    ↓
fork() + exec()
    ↓
App runs in separate window/background
```

---

## 🎮 Application Highlights

### Threading Examples
- **Notepad+**: Auto-save thread runs every 30 seconds
- **Music Player**: Playback worker thread simulates playback
- **Task Scheduler**: Each scheduled task runs in its own thread
- **File Operations**: Progress bar displayed by separate thread

### Real System Integration
- **System Info/Monitor**: Reads from `/proc/cpuinfo`, `/proc/meminfo`, `/proc/stat`
- **Process Manager**: Parses `/proc/[pid]/status`
- **Terminal**: Executes real commands via `fork()` + `execvp()`
- **File Explorer**: Uses `opendir()`, `readdir()`, `stat()`

### Advanced Features
- **Minesweeper**: Flood fill algorithm for cell revealing
- **Music Player**: Full playlist management with repeat/shuffle modes
- **Task Scheduler**: One-time and interval task scheduling
- **File Operations**: Threaded progress tracking for copy/move operations

---

## 🔧 Configuration

Edit `config/system.conf` to customize:

```ini
[HARDWARE]
RAM_SIZE_MB=2048
HDD_SIZE_GB=256
CPU_CORES=8

[SCHEDULER]
RR_TIME_QUANTUM_MS=100
MAX_PROCESSES=64
```

Or use the **Settings** application within MiniOS.

---

## 📊 Technical Specifications

| Component | Implementation |
|-----------|----------------|
| **Language** | C (POSIX) |
| **Threading** | pthreads |
| **Process Model** | fork() + exec() |
| **IPC** | Message queues, shared memory, pipes |
| **Synchronization** | Mutexes, semaphores, condition variables |
| **Scheduling** | Multilevel queue (RR/Priority/FCFS) |
| **Deadlock** | Banker's Algorithm |
| **Platform** | Linux/Unix |

### System Requirements
- **RAM**: 512MB+ (simulates 2GB)
- **Disk**: 100MB
- **OS**: Linux (kernel 3.2+)
- **Compiler**: GCC with pthread support
- **Optional**: tmux (recommended for best experience)

---

## 📚 Documentation

- **[TERMINAL_WINDOWS.md](TERMINAL_WINDOWS.md)** - Multi-terminal window usage guide
- **[IMPLEMENTATION_STATUS.md](IMPLEMENTATION_STATUS.md)** - Complete technical details
- **[config/system.conf](config/system.conf)** - System configuration

---

## 🧪 Testing

### Build and Run
```bash
# Clean build
make clean && make

# Run
./bin/minios

# Test individual apps
./bin/apps/calculator
./bin/apps/minesweeper
```

### Verification
- ✅ All 15 apps compile without errors or warnings
- ✅ Boot sequence completes successfully
- ✅ Apps launch as separate processes
- ✅ Resource allocation uses Banker's Algorithm
- ✅ Scheduling algorithms function correctly
- ✅ IPC mechanisms work between apps
- ✅ Terminal windows work in tmux/X11

---

## 🎓 Educational Value

This project demonstrates:
- **Process Management**: PCB, states, fork/exec
- **Memory Management**: Allocation, protection, tracking
- **CPU Scheduling**: Multiple algorithms, context switching
- **Deadlock**: Avoidance using Banker's Algorithm
- **IPC**: Message passing, shared memory
- **Synchronization**: Critical sections, race conditions
- **Threading**: Worker threads, background tasks
- **System Programming**: POSIX APIs, system calls
- **Real-world Integration**: `/proc` filesystem, process control

---

## 🛠️ Build Details

### Makefile Targets
```bash
make          # Build everything
make clean    # Remove build artifacts
make rebuild  # Clean + build
```

### Compilation
- **Kernel**: Compiled into object files in `build/`
- **Libraries**: Linked with kernel
- **Applications**: Compiled as standalone binaries in `bin/apps/`
- **Flags**: `-Wall -Wextra -pthread -lm`

---

## 🐛 Troubleshooting

**Q: Apps don't appear in separate windows**
- A: Run inside `tmux` for best results: `tmux` then `./bin/minios`

**Q: Build fails**
- A: Ensure you have `build-essential` installed: `sudo apt-get install build-essential`

**Q: Apps crash immediately**
- A: Check resource availability in System Monitor

**Q: Can't kill a process**
- A: Use Process Manager or `kill` command with the System PID

---

## 📝 License

This project is created for educational purposes. See individual source files for licensing information.

---

## 🤝 Contributing

This is an educational project. Contributions, issues, and feature requests are welcome!

---

## 👨‍💻 Author

Created as a comprehensive operating systems course project demonstrating:
- Process and thread management
- Memory allocation strategies
- CPU scheduling algorithms
- Deadlock avoidance techniques
- Inter-process communication
- System programming in C

---

## 🌟 Highlights

- ✅ **15 fully functional applications** (not stubs!)
- ✅ **Real process execution** with fork/exec
- ✅ **Multi-terminal window support** (tmux/xterm/gnome)
- ✅ **Complete OS simulation** with all core concepts
- ✅ **Production-quality code** with comprehensive documentation
- ✅ **Zero compiler warnings** - clean build
- ✅ **Threading demonstrations** in multiple apps
- ✅ **Real system integration** via `/proc` filesystem

---

**MiniOS v2.0** - A Complete Operating System Simulator
