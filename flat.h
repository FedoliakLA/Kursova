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

    QString getAddress() const {return address;}
    QString getDistrict() const {return district;}
    int getRooms() const {return rooms;}
    double getArea() const {return area;}
    int getFloor() const {return floor;}
    int getTotalFloors() const {return totalFloors;}
    double getRent() const {return rent;}
    bool getHasFurniture() const {return hasFurniture;}
    double pricePerM2() const {return area > 0 ? rent/area : 0;}

    void setAddress(const QString &_address) {address = _address;}
    void setDistrict(const QString &_district) {district = _district;}
    void setRooms(int _rooms) {rooms = _rooms;}
    void setArea(double _area) {area = _area;}
    void setFloor(int _floor) {floor = _floor;}
    void setTotalFloors(int _totalFloors) {totalFloors = _totalFloors;}
    void setRent(double _rent) {rent = _rent;}
    void setHasFurniture(bool _hasFurniture) {hasFurniture=_hasFurniture;}


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
