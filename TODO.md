# format
Use GitHub Flavored Markdown
- [ ] todo
- [x] done
--- 

# TODOs
## priority
- [X] 离屏渲染

- [ ] 识别内置纹理路径和嵌入的纹理

- [ ] 简单动画

- [ ] 使用 renderdoc 抓取渲染结果

- [ ] 使用 EnTT 

- [ ] 显示光源位置

- [X] android  platform 

- [ ] NDK build error fix, currently error: `fatal error: ktx.h: No such file or directory`

- [ ] NDK build by bazel

- [ ] NDK build state control 

- [X] res loading speed optimize，texture from >10s -> 3.3s

- [ ] 纹理格式转换，转换为GPU 友好的纹理格式`# 将 PNG 转换为 DDS（BC7 压缩）
texconv -f BC7_UNORM -m 10 input.png -o output.dds`

- [ ] 纹理加载异步化, debug 目前的加载瓶颈

- [ ] 代码结构优化

- [ ] BRDF

--- 

## android
- [ ] 首次运行解压文件到私有目录，通过标记文件记录是否首次运行


## low priority
- [ ] FPS optimize 
    - [ ]  static rendering 
        20250806 -> 50
        > swap buffer cost 20 - 30ms, no space to optimize in app side
        > wsl2 rendering speed not stable, can be 200fps or 50fps

- [ ] 控制线程和渲染线程分离

- [ ] 菲涅尔效果

- [ ] GUI 调节材质参数

- [ ] PBR ball

- [X] dump 图片

- [ ] dump 视频

- [ ] 使用开源的日志系统

---
## optional

- [ ] github CI

- [ ] 阴影 

- [ ] Deferred Shading

- [ ] scene based rendering
---

# BUGs
- [ ] [ERROR][Shader.cpp:42 Shader]: gl error: 0x502

# structure 
- [ ] meet C++ best practices
- [ ] meet modern Opengl best practice 
