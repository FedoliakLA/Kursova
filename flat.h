#ifndef FLAT_H
#define FLAT_H
#include <QString>
#include <QTextStream>

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
    Flat() : address(""), district(""), rooms(0), area(0), floor(0), totalFloors(0), rent(0), hasFurniture(false){};
    Flat(const QString &address, const QString &district, int _rooms, double _area, int _floor, int _totalFloors, double _rent, bool _hasFurniture):
        address(address), district(district), rooms(_rooms), area(_area), floor(_floor), totalFloors(_totalFloors), rent(_rent),
        hasFurniture(_hasFurniture) {};
    Flat(const Flat &other) : address(other.address), district(other.district), rooms(other.rooms), area(other.area), floor(other.floor),
        totalFloors(other.totalFloors), rent(other.rent), hasFurniture(other.hasFurniture){};
    Flat &operator=(const Flat &other){
        if(this != &other){
            address=other.address;
            district=other.district;
            rooms=other.rooms;
            area=other.area;
            floor=other.floor;
            totalFloors=other.totalFloors;
            rent=other.rent;
            hasFurniture=other.hasFurniture;
        }
        return *this;
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
        int furniture = 0;

        f.address  = in.readLine();
        f.district = in.readLine();
        in >> f.rooms >> f.area >> f.floor >> f.totalFloors
            >> f.rent >> furniture;
        in.readLine();

        f.hasFurniture = (furniture != 0);
        return in;
    }
};

#endif // FLAT_H
