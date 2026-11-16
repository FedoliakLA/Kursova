#include "flatcatalog.h"
#include "cerror.h"

FlatCatalog::FlatCatalog(const QString &filePath)
    : storage(filePath)
{
}

QStringList FlatCatalog::load()
{
    QStringList warnings;
    flats = storage.load(warnings);
    return warnings;
}

void FlatCatalog::add(const Flat &flat)
{
    flat.validate();
    storage.append(flat);
    flats.push_back(flat);
}

void FlatCatalog::replace(const Flat &oldFlat, const Flat &newFlat)
{
    int index = flats.indexOf(oldFlat);
    if (index < 0)
        throw CError("Квартиру не знайдено в каталозі");

    newFlat.validate();

    Flat previous = flats[index];
    flats[index] = newFlat;
    try {
        storage.saveAll(flats);
    }
    catch (...) {
        flats[index] = previous;
        throw;
    }
}

void FlatCatalog::remove(const Flat &flat)
{
    int index = flats.indexOf(flat);
    if (index < 0)
        throw CError("Квартиру не знайдено в каталозі");

    Flat removed = flats[index];
    flats.removeAt(index);
    try {
        storage.saveAll(flats);
    }
    catch (...) {
        flats.insert(index, removed);
        throw;
    }
}

const QVector<Flat> &FlatCatalog::all() const
{
    return flats;
}

QStringList FlatCatalog::districts() const
{
    QStringList result;
    for (const Flat &f : flats) {
        QString d = f.getDistrict().trimmed();
        if (!d.isEmpty() && !result.contains(d, Qt::CaseInsensitive))
            result.push_back(d);
    }
    result.sort(Qt::CaseInsensitive);
    return result;
}

FlatStats FlatCatalog::stats() const
{
    FlatStats s;
    if (flats.isEmpty())
        return s;

    s.empty = false;
    s.minArea = s.maxArea = flats.first().getArea();
    s.minRent = s.maxRent = flats.first().getRent();

    double maxAll = 0.0;
    double maxFurnished = 0.0;
    bool hasFurnished = false;

    for (const Flat &f : flats) {
        s.minArea = qMin(s.minArea, f.getArea());
        s.maxArea = qMax(s.maxArea, f.getArea());
        s.minRent = qMin(s.minRent, f.getRent());
        s.maxRent = qMax(s.maxRent, f.getRent());

        double perM2 = f.pricePerM2();
        maxAll = qMax(maxAll, perM2);
        if (f.getHasFurniture()) {
            hasFurnished = true;
            maxFurnished = qMax(maxFurnished, perM2);
        }
    }

    s.maxPricePerM2 = hasFurnished ? maxFurnished : maxAll;
    return s;
}

QVector<Flat> FlatCatalog::find(const FlatFilter &filter) const
{
    filter.validate();

    QVector<Flat> result;
    for (const Flat &f : flats) {
        if (filter.matches(f))
            result.push_back(f);
    }
    return result;
}

QMap<int, QVector<Flat>> FlatCatalog::groupByRooms() const
{
    QMap<int, QVector<Flat>> groups;
    for (const Flat &f : flats)
        groups[f.getRooms()].push_back(f);

    for (auto it = groups.begin(); it != groups.end(); ++it)
        selectionSort(it.value(), SortByRent, true);

    return groups;
}

QMap<QString, QPair<Flat, Flat>> FlatCatalog::minMaxRentByDistrict() const
{
    QMap<QString, QPair<Flat, Flat>> result;
    for (const Flat &f : flats) {
        QString key = f.getDistrict().trimmed();
        if (!result.contains(key)) {
            result.insert(key, qMakePair(f, f));
            continue;
        }

        QPair<Flat, Flat> &range = result[key];
        if (f.getRent() < range.first.getRent())
            range.first = f;
        if (f.getRent() > range.second.getRent())
            range.second = f;
    }
    return result;
}

double FlatCatalog::sortKey(const Flat &flat, SortField field)
{
    switch (field) {
    case SortByArea:       return flat.getArea();
    case SortByRent:       return flat.getRent();
    case SortByRooms:      return flat.getRooms();
    case SortByPricePerM2: return flat.pricePerM2();
    case SortByFloor:      return flat.getFloor();
    }
    return 0;
}

void FlatCatalog::selectionSort(QVector<Flat> &list, SortField field, bool ascending)
{
    for (int i = 0; i < list.size() - 1; ++i) {
        int best = i;
        for (int j = i + 1; j < list.size(); ++j) {
            bool better;
            if (ascending)
                better = sortKey(list[j], field) < sortKey(list[best], field);
            else
                better = sortKey(list[j], field) > sortKey(list[best], field);

            if (better)
                best = j;
        }
        if (best != i)
            list.swapItemsAt(i, best);
    }
}
