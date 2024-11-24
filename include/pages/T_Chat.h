#ifndef T_CHAT_H
#define T_CHAT_H

#include "T_BasePage.h"

#include "ElaPlainTextEdit.h"
#include"ElaLineEdit.h"
#include"ElaComboBox.h"
#include "http_tools.h"

#include <QTemporaryFile>
#include <QAudioSink>

class T_Chat : public T_BasePage
{
 Q_OBJECT
public:
    Q_INVOKABLE explicit T_Chat(QWidget* parent = nullptr);
    ~T_Chat();
Q_SIGNALS:
    // Q_SIGNAL void elaScreenNavigation();
    // Q_SIGNAL void elaBaseComponentNavigation();
    // Q_SIGNAL void elaSceneNavigation();
    // Q_SIGNAL void elaCardNavigation();
    // Q_SIGNAL void elaIconNavigation();

protected:
    Q_SLOT void inputTextEvent();

    // virtual void mouseReleaseEvent(QMouseEvent* event);

private:
    void playAudio(const QByteArray &audioData);

    void parseResponse(QNetworkReply *reply);

    ElaScrollArea *cardScrollArea{nullptr};
    QWidget *cardScrollAreaWidget{nullptr};
    QVBoxLayout *cardScrollAreaWidgetLayout{nullptr};
    ElaLineEdit *text_input_edit{nullptr};

    QTemporaryFile *tempFile=nullptr;

    QString receiving_txt;
    QString received_txt;
    QByteArray response_data;

    bool isPlayingAudio = false;
    QAudioSink *audio = nullptr;

public:
    void send_requests_to_tts();
    void send_requests_to_ollama();
    void send_requests_to_tts_after_ollama_auto();

    void receiveTextEvent();

    ElaText *current_receiveText = nullptr;
    ElaText *current_sendText = nullptr;
    QMap<int, ElaText *> receiveTexts;
    int conversationTimes = 0;
};


#endif
