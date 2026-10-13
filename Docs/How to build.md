# How to Build

## Requirements

Install the following using your Linux distribution's package manager:

- A C++17-compatible compiler (GCC or Clang).
- GNU Make and `pkg-config`.
- The development packages for ALSA, FreeType, Fontconfig, libcurl, GTK 3,
  and WebKitGTK.
  - The generated Makefile supports WebKitGTK 4.1 and falls back to 4.0.
    Install the development package for whichever version your distribution
    provides.
- PROJUCER to generate the Juce Library code and build files from the .jucer file. Configure or regenerate the Linux Makefile so its JUCE
  module and VST3 SDK include paths point to that checkout on your machine.

Choose the command for your distribution:

```sh
# Debian or Ubuntu
sudo apt install build-essential pkg-config libasound2-dev libfreetype6-dev libfontconfig1-dev libcurl4-openssl-dev libgtk-3-dev libwebkit2gtk-4.1-dev
# Fedora
sudo dnf install gcc-c++ make pkgconf-pkg-config alsa-lib-devel freetype-devel fontconfig-devel libcurl-devel gtk3-devel webkit2gtk4.1-devel
# Arch Linux
sudo pacman -S --needed base-devel pkgconf alsa-lib freetype2 fontconfig curl gtk3 webkit2gtk-4.1
```

Package names and WebKitGTK versions vary by release. If a WebKitGTK 4.1
package is unavailable, install the distribution's 4.0 development package.

## Build all targets

Run these commands from the repository root.

The Linux Makefile defaults to the `Debug` configuration:

```bash
make -C Builds/LinuxMakefile -j"$(nproc)"
```

To build all targets in `Release`:

```bash
make -C Builds/LinuxMakefile CONFIG=Release -j"$(nproc)"
```

The `all` target builds the VST3 plugin, the standalone application, and the
VST3 manifest helper.

Build products are placed under `Builds/LinuxMakefile/build/`.

Add `V=1` to a `make` command to print the full compiler commands.

## Build an individual target

```bash
make -C Builds/LinuxMakefile VST3
make -C Builds/LinuxMakefile Standalone
make -C Builds/LinuxMakefile VST3_MANIFEST_HELPER
```

Add `CONFIG=Release` to any command to select the Release configuration.

The VST3 build rule also copies the plugin to `~/.vst3`.

## Build and run with the debug script

`build_debug.sh` builds the VST3 target, installs the plugin into `~/.vst3`,
and launches it in AudioPluginHost. Before the first run, create the local
configuration file from the example:

```bash
cp build_debug.local.sh.example build_debug.local.sh
```

Edit `build_debug.local.sh` and set `HOST_BIN` to the path of your
AudioPluginHost executable. For example:

```bash
HOST_BIN="$HOME/path/to/AudioPluginHost"
```

Optionally set `VST3_DIR` in the config to install the plugin somewhere other
than `~/.vst3`. Then run the script from any directory:

```bash
./build_debug.sh
```
Arguments passed to the script are forwarded to AudioPluginHost:

```bash
./build_debug.sh /path/to/session-or-file
```
To save time you can create a default patch on the AudioPlguinHost that includes the plugin, audio analizers (oscilloscope, spectrograph, etc.) and midi monitors all wired up.