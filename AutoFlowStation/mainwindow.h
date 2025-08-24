#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ElaWindow.h"
#include "PluginManager.h"
#include "ElaMenuBar.h"
#include "ElaMenu.h"
#include "ElaToolBar.h"
#include "ElaStatusBar.h"

#include <QMainWindow>

class ElaContentDialog;

class MainWindow : public ElaWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    // 初始化窗口
    void initWindow();
    // 初始化边缘布局
    void initEdgeLayout();
    // 初始化内容区域
    void initContent();
    // 初始化信号槽连接
    void initConnect();

    // 配置菜单栏
	void configMenuBar();
	// 配置工具栏
	void configToolBar();
    // 配置停靠窗口
	void configDockWidget();
    // 配置状态栏
	void configStatusBar();
    // 创建页面
    void createPageFromJson(const QString& jsonFilePath);
    // 创建菜单
    void createMenuFromJson(const QString& jsonFilePath);
    // 创建工具栏
	void createToolBarFromJson(const QString& jsonFilePath);
    // 创建菜单项
    void createMenuItems(ElaMenu* SubMenu, const QJsonArray& SubMenuArray);
    // 创建动作连接
	void createActionsConnect(ElaMenu* Menu, QJsonArray& ActionsArray);
    // 执行插件功能
	void excutePluginFunction(const QString& pluginName, const QString& functionName);
    // 读取 JSON 数组
	QJsonArray readJsonArrayFromFile(const QString& filePath, const QString& arrayKey);

protected:
    // 重写鼠标释放事件
    virtual void mouseReleaseEvent(QMouseEvent* event);

private:
	PluginManager* plugin_manager_{ nullptr };   // 插件管理器
    ElaContentDialog* close_dialog_{ nullptr };  // 自定义关闭对话框
	ElaMenuBar* menu_bar_{ nullptr };            // 自定义菜单栏
	ElaToolBar* tool_bar_{ nullptr };            // 自定义工具栏
	ElaStatusBar* status_bar_{ nullptr };        // 自定义状态栏

	QString window_title_{ "AutoFlowStation" };  // 窗口标题
	QWidget* T_home_{ nullptr };
};
#endif // MAINWINDOW_H
