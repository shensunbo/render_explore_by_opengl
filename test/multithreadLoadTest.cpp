#include "log/mylog.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <string>
#include <set>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <cassert>


static std::set<std::string> pics {
    "res/model/halo/textures/Spartan_Arms_Mat_AO.png",
    "res/model/halo/textures/Spartan_Arms_Mat_BaseColor.png",
    "res/model/halo/textures/Spartan_Arms_Mat_Metallic.png",
    "res/model/halo/textures/Spartan_Arms_Mat_Normal.png",
    "res/model/halo/textures/Spartan_Arms_Mat_Roughness.png",
    "res/model/halo/textures/Spartan_Arms_Mat_Specular.png",
    "res/model/halo/textures/Spartan_Chest_Mat_AO.png",
    "res/model/halo/textures/Spartan_Chest_Mat_BaseColor.png",
    "res/model/halo/textures/Spartan_Chest_Mat_Metallic.png",
    "res/model/halo/textures/Spartan_Chest_Mat_Normal.png",
    "res/model/halo/textures/Spartan_Chest_Mat_Roughness.png",
    "res/model/halo/textures/Spartan_Chest_Mat_Specular.png",
    "res/model/halo/textures/Spartan_Ears_Mat_AO.png",
    "res/model/halo/textures/Spartan_Ears_Mat_BaseColor.png",
    "res/model/halo/textures/Spartan_Ears_Mat_Metallic.png",
    "res/model/halo/textures/Spartan_Ears_Mat_Normal.png",
    "res/model/halo/textures/Spartan_Ears_Mat_Roughness.png",
    "res/model/halo/textures/Spartan_Ears_Mat_Specular.png",
    "res/model/halo/textures/Spartan_Helmet_Mat_AO.png",
    "res/model/halo/textures/Spartan_Helmet_Mat_BaseColor.png",
    "res/model/halo/textures/Spartan_Helmet_Mat_Metallic.png",
    "res/model/halo/textures/Spartan_Helmet_Mat_Normal.png",
    "res/model/halo/textures/Spartan_Helmet_Mat_Roughness.png",
    "res/model/halo/textures/Spartan_Helmet_Mat_Specular.png",
    "res/model/halo/textures/Spartan_Legs_Mat_AO.png",
    "res/model/halo/textures/Spartan_Legs_Mat_BaseColor.png",
    "res/model/halo/textures/Spartan_Legs_Mat_Metallic.png",
    "res/model/halo/textures/Spartan_Legs_Mat_Normal.png",
    "res/model/halo/textures/Spartan_Legs_Mat_Roughness.png",
    "res/model/halo/textures/Spartan_Legs_Mat_Specular.png",
    "res/model/halo/textures/Spartan_Undersuit_Mat_AO.png",
    "res/model/halo/textures/Spartan_Undersuit_Mat_BaseColor.png",
    "res/model/halo/textures/Spartan_Undersuit_Mat_Metallic.png",
    "res/model/halo/textures/Spartan_Undersuit_Mat_Normal.png",
    "res/model/halo/textures/Spartan_Undersuit_Mat_Roughness.png",
    "res/model/halo/textures/Spartan_Undersuit_Mat_Specular.png"
};

struct pngParam{
    int width;
    int height;
    int nrChannels;
    unsigned char *data;
    std::string path;
};

unsigned int TextureFromFile(const char *path, pngParam& pngData);

unsigned int TextureFromFile(const char *path, pngParam& pngData)
{
    std::string filename = std::string(path);
    pngData.path = filename;

    unsigned char *data = stbi_load(filename.c_str(), &pngData.width, &pngData.height, &pngData.nrChannels, 0);
    if (data)
    {
        pngData.data = data;
        mylog(LogLevel::I, "Texture loaded at path: %s, width %d, height %d, channels: %d", path, pngData.width, pngData.height, pngData.nrChannels);
    }
    else
    {
        pngData.data = nullptr;
        mylog(LogLevel::E, "Texture failed to load at path: %s", path);
        return 1;
    }

    return 0;
}

// 线性加载版本
void linearLoadTest() {
    mylog(LogLevel::I, "=== Starting Linear Load Test ===");
    
    auto total_start = std::chrono::high_resolution_clock::now();
    std::vector<pngParam> loadedTextures;
    loadedTextures.reserve(pics.size());
    
    for (const auto& pic : pics) {
        pngParam pngData;
        auto start_time = std::chrono::high_resolution_clock::now();
        unsigned int result = TextureFromFile(pic.c_str(), pngData);
        auto end_time = std::chrono::high_resolution_clock::now();
        auto cost = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
        
        if (result == 0) {
            loadedTextures.push_back(pngData);
            mylog(LogLevel::I, "Loaded texture: %s in %lld ms", pic.c_str(), cost);
        } else {
            mylog(LogLevel::E, "Failed to load texture: %s", pic.c_str());
        }
    }
    
    auto total_end = std::chrono::high_resolution_clock::now();
    auto total_cost = std::chrono::duration_cast<std::chrono::milliseconds>(total_end - total_start).count();
    mylog(LogLevel::I, "Linear loading: loaded %zu textures in %lld ms", loadedTextures.size(), total_cost);
    
    // 统一释放资源
    mylog(LogLevel::I, "Releasing textures...");
    for (auto& tex : loadedTextures) {
        if (tex.data) {
            stbi_image_free(tex.data);
            tex.data = nullptr;
        }
    }
    mylog(LogLevel::I, "All textures released");
    mylog(LogLevel::I, "=== Linear Load Test Completed ===\n");
}

// 多线程加载版本
void multithreadLoadTest() {
    mylog(LogLevel::I, "=== Starting Multithread Load Test ===");
    
    auto total_start = std::chrono::high_resolution_clock::now();
    
    std::vector<pngParam> loadedTextures(pics.size());
    std::vector<std::thread> threads;
    // std::mutex logMutex;
    size_t index = 0;
    
    for (const auto& pic : pics) {
        size_t currentIndex = index++;
        threads.emplace_back([&loadedTextures, currentIndex, pic]() {
            // auto start_time = std::chrono::high_resolution_clock::now();
            unsigned int result = TextureFromFile(pic.c_str(), loadedTextures[currentIndex]);
            // auto end_time = std::chrono::high_resolution_clock::now();
            // auto cost = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
            
            // std::lock_guard<std::mutex> lock(logMutex);
            if (result == 0) {
                // mylog(LogLevel::I, "Loaded texture: %s in %lld ms", pic.c_str(), cost);
            } else {
                // mylog(LogLevel::E, "Failed to load texture: %s", pic.c_str());
                assert(0);
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    auto total_end = std::chrono::high_resolution_clock::now();
    auto total_cost = std::chrono::duration_cast<std::chrono::milliseconds>(total_end - total_start).count();
    
    // 统计成功加载的纹理数量
    size_t successCount = 0;
    for (const auto& tex : loadedTextures) {
        if (tex.data != nullptr) {
            successCount++;
        }
    }
    
    mylog(LogLevel::I, "Multithread loading: loaded %zu textures in %lld ms", successCount, total_cost);
    
    // 统一释放资源
    mylog(LogLevel::I, "Releasing textures...");
    for (auto& tex : loadedTextures) {
        if (tex.data) {
            stbi_image_free(tex.data);
            tex.data = nullptr;
        }
    }
    mylog(LogLevel::I, "All textures released");
    mylog(LogLevel::I, "=== Multithread Load Test Completed ===\n");
}

int main()
{
    mylog(LogLevel::I, "======================================");
    mylog(LogLevel::I, "Starting Texture Loading Comparison Test");
    mylog(LogLevel::I, "Total textures to load: %zu", pics.size());
    mylog(LogLevel::I, "======================================\n");
    
    // 运行线性加载测试
    // linearLoadTest();
    
    // // 运行多线程加载测试
    multithreadLoadTest();
    
    mylog(LogLevel::I, "======================================");
    mylog(LogLevel::I, "All tests completed!");
    mylog(LogLevel::I, "======================================");
    
    return 0;
}