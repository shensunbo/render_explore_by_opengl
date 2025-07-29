# render_explore_by_opengl
render_explore_by_opengl

# build
```
mkdir build
cd build
cmake ..
make -j6

./src/render_explore_by_opengl
```

# bazel build
```
bazel build //src:refactor_test 
```

# result
## phong model effect
diffuse + specular + normal + ao textures 
![phong](./readme_res/halo.png)