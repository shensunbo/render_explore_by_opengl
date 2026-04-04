# pimpl practice
1. ModelLoader
2. Renderer

# loading time
## 2026-4-4 (dell precision 5570, 64GB RAM)
- total loading time: 2.483s
    - textures: 1.275s
    - things creation and GPU upload: 0.542s  
[perf](./readme_res/perf.svg)

```
[12:00:19.447][info ][refactor_test.cpp:78] Starting Refactor
[12:00:19.447][info ][refactor_test.cpp:452] Loaded config from res/config/render_config.json
[12:00:19.597][info ][Renderer.cpp:101] Loading config file: res/model/halo/vehicle_info.json
[12:00:19.597][info ][ConfigParser.cpp:334] ---> diffuse texture
[12:00:19.597][info ][ConfigParser.cpp:353] ---> specular texture
[12:00:19.597][info ][ConfigParser.cpp:373] ---> normal texture
[12:00:19.597][info ][ConfigParser.cpp:393] ---> ao texture
[12:00:19.597][info ][ConfigParser.cpp:427] ---> no alpha texture
[12:00:19.597][info ][ConfigParser.cpp:431] ---> roughness texture
[12:00:19.597][info ][ConfigParser.cpp:450] ---> metallic texture
[12:00:19.598][info ][ConfigParser.cpp:469] total texture files: 44
[12:00:19.610][info ][Renderer.cpp:152] Starting multithreaded texture loading for 50 unique paths
[12:00:19.628][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/Spartan_Ears_Mat_AO.png, width 1024, height 1024, channels: 1
[12:00:19.640][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/Spartan_Ears_Mat_Specular.png, width 1024, height 1024, channels: 1
[12:00:19.644][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/Spartan_Ears_Mat_Metallic.png, width 1024, height 1024, channels: 1
[12:00:19.654][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/Spartan_Ears_Mat_Roughness.png, width 1024, height 1024, channels: 1
[12:00:19.696][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/Spartan_Arms_Mat_Specular.png, width 2048, height 2048, channels: 1
[12:00:19.703][info ][tool.cpp:12] Texture loaded at path: res/model/skybox/ny.png, width 1024, height 1024, channels: 3
[12:00:19.704][info ][tool.cpp:12] Texture loaded at path: res/model/skybox/py.png, width 1024, height 1024, channels: 3
[12:00:19.704][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/ODST_Shoulder_Mat_Specular.png, width 2048, height 2048, channels: 1
[12:00:19.705][info ][tool.cpp:12] Texture loaded at path: res/model/skybox/nx.png, width 1024, height 1024, channels: 3
[12:00:19.705][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/Spartan_Arms_Mat_Metallic.png, width 2048, height 2048, channels: 1
[12:00:19.706][info ][tool.cpp:12] Texture loaded at path: res/model/skybox/px.png, width 1024, height 1024, channels: 3
[12:00:19.706][info ][tool.cpp:12] Texture loaded at path: res/model/skybox/pz.png, width 1024, height 1024, channels: 3
[12:00:19.715][info ][tool.cpp:12] Texture loaded at path: res/model/skybox/nz.png, width 1024, height 1024, channels: 3
[12:00:19.721][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/Spartan_Legs_Mat_Specular.png, width 2048, height 2048, channels: 1
[12:00:19.722][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/ODST_Shoulder_Mat_AO.png, width 2048, height 2048, channels: 1
[12:00:19.728][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/Spartan_Helmet_Mat_AO.png, width 2048, height 2048, channels: 1
[12:00:19.729][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/Spartan_Chest_Mat_Specular.png, width 2048, height 2048, channels: 1
[12:00:19.730][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/ODST_Shoulder_Mat_Metallic.png, width 2048, height 2048, channels: 1
[12:00:19.740][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/Spartan_Ears_Mat_Normal.png, width 1024, height 1024, channels: 3
[12:00:19.750][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/Spartan_Ears_Mat_BaseColor.png, width 1024, height 1024, channels: 3
[12:00:19.754][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/Spartan_Legs_Mat_Metallic.png, width 2048, height 2048, channels: 1
[12:00:19.754][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/Spartan_Chest_Mat_Roughness.png, width 2048, height 2048, channels: 1
[12:00:19.777][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/Spartan_Helmet_Mat_Specular.png, width 2048, height 2048, channels: 1
[12:00:19.779][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/Spartan_Chest_Mat_Metallic.png, width 2048, height 2048, channels: 1
[12:00:19.786][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/lambert1_Roughness.png, width 2048, height 2048, channels: 1
[12:00:19.798][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/Spartan_Arms_Mat_Roughness.png, width 2048, height 2048, channels: 1
[12:00:19.798][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/Spartan_Helmet_Mat_Metallic.png, width 2048, height 2048, channels: 1
[12:00:19.810][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/Spartan_Helmet_Mat_Roughness.png, width 2048, height 2048, channels: 1
[12:00:19.810][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/ODST_Shoulder_Mat_Roughness.png, width 2048, height 2048, channels: 1
[12:00:19.830][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/Spartan_Arms_Mat_AO.png, width 2048, height 2048, channels: 1
[12:00:19.834][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/Spartan_Legs_Mat_Roughness.png, width 2048, height 2048, channels: 1
[12:00:19.837][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/Spartan_Legs_Mat_AO.png, width 2048, height 2048, channels: 1
[12:00:19.857][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/Spartan_Chest_Mat_AO.png, width 2048, height 2048, channels: 1
[12:00:19.879][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/Spartan_Undersuit_Mat_Specular.png, width 4096, height 4096, channels: 1
[12:00:19.964][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/Spartan_Undersuit_Mat_Metallic.png, width 4096, height 4096, channels: 1
[12:00:20.029][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/Spartan_Undersuit_Mat_AO.png, width 4096, height 4096, channels: 1
[12:00:20.030][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/Spartan_Helmet_Mat_BaseColor.png, width 2048, height 2048, channels: 3
[12:00:20.050][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/ODST_Shoulder_Mat_BaseColor.png, width 2048, height 2048, channels: 3
[12:00:20.070][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/Spartan_Legs_Mat_BaseColor.png, width 2048, height 2048, channels: 3
[12:00:20.087][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/Spartan_Arms_Mat_BaseColor.png, width 2048, height 2048, channels: 3
[12:00:20.089][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/ODST_Shoulder_Mat_Normal.png, width 2048, height 2048, channels: 3
[12:00:20.119][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/Spartan_Undersuit_Mat_Roughness.png, width 4096, height 4096, channels: 1
[12:00:20.122][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/Spartan_Chest_Mat_Normal.png, width 2048, height 2048, channels: 3
[12:00:20.129][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/Spartan_Helmet_Mat_Normal.png, width 2048, height 2048, channels: 3
[12:00:20.137][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/Spartan_Legs_Mat_Normal.png, width 2048, height 2048, channels: 3
[12:00:20.150][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/Spartan_Chest_Mat_BaseColor.png, width 2048, height 2048, channels: 3
[12:00:20.161][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/Spartan_Arms_Mat_Normal.png, width 2048, height 2048, channels: 3
[12:00:20.165][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/lambert1_Colour-Opacity.png, width 2048, height 2048, channels: 4
[12:00:20.591][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/Spartan_Undersuit_Mat_BaseColor.png, width 4096, height 4096, channels: 3
[12:00:20.885][info ][tool.cpp:12] Texture loaded at path: res/model/halo/textures/Spartan_Undersuit_Mat_Normal.png, width 4096, height 4096, channels: 3
[12:00:20.885][info ][Renderer.cpp:172] All texture-loading threads completed
[12:00:21.140][info ][ModelLoader.cpp:263] [ModelLoader] mesh Helmet material Spartan_Helmet_Mat texture load cost: 58 ms
[12:00:21.162][info ][ModelLoader.cpp:263] [ModelLoader] mesh Helmet material Spartan_Ear_Mat texture load cost: 20 ms
[12:00:21.275][info ][ModelLoader.cpp:263] [ModelLoader] mesh Armour_LP material Spartan_Undersuit_Mat texture load cost: 110 ms
[12:00:21.313][info ][ModelLoader.cpp:263] [ModelLoader] mesh Armour material chest_Mat texture load cost: 34 ms
[12:00:21.342][info ][ModelLoader.cpp:263] [ModelLoader] mesh Armour material arms_Mat texture load cost: 27 ms
[12:00:21.369][info ][ModelLoader.cpp:263] [ModelLoader] mesh Armour material Spartan_Legs_Mat texture load cost: 24 ms
[12:00:21.394][info ][ModelLoader.cpp:263] [ModelLoader] mesh Armour material Spartan_Shoulders_Mat texture load cost: 23 ms
[12:00:21.401][info ][ModelLoader.cpp:263] [ModelLoader] mesh Armour material lambert1 texture load cost: 5 ms
[12:00:21.403][info ][ModelLoader.cpp:263] [ModelLoader] mesh polySurface436 material Spartan_Ear_Mat texture load cost: 0 ms
[12:00:21.405][debug][Renderer.cpp:187] Renderer::create: mesh name: Floor, MaterialName: lambert2
[12:00:21.405][debug][Renderer.cpp:187] Renderer::create: mesh name: Helmet, MaterialName: Spartan_Helmet_Mat
[12:00:21.405][debug][Renderer.cpp:187] Renderer::create: mesh name: Helmet, MaterialName: Spartan_Ear_Mat
[12:00:21.405][debug][Renderer.cpp:187] Renderer::create: mesh name: Armour_LP, MaterialName: Spartan_Undersuit_Mat
[12:00:21.405][debug][Renderer.cpp:187] Renderer::create: mesh name: Armour, MaterialName: chest_Mat
[12:00:21.405][debug][Renderer.cpp:187] Renderer::create: mesh name: Armour, MaterialName: arms_Mat
[12:00:21.405][debug][Renderer.cpp:187] Renderer::create: mesh name: Armour, MaterialName: Spartan_Legs_Mat
[12:00:21.405][debug][Renderer.cpp:187] Renderer::create: mesh name: Armour, MaterialName: Spartan_Shoulders_Mat
[12:00:21.405][debug][Renderer.cpp:187] Renderer::create: mesh name: Armour, MaterialName: lambert1
[12:00:21.405][debug][Renderer.cpp:187] Renderer::create: mesh name: polySurface436, MaterialName: Spartan_Ear_Mat
[12:00:21.420][info ][Skybox.cpp:190] Skybox created
[12:00:21.420][error][Shader.cpp:43] gl error: 0x502
[12:00:21.427][info ][Renderer.cpp:387] All texture data released.
[12:00:21.427][info ][Renderer.cpp:209] Renderer::create
[12:00:24.483][info ][refactor_test.cpp:386] FPS: 61
[12:00:27.484][info ][refactor_test.cpp:386] FPS: 61
```