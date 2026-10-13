#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CONFIG_FILE="$ROOT_DIR/build_debug.local.sh"

cd "$ROOT_DIR"

if [[ ! -f "$CONFIG_FILE" ]]; then
  echo "Local build config not found: $CONFIG_FILE" >&2
  echo "Copy build_debug.local.sh.example to build_debug.local.sh and set HOST_BIN." >&2
  exit 1
fi

source "$CONFIG_FILE"

if [[ -z "${HOST_BIN:-}" ]]; then
  echo "HOST_BIN is not set in $CONFIG_FILE" >&2
  exit 1
fi

if [[ ! -x "$HOST_BIN" ]]; then
  echo "AudioPluginHost executable not found: $HOST_BIN" >&2
  exit 1
fi

make -s -C "$ROOT_DIR/Builds/LinuxMakefile" VST3 -j10 CXXFLAGS="-w"

VST3_DIR="${VST3_DIR:-$HOME/.vst3}"
mkdir -p "$VST3_DIR"
cp -a "$ROOT_DIR/Builds/LinuxMakefile/build/TccSynth.vst3" "$VST3_DIR/"

exec "$HOST_BIN" "$@"
