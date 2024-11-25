#ifndef T_SETTING_H
#define T_SETTING_H

#include<QProcess>

#include "T_BasePage.h"
class ElaRadioButton;
class ElaToggleSwitch;
class ElaComboBox;
class T_Setting : public T_BasePage
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit T_Setting(QWidget* parent = nullptr);
    ~T_Setting();

private:
    ElaComboBox* _themeComboBox{nullptr};
    ElaToggleSwitch* _micaSwitchButton{nullptr};
    ElaToggleSwitch* _logSwitchButton{nullptr};
    ElaRadioButton* _minimumButton{nullptr};
    ElaRadioButton* _compactButton{nullptr};
    ElaRadioButton* _maximumButton{nullptr};
    ElaRadioButton* _autoButton{nullptr};

    QProcess *tts_progress{nullptr};
    QProcess *ollama_progress{nullptr};

public:
    ElaToggleSwitch *_enableTTsButton{nullptr};

public:
    void open_tts_bat(bool enable,QString bat_path);
    void open_ollama(bool enable);
};

#endif // T_SETTING_H
