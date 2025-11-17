# Terminal Window Support

MiniOS now supports launching each application in a **separate terminal window**, making it easy to interact with multiple apps simultaneously.

## How It Works

When you launch an application from MiniOS, the system automatically detects your environment and chooses the best method to display the app:

### Launch Methods (in order of preference):

1. **tmux** (Recommended) ⭐
   - If you're running MiniOS inside a tmux session
   - Each app appears in a new tmux window
   - Switch between apps using `Ctrl+B` then number keys
   - Works perfectly over SSH and in terminal-only environments

2. **xterm**
   - If X11/GUI is available (DISPLAY environment variable is set)
   - Each app opens in a separate xterm window
   - Works on Linux with GUI desktop

3. **gnome-terminal**
   - If gnome-terminal is available
   - Alternative GUI terminal option

4. **Background Execution** (Fallback)
   - If no terminal emulator is available
   - Apps run in background with output to /dev/null

## Best Experience: Using tmux

For the **best experience**, run MiniOS inside tmux:

```bash
# Install tmux if not already installed
sudo apt-get install tmux   # Debian/Ubuntu
sudo yum install tmux       # CentOS/RHEL

# Start a tmux session
tmux

# Run MiniOS
./bin/minios
```

### Using tmux

Once running in tmux:
- Launch apps normally from the MiniOS menu
- Each app appears in a new tmux window
- **Switch between windows**: `Ctrl+B` then `0`, `1`, `2`, etc.
- **List all windows**: `Ctrl+B` then `w`
- **Kill a window**: `Ctrl+B` then `&`
- **Detach from session**: `Ctrl+B` then `d`
- **Reattach to session**: `tmux attach`

## Examples

### Launching Calculator in tmux:
```
1. Run tmux
2. Start ./bin/minios
3. Launch Calculator app
4. Calculator appears in new tmux window
5. Press Ctrl+B then 0 to return to MiniOS
6. Press Ctrl+B then 1 to go back to Calculator
```

### Launching Multiple Apps:
```
- Launch Calculator (appears in window 1)
- Launch Notepad (appears in window 2)
- Launch Minesweeper (appears in window 3)
- Switch between them using Ctrl+B + number
```

## Status Messages

MiniOS will inform you which method it's using:

- `[Terminal] Detected tmux session, creating new window...`
- `[Terminal] Launched in tmux window: Calculator`
- `[Terminal] Detected X11 display, using xterm...`
- `[Terminal] Launched in xterm: Notepad`
- `[Terminal] No terminal emulator available, launching in background...`

## Troubleshooting

**Q: Apps launch but I can't see them**
- A: You're likely not in tmux and have no GUI. Run `tmux` first, then `./bin/minios`

**Q: tmux windows close immediately**
- A: The app completed or crashed. Check the app's exit message before the window closes

**Q: I want to use a different terminal**
- A: The code automatically detects available terminals. Preference: tmux > xterm > gnome-terminal

**Q: How do I close all app windows at once?**
- A: In tmux, you can close MiniOS (which terminates child processes) or use `Ctrl+B` then `:kill-session`

## Architecture

The terminal window support is implemented in `lib/app_framework.c`:

- `launch_in_terminal()` - Main dispatcher, tries each method in order
- `launch_in_tmux()` - Creates new tmux window
- `launch_in_xterm()` - Launches xterm window
- `launch_in_gnome_terminal()` - Launches gnome-terminal
- `launch_direct()` - Fallback to background execution

Each method properly forks a new process and uses `setsid()` to create an independent session.
