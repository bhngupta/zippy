#!/bin/sh
# Change directory to parent directory of 'scripts'
cd "$(dirname "$0")/../"

HOOKS_DIR=$(pwd)/git-hooks

# hooks directory exists
if [ ! -d "$HOOKS_DIR" ]; then
  echo "Hooks directory '$HOOKS_DIR' does not exist."
  exit 1
fi

echo "Hooks directory: $HOOKS_DIR"

cp -r "$HOOKS_DIR/"* .git/hooks/
chmod +x .git/hooks/*

echo "Hooks have been set up."
