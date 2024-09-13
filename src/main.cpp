#include <iostream>
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

int main() {
    Assimp::Importer importer;
    // 这里可以添加一些日志输出或简单的检查，例如：
    std::cout << "Assimp version: " << importer.GetErrorString() << std::endl;
    return 0;
}