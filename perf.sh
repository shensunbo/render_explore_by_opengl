#!/bin/bash

# receive a param as bin be tested
BIN_PATH=$1
if [ -z "$BIN_PATH" ]; then
  echo "Usage: $0 /path/to/binary"
  exit 1
fi

# create perf directory if not exists
PERF_DIR="perf"
mkdir -p $PERF_DIR
PERF_DATA="$PERF_DIR/perf.data"
PERF_SCRIPT="$PERF_DIR/perf.script"
PERF_SVG="$PERF_DIR/perf.svg"

# record performance data
sudo /usr/lib/linux-tools/5.15.0-173-generic/perf record  -g -o $PERF_DATA -- $BIN_PATH
sudo /usr/lib/linux-tools/5.15.0-173-generic/perf script  -i $PERF_DATA > $PERF_SCRIPT

# generate flame graph
/home/shensunbo/FlameGraph/stackcollapse-perf.pl < $PERF_SCRIPT | /home/shensunbo/FlameGraph/flamegraph.pl > $PERF_SVG

echo "Flame graph generated at $PERF_SVG"
