#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ElaWindow.h"
#include "PluginManager.h"
#include "ElaMenuBar.h"
#include "ElaMenu.h"

#include <QMainWindow>

class ElaContentDialog;

class MainWindow : public ElaWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void initWindow();
    void initEdgeLayout();
    void initContent();
    void initConnect();

    void createMenuFromJson(const QString& jsonFilePath);
    void addMenuItems(ElaMenu* SubMenu, const QJsonArray& SubMenuArray);
	void createActionsConnect(ElaMenu* Menu, QJsonArray& ActionsArray);

protected:
    virtual void mouseReleaseEvent(QMouseEvent* event);

private:
	PluginManager* plugin_manager_{ nullptr };   // 插件管理器
    ElaContentDialog* close_dialog_{ nullptr };  // 自定义关闭对话框
	ElaMenuBar* menu_bar_{ nullptr };            // 自定义菜单栏
};
#endif // MAINWINDOW_H
