#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QPropertyAnimation>
#include <QVector>
#include "flat.h"
#include "flatcatalog.h"
#include "flatfilter.h"

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
    void on_btnSort_clicked();
    void on_btnSortDir_toggled(bool checked);
    void on_btnFilters_toggled(bool checked);
    void on_actionAbout_triggered();

private:
    Ui::MainWindow *ui;
    FlatCatalog catalog;
    QVector<Flat> currentList;
    QPropertyAnimation *filterAnimation;

    void loadCatalog();
    void refreshAll();
    void fillDistricts();
    void updateFilterHints();

    FlatFilter readFilter() const;
    void applyFilters();
    void resetFilters();

    void clearFlatsGrid();
    void showFlats(const QVector<Flat> &list);
    void showGroupedFlats(const QMap<int, QVector<Flat>> &groups);
    void addCardToGrid(const Flat &flat, int row, int column, int order);
    QWidget *createGroupHeader(const QString &title);

    void editFlat(const Flat &flat);
    void deleteFlat(const Flat &flat);

    void showMinMaxReport();
    void showGroupReport();
    void logMessage(const QString &text);
};

#endif // MAINWINDOW_H