# 2026-4
## 2026-4-4
### 1 instanced rendering
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
