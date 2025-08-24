#ifndef T_HOME_H
#define T_HOME_H

#include "PluginManager.h"
#include "T_BasePage.h"

class ElaMenu;
class T_Home : public T_BasePage
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit T_Home(QWidget* parent = nullptr);
    ~T_Home();
Q_SIGNALS:
    Q_SIGNAL void elaScreenNavigation();
    Q_SIGNAL void elaBaseComponentNavigation();
    Q_SIGNAL void elaSceneNavigation();
    Q_SIGNAL void elaCardNavigation();
    Q_SIGNAL void elaIconNavigation();

protected:
    virtual void mouseReleaseEvent(QMouseEvent* event);

private:
    ElaMenu* _homeMenu{nullptr};
};

// 插件接口类，不包含 Q_OBJECT
class T_Home_Plugin : public IPlugin
{
public:
    void* createInstance(void* context) override;
    bool isUIPlugin() const override { return true; }
};
#endif // T_HOME_H
