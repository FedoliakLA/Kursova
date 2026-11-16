#ifndef FLATSTORAGE_H
#define FLATSTORAGE_H

#include <QString>
#include <QStringList>
#include <QVector>
#include "flat.h"

class FlatStorage
{
private:
    QString filePath;

public:
    explicit FlatStorage(const QString &filePath);
    ~FlatStorage() = default;

    QString getFilePath() const;
    QVector<Flat> load(QStringList &warnings) const;
    void append(const Flat &flat) const;
    void saveAll(const QVector<Flat> &flats) const;
};

#endif // FLATSTORAGE_H
