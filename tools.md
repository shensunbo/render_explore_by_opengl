# perf
# heaptrack
## install
- sudo apt install heaptrack heaptrack-gui
- heaptrack --version

## uasge
- bazel build --cxxopt="-g" --strip=never  //src:refactor_test 
- heaptrack -- bazel-bin/src/refactor_test -o heap_track

- can use gui or command line to analyze the result
    - heaptrack_gui heaptrack.refactor_test.12345.gz
    - heaptrack_print heaptrack.refactor_test.12345.gz > heaptrack_report.txt
# renderDoc