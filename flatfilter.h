#ifndef FLATFILTER_H
#define FLATFILTER_H

#include <QString>
#include "cerror.h"
#include "flat.h"

struct FlatFilter
{
    static constexpr double NoLimit = 1e12;

    bool useDistrict = false;
    QString district;

    bool useRooms = false;
    int rooms = FlatLimits::MinRooms;

    bool useArea = false;
    double minArea = 0.0;
    double maxArea = NoLimit;

    bool usePrice = false;
    double minRent = 0.0;
    double maxRent = NoLimit;

    bool useFurniture = false;
    double maxPricePerM2 = NoLimit;

    void validate() const
    {
        if (useArea && minArea > maxArea)
            throw CError("Мінімальна площа не може бути більшою за максимальну");
        if (usePrice && minRent > maxRent)
            throw CError("Мінімальна оренда не може бути більшою за максимальну");
    }

    bool matches(const Flat &flat) const
    {
        if (useDistrict && flat.getDistrict().compare(district, Qt::CaseInsensitive) != 0)
            return false;
        if (useRooms && flat.getRooms() != rooms)
            return false;
        if (useArea && (flat.getArea() < minArea || flat.getArea() > maxArea))
            return false;
        if (usePrice && (flat.getRent() < minRent || flat.getRent() > maxRent))
            return false;
        if (useFurniture && (!flat.getHasFurniture() || flat.pricePerM2() > maxPricePerM2))
            return false;
        return true;
    }
};

#endif // FLATFILTER_H
