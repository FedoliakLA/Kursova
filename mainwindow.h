#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
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
    void showFlats();
    void sortByArea();
    void groupByRooms();
    void analyzeMinMaxRentByDistrict();
    void logMessage(const QString &text);
};

#endif // MAINWINDOW_H