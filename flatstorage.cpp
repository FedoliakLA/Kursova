#include "flatstorage.h"
#include "cerror.h"

#include <QFile>
#include <QLocale>
#include <QStringConverter>
#include <QTextStream>

FlatStorage::FlatStorage(const QString &filePath)
    : filePath(filePath)
{
}

QString FlatStorage::getFilePath() const
{
    return filePath;
}

QVector<Flat> FlatStorage::load(QStringList &warnings) const
{
    QVector<Flat> result;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return result;

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    in.setLocale(QLocale::c());

    int recordNumber = 0;
    while (!in.atEnd()) {
        ++recordNumber;
        Flat flat;
        try {
            in >> flat;
            if (flat.getAddress().isEmpty())
                break;
            result.push_back(flat);
        }
        catch (const CError &e) {
            warnings.push_back(
                QString("Запис №%1 пропущено: %2").arg(recordNumber).arg(e.getMessage()));
        }
    }
    return result;
}

void FlatStorage::append(const Flat &flat) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::Append | QIODevice::Text))
        throw CError("Не вдалося відкрити файл для запису: " + file.errorString());

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out.setLocale(QLocale::c());
    out << flat;
}

void FlatStorage::saveAll(const QVector<Flat> &flats) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        throw CError("Не вдалося відкрити файл для запису: " + file.errorString());

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out.setLocale(QLocale::c());

    for (const Flat &f : flats)
        out << f;
}
