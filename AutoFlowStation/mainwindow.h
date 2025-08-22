#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ElaWindow.h"

#include <QMainWindow>

class T_StartPage;
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

protected:
    virtual void mouseReleaseEvent(QMouseEvent* event);

private:

    T_StartPage* _startPage{ nullptr };        // 启动页
    ElaContentDialog* _closeDialog{ nullptr }; // 自定义关闭对话框
};
#endif // MAINWINDOW_H
