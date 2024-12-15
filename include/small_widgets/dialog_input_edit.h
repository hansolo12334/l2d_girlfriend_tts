#ifndef DIALOG_INPUT_EDIT_H
#define DIALOG_INPUT_EDIT_H


#include <QCloseEvent>
#include <QGuiApplication>
#include <QHBoxLayOut>
#include <QMouseEvent>
#include <QPoint>
#include <QPushButton>
#include <QVBoxLayOut>
#include <QLineEdit>
#include<QMouseEvent>

#include "ElaLineEdit.h"
#include "ElaIconButton.h"

#include"sence_voice_webserver_api.h"
#include "media_manager.h"
#include "T_AnimationBubble.h"

class dialogInputEdit : public QWidget
{
Q_OBJECT

public:
    dialogInputEdit(QWidget *parent=nullptr);


    void moveToButtom();

private:
    QHBoxLayout *_mainLayout = nullptr;
    QLineEdit *_inputLineEdt = nullptr;

    ElaIconButton *_sendChatBt=nullptr;
    ElaIconButton *_closeCurrentBt=nullptr;
    ElaIconButton *_voiceInputBt=nullptr;
    // ElaIconType::DeleteLeft; ElaIconType::ArrowUpFromArc
    bool m_dragging = false;
    QPoint m_dragPosition;

    bool start_talkong = false;
    senceVoiceWebServerApi *senceVoice_ws = nullptr;
    AudioHandler *_audio_handle = nullptr;

private:
    Q_SLOT void inputTextEvent();
    Q_SLOT void connectToSenceVoiceWebserver();
    Q_SLOT void auto_process_tts_ollam(QString text);

Q_SIGNALS:
    void audio_rms(double rms);
    void input_content(QString text);
    void responce_content(QString text);
    void hideChatScrollArea();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;


};

#endif
