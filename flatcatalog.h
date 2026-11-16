#ifndef FLATCATALOG_H
#define FLATCATALOG_H

#include <QMap>
#include <QPair>
#include <QString>
#include <QStringList>
#include <QVector>
#include "flat.h"
#include "flatfilter.h"
#include "flatstorage.h"

struct FlatStats
{
    bool empty = true;
    double minArea = 0.0;
    double maxArea = 0.0;
    double minRent = 0.0;
    double maxRent = 0.0;
    double maxPricePerM2 = 0.0;
};

class FlatCatalog
{
public:
    enum SortField { SortByArea, SortByRent, SortByRooms, SortByPricePerM2, SortByFloor };

private:
    FlatStorage storage;
    QVector<Flat> flats;

    static double sortKey(const Flat &flat, SortField field);

public:
    explicit FlatCatalog(const QString &filePath);
    ~FlatCatalog() = default;

    QStringList load();
    void add(const Flat &flat);
    void replace(const Flat &oldFlat, const Flat &newFlat);
    void remove(const Flat &flat);

    const QVector<Flat> &all() const;
    QStringList districts() const;
    FlatStats stats() const;

    QVector<Flat> find(const FlatFilter &filter) const;
    QMap<int, QVector<Flat>> groupByRooms() const;
    QMap<QString, QPair<Flat, Flat>> minMaxRentByDistrict() const;

    static void selectionSort(QVector<Flat> &list, SortField field, bool ascending);
};

#endif // FLATCATALOG_H
