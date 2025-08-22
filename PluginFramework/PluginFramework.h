#pragma once
#include <string>
#include <vector>

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
class IPlugin 
{
public:
    virtual ~IPlugin() = default;
    virtual std::string name() const = 0;
    virtual void execute() = 0;
};

// 工厂函数签名
typedef IPlugin* (*CreatePluginFunc)();
typedef void (*DestroyPluginFunc)(IPlugin*);

// ------------------ 插件管理器 ----------------------
struct PluginHandle 
{
#ifdef _WIN32
    HMODULE hModule;
#else
    void* hModule;
#endif
    IPlugin* instance;
    DestroyPluginFunc destroy;
};

class PLUGINFRAMEWORK_API PluginManager 
{
public:
    PluginManager();
    ~PluginManager();

	// 加载单个插件
    void loadPlugin(const std::string& path);
	// 从指定目录加载所有插件
    void loadPluginsFromDirectory(const std::string& dirPath);
	// 获取已加载插件列表
    void runAll();
	// 卸载所有插件
    void unloadAll();

private:
    std::vector<PluginHandle> plugins;
};
