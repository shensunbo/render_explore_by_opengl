# render_explore_by_opengl
render_explore_by_opengl

# TODO:
1. 在着色器中区分纹理是单通道还是多通道, 对于ao，specular，Metallic, Roughness 这些贴图，一般是单通道的
2. 3D 纹理和 2D 纹理不能混淆

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

// debug version
bazel build --cxxopt="-g" --strip=never  //src:refactor_test 
```

# questions
1. AO 贴图用于中间阶段还是应用于最后的结果中

# result
## phong model effect
diffuse + specular + normal + ao textures 
![phong](./readme_res/halo.png)

diffuse + specular + normal + ao textures + skybox reflection 
![skybox](./readme_res/halo_skybox.png)

use ao texture to the final result
![ao](./readme_res/halo_ao_final.png)

