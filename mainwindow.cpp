#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMenu>
#include <QCoreApplication>
#include "addflatdialog.h"
#include <QFile>
#include <QLabel>
#include <QVBoxLayout>

#include <QDebug>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    loadFlats();
    showFlats(flats);
    ui->lblLogoIcon->setPixmap(QPixmap(":/app_icon.jpg").scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    qDebug() << dataFilePath();
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

void MainWindow::applyFilters()
{
    // TODO: фільтрація за районом, кімнатами, площею, ціною, меблями
}

void MainWindow::showFlats(const QVector<Flat> &list)
{
    QLayoutItem *item;
    while ((item = ui->flatsGrid->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    const int columns = 3;
    for (int i = 0; i < list.size(); ++i) {
        QFrame *card = createFlatCard(list[i]);
        ui->flatsGrid->addWidget(card, i / columns, i % columns);
    }

    int rows = (list.size() + columns - 1) / columns;
    ui->flatsGrid->addItem(
        new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), rows, 0);
}

QFrame *MainWindow::createFlatCard(const Flat &flat)
{
    QFrame *card = new QFrame;
    card->setObjectName("flatCard");
    card->setMinimumSize(200, 150);

    QVBoxLayout *layout = new QVBoxLayout(card);

    QLabel *price = new QLabel(QString::number(flat.getRent(), 'f', 0) + " грн");
    price->setObjectName("lblCardPrice");

    QLabel *address = new QLabel(flat.getAddress());
    address->setObjectName("lblAdressCap");
    address->setWordWrap(true);

    QLabel *meta = new QLabel(
        QString("%1 м², %2 кімн., %3/%4 поверх")
            .arg(flat.getArea(), 0, 'f', 1)
            .arg(flat.getRooms())
            .arg(flat.getFloor())
            .arg(flat.getTotalFloors())
        );
    meta->setObjectName("lblMeta");

    layout->addWidget(price, 0, Qt::AlignLeft);
    layout->addWidget(address);
    layout->addWidget(meta);
    return card;
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
    showFlats(flats);
}

QString MainWindow::dataFilePath() const
{
    QString folder = QCoreApplication::applicationDirPath();
    return folder+"/flats.txt";
}

void MainWindow::appendFlatToFile(const Flat &flat)
{
    QFile file(dataFilePath());
    if (!file.open(QIODevice::Append | QIODevice::Text)){
        return;
    }
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out.setLocale(QLocale::c());
    out << flat;
}

void MainWindow::loadFlats()
{
    flats.clear();
    QFile file(dataFilePath());

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        return;
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    in.setLocale(QLocale::c());

    while(!in.atEnd()){
        Flat f;
        in >> f;
        if(f.getAddress().isEmpty()) break;
        flats.push_back(f);
    }
}