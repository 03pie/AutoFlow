#include <QObject>

namespace MainWindowDefine
{
    const QString kMainWindowTitle           = "AutoFlow Station"; // 窗口标题
    const QString kToolBarTitle              = "工具栏"; // 工具栏标题

    const QString kMainWindowIconPath        = ":/Resource/Image/Icon.png"; // 窗口图标路径
    const QString kGeneralPluginUIConfigPath = ":/Resource/Json/GeneralPluginUIConfig.json"; // 菜单栏配置文件路径

    const QString kJsonMenusKey              = "menus"; // JSON 菜单键
    const QString kJsonPageConfigKey         = "pages"; // JSON 页面配置键
    const QString kJsonToolbarsKey           = "toolbars"; // JSON 工具栏键

    const QString kJsonActionsKey            = "actions"; // JSON 动作键
    const QString kJsonSubmenusKey           = "submenus"; // JSON 子菜单键

    const QString kJsonIconKey               = "icon"; // JSON 图标键
    const QString kJsonNameKey               = "name"; // JSON 名称键
    const QString kJsonShortcutKey           = "shortcut"; // JSON 快捷键键
    const QString kJsonPluginKey             = "plugin"; // JSON 插件键
    const QString kJsonFunctionKey           = "function"; // JSON 函数键
    const QString kJsonTypeKey               = "type"; // JSON 类型键
    const QString kJsonSeparatorKey          = "separator"; // JSON 分隔符类型


    const int kMainWindowWidth               = 1200; // 窗口宽度
	const int kMainWindowHeight              = 740; // 窗口高度
    const int kNavigationBarWidth            = 260; // 导航栏宽度
}
