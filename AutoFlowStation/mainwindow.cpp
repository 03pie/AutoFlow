#include "MainWindow.h"
#include "GeneralDefine.h"
#include "ElaEventBus.h"
#include "ElaLog.h"
#include "ElaTheme.h"
#include "ElaContentDialog.h"

#include "ElaText.h"
#include "ElaStatusBar.h"
#include "ElaProgressBar.h"
#include "ElaToolButton.h"
#include "ElaToolBar.h"
#include "ElaMenuBar.h"
#include "ElaMenu.h"
#include "Logger.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>

#include <QDebug>
#include <QHBoxLayout>
#include <QMouseEvent>
 
/***************************************************************************
 * @brief       构造函数
 * @details     该方法负责初始化 MainWindow 的基本属性，包括创建关闭对话框、插件管理器等
 * @param       parent 父窗口指针
 * @return      void 无返回值
 * @see         MainWindow::~MainWindow()
 * @warning     如果未正确初始化资源，可能导致窗口显示异常。
 ***************************************************************************/
MainWindow::MainWindow(QWidget* parent)
    : ElaWindow(parent),
      close_dialog_(new ElaContentDialog(this)),
	  plugin_manager_(new PluginManager()
    ) 
{
    // 拦截默认关闭事件
    this->setIsDefaultClosed(false);

	//初始化窗口
    initWindow();

    //初始化额外布局
    initEdgeLayout();

    //初始化中心窗口
    initContent();

    //初始化信号绑定
    initConnect();

    //主窗口移动到中心
    moveToCenter();
}

/***************************************************************************
 * @brief       析构函数
 * @details     该方法负责释放 MainWindow 的资源，包括关闭对话框、插件管理器等
 * @return      void 无返回值
 * @see         MainWindow::MainWindow(QWidget* parent)
 * @warning     如果未正确释放资源，可能导致内存泄漏。
 ***************************************************************************/
MainWindow::~MainWindow()
{
   
}

/***************************************************************************
 * @brief       初始化 MainWindow 窗口
 * @details     设置窗口的图标、标题、主题等基本属性，并添加一个欢迎页面。
 *              该方法应在构造函数中调用，以确保窗口在显示前完成基本配置。
 * @note        该方法不应包含任何与布局或内容相关的初始化逻辑。
 * @return      void 无返回值
 * @see         MainWindow::MainWindow(QWidget* parent)
 * @warning     如果未设置图标或标题，可能导致窗口显示异常。
 ***************************************************************************/
void MainWindow::initWindow()
{
    // 设置中心堆栈透明
    setIsCentralStackedWidgetTransparent(true);
    // 设置窗口大小
    resize(MainWindowDefine::kMainWindowWidth, MainWindowDefine::kMainWindowHeight);
    // 初始化日志
    ElaLog::getInstance()->initMessageLog(true);
    // 设置主题
    eTheme->setThemeMode(ElaThemeType::Dark);
    // 设置导航栏启用
    setIsNavigationBarEnable(true);
    // 设置导航栏显示模式
    setNavigationBarDisplayMode(ElaNavigationType::Auto);
    // 设置窗口按钮标志
    //setWindowButtonFlag(ElaAppBarType::MinimizeButtonHint, false);
    // 设置用户信息卡片不可见
    setUserInfoCardVisible(false);
    // 设置用户信息卡片
    //setUserInfoCardPixmap(QPixmap(":/Resource/Image/Cirno.jpg"));
    //setUserInfoCardTitle("Ela Tool");
    //setUserInfoCardSubTitle("Liniyous@gmail.com");
    // 设置窗口置顶
    setIsStayTop(true);
    // 设置导航栏宽度
    setNavigationBarWidth(MainWindowDefine::kNavigationBarWidth);
    // 设置窗口显示图标
    setWindowIcon(QIcon(MainWindowDefine::kMainWindowIconPath));
	// 设置窗口标题
    //setWindowTitle(QString{ MainWindowDefine::kMainWindowTitle });
}

/***************************************************************************
 * @brief       初始化 MainWindow 的边缘布局
 * @details     该方法负责设置主窗口的边缘布局，包括工具栏、状态栏等
 *              该方法应在 initWindow() 之后调用，以确保窗口基本属性已设置。
 * @return      void 无返回值
 * @see         MainWindow::initWindow()
 * @warning     如果未正确设置边缘布局，可能导致窗口显示异常。
 ***************************************************************************/
void MainWindow::initEdgeLayout()
{
    //菜单栏
    menu_bar_ = new ElaMenuBar(this);
    //menuBar->setFixedHeight(30);
    QWidget* customWidget = new QWidget(this);
    QVBoxLayout* customLayout = new QVBoxLayout(customWidget);
    customLayout->setContentsMargins(0, 0, 0, 0);
    customLayout->addWidget(menu_bar_);
    customLayout->addStretch();
    this->setCustomWidget(ElaAppBarType::LeftArea, customWidget);
    this->setCustomWidgetMaximumWidth(700);

	// 创建菜单栏内容
    createMenuFromJson(MainWindowDefine::kGeneralPluginUIConfigPath);

    //工具栏
    ElaToolBar* toolBar = new ElaToolBar("工具栏", this);
    toolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    toolBar->setToolBarSpacing(3);
    toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    toolBar->setIconSize(QSize(25, 25));
    // toolBar->setFloatable(false);
    // toolBar->setMovable(false);
    ElaToolButton* toolButton1 = new ElaToolButton(this);
    toolButton1->setElaIcon(ElaIconType::BadgeCheck);
    toolBar->addWidget(toolButton1);
    ElaToolButton* toolButton2 = new ElaToolButton(this);
    toolButton2->setElaIcon(ElaIconType::ChartUser);
    toolBar->addWidget(toolButton2);
    toolBar->addSeparator();
    ElaToolButton* toolButton3 = new ElaToolButton(this);
    toolButton3->setElaIcon(ElaIconType::Bluetooth);
    toolButton3->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolButton3->setText("Bluetooth");
    toolBar->addWidget(toolButton3);
    ElaToolButton* toolButton4 = new ElaToolButton(this);
    toolButton4->setElaIcon(ElaIconType::BringFront);
    toolBar->addWidget(toolButton4);
    toolBar->addSeparator();
    ElaToolButton* toolButton5 = new ElaToolButton(this);
    toolButton5->setElaIcon(ElaIconType::ChartSimple);
    toolBar->addWidget(toolButton5);
    ElaToolButton* toolButton6 = new ElaToolButton(this);
    toolButton6->setElaIcon(ElaIconType::FaceClouds);
    toolBar->addWidget(toolButton6);
    ElaToolButton* toolButton8 = new ElaToolButton(this);
    toolButton8->setElaIcon(ElaIconType::Aperture);
    toolBar->addWidget(toolButton8);
    ElaToolButton* toolButton9 = new ElaToolButton(this);
    toolButton9->setElaIcon(ElaIconType::ChartMixed);
    toolBar->addWidget(toolButton9);
    ElaToolButton* toolButton10 = new ElaToolButton(this);
    toolButton10->setElaIcon(ElaIconType::Coins);
    toolBar->addWidget(toolButton10);
    ElaToolButton* toolButton11 = new ElaToolButton(this);
    toolButton11->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolButton11->setElaIcon(ElaIconType::AlarmPlus);
    toolButton11->setText("AlarmPlus");
    toolBar->addWidget(toolButton11);
    ElaToolButton* toolButton12 = new ElaToolButton(this);
    toolButton12->setElaIcon(ElaIconType::Crown);
    toolBar->addWidget(toolButton12);
    QAction* test = new QAction(this);
    test->setMenu(new QMenu(this));

    ElaProgressBar* progressBar = new ElaProgressBar(this);
    progressBar->setMinimum(0);
    progressBar->setMaximum(0);
    progressBar->setFixedWidth(350);
    toolBar->addWidget(progressBar);

    this->addToolBar(Qt::TopToolBarArea, toolBar);

    ////停靠窗口
    //ElaDockWidget* logDockWidget = new ElaDockWidget("日志信息", this);
    //logDockWidget->setWidget(new T_LogWidget(this));
    //this->addDockWidget(Qt::RightDockWidgetArea, logDockWidget);
    //resizeDocks({ logDockWidget }, { 200 }, Qt::Horizontal);

    //ElaDockWidget* updateDockWidget = new ElaDockWidget("更新内容", this);
    //updateDockWidget->setWidget(new T_UpdateWidget(this));
    //this->addDockWidget(Qt::RightDockWidgetArea, updateDockWidget);
    //resizeDocks({ updateDockWidget }, { 200 }, Qt::Horizontal);

    //状态栏
    ElaStatusBar* statusBar = new ElaStatusBar(this);
    ElaText* statusText = new ElaText("初始化成功!!！", this);
    statusText->setTextPixelSize(14);
    statusBar->addWidget(statusText);
    this->setStatusBar(statusBar);
}

/***************************************************************************
 * @brief       初始化 MainWindow 的内容区域
 * @details     该方法负责设置主窗口的内容区域，包括欢迎页面、功能区等
 *              该方法应在 initEdgeLayout() 之后调用，以确保边缘布局已设置。
 * @return      void 无返回值
 * @see         MainWindow::initEdgeLayout()
 * @warning     如果未正确设置内容区域，可能导致窗口显示异常。
 ***************************************************************************/
void MainWindow::initContent()
{

	createPageFromJson(MainWindowDefine::kGeneralPluginUIConfigPath);
    qDebug() << "已注册的事件列表" << ElaEventBus::getInstance()->getRegisteredEventsName();
}

/***************************************************************************
 * @brief       初始化 MainWindow 的信号槽连接
 * @details     将 MainWindow 内部以及子控件（如 ElaAppBar、对话框、菜单等）的信号
 *              统一绑定到对应的槽函数或 Lambda，用于管理窗口交互逻辑。
 *              例如：
 *              - 点击关闭按钮弹出自定义对话框
 *              - 对话框按钮执行最小化或关闭操作
 *              - 自定义菜单或控件的事件响应
 * @return      void 无返回值
 * @note        本方法应在构造函数或 UI 初始化后调用，确保控件已创建，否则 connect 可能失败。
 * @warning     如果控件尚未初始化，可能导致连接失效。
 * @see         MainWindow::MainWindow(QWidget* parent)
 ***************************************************************************/
void MainWindow::initConnect()
{
    /* 拦截关闭窗口信号与动作绑定 */
    // 绑定右侧按钮为关闭主窗口
    connect(close_dialog_, &ElaContentDialog::rightButtonClicked, this, &MainWindow::closeWindow);
    // 绑定中间按钮为最小化主窗口
    connect(close_dialog_, &ElaContentDialog::middleButtonClicked, this, [=]() {close_dialog_->close(); showMinimized(); });
    // 绑定左侧窗口位取消关闭，关闭拦截窗口
    connect(this, &MainWindow::closeButtonClicked, this, [=]() {close_dialog_->exec(); });
}

/***************************************************************************
 * @brief       从 JSON 文件创建页面
 * @details     该方法负责读取指定的 JSON 文件，并根据文件内容动态创建页面
 *              该方法应在 initUI() 之后调用，以确保 UI 已初始化。
 * @param[in]   jsonFilePath JSON 文件路径
 * @return      void 无返回值
 * @see         MainWindow::initUI()
 * @warning     如果 JSON 文件格式不正确，可能导致页面创建失败。
 ***************************************************************************/
void MainWindow::createPageFromJson(const QString& jsonFilePath)
{
    QJsonArray pages_array = readJsonArrayFromFile(jsonFilePath, MainWindowDefine::kPageConfigKey); // 获取 "pages" 数组

    for (const QJsonValue& item_page : pages_array)
    {
        if (!item_page.isObject())
        {
            qDebug() << "页面项格式错误，期望为 JSON 对象";
            continue;
        }
        // 创建页面节点
        QJsonObject page_obj = item_page.toObject();
        QString page_name = page_obj.value(MainWindowDefine::kJsonNameKey).toString();
        QString page_icon = page_obj.value(MainWindowDefine::kJsonIconKey).toString();
        QString page_plugin = page_obj.value(MainWindowDefine::kJsonPluginKey).toString().append(QString::fromStdString(SHARED_LIB_SUFFIX));

        // 创建页面并添加到主窗口
        plugin_manager_->LoadPluginFromPath(page_plugin.toStdString());
		auto plugin_info = plugin_manager_->GetPluginInformation(plugin_manager_->GetPluginCount() - 1);
        try
        {
            QWidget* plugin_widget = static_cast<QWidget*>(plugin_info.plugin_instance->createInstance(this));
            if (plugin_widget)
            {
                addPageNode(page_name,
                    plugin_widget,
                    static_cast<ElaIconType::IconName>(page_icon.toLongLong({}, 16)));
            }
        }
        catch (const std::exception& e) {
            qDebug() << "Plugin exception:" << e.what();
        }
        catch (...) {
            qDebug() << "Unknown plugin exception";
        }
    }
}

/***************************************************************************
 * @brief       从 JSON 文件创建菜单
 * @details     该方法负责读取指定的 JSON 文件，并根据文件内容动态创建菜单
 *              该方法应在 initUI() 之后调用，以确保 UI 已初始化。
 * @param[in]   jsonFilePath JSON 文件路径
 * @return      void 无返回值
 * @see         MainWindow::initUI()
 * @warning     如果 JSON 文件格式不正确，可能导致菜单创建失败。
 ***************************************************************************/
void MainWindow::createMenuFromJson(const QString& jsonFilePath)
{
    QJsonArray menus_array = readJsonArrayFromFile(jsonFilePath, MainWindowDefine::kJsonMenusKey); // 获取 "menus" 数组
    for (const QJsonValue& item_menu : menus_array)
    {
        if (!item_menu.isObject())
        {
            qDebug() << "菜单项格式错误，期望为 JSON 对象";
            continue;
        }
		// 创建菜单节点
        QJsonObject menu_obj = item_menu.toObject();
        QString menu_name = menu_obj.value(MainWindowDefine::kJsonNameKey).toString();
        QString menu_type = menu_obj.value(MainWindowDefine::kJsonTypeKey).toString();
		ElaMenu* menu_root = new ElaMenu(menu_name, this);  // 创建菜单，不添加图标
        menu_bar_->addMenu(menu_root);
		// 添加菜单的动作
        QJsonArray menu_actions = menu_obj.value(MainWindowDefine::kJsonActionsKey).toArray();
        createActionsConnect(menu_root, menu_actions);
		// 递归添加子菜单
        QJsonArray submenu_array = menu_obj.value(MainWindowDefine::kJsonSubmenusKey).toArray();
        addMenuItems(menu_root, submenu_array); 
    }
}

/***************************************************************************
 * @brief       添加子菜单项
 * @details     该方法负责递归添加子菜单项
 * @param[in]   SubMenu 子菜单对象
 * @param[in]   SubMenuArray 子菜单 JSON 数组
 * @return      void 无返回值
 * @see         MainWindow::createMenuFromJson()
 * @warning     如果 JSON 格式不正确，可能导致菜单项添加失败。
 ***************************************************************************/
void MainWindow::addMenuItems(ElaMenu* SubMenu, const QJsonArray& SubMenuArray)
{
    for (const QJsonValue& iterm_subMenu : SubMenuArray)
    {
        if (!iterm_subMenu.isObject())
        {
            qDebug() << "子菜单项格式错误，期望为 JSON 对象";
            continue;
        }
		// 创建子菜单节点
        QJsonObject subitem_obj = iterm_subMenu.toObject();
        QString submenu_name = subitem_obj.value(MainWindowDefine::kJsonNameKey).toString();
        ElaMenu* submenu = SubMenu->addMenu(submenu_name);
		// 添加子菜单的动作
        QJsonArray subactions = subitem_obj.value(MainWindowDefine::kJsonActionsKey).toArray();
		createActionsConnect(submenu, subactions);
		// 递归添加子菜单
        QJsonArray submenu_array = subitem_obj.value(MainWindowDefine::kJsonSubmenusKey).toArray();
        addMenuItems(submenu, submenu_array); 
    }
}

/***************************************************************************
 * @brief       创建菜单动作并连接信号槽
 * @details     该方法负责为指定菜单创建动作，并将动作的触发信号连接到对应的槽函数
 * @param[in]   Menu 菜单对象
 * @param[in]   ActionsArray 动作 JSON 数组
 * @return      void 无返回值
 * @see         MainWindow::createMenuFromJson()
 * @warning     如果 JSON 格式不正确，可能导致动作创建或连接失败。
 ***************************************************************************/
void MainWindow::createActionsConnect(ElaMenu* Menu, QJsonArray& ActionsArray)
{
    if(Menu == nullptr || ActionsArray.isEmpty())
    {
        qDebug() << "菜单对象为空，无法添加动作";
        return;
	}
    for (const QJsonValue& item_action : ActionsArray)
    {
        if (!item_action.isObject())
        {
            qDebug() << "菜单项格式错误，期望为 JSON 对象";
            continue;
        }
        QJsonObject action_obj = item_action.toObject();
        QString action_name = action_obj.value(MainWindowDefine::kJsonNameKey).toString();
        QString action_shortcut = action_obj.value(MainWindowDefine::kJsonShortcutKey).toString();
        QString action_icon = action_obj.value(MainWindowDefine::kJsonIconKey).toString();
        QString action_plugin = action_obj.value(MainWindowDefine::kJsonPluginKey).toString().append(QString::fromStdString(SHARED_LIB_SUFFIX));
        QString action_function = action_obj.value(MainWindowDefine::kJsonFunctionKey).toString();

        connect(Menu->addElaIconAction(static_cast<ElaIconType::IconName>(action_icon.toULongLong({}, 16)), action_name, static_cast<QKeySequence::StandardKey>(action_shortcut.toUInt())),
            &QAction::triggered, this, [this, action_plugin, action_function]()
            {
                // 处理插件和功能的调用
                if (!action_plugin.isEmpty() && !action_function.isEmpty())
                {
                    // TODO: 调用插件的功能
                }
            }
        );

        if (action_obj.value(MainWindowDefine::kJsonSeparatorKey).toBool(false))
        {
            Menu->addSeparator();
        }
    }
}

/***************************************************************************
 * @brief       从文件中读取 JSON 数组
 * @details     该方法负责从指定的 JSON 文件中读取数组
 * @param[in]   filePath 文件路径
 * @param[in]   arrayKey 数组键
 * @return      QJsonArray 读取到的 JSON 数组
 * @see         MainWindow::readJsonArrayFromFile()
 * @warning     如果文件无法打开或格式不正确，可能导致读取失败。
 ***************************************************************************/
QJsonArray MainWindow::readJsonArrayFromFile(const QString& filePath, const QString& arrayKey)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "无法打开菜单配置文件:" << filePath;
        return QJsonArray{};
    }

    QByteArray json_data = file.readAll();
    file.close();

    QJsonDocument json_doc = QJsonDocument::fromJson(json_data);
    if (!json_doc.isObject())
    {
        qDebug() << "菜单配置文件格式错误，期望为 JSON 对象";
        return QJsonArray{};
    }

    QJsonObject json_root_obj = json_doc.object();
    return json_root_obj[arrayKey].toArray();
}

/***************************************************************************
 * @brief       处理鼠标释放事件
 * @details     该方法负责处理主窗口的鼠标释放事件，包括导航按钮的点击响应
 * @param       event 鼠标事件对象
 * @return      void 无返回值
 * @see         MainWindow::mousePressEvent()
 * @warning     如果未正确处理鼠标事件，可能导致窗口交互异常。
 ***************************************************************************/
void MainWindow::mouseReleaseEvent(QMouseEvent* event)
{
    if (getCurrentNavigationIndex() != 2)
    {
        switch (event->button())
        {
        case Qt::BackButton:
        {
            this->setCurrentStackIndex(0);
            break;
        }
        case Qt::ForwardButton:
        {
            this->setCurrentStackIndex(1);
            break;
        }
        default:
        {
            break;
        }
        }
    }
    ElaWindow::mouseReleaseEvent(event);
}
