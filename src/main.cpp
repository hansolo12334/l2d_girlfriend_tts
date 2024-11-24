#include<QApplication>

#include "ElaApplication.h"
#include "main_window.h"

#include "event_handler.hpp"
#include "resource_loader.hpp"


int main(int argc,char **argv)
{
    if(event_handler::get_instance().initialize() == false)
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
    w.show();


    return a.exec();
}
