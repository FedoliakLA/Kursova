#ifndef FLAT_H
#define FLAT_H

#include <QString>
#include <QStringList>
#include <QTextStream>
#include "cerror.h"

namespace FlatLimits {
constexpr int MinRooms = 1;
constexpr int MaxRooms = 10;
constexpr int MinFloor = 1;
constexpr int MaxFloors = 60;
constexpr double MinArea = 5.0;
constexpr double MaxArea = 1000.0;
constexpr double MaxRent = 1000000.0;
}

class Flat
{
private:
    QString address;
    QString district;
    int rooms;
    double area;
    int floor;
    int totalFloors;
    double rent;
    bool hasFurniture;

public:
    Flat()
        : address(""), district(""), rooms(0), area(0), floor(0),
        totalFloors(0), rent(0), hasFurniture(false)
    {
    }

    Flat(const QString &address, const QString &district, int _rooms, double _area,
         int _floor, int _totalFloors, double _rent, bool _hasFurniture)
        : address(address), district(district), rooms(_rooms), area(_area),
        floor(_floor), totalFloors(_totalFloors), rent(_rent),
        hasFurniture(_hasFurniture)
    {
    }

    Flat(const Flat &other)
        : address(other.address), district(other.district), rooms(other.rooms),
        area(other.area), floor(other.floor), totalFloors(other.totalFloors),
        rent(other.rent), hasFurniture(other.hasFurniture)
    {
    }

    ~Flat() = default;

    Flat &operator=(const Flat &other)
    {
        if (this != &other) {
            address = other.address;
            district = other.district;
            rooms = other.rooms;
            area = other.area;
            floor = other.floor;
            totalFloors = other.totalFloors;
            rent = other.rent;
            hasFurniture = other.hasFurniture;
        }
        return *this;
    }

    bool operator==(const Flat &other) const
    {
        return address == other.address && district == other.district
               && rooms == other.rooms && area == other.area
               && floor == other.floor && totalFloors == other.totalFloors
               && rent == other.rent && hasFurniture == other.hasFurniture;
    }

    QString getAddress() const { return address; }
    QString getDistrict() const { return district; }
    int getRooms() const { return rooms; }
    double getArea() const { return area; }
    int getFloor() const { return floor; }
    int getTotalFloors() const { return totalFloors; }
    double getRent() const { return rent; }
    bool getHasFurniture() const { return hasFurniture; }
    double pricePerM2() const { return area > 0 ? rent / area : 0; }

    void setAddress(const QString &_address) { address = _address; }
    void setDistrict(const QString &_district) { district = _district; }
    void setRooms(int _rooms) { rooms = _rooms; }
    void setArea(double _area) { area = _area; }
    void setFloor(int _floor) { floor = _floor; }
    void setTotalFloors(int _totalFloors) { totalFloors = _totalFloors; }
    void setRent(double _rent) { rent = _rent; }
    void setHasFurniture(bool _hasFurniture) { hasFurniture = _hasFurniture; }

    void validate() const
    {
        if (address.trimmed().isEmpty())
            throw CError("Адреса не може бути порожньою");
        if (district.trimmed().isEmpty())
            throw CError("Район не може бути порожнім");
        if (rooms < FlatLimits::MinRooms || rooms > FlatLimits::MaxRooms)
            throw CError(QString("Кількість кімнат має бути від %1 до %2")
                             .arg(FlatLimits::MinRooms).arg(FlatLimits::MaxRooms));
        if (area < FlatLimits::MinArea || area > FlatLimits::MaxArea)
            throw CError(QString("Площа має бути від %1 до %2 м²")
                             .arg(FlatLimits::MinArea).arg(FlatLimits::MaxArea));
        if (rent <= 0 || rent > FlatLimits::MaxRent)
            throw CError(QString("Оренда має бути від 1 до %1 грн")
                             .arg(FlatLimits::MaxRent, 0, 'f', 0));
        if (totalFloors < FlatLimits::MinFloor || totalFloors > FlatLimits::MaxFloors)
            throw CError(QString("Кількість поверхів у будинку має бути від %1 до %2")
                             .arg(FlatLimits::MinFloor).arg(FlatLimits::MaxFloors));
        if (floor < FlatLimits::MinFloor || floor > totalFloors)
            throw CError("Поверх не може бути більшим за кількість поверхів у будинку");
    }

    friend QTextStream &operator<<(QTextStream &out, const Flat &f)
    {
        out << f.address << '\n'
            << f.district << '\n'
            << f.rooms << ' '
            << f.area << ' '
            << f.floor << ' '
            << f.totalFloors << ' '
            << f.rent << ' '
            << (f.hasFurniture ? 1 : 0) << '\n';
        return out;
    }

    friend QTextStream &operator>>(QTextStream &in, Flat &f)
    {
        QString address;
        do {
            address = in.readLine();
        } while (address.trimmed().isEmpty() && !in.atEnd());

        if (address.trimmed().isEmpty()) {
            f = Flat();
            return in;
        }

        QString district = in.readLine();
        QStringList parts = in.readLine().split(' ', Qt::SkipEmptyParts);
        if (parts.size() != 6)
            throw CError("Рядок з числами має містити 6 значень");

        bool okRooms = false, okArea = false, okFloor = false;
        bool okTotal = false, okRent = false, okFurniture = false;

        int rooms = parts[0].toInt(&okRooms);
        double area = parts[1].replace(',', '.').toDouble(&okArea);
        int floor = parts[2].toInt(&okFloor);
        int totalFloors = parts[3].toInt(&okTotal);
        double rent = parts[4].replace(',', '.').toDouble(&okRent);
        int furniture = parts[5].toInt(&okFurniture);

        if (!(okRooms && okArea && okFloor && okTotal && okRent && okFurniture))
            throw CError("Рядок з числами містить некоректні значення");

        Flat parsed(address.trimmed(), district.trimmed(), rooms, area, floor,
                    totalFloors, rent, furniture != 0);
        parsed.validate();
        f = parsed;
        return in;
    }
};

#endif // FLAT_H