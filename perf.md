# prepare environment
## 非wsl环境安装perf
sudo apt-get install linux-tools-common linux-tools-$(uname -r)
## wsl环境安装perf
sudo apt install linux-tools-common linux-tools-generic

## 下载 FlameGraph
git clone https://github.com/brendangregg/FlameGraph.git

# wsl2 使用perf
## 查看内核版本
```
ls  /usr/lib/linux-tools/
-> 5.15.0-161-generic
```
## 绝对路径使用perf
sudo /usr/lib/linux-tools/5.15.0-161-generic/perf record  -g -- ./your_app
会生成 perf.data 文件

## 使用 perf script导出数据
sudo /usr/lib/linux-tools/5.15.0-161-generic/perf script > perf.script

# 生成火焰图
## 生成火焰图数据
# 请将 /path/to/ 替换为你实际的 FlameGraph 路径，例如 ~/FlameGraph/
/home/shensunbo/FlameGraph/stackcollapse-perf.pl < perf.script | /home/shensunbo/FlameGraph/flamegraph.pl > perf.svg
会生成 perf.svg 文件

## 使用浏览器查看火焰图
![FlameGraph](/img/in-post/blah/FlameGraph.png)

# 简化命令
## 生成 perf.data
echo "alias perf-wsl='sudo /usr/lib/linux-tools/5.15.0-161-generic/perf'" >> ~/.bashrc
source ~/.bashrc

perf-wsl record  -g -o perf/perf.data -- bazel-bin/src/refactor_test
perf-wsl script  -i perf/perf.data > perf/perf.script

## 生成火焰图
/home/shensunbo/FlameGraph/stackcollapse-perf.pl < perf/perf.script | /home/shensunbo/FlameGraph/flamegraph.pl > perf/perf.svg