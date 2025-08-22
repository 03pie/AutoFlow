#include "PluginFramework.h"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

PluginManager::PluginManager() {}
PluginManager::~PluginManager() { unloadAll(); }

void PluginManager::loadPlugin(const std::string& path) 
{
#ifdef _WIN32
    HMODULE hModule = LoadLibraryA(path.c_str());
    if (!hModule)
    {
        std::cerr << "Failed to load " << path << std::endl;
        return;
    }

    auto create = (CreatePluginFunc)GetProcAddress(hModule, "CreatePlugin");
    auto destroy = (DestroyPluginFunc)GetProcAddress(hModule, "DestroyPlugin");
#else
    void* hModule = dlopen(path.c_str(), RTLD_LAZY);
    if (!hModule) 
    {
        std::cerr << "Failed to load " << path << " : " << dlerror() << std::endl;
        return;
    }

    auto create = (CreatePluginFunc)dlsym(hModule, "CreatePlugin");
    auto destroy = (DestroyPluginFunc)dlsym(hModule, "DestroyPlugin");
#endif

    if (create && destroy) 
    {
        IPlugin* plugin = create();
        plugins.push_back({hModule, plugin, destroy});
        std::cout << "Loaded plugin: " << plugin->name() << std::endl;
    } else {
        std::cerr << "Invalid plugin: " << path << std::endl;
#ifdef _WIN32
        FreeLibrary(hModule);
#else
        dlclose(hModule);
#endif
    }
}

void PluginManager::loadPluginsFromDirectory(const std::string& dirPath)
{
    for (const auto& entry : fs::directory_iterator(dirPath)) 
    {
        if (entry.is_regular_file()) 
        {
            auto ext = entry.path().extension().string();
#ifdef _WIN32
            if (ext == ".dll") loadPlugin(entry.path().string());
#else
            if (ext == ".so" || ext == ".dylib") loadPlugin(entry.path().string());
#endif
        }
    }
}

void PluginManager::runAll() 
{
    for (auto& p : plugins) p.instance->execute();
}

void PluginManager::unloadAll() 
{
    for (auto& p : plugins) 
    {
        p.destroy(p.instance);
#ifdef _WIN32
        FreeLibrary(p.hModule);
#else
        dlclose(p.hModule);
#endif
    }
    plugins.clear();
}
