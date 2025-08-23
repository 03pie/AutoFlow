#include "PluginManager.h"

#include <vector>
#include <stdexcept>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;


struct PluginManager::ImplementationDetail 
{
    std::vector<PluginInformation> module_handle_;
};


PluginManager::PluginManager()
    : implementation_detail_pointer_(new ImplementationDetail()) 
{
}

PluginManager::~PluginManager() 
{
    UnloadAllPlugins();
    delete implementation_detail_pointer_;
    implementation_detail_pointer_ = nullptr;
}


void PluginManager::LoadPluginFromPath(const std::string& plugin_file_path) 
{
#ifdef _WIN32
    HMODULE module_handle_ = LoadLibraryA(plugin_file_path.c_str());
    if (!module_handle_) {
        std::cerr << "Failed to load plugin: " << plugin_file_path << "\n";
        return;
    }

    auto create_function = reinterpret_cast<CreatePluginFunction>(
        GetProcAddress(module_handle_, "CreatePlugin"));
    auto destroy_function_ = reinterpret_cast<DestroyPluginFunction>(
        GetProcAddress(module_handle_, "DestroyPlugin"));

    if (!create_function || !destroy_function_) 
    {
        std::cerr << "Invalid plugin: " << plugin_file_path << "\n";
        FreeLibrary(module_handle_);
        return;
    }
#else
    void* module_handle_ = dlopen(plugin_file_path.c_str(), RTLD_LAZY);
    if (!module_handle_) {
        std::cerr << "Failed to load plugin: " << plugin_file_path
            << " - " << dlerror() << "\n";
        return;
    }

    auto create_function = reinterpret_cast<CreatePluginFunction>(
        dlsym(module_handle_, "CreatePlugin"));
    auto destroy_function_ = reinterpret_cast<DestroyPluginFunction>(
        dlsym(module_handle_, "DestroyPlugin"));

    if (!create_function || !destroy_function_) {
        std::cerr << "Invalid plugin: " << plugin_file_path << "\n";
        dlclose(module_handle_);
        return;
    }
#endif

    IPlugin* plugin_instance_ = create_function();
    if (!plugin_instance_) 
    {
        std::cerr << "Failed to create plugin instance: " << plugin_file_path
            << "\n";
#ifdef _WIN32
        FreeLibrary(module_handle_);
#else
        dlclose(module_handle_);
#endif
        return;
    }

    PluginInformation plugin_info;
    plugin_info.module_handle_ = module_handle_;
    plugin_info.plugin_instance_ = plugin_instance_;
    plugin_info.destroy_function_ = destroy_function_;

    implementation_detail_pointer_->module_handle_.push_back(plugin_info);
}

void PluginManager::LoadPluginsFromDirectory(const std::string& directory_path) 
{
    if (!fs::exists(directory_path) || !fs::is_directory(directory_path)) 
    {
        std::cerr << "Directory does not exist: " << directory_path << "\n";
        return;
    }

    for (const auto& entry : fs::directory_iterator(directory_path)) 
    {
        if (!entry.is_regular_file()) continue;

#ifdef _WIN32
        if (entry.path().extension() == ".dll") 
        {
#else
        if (entry.path().extension() == ".so") 
        {
#endif
            LoadPluginFromPath(entry.path().string());
        }
    }
}

void PluginManager::ExecuteAllPlugins() 
{
    for (const auto& plugin_info : implementation_detail_pointer_->module_handle_) 
    {
        if (plugin_info.plugin_instance_) 
        {
            plugin_info.plugin_instance_->Execute();
        }
    }
}

void PluginManager::UnloadAllPlugins() 
{
    for (auto& plugin_info : implementation_detail_pointer_->module_handle_) 
    {
        if (plugin_info.destroy_function_ && plugin_info.plugin_instance_) 
        {
            plugin_info.destroy_function_(plugin_info.plugin_instance_);
            plugin_info.plugin_instance_ = nullptr;
        }

#ifdef _WIN32
        if (plugin_info.module_handle_) FreeLibrary(plugin_info.module_handle_);
#else
        if (plugin_info.module_handle_) dlclose(plugin_info.module_handle_);
#endif
        plugin_info.module_handle_ = nullptr;
    }

    implementation_detail_pointer_->module_handle_.clear();
}


size_t PluginManager::GetPluginCount() const 
{
    return implementation_detail_pointer_->module_handle_.size();
}

PluginInformation PluginManager::GetPluginInformation(size_t plugin_index) const 
{
    if (plugin_index >= implementation_detail_pointer_->module_handle_.size()) 
    {
        throw std::out_of_range("Plugin index out of range");
    }
    return implementation_detail_pointer_->module_handle_[plugin_index];
}
