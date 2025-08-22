#include "mainwindow.h"
#include "GeneralDefine.h"
#include "ElaEventBus.h"
#include "ElaLog.h"
#include "ElaTheme.h"
#include "ElaContentDialog.h"
#include "PluginManager.h"
#include "Logger.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QMouseEvent>
 

MainWindow::MainWindow(QWidget* parent)
    : ElaWindow(parent),
    _closeDialog(new ElaContentDialog(this)) 
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

MainWindow::~MainWindow()
{
   
}

/**
 * @brief       初始化 MainWindow 窗口
 * @details     设置窗口的图标、标题、主题等基本属性，并添加一个欢迎页面。
 *              该方法应在构造函数中调用，以确保窗口在显示前完成基本配置。
 * @note        该方法不应包含任何与布局或内容相关的初始化逻辑。
 * @return      void 无返回值
 * @see         MainWindow::MainWindow(QWidget* parent)
 * @warning     如果未设置图标或标题，可能导致窗口显示异常。
 */
void MainWindow::initWindow()
{
    // 设置中心堆栈透明
    setIsCentralStackedWidgetTransparent(true);
    // 设置窗口大小
    resize(MainWindowDefine::mainWindowWidth, MainWindowDefine::mainWindowHeight);
    // 初始化日志
    ElaLog::getInstance()->initMessageLog(true);
    // 设置主题
    eTheme->setThemeMode(ElaThemeType::Dark);
    // 设置导航栏启用
    setIsNavigationBarEnable(true);
    // 设置导航栏显示模式
    setNavigationBarDisplayMode(ElaNavigationType::Compact);
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
    setNavigationBarWidth(MainWindowDefine::navigationBarWidth);
    // 设置窗口显示图标
    setWindowIcon(QIcon(MainWindowDefine::mainWindowIconPath));
	// 设置窗口标题
    setWindowTitle(QString{ MainWindowDefine::mainWindowTitle });
}

void MainWindow::initEdgeLayout()
{

}

void MainWindow::initContent()
{
    qDebug() << "已注册的事件列表" << ElaEventBus::getInstance()->getRegisteredEventsName();
}

/**
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
 */
void MainWindow::initConnect()
{
    /* 拦截关闭窗口信号与动作绑定 */
    // 绑定右侧按钮为关闭主窗口
    connect(_closeDialog, &ElaContentDialog::rightButtonClicked, this, &MainWindow::closeWindow);
    // 绑定中间按钮为最小化主窗口
    connect(_closeDialog, &ElaContentDialog::middleButtonClicked, this, [=]() {_closeDialog->close(); showMinimized(); });
    // 绑定左侧窗口位取消关闭，关闭拦截窗口
    connect(this, &MainWindow::closeButtonClicked, this, [=]() {_closeDialog->exec(); });
}

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
