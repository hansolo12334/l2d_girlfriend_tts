// #ifndef APP_LOG_H
// #define APP_LOG_H
// #include <qdebug.h>
// #ifdef QF_DEBUG
// #define APP_LOG_DEBUG(fmt, ...) qDebug("[%s,%d] debug:"fmt"\n",__FUNCTION__,__LINE__,##__VA_ARGS__)
// #else
// #define APP_LOG_DEBUG(fmt, ...)
// #endif

// #define APP_LOG_INFO(fmt, ...) qDebug("[%s,%d] info:"fmt"\n",__FUNCTION__,__LINE__,##__VA_ARGS__)
// #define APP_LOG_ERROR(fmt, ...) qDebug("[%s,%d] error:"fmt"\n",__FUNCTION__,__LINE__,##__VA_ARGS__)

// #endif // APP_LOG_H

#ifndef APP_LOG_H
#define APP_LOG_H


#include <QDebug>
#include <clocale>

#ifdef APP_DEBUG
#define APP_LOG_DEBUG(...) \
    do { \
        QDebug debug = QDebug(QtDebugMsg); \
        debug.nospace() << "[" << __FUNCTION__ << "," << __LINE__ << "] debug: " << __VA_ARGS__ << "\n"; \
    } while (0)
#else
#define APP_LOG_DEBUG(msg)
#endif

#define APP_LOG_INFO(...) \
    do { \
        QDebug debug = QDebug(QtInfoMsg); \
        debug.nospace() << "[" << __FUNCTION__ << "," << __LINE__ << "] info: " << __VA_ARGS__ << "\n"; \
    } while (0)

#define APP_LOG_ERROR(...) \
    do { \
        QDebug debug = QDebug(QtCriticalMsg); \
        debug.nospace() << "[" << __FUNCTION__ << "," << __LINE__ << "] error: " << __VA_ARGS__ << "\n"; \
    } while (0)

#endif // APP_LOG_H
