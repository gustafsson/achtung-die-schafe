#include "logger.h"

#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <memory>
#include <iostream>

void Logger::logMessage(const QString& text)
{
    static std::unique_ptr<QFile> log;
    if (!log)
    {
        log.reset(new QFile("achtung.log"));
        log->open(QFile::Append);
    }

    if (log->isOpen())
    {
        //auto format = Qt::ISODate;
        auto format = "yyyy-MM-ddTHH:mm:ss.zzz";
        QTextStream(log.get())
                << QDateTime::currentDateTime().toString(format) << " "
                << text << "\n";
    }

#ifdef NOGUI
    std::cout << text.toStdString() << std::endl;
#endif
}
