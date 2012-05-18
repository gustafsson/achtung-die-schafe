#include "logger.h"

#include <QFile>
#include <QTextStream>
#include <QDateTime>

void Logger::logMessage(const QString& text)
{
    QFile log("achtung.log");

    if (log.open(QFile::Append))
    {
        QTextStream(&log)
                << QDateTime::currentDateTime().toString(Qt::ISODate) << " "
                << text << "\n";
    }
}
