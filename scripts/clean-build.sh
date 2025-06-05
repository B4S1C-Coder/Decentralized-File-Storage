#!/bin/bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR/.."

if [[ -f .env ]]; then
  source .env
else
  echo "No .env file found in project root, it is assumed you have already set the required variables, else the build may fail."
fi

rm -rf build || true
mkdir -p build
cd build
cmake ..
make -j"$(nproc)"