#ifndef CERROR_H
#define CERROR_H

#include <exception>
#include <QByteArray>
#include <QString>

class CError : public std::exception
{
private:
    QString message;
    QByteArray utf8Message;

public:
    explicit CError(const QString &message)
        : message(message), utf8Message(message.toUtf8())
    {}

    QString getMessage() const { return message; }

    const char *what() const noexcept override { return utf8Message.constData(); }
};

#endif // CERROR_H