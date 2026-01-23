#!/usr/bin/env bash

ROM_PATH="$1"

if [[ -z "$ROM_PATH" ]]; then
  echo "Usage: $0 <path-to-rom>"
  exit 1
fi

MIN_LAYER=0
MAX_LAYER=326
SLEEP_TIME=5

while true; do
  LAYER1=$((RANDOM % (MAX_LAYER - MIN_LAYER + 1) + MIN_LAYER))
  LAYER2=$((RANDOM % (MAX_LAYER - MIN_LAYER + 1) + MIN_LAYER))

  echo "Running ebbg with layers: layer1=$LAYER1 layer2=$LAYER2"

  bin/Debug/ebbg "$ROM_PATH" "$LAYER1" "$LAYER2" &
  EBBG_PID=$!

  sleep "$SLEEP_TIME"

  echo "Stopping ebbg (PID $EBBG_PID)"
  kill "$EBBG_PID" 2>/dev/null
  wait "$EBBG_PID" 2>/dev/null

  echo "----------------------------------------"
done

