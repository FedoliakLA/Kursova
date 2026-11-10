#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QFrame>
#include "flat.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnAdd_clicked();
    void on_btnApply_clicked();
    void on_btnReports_clicked();

private:
    Ui::MainWindow *ui;
    QVector<Flat> flats;
    QString dataFilePath() const;
    void appendFlatToFile(const Flat &flat);
    void loadFlats();
    void applyFilters();
    void showFlats(const QVector<Flat> &list);
    QFrame *createFlatCard(const Flat &flat);
    void sortByArea();
    void groupByRooms();
    void analyzeMinMaxRentByDistrict();
    void logMessage(const QString &text);
    QVector<Flat> searchByDistrictAndRooms(const QVector<Flat> &src, const QString &district, int rooms) const;
    QVector<Flat> filterByAreaAndRent(const QVector<Flat> &src, double minArea, double minRent, double maxRent) const;
};

#endif // MAINWINDOW_H