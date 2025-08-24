#pragma once

#include <string>
#include <filesystem>
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#ifdef PLUGINFRAMEWORK_EXPORTS
#define PLUGINFRAMEWORK_API __declspec(dllexport)
#else
#define PLUGINFRAMEWORK_API __declspec(dllimport)
#endif

// ---------------------- 插件接口 ----------------------
struct IPlugin {
    virtual ~IPlugin() = default;
    virtual void* createInstance(void* context) = 0;
    virtual bool isUIPlugin() const { return false; }
};

using CreatePluginFunction  = IPlugin* (*)();
using DestroyPluginFunction = void (*)(IPlugin*);

// ------------------ 插件管理器 ----------------------
namespace fs = std::filesystem;

struct PluginInformation {
#ifdef _WIN32
    HMODULE module_handle{};
#else
    void* module_handle{};
#endif
    IPlugin* plugin_instance{};
    DestroyPluginFunction destroy_function{};
    std::string file_path;
    void* ui_widget{nullptr}; // 宿主可以 cast 成 QWidget*
};

class PLUGINFRAMEWORK_API PluginManager {
public:
    PluginManager() = default;
    ~PluginManager() { UnloadAllPlugins(); }

    void LoadPluginFromPath(const std::string& path);
    void LoadPluginsFromDirectory(const std::string& dir_path);
    void UnloadAllPlugins();
    int FindPluginIndexByPath(const std::string& path) const;

    size_t GetPluginCount() const { return plugins_.size(); }
    PluginInformation& GetPluginInformation(size_t index) { return plugins_.at(index); }

    template<typename T>
    T* GetPluginInstance(size_t index, void* parent = nullptr) {
        if (index >= plugins_.size()) return nullptr;
        IPlugin* plugin = plugins_[index].plugin_instance;
        if (!plugin) return nullptr;
        void* ptr = plugin->createInstance(parent);
        return static_cast<T*>(ptr);
    }

private:
    std::vector<PluginInformation> plugins_;
};