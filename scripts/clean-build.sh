#!/bin/bash
set -euo pipefail

# Check for --no-env flag
NO_ENV=false
for arg in "$@"; do
  if [[ "$arg" == "--no-env" ]]; then
    NO_ENV=true
    break
  fi
done

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR/.."

if [[ "$NO_ENV" == false ]]; then
  if [[ -f .env ]]; then
    source .env
  else
    echo "No .env file found in project root, it is assumed you have already set the required variables, else the build may fail."
  fi
else
  echo "Skipping .env sourcing due to --no-env flag."
fi

rm -rf build || true
mkdir -p build
cd build
cmake ..
make -j"$(nproc)"