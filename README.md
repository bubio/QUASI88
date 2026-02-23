# QUASI88

<p align="center">
  <a href="https://github.com/bubio/QUASI88/releases/latest">
    <img src="https://img.shields.io/github/v/release/bubio/QUASI88" alt="Latest Release">
  </a>
  <a href="https://github.com/bubio/QUASI88/blob/main/LICENSE">
    <img src="https://img.shields.io/github/license/bubio/QUASI88" alt="License">
  </a>
  <a href="https://github.com/bubio/QUASI88/releases/latest">
    <img src="https://img.shields.io/github/downloads/bubio/QUASI88/total.svg" alt="Downloads">
  </a>
</p>

This is a macOS build of QUASI88 (https://www.eonet.ne.jp/~showtime/quasi88/).

QUASI88 is a PC-8801 emulator that allows you to run software for the classic Japanese 8-bit computer on modern systems. It aims to accurately reproduce the PC-8801mkIISR and later models, with support for various features like sound, graphics, and peripherals.

<p align="center">
  <img src="doc/screenshot.png" alt="QUASI88 Screenhot">
</p>

## Features

QUASI88 emulates various components of the PC-8801:

*   **Z80 CPU**: Both main and sub CPUs.
*   **Memory**: PC-8801mkIISR equivalent, extended RAM, dictionary ROM.
*   **Display**: Text VRAM, VRAM, palette, ALU. Supports 640x200 (8 colors) and 640x400 (monochrome) modes.
*   **Kanji ROM**: Level 1 and Level 2 Kanji ROM.
*   **FDC/FDD**: Major commands for floppy disk control, supporting D88 format.
*   **PIO, Keyboard, Mouse** (Bus mouse, serial mouse), **Joystick**.
*   **Sound**: YM2203, YM2608, BEEP sound. Utilizes MAME/XMAME sound drivers and fmgen (FM Sound Generator).
*   **Tape**: Load/save functionality.
*   **Printer**: Output to file.
*   **RS232C**: Output to file / input from file.
*   **PCG-8100**.

**Not Emulated / Partial Emulation:**

*   Exact timing of interrupts.
*   Most CRTC/DMAC related functions.
*   Non-essential FDC commands, 2DD/2HD disk operations.
*   Clock functions other than time acquisition.
*   Accurate waits.

The emulator's compatibility is generally good, running 70-90% of applications. Performance may vary depending on the complexity of the emulation and your system's hardware.

## Supported Platforms

*   **Windows**
*   **macOS** (Universal a.k.a. x86_64 and arm64)
*   **Linux**

## Getting Started

### Prerequisites

Before you can build QUASI88, you need the following tools and libraries:

*   **A C/C++ compiler** (e.g., GCC, Clang, Visual Studio C++ with C++ development workload)
*   **CMake** (version 3.10 or later)
*   **SDL2 development library** (version 2.0.0 or later)

**SDL2 Installation Examples:**

*   **macOS (Homebrew):** `brew install sdl2`
*   **Linux (Debian/Ubuntu):** `sudo apt-get install libsdl2-dev`
*   **Windows:** Download the development library from the [SDL website](https://www.libsdl.org/) and place the `include` and `lib` folders into a `visualc/SDL2` directory within the project, or use a package manager like `vcpkg`. Additionally, for Windows, you might need the Visual C++ Redistributable Package if `VCRUNTIME140.dll` is not found.

### Building

QUASI88 uses CMake for its build system.

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/your-username/quasi88.git
    cd quasi88
    ```

2.  **Create a build directory:**
    ```bash
    mkdir build
    cd build
    ```

3.  **Configure and build the project:**

    **For macOS and Linux:**
    ```bash
    cmake ..
    make
    ```
    This will generate a `quasi88` executable in the build directory.

    **For Windows (using Visual Studio):**
    Open a developer command prompt and run:
    ```bash
    cmake ..
    cmake --build . --config Release
    ```
    Alternatively, you can open the generated `QUASI88.sln` file in Visual Studio and build the project from there. The executable will typically be in the `build/Release` directory.

### Installation

After building, you can install the executable and resource files to your system (optional):

```bash
make install
```
This will typically install to `/usr/local/bin` on Unix-like systems. You can change the install prefix by configuring CMake with `-DCMAKE_INSTALL_PREFIX=/path/to/install`.

## Usage

### ROM Images

QUASI88 requires PC-8801 BIOS ROM images to function correctly. These ROM images are not included with the emulator due to copyright reasons. You need to obtain them yourself from a physical PC-8801 or through other means.

**Required ROM files typically include:**

*   `N88.ROM` (N88 BASIC ROM, 32KB)
*   `N88EXT0.ROM` (N88 BASIC Extended ROM Bank 0, 8KB)
*   `N88EXT1.ROM` (N88 BASIC Extended ROM Bank 1, 8KB)
*   `N88EXT2.ROM` (N88 BASIC Extended ROM Bank 2, 8KB)
*   `N88EXT3.ROM` (N88 BASIC Extended ROM Bank 3, 8KB)
*   `N88N.ROM` (N BASIC ROM, 32KB)
*   `N88SUB.ROM` (Sub CPU ROM, 2KB or 8KB)
*   `N88KNJ1.ROM` (Kanji ROM Level 1, 128KB)
*   `N88KNJ2.ROM` (Kanji ROM Level 2, 128KB)
*   `FONT.ROM` (Font Image, optional)

Place your ROM files (case-insensitive filenames are usually fine) in the designated ROM directory. For more details on ROM images and their preparation, refer to `doc/manual.txt` and `doc/get-image.txt`.

### Disk and Tape Images

QUASI88 supports disk images (D88 format) and tape images (CMT/T88 format for loading, CMT only for saving).

*   **D88 Format**: Disk images should be in the D88 format, which is compatible with P88SR.EXE (PC8801mk-2SR emulator). For a detailed technical specification of the D88 format, see `doc/format.txt`.
*   **CMT/T88 Format**: Tape images can be in CMT or T88 format.

You can place your disk and tape images in the respective default directories or specify them via command-line options.

### Running the Emulator

You can launch QUASI88 from your terminal. If you've built it in the `build` directory, navigate there and execute:

```bash
./quasi88 [options] [disk_image.d88]
```

**Examples:**

*   `./quasi88` - Starts the emulator with ROM BASIC.
*   `./quasi88 game.d88` - Starts the emulator and loads `game.d88` into Drive 1.
*   `./quasi88 game.d88 2 4` - Loads the 2nd image of `game.d88` into Drive 1 and the 4th image into Drive 2.

For a complete list of command-line options and their usage, please consult `doc/manual.txt`.

### Default Directories

For this macOS build, QUASI88 uses the following default directories. These paths can be overridden by environment variables or command-line options.

*   **ROMs:** `~/Library/Application Support/quasi88/rom/`
*   **Disks:** `~/`
*   **Tapes:** `~/`
*   **Snapshots:** `~/Pictures/quasi88/`
*   **State Saves:** `~/Library/Application Support/quasi88/state/`
*   **Local Config:** `~/Library/Application Support/quasi88/rc/` (for per-image settings)
*   **Global Config:** `~/Library/Application Support/quasi88/` (for general settings)

### Key Operations

QUASI88 emulates a 106-key Japanese keyboard. Some key mappings for PC-8801 specific keys are:

*   **STOP**: `Pause` / `ScrollLock`
*   **COPY**: `PrintScreen`
*   **HOME CLR**: `Home`
*   **HELP**: `End`
*   **ROLL DOWN**: `PageUp`
*   **ROLL UP**: `PageDown`
*   **カナ**: Right `Alt`
*   **GRPH**: Left `Alt`
*   **決定**: Unmapped (often `Muhenkan` key on JP keyboards)
*   **変換**: Unmapped (often `Henkan` key on JP keyboards)
*   **全角**: `Application` key

**Emulator Specific Keys:**

*   **F11**: Toggles toolbar and status display.
*   **F12**: Enters **Menu Mode**.

You can customize key assignments using command-line options (`-f6` to `-f10`) or through the Menu Mode. For more detailed key mappings and customization, refer to `doc/manual.txt`.

### Configuration Files

QUASI88 reads various configuration files to customize its behavior.

*   **Global Configuration File**: Contains general emulator settings.
    *   **Unix-like:** `~/.quasi88/quasi88.rc`
    *   **Windows:** `QUASI88.ini` (in the same directory as the executable)
    *   The format is one option per line, identical to command-line options (e.g., `-frameskip 2`). Comments start with `#`.
*   **Individual Configuration File**: Loaded if a disk image is specified, allowing per-image settings.
    *   **Unix-like:** `~/.quasi88/rc/<disk_image_basename>.rc`
    *   **Windows:** `INI\<disk_image_basename>.ini`
*   **Keyboard Configuration File**: Customizes keyboard mappings.
    *   **Unix-like:** `~/.quasi88/keyconf.rc`
    *   **Windows:** `KEYCONF.ini`
*   **Touch Key Configuration File**: Customizes the on-screen touch keyboard.
    *   **Unix-like:** `~/.quasi88/touchkey.rc`
    *   **Windows:** `TOUCHKEY.ini`

Command-line options take precedence over individual config files, which take precedence over global config files.

### Menu Mode

Pressing `F12` or clicking the Menu icon in the toolbar enters **Menu Mode**. This graphical interface allows you to change various emulator settings, manage disk images, and perform other actions using mouse or keyboard navigation.

**Key sections in Menu Mode include:**

*   **Reset**: Change BASIC mode, CPU clock, sound board type, and reset the emulator.
*   **CPU**: Adjust CPU options, clock speed, wait adjustments.
*   **Screen**: Configure frame rate, auto-skip, screen size, PCG-8100, and screen effects.
*   **Volume**: Adjust sound levels for FM, PSG, BEEP, etc.
*   **Disk**: Open/close disk images, swap images, create blank disks.
*   **Key**: Assign functions to function keys, customize cursor keys, toggle key states.
*   **Mouse**: Configure mouse/joystick connections and mappings.
*   **Tape**: Set tape image files for loading/saving.
*   **Other**: State save/load, screenshot options, sound output recording.

For detailed instructions on using Menu Mode, refer to `doc/manual.txt`.

## Troubleshooting

If you encounter issues, here are some common problems and solutions:

*   **Emulator does not start / crashes immediately:**
    *   Ensure SDL2 is correctly installed and its DLL (`SDL2.dll` on Windows) is accessible.
    *   Check for error messages when launching with `-verbose 1` (add this to `quasi88.rc` or as a command-line option).
    *   Try deleting your configuration files (`quasi88.rc`/`QUASI88.ini`) to reset settings.
*   **Black screen after launch:**
    *   Verify your ROM images are present in the correct directory and are readable. Use `-verbose 1` to check for ROM loading errors.
*   **Corrupted display:**
    *   The font files (`FONT.ROM`, `N88KNJ1.ROM`) might be an issue. Try temporarily removing them (QUASI88 can use built-in fonts).
*   **Application does not run correctly / freezes:**
    *   Adjust CPU timing options (`-cpu 0`, `-cpu 1`, `-cpu 2`) in Menu Mode (CPU tab) or via command line.
    *   Try `-fdc_wait` for applications sensitive to FDD timing.
*   **Sound issues (no sound, noise, incorrect tempo):**
    *   Ensure sound is enabled (not using `-nosound`).
    *   Adjust sound buffer sizes (`-sdlbufsize`, `-sdlbufnum`) via command line or Menu Mode (Volume tab).
    *   Check `-nowait` option if tempo is too fast/slow. For details, refer to `doc/faq.txt`.
*   **Slow performance:**
    *   QUASI88 is resource-intensive. Consider options like `-autoskip`, `-frameskip`, `-half`, `-nowait` to improve speed. Refer to `doc/faq.txt` for detailed optimization tips.
*   **Persistent key presses / auto-repeat issues:**
    *   If a key is stuck, press and release it again, or enter Menu Mode (which clears most key states). For persistent auto-repeat issues after closing QUASI88, you might need to manually reset (`xset r on` on X11 systems).

For a comprehensive list of FAQs and troubleshooting tips, please refer to `doc/faq.txt`.

## License

This project is distributed under the terms of its original license, primarily a revised BSD license. See the `LICENSE` file for more details.

This project incorporates code from:
*   **SDL2**: Licensed under Zlib license.
*   **MAME/XMAME**: Please refer to their respective licenses for details.
*   **fmgen** (FM Sound Generator by cisc): Please refer to its documentation for license information.

## Acknowledgements

QUASI88 was originally created by Shozo Fukunaga (福永省三). This repository aims to maintain and update this classic PC-8801 emulator for modern systems. Special thanks to all original creators, contributors, and those who have provided tools and inspiration, including Marat Fayzullin (fMSX), Nicola Salmoria (MAME), Juan Antonio Marmnez (XMAME), Yumitaro (PC6001V), and cisc (fmgen).
