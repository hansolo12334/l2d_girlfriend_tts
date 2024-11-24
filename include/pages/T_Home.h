#ifndef T_HOME_H
#define T_HOME_H

#include "T_BasePage.h"

#include "ElaPlainTextEdit.h"
#include"ElaLineEdit.h"
#include"ElaComboBox.h"

#include "http_tools.h"
#include <QTemporaryFile>

class ElaMenu;
class T_Home : public T_BasePage
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit T_Home(QWidget* parent = nullptr);
    ~T_Home();
Q_SIGNALS:
    Q_SIGNAL void elaScreenNavigation();
    Q_SIGNAL void elaBaseComponentNavigation();
    Q_SIGNAL void elaSceneNavigation();
    Q_SIGNAL void elaCardNavigation();
    Q_SIGNAL void elaIconNavigation();

protected:
    virtual void mouseReleaseEvent(QMouseEvent* event);

private:
    ElaMenu* _homeMenu{nullptr};

public:
    Q_SLOT void send_requests();
    Q_SLOT void send_requests_to_ollama();
    void send_requests_after_ollama_auto();

private:
    QTemporaryFile *tempFile=nullptr;


    ElaComboBox *combx_voice = nullptr;
    ElaPlainTextEdit *plainTextEdit1 = nullptr;
    ElaPlainTextEdit *plainTextEdit2 = nullptr;
    ElaLineEdit *textEdit1 = nullptr;
    ElaLineEdit *promot_size_Edit1 = nullptr;

    QString receiving_txt;
    QString received_txt;
    QByteArray response_data;

private:
    void playAudio(const QByteArray &audioData);

    void parseResponse(QNetworkReply *reply);

public:
    bool enableTTS = true;
};

#endif // T_HOME_H
