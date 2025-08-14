#ifndef T_STARTPAGE_H
#define T_STARTPAGE_H

#include "T_BasePage.h"

class ElaPivot;
class ElaTabWidget;
class ElaBreadcrumbBar;
class T_StartPage : public T_BasePage
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit T_StartPage(QWidget* parent = nullptr);
    ~T_StartPage();

private:
    ElaBreadcrumbBar* _breadcrumbBar{nullptr};
    ElaPivot* _pivot{nullptr};
    ElaTabWidget* _tabWidget{nullptr};
};

#endif // T_STARTPAGE_H
