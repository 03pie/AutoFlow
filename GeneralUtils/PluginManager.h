#pragma once

#include <string>

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
class IPlugin {
public:
	virtual ~IPlugin() = default;

	virtual std::string GetName() const = 0;
	virtual void Execute() = 0;
};

// 工厂函数签名
using CreatePluginFunction = IPlugin * (*)();
using DestroyPluginFunction = void (*)(IPlugin*);

// ------------------ 插件管理器 ----------------------
struct PluginInformation {
#ifdef _WIN32
	HMODULE module_handle_{};
#else
	void* module_handle_{};
#endif
	IPlugin* plugin_instance_{};
	DestroyPluginFunction destroy_function_{};
};

class PLUGINFRAMEWORK_API PluginManager {
public:
	PluginManager();
	~PluginManager();

	void LoadPluginFromPath(const std::string& plugin_file_path);
	void LoadPluginsFromDirectory(const std::string& directory_path);
	void ExecuteAllPlugins();
	void UnloadAllPlugins();

	size_t GetPluginCount() const;
	PluginInformation GetPluginInformation(size_t plugin_index) const;

private:
	struct ImplementationDetail;
	ImplementationDetail* implementation_detail_pointer_{};
};
