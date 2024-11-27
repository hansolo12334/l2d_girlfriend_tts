#ifndef T_TTS_SETTING
#define T_TTS_SETTING


#include<QProcess>

#include "T_BasePage.h"
class ElaRadioButton;
class ElaToggleSwitch;
class ElaComboBox;
class T_TTS_Setting : public T_BasePage
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit T_TTS_Setting(QWidget* parent = nullptr);
    ~T_TTS_Setting();

private:
    QProcess *tts_progress{nullptr};

public:
    ElaToggleSwitch *_enableTTsButton{nullptr};

public:
    void open_tts_bat(bool enable,QString bat_path);
};






#endif
