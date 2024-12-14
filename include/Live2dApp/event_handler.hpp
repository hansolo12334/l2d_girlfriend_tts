#ifndef EVENT_HANDLER_HPP
#define EVENT_HANDLER_HPP

#include <QMainWindow>
#include <QEvent>
#include <QObject>


class QfQevent:public QEvent
{
public:
    enum class event_type
    {
        none,
        no_modle,
        load_default_model,
    };
    QfQevent(const char* why,event_type e);
    const char* why;
    event_type e;
};

class event_handler: public QObject
{
Q_OBJECT
public:
    enum class event_type
    {
        app_exit,
        app_all_modle_load_fail,
        app_current_modle_fail_by_initialize,
        app_config_change,
    };
    static event_handler& get_instance();
    bool initialize();
    void release();
    void report(event_type e,void* data);
    void resgist_main_window(QMainWindow* mw);

    void set_task_done(bool done);
    bool get_task_done();

private:
    bool is_init;
    event_handler();
    ~event_handler();
};



#endif
