#include<QApplication>

#include "ElaApplication.h"
#include "main_window.h"

#include "event_handler.hpp"
#include "resource_loader.h"

#include <QtCore5Compat/QTextCodec>

int main(int argc,char **argv)
{

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    if (event_handler::get_instance().initialize() == false)
    {
        return 0;
    }

    if(resource_loader::get_instance().initialize() == false)
    {
        return 0;
    }

    QApplication a(argc, argv);
    eApp->init();


    main_window w;
    // w.show();

    QApplication::setQuitOnLastWindowClosed(false);
    return a.exec();
}
