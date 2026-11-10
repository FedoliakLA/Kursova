#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMenu>
#include "addflatdialog.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    loadFlats();
    showFlats();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnApply_clicked()
{
}

void MainWindow::on_btnReports_clicked()
{
    QMenu menu(this);
    QAction *minMax = menu.addAction("Мін. і макс. оренда по районах");
    QAction *groups = menu.addAction("Групування за кількістю кімнат");

    QAction *chosen = menu.exec(ui->btnReports->mapToGlobal(QPoint(ui->btnReports->width(), 0)));

    if (chosen == minMax)
        analyzeMinMaxRentByDistrict();
    else if (chosen == groups)
        groupByRooms();
}

void MainWindow::loadFlats()
{
    // TODO: завантаження квартир з файлу через operator>>
}

void MainWindow::applyFilters()
{
    // TODO: фільтрація за районом, кімнатами, площею, ціною, меблями
}

void MainWindow::showFlats()
{
    // TODO: вивід карток у flatsGrid
}

void MainWindow::sortByArea()
{
    // TODO: сортування простою вибіркою за площею
}

void MainWindow::groupByRooms()
{
    // TODO: групування за кількістю кімнат, сортування за ціною в групі
}

void MainWindow::analyzeMinMaxRentByDistrict()
{
    // TODO: мін і макс оренда у кожному районі
}

void MainWindow::logMessage(const QString &text)
{
    // TODO: додати рядок у logView
    Q_UNUSED(text);
}

void MainWindow::on_btnAdd_clicked()
{
    AddFlatDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted)
        return;

    Flat flat = dialog.getFlat();
    flats.push_back(flat);
    appendFlatToFile(flat);
    showFlats();
}

QString MainWindow::dataFilePath() const
{
    return QString();
}

void MainWindow::appendFlatToFile(const Flat &flat)
{
    Q_UNUSED(flat);
}