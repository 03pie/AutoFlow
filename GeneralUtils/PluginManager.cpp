#include "PluginManager.h"

void PluginManager::LoadPluginFromPath(const std::string& path) 
{
#ifdef _WIN32
    HMODULE hModule = LoadLibraryA(path.c_str());
    if (!hModule) {  return; }

    auto createFunc  = (CreatePluginFunction)GetProcAddress(hModule, "CreatePlugin");
    auto destroyFunc = (DestroyPluginFunction)GetProcAddress(hModule, "DestroyPlugin");

    if (!createFunc || !destroyFunc) {
        FreeLibrary(hModule);
        return;
    }

    IPlugin* instance = createFunc();
    plugins_.push_back({ hModule, instance, destroyFunc, path });
#else
    // Linux/macOS dlopen / dlsym
#endif
}

void PluginManager::LoadPluginsFromDirectory(const std::string& dir_path) {
    if (!fs::exists(dir_path) || !fs::is_directory(dir_path)) return;
    for (const auto& entry : fs::directory_iterator(dir_path)) {
        if (!entry.is_regular_file()) continue;
#ifdef _WIN32
        if (entry.path().extension() != ".dll") continue;
#else
        if (entry.path().extension() != ".so") continue;
#endif
        LoadPluginFromPath(entry.path().string());
    }
}

void PluginManager::UnloadAllPlugins() {
    for (auto& info : plugins_) {
        if (info.destroy_function && info.plugin_instance)
            info.destroy_function(info.plugin_instance);
#ifdef _WIN32
        if (info.module_handle) FreeLibrary(info.module_handle);
#else
        // dlclose(info.module_handle);
#endif
    }
    plugins_.clear();
}

int PluginManager::FindPluginIndexByPath(const std::string& path) const {
    for (size_t i = 0; i < plugins_.size(); ++i)
        if (plugins_[i].file_path == path) return static_cast<int>(i);
    return -1;
}
