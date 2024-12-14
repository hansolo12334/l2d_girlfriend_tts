#include "event_handler.hpp"
#include "message_queue.hpp"
#include <thread>
#include "resource_loader.h"
#include "app_log.h"

#include <QtEvents>
#include <QApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace
{
    constexpr int max_msg_count = 8;

    struct msg_packet:msg_queue::message
    {
        event_handler::event_type type;
        void* data;
        void release() override
        {
            free(data);
            data = NULL;
        }
    };

    std::thread handle_thread;
    msg_queue mq;
    QMainWindow* qmw;

    bool one_task_done=true;

    void handle_task()
    {
        msg_packet packet;
        while(1)
        {
            if(mq.get(&packet)==msg_queue::status::success)
            {
                switch (packet.type)
                {
                case event_handler::event_type::app_exit:
                {
                     APP_LOG_INFO("thread exit");
                     return;
                }
                break;
                case event_handler::event_type::app_config_change:
                {

                    char *config = (char *)packet.data;
                    // APP_LOG_DEBUG("config address: " << static_cast<void*>(config));
                    // APP_LOG_DEBUG("config content: " << config);
                    const char *config_path = resource_loader::get_instance().get_config_path();

                    // APP_LOG_DEBUG("config address (again): " << static_cast<void*>(config));
                    // APP_LOG_DEBUG("config content (again): " << config);

                    if (config != NULL)
                    {
                        FILE* fd;
                        fopen_s(&fd,config_path ,"w");
                        if(fd == NULL)
                        {
                            APP_LOG_ERROR("open file fail: "<<config_path);
                        }
                        else
                        {
                            APP_LOG_INFO("save config success");
                            fwrite(config,1,strlen(config),fd);
                        }
                        // cJSON_free(config);
                        fclose(fd);
                    }
                }
                break;
                case event_handler::event_type::app_all_modle_load_fail:
                    if(qmw)
                    {
                        APP_LOG_INFO("no modle load");
                        QApplication::postEvent(qmw,new QfQevent("no model use",QfQevent::event_type::no_modle));
                    }
                    break;
                case event_handler::event_type::app_current_modle_fail_by_initialize:
                    if(qmw)
                    {
                        APP_LOG_INFO("position update");
                        QApplication::postEvent(qmw,new QfQevent("load current model fail,so load default model",QfQevent::event_type::load_default_model));
                    }
                default:
                    APP_LOG_INFO("error msg: "<<packet.data);
                break;

                }
                one_task_done = true;
            }
        }
    }
    const QEvent::Type mtype = (QEvent::Type)QEvent::registerEventType();
}

QfQevent::QfQevent(const char* why,event_type e):QEvent(mtype)
{
    this->why = why;
    this->e = e;
}

event_handler& event_handler::get_instance()
{
    static event_handler handler;
    return handler;
}

void event_handler::report(event_handler::event_type e,void* data)
{
    APP_LOG_DEBUG(data);
    msg_packet pack;
    pack.data = data;
    pack.type = e;

    msg_queue::status success=mq.post(&pack);
    switch(success)
    {
        case(msg_queue::status::error):
            APP_LOG_DEBUG("msg_queue::status::error");
            break;
        case(msg_queue::status::success):
            APP_LOG_DEBUG("msg_queue::status::success");
            break;
        case(msg_queue::status::fail):
            APP_LOG_DEBUG("msg_queue::status::fail");
            break;

        default:
            break;
    }

}

void event_handler::release()
{
    if(is_init)
    {
        msg_packet pack;
        pack.data = NULL;
        pack.type = event_handler::event_type::app_exit;
        mq.post(&pack);
        handle_thread.join();
        mq.release();
        is_init = false;
        APP_LOG_INFO("release");
    }
    else
    {
        APP_LOG_INFO("no release");
    }
}

bool event_handler::initialize()
{
    if(is_init)
    {
        return true;
    }

    if(mq.initialize(sizeof(msg_packet),max_msg_count)!=msg_queue::status::success)
    {
        APP_LOG_ERROR("initialize mq fail");
        return false;
    }
    APP_LOG_DEBUG("initialize mq success");

    handle_thread = std::thread(handle_task);
    is_init = true;
    return true;
}

event_handler::event_handler()
{
    is_init = false;
}

event_handler::~event_handler()
{
    this->release();
}

void event_handler::resgist_main_window(QMainWindow* mw)
{
    qmw = mw;
}


void event_handler::set_task_done(bool done)
{
    one_task_done = done;
}

bool event_handler::get_task_done()
{
    return one_task_done;
}
