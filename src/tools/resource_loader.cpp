#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <QFile>

#include "app_log.h"

#include "event_handler.hpp"
#include "message_queue.hpp"
#include "resource_loader.h"
#include <stdio.h>
#include <string.h>

#define CONFIG_JSON_FILE_PATH "Resources/config.json"
namespace
{
constexpr int config_file_size = 4096;
}

// QWeakPointer<resource_loader> resource_loader::instance;


// QSharedPointer<resource_loader> resource_loader::get_instance()
// {
//     QSharedPointer<resource_loader> shared_instance = instance.lock();
//     if(!shared_instance){
//         shared_instance = QSharedPointer<resource_loader>(new resource_loader());
//         instance = shared_instance;
//     }
//     return shared_instance;
// }


bool resource_loader::initialize()
{
    if (is_init == true)
    {
        APP_LOG_ERROR("initialize finished");
        return true;
    }

    QFile file(CONFIG_JSON_FILE_PATH);
    if (!file.open(QIODevice::ReadOnly))
    {
        APP_LOG_ERROR("无法打开Json文件进行读取: " << CONFIG_JSON_FILE_PATH);
        return false;
    }
    QByteArray json_file = file.readAll();
    file.close();
    if (json_file.size() >= config_file_size)
    {
        APP_LOG_ERROR("Json文件太大");
    }

    QJsonDocument jsonDoc = QJsonDocument::fromJson(json_file);
    if (jsonDoc.isNull() || !jsonDoc.isObject())
    {
        return false;
    }

    json_root= jsonDoc.object();
    //备份json数据
    // json_root = &jsonObject;

    if (json_root.contains("systemtray") && json_root["systemtray"].isString())
    {
        // APP_LOG_DEBUG("systemtray " << json_root["systemtray"].toString());
        system_tray_icon_path = json_root["systemtray"].toString();
    }
    else
    {
        system_tray_icon_path = "resources/image/640.PNG";
    }

    if (json_root.contains("module") && json_root["module"].isArray())
    {
        QJsonArray modules = json_root["module"].toArray();
        for (const QJsonValue &moduleValue : modules)
        {
            if (moduleValue.isObject())
            {
                QJsonObject module = moduleValue.toObject();
                resource_loader::model tmp_model;

                if (module.contains("name") && module["name"].isString())
                {
                    // APP_LOG_DEBUG("name " << module["name"].toString());
                    tmp_model.name = module["name"].toString();
                }
                else
                {
                    continue;
                }
                if (module.contains("window_x") && module["window_x"].isDouble())
                {
                    // APP_LOG_DEBUG("window_x " << module["window_x"].toInt());
                    tmp_model.x = module["window_x"].toInt();
                }
                else
                {
                    continue;
                }
                if (module.contains("window_y") && module["window_y"].isDouble())
                {
                    // APP_LOG_DEBUG("window_y " << module["window_y"].toInt());
                    tmp_model.y = module["window_y"].toInt();
                }
                else
                {
                    continue;
                }

                model_list.emplace_back(tmp_model);
            }
        }
    }
    else
    {
        return false;
    }
    if (model_list.size() == 0)
    {
        return false;
    }

    current_model_index = 0;
    current_model = &model_list[0];

    if (json_root.contains("userdata") && json_root["userdata"].isObject())
    {
        QJsonObject userdata = json_root["userdata"].toObject();
        if (userdata.contains("current_model") && userdata["current_model"].isString())
        {
            // APP_LOG_DEBUG("Current model: " << userdata["current_model"].toString());
            update_current_model(userdata["current_model"].toString());
        }
        if (userdata.contains("top") && userdata["top"].isBool())
        {
            // APP_LOG_DEBUG("Top: " << userdata["top"].toBool());
            top = userdata["top"].toBool();
        }
        else
        {
            top = false;
        }
        if (userdata.contains("move") && userdata["move"].isBool())
        {
            // APP_LOG_DEBUG("Move: " << userdata["move"].toBool());
            move = userdata["move"].toBool();
        }
        else
        {
            move = false;
        }
    }
    else
    {
        top = false;
        move = false;
    }
    //cJSON_Delete(root);
    // json_root = (void*)root;
    is_init = true;
    return true;
}

void resource_loader::release()
{
    if (is_init == false)
    {
        return;
    }


    QJsonDocument jsonDoc(json_root);
    QByteArray jsonData = jsonDoc.toJson();

    std::string temp = jsonData.toStdString();
    char *new_config = const_cast<char *>(temp.c_str());

    event_handler::get_instance().report(event_handler::event_type::app_config_change,
                                         reinterpret_cast<void *>(new_config));
    event_handler::get_instance().set_task_done(false);

    while(!event_handler::get_instance().get_task_done()){

    }
    // cJSON_Delete((cJSON*)json_root);
    is_init = false;
    APP_LOG_INFO("release");
}
const QVector<resource_loader::model> &resource_loader::get_model_list()
{
    return model_list;
}

const QString resource_loader::get_system_tray_icon_path()
{
    return system_tray_icon_path;
}

const resource_loader::model *resource_loader::get_current_model()
{
    return current_model;
}

int resource_loader::get_current_model_index() const
{
    return current_model_index;
}

bool resource_loader::update_current_model(const QString name)
{
    for (auto i = 0; i < model_list.size(); i++)
    {
        if (name == model_list[i].name)
        {
            current_model_index = i;
            current_model = &model_list[i];
            return true;
        }
    }
    return false;
}

bool resource_loader::update_current_model(uint32_t index)
{

    if (index < model_list.size())
    {
        if (&model_list[index] != current_model)
        {
            current_model = &model_list[index];
            if (json_root.contains("userdata") && json_root["userdata"].isObject())
            {
                QJsonObject userdata = (json_root)["userdata"].toObject();
                userdata.insert("current_model", current_model->name);
                json_root.insert("userdata", userdata);
            }
            else
            {
                QJsonObject obj;
                obj.insert("current_model", current_model->name);
                json_root.insert("userdata", obj);
            }
            current_model_index = index;

            QJsonDocument jsonDoc(json_root);
            QByteArray jsonData = jsonDoc.toJson();
            std::string temp = jsonData.toStdString();
            char *new_config = const_cast<char *>(temp.c_str());

            event_handler::get_instance().report(event_handler::event_type::app_config_change,
                                                 reinterpret_cast<void *>(new_config));

            event_handler::get_instance().set_task_done(false);

            while(!event_handler::get_instance().get_task_done()){

            }
            APP_LOG_DEBUG("update_current_model done");
            return true;
        }
    }


    // if(index<model_list.size())
    // {
    //     if(&model_list[index] != current_model)
    //     {
    //         current_model = &model_list[index];
    //         cJSON* node = cJSON_GetObjectItem((cJSON*)json_root,"userdata");
    //         if(node!=NULL&&cJSON_IsObject(node))
    //         {
    //             cJSON* tmp_value = cJSON_GetObjectItem(node,"current_model");
    //             if(tmp_value!=NULL && cJSON_IsString(tmp_value))
    //             {
    //                 cJSON_SetValuestring(tmp_value,current_model->name);
    //             }
    //             else
    //             {
    //                 cJSON* m = cJSON_CreateString(current_model->name);
    //                 cJSON_AddItemToObject(node,"current_model",m);
    //             }
    //         }
    //         else
    //         {
    //             cJSON* obj = cJSON_CreateObject();
    //             cJSON_AddItemToObject((cJSON*)json_root,"userdata",obj);
    //             cJSON* m = cJSON_CreateString(current_model->name);
    //             cJSON_AddItemToObject(node,"current_model",m);
    //         }
    //         current_model_index = index;
    //         char* new_config = cJSON_Print((cJSON*)json_root);
    //         event_handler::get_instance().report(event_handler::event_type::app_config_change,new_config);
    //         return true;
    //     }
    // }
    return false;
}

bool resource_loader::is_top()
{
    return top;
}
void resource_loader::set_top(bool top)
{
    if (this->top != top)
    {
        this->top = top;
        if (json_root.contains("userdata") && json_root["userdata"].isObject())
        {
            QJsonObject userdata = json_root["userdata"].toObject();
            userdata.insert("top", top);
            json_root.insert("userdata", userdata);
        }
        else
        {
            QJsonObject userdata;
            userdata.insert("top", top);
            json_root.insert("userdata", userdata);
        }
    }
    QJsonDocument jsonDoc(json_root);
    QByteArray jsonData = jsonDoc.toJson();
    std::string temp = jsonData.toStdString();
    char *new_config = const_cast<char *>(temp.c_str());

    event_handler::get_instance().report(event_handler::event_type::app_config_change,
                                         reinterpret_cast<void *>(new_config));

    event_handler::get_instance().set_task_done(false);

    while(!event_handler::get_instance().get_task_done()){

    }
    APP_LOG_DEBUG("set_top done");
    // if(this->top!=top)
    // {
    //     this->top = top;
    //     cJSON* node = cJSON_GetObjectItem((cJSON*)json_root,"userdata");
    //     if(node!=NULL&&cJSON_IsObject(node))
    //     {

    //         cJSON* new_item = cJSON_CreateBool(top);
    //         cJSON_ReplaceItemInObject(node,"top",new_item);
    //     }
    //     else
    //     {
    //         cJSON* obj = cJSON_CreateObject();
    //         cJSON_AddItemToObject((cJSON*)json_root,"userdata",obj);
    //         cJSON* m = cJSON_CreateBool(top);
    //         cJSON_AddItemToObject(node,"top",m);
    //     }
    //     char* new_config = cJSON_Print((cJSON*)json_root);
    //     event_handler::get_instance().report(event_handler::event_type::app_config_change,new_config);
    // }
}

bool resource_loader::moveable()
{
    return move;
}
void resource_loader::set_moveable(bool m)
{

    if (this->move != m)
    {

        this->move = m;
        // APP_LOG_DEBUG(json_root);
        // APP_LOG_DEBUG(json_root.contains("userdata"));
        // APP_LOG_DEBUG(json_root["userdata"].isObject());

        if (json_root.contains("userdata") && json_root["userdata"].isObject())
        {

            QJsonObject userdata = json_root["userdata"].toObject();
            userdata.insert("move", m);
            json_root.insert("userdata", userdata);
        }
        else
        {

            QJsonObject userdata;
            userdata.insert("move", m);
            json_root.insert("userdata", userdata);
        }
    }

    // APP_LOG_DEBUG(*json_root);
    QJsonDocument jsonDoc(json_root);
    // APP_LOG_DEBUG(jsonDoc);
    QByteArray jsonData = jsonDoc.toJson();
    std::string temp = jsonData.toStdString();
    char *new_config = const_cast<char *>(temp.c_str());
    // APP_LOG_DEBUG(new_config);
    // void *cffg = reinterpret_cast<void *>(new_config);
    // APP_LOG_DEBUG(cffg);
    // char* config = (char*)cffg;
    // APP_LOG_DEBUG(config);
    event_handler::get_instance().report(event_handler::event_type::app_config_change,
                                         new_config);

    event_handler::get_instance().set_task_done(false);

    while(!event_handler::get_instance().get_task_done()){

    }
    APP_LOG_DEBUG("set_moveable done");
    // if(this->move!=m)
    // {
    //     this->move = m;
    //     cJSON* node = cJSON_GetObjectItem((cJSON*)json_root,"userdata");
    //     if(node!=NULL&&cJSON_IsObject(node))
    //     {
    //         cJSON* new_item = cJSON_CreateBool(this->move);
    //         cJSON_ReplaceItemInObject(node,"move",new_item);
    //     }
    //     else
    //     {
    //         cJSON* obj = cJSON_CreateObject();
    //         cJSON_AddItemToObject((cJSON*)json_root,"userdata",obj);
    //         cJSON* m = cJSON_CreateBool(this->move);
    //         cJSON_AddItemToObject(node,"move",m);
    //     }

    //     char* new_config = cJSON_Print((cJSON*)json_root);
    //     event_handler::get_instance().report(event_handler::event_type::app_config_change,new_config);
    // }
}


bool resource_loader::update_current_model_position(int x, int y)
{

    if (json_root.contains("module") && json_root["module"].isArray())
    {
        QJsonArray modules = json_root["module"].toArray();

        int idx = 0;
        bool found = false;
        for (; idx < modules.size(); idx++)
        {
            QJsonValueRef moduleValue = modules[idx];
            if (moduleValue.isObject())
            {
                QJsonObject module = moduleValue.toObject();
                if (module.contains("name") && module["name"].isString())
                {
                    if (module["name"].toString() == current_model->name)
                    {
                        found = true;
                        break;
                    }
                }
            }
        }

        if (!found)
        {
            return false;
        }

        QJsonObject module = modules[idx].toObject();
        if (module.contains("window_x") && module["window_x"].isDouble())
        {
            module.insert("window_x", x);
        }
        if (module.contains("window_y") && module["window_y"].isDouble())
        {
            module.insert("window_y", y);
        }
        modules.replace(idx, module);
        json_root.insert("module", modules);
    }
    else
    {
        return false;
    }

    return false;
}

const char *resource_loader::get_config_path()
{
    return CONFIG_JSON_FILE_PATH;
}

resource_loader::~resource_loader()
{
    release();
}
