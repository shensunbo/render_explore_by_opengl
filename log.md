# pimpl practice 2026-4-4
1. ModelLoader
2. Renderer

# memory usage analysis and optimism 2026-4-11
## before
### TOP
24561 shensun+  20   0 2908200 750700  93952 S 334.7   4.6   7:53.70 refactor_test

### heap track
```
total runtime: 57.95s.
calls to allocation functions: 959512 (16558/s)
temporary memory allocations: 196971 (3399/s)
peak heap memory consumption: 737.16M
peak RSS (including heaptrack overhead): 784.24M
total memory leaked: 60.98K
suppressed leaks: 185.76K
```

## after
### 结论
```
 - RSS/HWM 峰值约 625MB（另一次 smaps_rollup RSS ~673MB，Private_Dirty ~585MB，Anonymous ~579MB）。
 - heaptrack.after_opt.* 显示应用侧主要峰值：
 - stbi 纹理解码峰值约 12–19MB（加载期临时）
 - 模型/纹理上传路径未见大额长期堆占用
 - 大头在 libgallium + libLLVM（调用栈经 rhi::drawIndexedTrianglesInstanced），说明主要是 Mesa/LLVM
软件栈（驱动/JIT/缓存）占用，不是你的模型 CPU 副本
```

> wsl2 上显卡无法使用，使用软件渲染，所以内存占用很大
```
glxinfo -B | grep -E "OpenGL renderer|OpenGL version|OpenGL core profile"
OpenGL renderer string: llvmpipe (LLVM 20.1.2, 256 bits)
OpenGL core profile version string: 4.5 (Core Profile) Mesa 25.2.8-0ubuntu0.24.04.1
OpenGL core profile shading language version string: 4.50
OpenGL core profile context flags: (none)
OpenGL core profile profile mask: core profile
OpenGL version string: 4.5 (Compatibility Profile) Mesa 25.2.8-0ubuntu0.24.04.1
```
> 更新GPU驱动，设置`export GALLIUM_DRIVER=d3d12`,解决了这个问题 ✅️

### top
29815 shensun+  20   0 2905376 678820  93696 S 377.4   4.2   2:22.06 refactor_test

### top GPU acceleration
` 2701 shensun+  20   0 2578356 267680 117376 S  32.3   1.6   0:29.99 refactor_test`



# loading time
## 2026-4-4 (dell precision 5570, 64GB RAM with ktx textures)
- total loading time: 1.299s(1.2s faster than png version)
```
[13:16:22.579][info ][refactor_test.cpp:78] Starting Refactor
[13:16:22.579][info ][refactor_test.cpp:452] Loaded config from res/config/render_config_ktx.json
[13:16:22.733][info ][Renderer.cpp:101] Loading config file: res/model/halo/vehicle_info_ktx.json
[13:16:22.733][info ][ConfigParser.cpp:334] ---> diffuse texture
[13:16:22.734][info ][ConfigParser.cpp:353] ---> specular texture
[13:16:22.734][info ][ConfigParser.cpp:373] ---> normal texture
[13:16:22.734][info ][ConfigParser.cpp:393] ---> ao texture
[13:16:22.734][info ][ConfigParser.cpp:427] ---> no alpha texture
[13:16:22.734][info ][ConfigParser.cpp:431] ---> roughness texture
[13:16:22.734][info ][ConfigParser.cpp:450] ---> metallic texture
[13:16:22.734][info ][ConfigParser.cpp:469] total texture files: 44
[13:16:22.746][info ][Renderer.cpp:152] Starting multithreaded texture loading for 50 unique paths
[13:16:22.860][info ][tool.cpp:12] Texture loaded at path: res/model/skybox/py.png, width 1024, height 1024, channels: 3
[13:16:22.860][info ][tool.cpp:12] Texture loaded at path: res/model/skybox/ny.png, width 1024, height 1024, channels: 3
[13:16:22.864][info ][tool.cpp:12] Texture loaded at path: res/model/skybox/nz.png, width 1024, height 1024, channels: 3
[13:16:22.868][info ][tool.cpp:12] Texture loaded at path: res/model/skybox/nx.png, width 1024, height 1024, channels: 3
[13:16:22.872][info ][tool.cpp:12] Texture loaded at path: res/model/skybox/px.png, width 1024, height 1024, channels: 3
[13:16:22.900][info ][tool.cpp:12] Texture loaded at path: res/model/skybox/pz.png, width 1024, height 1024, channels: 3
[13:16:23.351][info ][Renderer.cpp:194] All texture-loading threads completed
[13:16:23.570][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/Spartan_Helmet_Mat_BaseColor.ktx2 id=1
[13:16:23.573][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/Spartan_Helmet_Mat_Specular.ktx2 id=2
[13:16:23.578][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/Spartan_Helmet_Mat_Normal.ktx2 id=3
[13:16:23.584][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/Spartan_Helmet_Mat_AO.ktx2 id=4
[13:16:23.589][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/Spartan_Helmet_Mat_Roughness.ktx2 id=5
[13:16:23.594][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/Spartan_Helmet_Mat_Metallic.ktx2 id=6
[13:16:23.594][info ][ModelLoader.cpp:263] [ModelLoader] mesh Helmet material Spartan_Helmet_Mat texture load cost: 29 ms
[13:16:23.598][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/Spartan_Ears_Mat_BaseColor.ktx2 id=7
[13:16:23.602][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/Spartan_Ears_Mat_Specular.ktx2 id=8
[13:16:23.607][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/Spartan_Ears_Mat_Normal.ktx2 id=9
[13:16:23.609][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/Spartan_Ears_Mat_AO.ktx2 id=10
[13:16:23.611][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/Spartan_Ears_Mat_Roughness.ktx2 id=11
[13:16:23.614][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/Spartan_Ears_Mat_Metallic.ktx2 id=12
[13:16:23.614][info ][ModelLoader.cpp:263] [ModelLoader] mesh Helmet material Spartan_Ear_Mat texture load cost: 18 ms
[13:16:23.629][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/Spartan_Undersuit_Mat_BaseColor.ktx2 id=13
[13:16:23.641][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/Spartan_Undersuit_Mat_Specular.ktx2 id=14
[13:16:23.654][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/Spartan_Undersuit_Mat_Normal.ktx2 id=15
[13:16:23.664][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/Spartan_Undersuit_Mat_AO.ktx2 id=16
[13:16:23.676][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/Spartan_Undersuit_Mat_Roughness.ktx2 id=17
[13:16:23.688][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/Spartan_Undersuit_Mat_Metallic.ktx2 id=18
[13:16:23.688][info ][ModelLoader.cpp:263] [ModelLoader] mesh Armour_LP material Spartan_Undersuit_Mat texture load cost: 70 ms
[13:16:23.694][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/Spartan_Chest_Mat_BaseColor.ktx2 id=19
[13:16:23.699][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/Spartan_Chest_Mat_Specular.ktx2 id=20
[13:16:23.706][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/Spartan_Chest_Mat_Normal.ktx2 id=21
[13:16:23.710][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/Spartan_Chest_Mat_AO.ktx2 id=22
[13:16:23.715][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/Spartan_Chest_Mat_Roughness.ktx2 id=23
[13:16:23.721][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/Spartan_Chest_Mat_Metallic.ktx2 id=24
[13:16:23.721][info ][ModelLoader.cpp:263] [ModelLoader] mesh Armour material chest_Mat texture load cost: 31 ms
[13:16:23.729][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/Spartan_Arms_Mat_BaseColor.ktx2 id=25
[13:16:23.736][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/Spartan_Arms_Mat_Specular.ktx2 id=26
[13:16:23.740][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/Spartan_Arms_Mat_Normal.ktx2 id=27
[13:16:23.743][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/Spartan_Arms_Mat_AO.ktx2 id=28
[13:16:23.749][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/Spartan_Arms_Mat_Roughness.ktx2 id=29
[13:16:23.756][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/Spartan_Arms_Mat_Metallic.ktx2 id=30
[13:16:23.756][info ][ModelLoader.cpp:263] [ModelLoader] mesh Armour material arms_Mat texture load cost: 32 ms
[13:16:23.764][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/Spartan_Legs_Mat_BaseColor.ktx2 id=31
[13:16:23.770][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/Spartan_Legs_Mat_Specular.ktx2 id=32
[13:16:23.775][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/Spartan_Legs_Mat_Normal.ktx2 id=33
[13:16:23.779][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/Spartan_Legs_Mat_AO.ktx2 id=34
[13:16:23.785][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/Spartan_Legs_Mat_Roughness.ktx2 id=35
[13:16:23.790][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/Spartan_Legs_Mat_Metallic.ktx2 id=36
[13:16:23.790][info ][ModelLoader.cpp:263] [ModelLoader] mesh Armour material Spartan_Legs_Mat texture load cost: 31 ms
[13:16:23.796][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/ODST_Shoulder_Mat_BaseColor.ktx2 id=37
[13:16:23.802][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/ODST_Shoulder_Mat_Specular.ktx2 id=38
[13:16:23.807][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/ODST_Shoulder_Mat_Normal.ktx2 id=39
[13:16:23.811][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/ODST_Shoulder_Mat_AO.ktx2 id=40
[13:16:23.815][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/ODST_Shoulder_Mat_Roughness.ktx2 id=41
[13:16:23.821][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/ODST_Shoulder_Mat_Metallic.ktx2 id=42
[13:16:23.821][info ][ModelLoader.cpp:263] [ModelLoader] mesh Armour material Spartan_Shoulders_Mat texture load cost: 28 ms
[13:16:23.826][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/lambert1_Colour-Opacity.ktx2 id=43
[13:16:23.830][info ][TextureCache.cpp:158] KTX texture loaded: res/model/halo/textures_ktx/lambert1_Roughness.ktx2 id=44
[13:16:23.830][info ][ModelLoader.cpp:263] [ModelLoader] mesh Armour material lambert1 texture load cost: 8 ms
[13:16:23.832][info ][ModelLoader.cpp:263] [ModelLoader] mesh polySurface436 material Spartan_Ear_Mat texture load cost: 0 ms
[13:16:23.834][debug][Renderer.cpp:209] Renderer::create: mesh name: Floor, MaterialName: lambert2
[13:16:23.834][debug][Renderer.cpp:209] Renderer::create: mesh name: Helmet, MaterialName: Spartan_Helmet_Mat
[13:16:23.834][debug][Renderer.cpp:209] Renderer::create: mesh name: Helmet, MaterialName: Spartan_Ear_Mat
[13:16:23.834][debug][Renderer.cpp:209] Renderer::create: mesh name: Armour_LP, MaterialName: Spartan_Undersuit_Mat
[13:16:23.834][debug][Renderer.cpp:209] Renderer::create: mesh name: Armour, MaterialName: chest_Mat
[13:16:23.834][debug][Renderer.cpp:209] Renderer::create: mesh name: Armour, MaterialName: arms_Mat
[13:16:23.834][debug][Renderer.cpp:209] Renderer::create: mesh name: Armour, MaterialName: Spartan_Legs_Mat
[13:16:23.834][debug][Renderer.cpp:209] Renderer::create: mesh name: Armour, MaterialName: Spartan_Shoulders_Mat
[13:16:23.834][debug][Renderer.cpp:209] Renderer::create: mesh name: Armour, MaterialName: lambert1
[13:16:23.834][debug][Renderer.cpp:209] Renderer::create: mesh name: polySurface436, MaterialName: Spartan_Ear_Mat
[13:16:23.871][info ][Skybox.cpp:190] Skybox created
[13:16:23.871][error][Shader.cpp:43] gl error: 0x502
[13:16:23.896][info ][Renderer.cpp:409] All texture data released.
[13:16:23.896][info ][Renderer.cpp:231] Renderer::create
[13:16:26.960][info ][refactor_test.cpp:386] FPS: 61
[13:16:29.970][info ][refactor_test.cpp:386] FPS: 61
[13:16:32.986][info ][refactor_test.cpp:386] FPS: 61
[13:16:35.998][info ][refactor_test.cpp:386] FPS: 61
```
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