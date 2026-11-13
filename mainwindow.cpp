#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMenu>
#include <QCoreApplication>
#include "addflatdialog.h"
#include <QFile>
#include <QLabel>
#include <QVBoxLayout>
#include <QTime>

#include <QDebug>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    loadFlats();
    showFlats(flats);
    fillDistricts();
    ui->cbSortBy->addItem("Площа", SortByArea);
    ui->cbSortBy->addItem("Оренда", SortByRent);
    ui->cbSortBy->addItem("Кількість кімнат", SortByRooms);
    ui->cbSortBy->addItem("Ціна за м²", SortByPricePerM2);
    ui->cbSortBy->addItem("Поверх", SortByFloor);
    ui->lblLogoIcon->setPixmap(QPixmap(":/app_icon.jpg").scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    qDebug() << dataFilePath();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnApply_clicked()
{
    applyFilters();
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

void MainWindow::on_btnSortDir_toggled(bool checked)
{
    ui->btnSortDir->setText(checked ? "↓" : "↑");
}

void MainWindow::fillDistricts()
{
    ui->cbDistrict->clear();
    for (const Flat &f : flats) {
        QString d = f.getDistrict().trimmed();
        if (!d.isEmpty() && ui->cbDistrict->findText(d, Qt::MatchFixedString) < 0)
            ui->cbDistrict->addItem(d);
    }
}

void MainWindow::applyFilters()
{
    bool useDistrict  = ui->chkUseDistrict->isChecked();
    bool useRooms     = ui->chkUseRooms->isChecked();
    bool useArea      = ui->chkUseArea->isChecked();
    bool usePrice     = ui->chkUsePrice->isChecked();
    bool useFurniture = ui->chkFurniture->isChecked();

    QString district = ui->cbDistrict->currentText();
    int roomsIndex   = ui->cbRooms->currentIndex();

    double minArea = ui->leAreaMin->text().replace(',', '.').toDouble();
    QString areaMaxText = ui->leAreaMax->text();
    double maxArea = areaMaxText.isEmpty() ? 1e12 : areaMaxText.replace(',', '.').toDouble();

    double minRent = ui->lePriceMin->text().replace(',', '.').toDouble();

    QString rentMaxText = ui->lePriceMax->text();
    double maxRent = rentMaxText.isEmpty() ? 1e12 : rentMaxText.replace(',', '.').toDouble();

    QString perM2Text = ui->leMaxPerM2->text();
    double maxPerM2 = perM2Text.isEmpty() ? 1e12 : perM2Text.replace(',', '.').toDouble();

    QVector<Flat> result;
    for (const Flat &f : flats) {
        if (useDistrict && f.getDistrict().compare(district, Qt::CaseInsensitive) != 0)
            continue;
        if (useRooms && (roomsIndex == 4 ? f.getRooms() < 5 : f.getRooms() != roomsIndex + 1))
            continue;
        if (useArea && (f.getArea() < minArea || f.getArea() > maxArea))
            continue;
        if (usePrice && (f.getRent() < minRent || f.getRent() > maxRent))
            continue;
        if (useFurniture && (!f.getHasFurniture() || f.pricePerM2() > maxPerM2))
            continue;
        result.push_back(f);
    }
    showFlats(result);
    logMessage(QString("Знайдено квартир: %1").arg(result.size()));
}

void MainWindow::showFlats(const QVector<Flat> &list)
{
    currentList = list;
    clearFlatsGrid();

    const int columns = 3;
    for (int i = 0; i < list.size(); ++i)
        ui->flatsGrid->addWidget(createFlatCard(list[i]), i / columns, i % columns);

    int rows = (list.size() + columns - 1) / columns;
    ui->flatsGrid->addItem(
        new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), rows, 0);
}

QFrame *MainWindow::createFlatCard(const Flat &flat)
{
    QFrame *card = new QFrame;
    card->setObjectName("flatCard");
    card->setFixedWidth(250);
    card->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum);

    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(8);

    QLabel *price = new QLabel(QString::number(flat.getRent(), 'f', 0) + " грн");
    price->setObjectName("lblCardPrice");

    QLabel *address = new QLabel(flat.getAddress());
    address->setObjectName("lblAddress");
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

void MainWindow::clearFlatsGrid()
{
    QLayoutItem *item;
    while ((item = ui->flatsGrid->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
}

QWidget *MainWindow::createGroupHeader(const QString &title)
{
    QWidget *box = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(box);
    layout->setContentsMargins(0, 12, 0, 4);
    layout->setSpacing(6);

    QLabel *label = new QLabel(title);
    label->setObjectName("lblGroupTitle");

    QFrame *line = new QFrame;
    line->setObjectName("groupLine");
    line->setFixedHeight(1);

    layout->addWidget(label);
    layout->addWidget(line);
    return box;
}

void MainWindow::showGroupedFlats(const QMap<int, QVector<Flat>> &groups)
{
    currentList.clear();
    clearFlatsGrid();

    const int columns = 3;
    int row = 0;
    for (auto it = groups.constBegin(); it != groups.constEnd(); ++it) {
        const QVector<Flat> &group = it.value();
        QString title = QString("%1 кімн. · %2 шт.").arg(it.key()).arg(group.size());
        ui->flatsGrid->addWidget(createGroupHeader(title), row, 0, 1, columns);
        ++row;

        for (int i = 0; i < group.size(); ++i) {
            ui->flatsGrid->addWidget(createFlatCard(group[i]), row + i / columns, i % columns);;
            currentList.push_back(group[i]);
        }
        row += (group.size() + columns - 1) / columns;
    }
    ui->flatsGrid->addItem(
        new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), row, 0);
}

void MainWindow::groupByRooms()
{
    QMap<int, QVector<Flat>> groups;
    for (const Flat &f : flats)
        groups[f.getRooms()].push_back(f);

    for (auto it = groups.begin(); it != groups.end(); ++it)
        selectionSort(it.value(), SortByRent, true);

    showGroupedFlats(groups);
    logMessage(QString("Групування за кімнатами: груп %1").arg(groups.size()));
}

void MainWindow::analyzeMinMaxRentByDistrict()
{
    QMap<QString, QVector<Flat>> byDistrict;
    for (const Flat &f : flats)
        byDistrict[f.getDistrict().trimmed()].push_back(f);

    for (auto it = byDistrict.constBegin(); it != byDistrict.constEnd(); ++it) {
        const QVector<Flat> &group = it.value();
        Flat minFlat = group.first();
        Flat maxFlat = group.first();
        for (const Flat &f : group) {
            if (f.getRent() < minFlat.getRent()) minFlat = f;
            if (f.getRent() > maxFlat.getRent()) maxFlat = f;
        }
        logMessage(QString("%1: мін. %2 грн (%3), макс. %4 грн (%5)")
                       .arg(it.key())
                       .arg(minFlat.getRent(), 0, 'f', 0)
                       .arg(minFlat.getAddress())
                       .arg(maxFlat.getRent(), 0, 'f', 0)
                       .arg(maxFlat.getAddress()));
    }
}

void MainWindow::logMessage(const QString &text)
{
    QString time = QTime::currentTime().toString("HH:mm");
    ui->logView->QTextEdit::append("["+time+"] > "+text);
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

double MainWindow::sortKey(const Flat &f, SortField field) const
{
    switch (field) {
    case SortByArea:       return f.getArea();
    case SortByRent:       return f.getRent();
    case SortByRooms:      return f.getRooms();
    case SortByPricePerM2: return f.pricePerM2();
    case SortByFloor:      return f.getFloor();
    }
    return 0;
}

void MainWindow::selectionSort(QVector<Flat> &list, SortField field, bool ascending)
{
    for (int i = 0; i < list.size() - 1; ++i) {
        int best = i;
        for (int j = i + 1; j < list.size(); ++j) {
            bool better = ascending;
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

void MainWindow::on_btnSort_clicked()
{
    QVector<Flat> sorted = currentList;
    SortField field = static_cast<SortField>(ui->cbSortBy->currentData().toInt());
    bool ascending = !ui->btnSortDir->isChecked();

    selectionSort(sorted, field, ascending);
    showFlats(sorted);
    logMessage(QString("Відсортовано: %1").arg(ui->cbSortBy->currentText()));
}

