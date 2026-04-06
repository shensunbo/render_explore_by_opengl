# 2026-4
## 2026-4-4
### 1 instanced rendering (done)
- use instanced rendering to render 3 objects by default, one in the front, and 2 in the back in a row
- can use imgui to control how many objects to render, and the distance between them
- build and test 
    - bazel build //src:refactor_test 
### 2 ktx texture
1. explore how to replace the current texture loading code with ktx texture loading, and use ktx texture to load the textures in the project
2. explore how to convert current textures to ktx format, should I do it by hand or write a script to do it
3. give me a detail plan about how to achieve these
    - how to do the format conversion, what tools to use, how to automate it
    - how to change the code to support ktx texture loading, what changes to make,
    - where to put these files 
    - what code changes need to be done 
4. note these in the @development_log folder in .md format
#### tools
```
shensunbo@DLFGVL3T3:~/tools/KTX-Software/build$ ktx
ktx: Missing command.

Usage:
  ktx [<command>] [OPTION...]

  -h, --help     Print this usage message and exit
  -v, --version  Print the version number of this program and exit
      --testrun  Indicates test run. If enabled the tool will produce deterministic output whenever
                 possible

Available commands:
  convert    Convert another texture file type to a KTX2 file
  create     Create a KTX2 file from various input files
  deflate    Deflate (supercompress) a KTX2 file
  encode     Encode a KTX2 file
  extract    Extract selected images from a KTX2 file
  transcode  Transcode a KTX2 file
  info       Print information about a KTX2 file
  validate   Validate a KTX2 file
  compare    Compare two KTX2 files
  help       Display help information about the ktx tool

For detailed usage and description of each subcommand use 'ktx help <command>'
or 'ktx <command> --help'
```
