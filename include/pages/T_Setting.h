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



private:
    ElaComboBox *_characterSelectCombobox{nullptr};
    QList<QAction*> model_action_list;
    QActionGroup *g_change;

    void init_characterSelectCombobox();

private:
    Q_SLOT void on_character_switch(int index);

Q_SIGNALS:
    Q_SIGNAL void error_exit_app();
    Q_SIGNAL void switch_character_xy(int x,int y);
};

#endif // T_SETTING_H
