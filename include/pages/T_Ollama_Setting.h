#ifndef T_OLLAMA_SETTING
#define T_OLLAMA_SETTING


#include<QProcess>

#include "T_BasePage.h"
class ElaRadioButton;
class ElaToggleSwitch;
class ElaComboBox;
class T_Ollama_Setting : public T_BasePage
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit T_Ollama_Setting(QWidget* parent = nullptr);
    ~T_Ollama_Setting();

private:
    QProcess *ollama_progress{nullptr};


public:
    void open_ollama(bool enable);
};






#endif
