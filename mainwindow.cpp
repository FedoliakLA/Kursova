#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "addflatdialog.h"
#include "cerror.h"
#include "flatcard.h"

#include <QCoreApplication>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QPixmap>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QTime>
#include <QVBoxLayout>

namespace {

double parseNumber(const QString &text, double emptyValue, const QString &fieldName)
{
    QString trimmed = text.trimmed();
    if (trimmed.isEmpty())
        return emptyValue;

    bool ok = false;
    double value = trimmed.replace(',', '.').toDouble(&ok);
    if (!ok || value < 0)
        throw CError(QString("Поле «%1» має містити невід'ємне число").arg(fieldName));
    return value;
}

}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , catalog(QCoreApplication::applicationDirPath() + "/flats.txt")
    , filterAnimation(nullptr)
{
    ui->setupUi(this);

    ui->spRooms->setRange(FlatLimits::MinRooms, FlatLimits::MaxRooms);
    ui->spRooms->setToolTip(QString("Кількість кімнат (від %1 до %2)")
                                .arg(FlatLimits::MinRooms).arg(FlatLimits::MaxRooms));

    QRegularExpression areaPattern(R"(^\d{0,4}([.,]\d{0,2})?$)");
    QRegularExpression rentPattern(R"(^\d{0,7}([.,]\d{0,2})?$)");
    QRegularExpression perM2Pattern(R"(^\d{0,5}([.,]\d{0,2})?$)");
    ui->leAreaMin->setValidator(new QRegularExpressionValidator(areaPattern, this));
    ui->leAreaMax->setValidator(new QRegularExpressionValidator(areaPattern, this));
    ui->lePriceMin->setValidator(new QRegularExpressionValidator(rentPattern, this));
    ui->lePriceMax->setValidator(new QRegularExpressionValidator(rentPattern, this));
    ui->leMaxPerM2->setValidator(new QRegularExpressionValidator(perM2Pattern, this));

    filterAnimation = new QPropertyAnimation(ui->filterPanel, "maximumHeight", this);
    filterAnimation->setDuration(250);
    filterAnimation->setEasingCurve(QEasingCurve::InOutCubic);
    connect(filterAnimation, &QPropertyAnimation::finished, this, [this]() {
        if (ui->btnFilters->isChecked())
            ui->filterPanel->setMaximumHeight(QWIDGETSIZE_MAX);
        else
            ui->filterPanel->hide();
    });

    connect(ui->actionAddFlat, &QAction::triggered, this, &MainWindow::on_btnAdd_clicked);
    connect(ui->actionApplyFilters, &QAction::triggered, this, &MainWindow::on_btnApply_clicked);
    connect(ui->actionResetFilters, &QAction::triggered, this, &MainWindow::resetFilters);
    connect(ui->actionSort, &QAction::triggered, this, &MainWindow::on_btnSort_clicked);
    connect(ui->actionReportMinMax, &QAction::triggered, this, &MainWindow::showMinMaxReport);
    connect(ui->actionReportGroups, &QAction::triggered, this, &MainWindow::showGroupReport);

    ui->cbSortBy->addItem("Площа", int(FlatCatalog::SortByArea));
    ui->cbSortBy->addItem("Оренда", int(FlatCatalog::SortByRent));
    ui->cbSortBy->addItem("Кількість кімнат", int(FlatCatalog::SortByRooms));
    ui->cbSortBy->addItem("Ціна за м²", int(FlatCatalog::SortByPricePerM2));
    ui->cbSortBy->addItem("Поверх", int(FlatCatalog::SortByFloor));

    QPixmap logo(":/app_icon.jpg");
    if (!logo.isNull())
        ui->lblLogoIcon->setPixmap(logo.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    ui->flatsContainer->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->flatsContainer, &QWidget::customContextMenuRequested, this,
            [this](const QPoint &pos) {
                QMenu menu(this);
                menu.addAction(ui->actionAddFlat);
                menu.addAction(ui->actionResetFilters);
                menu.exec(ui->flatsContainer->mapToGlobal(pos));
            });

    loadCatalog();
    refreshAll();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnAdd_clicked()
{
    AddFlatDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted)
        return;

    try {
        catalog.add(dialog.getFlat());
    }
    catch (const CError &e) {
        QMessageBox::warning(this, "Помилка збереження", e.getMessage());
        return;
    }

    refreshAll();
    logMessage("Квартиру додано");
}

void MainWindow::on_btnApply_clicked()
{
    applyFilters();
}

void MainWindow::on_btnReports_clicked()
{
    QMenu menu(this);
    menu.addAction(ui->actionReportMinMax);
    menu.addAction(ui->actionReportGroups);
    menu.exec(ui->btnReports->mapToGlobal(QPoint(ui->btnReports->width(), 0)));
}

void MainWindow::on_btnSort_clicked()
{
    QVector<Flat> sorted = currentList;
    FlatCatalog::SortField field =
        static_cast<FlatCatalog::SortField>(ui->cbSortBy->currentData().toInt());
    bool ascending = !ui->btnSortDir->isChecked();

    FlatCatalog::selectionSort(sorted, field, ascending);
    showFlats(sorted);
    logMessage(QString("Відсортовано: %1").arg(ui->cbSortBy->currentText()));
}

void MainWindow::on_btnSortDir_toggled(bool checked)
{
    ui->btnSortDir->setText(checked ? "↓" : "↑");
}

void MainWindow::on_btnFilters_toggled(bool checked)
{
    QWidget *panel = ui->filterPanel;
    filterAnimation->stop();

    if (checked) {
        panel->setMaximumHeight(0);
        panel->show();
        filterAnimation->setStartValue(0);
        filterAnimation->setEndValue(panel->sizeHint().height());
    } else {
        filterAnimation->setStartValue(panel->height());
        filterAnimation->setEndValue(0);
    }
    filterAnimation->start();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "Про програму",
                       "SayMyНайм\nКаталог квартир для довгострокової оренди\nКурсова робота, C++ / Qt");
}

void MainWindow::loadCatalog()
{
    QStringList warnings = catalog.load();

    for (const QString &w : warnings)
        logMessage(w);
    if (!warnings.isEmpty())
        logMessage(QString("Пропущено пошкоджених записів: %1").arg(warnings.size()));
}

void MainWindow::refreshAll()
{
    fillDistricts();
    updateFilterHints();
    showFlats(catalog.all());
}

void MainWindow::fillDistricts()
{
    QString current = ui->cbDistrict->currentText();

    ui->cbDistrict->clear();
    ui->cbDistrict->addItems(catalog.districts());

    int index = ui->cbDistrict->findText(current, Qt::MatchFixedString);
    if (index >= 0)
        ui->cbDistrict->setCurrentIndex(index);
}

void MainWindow::updateFilterHints()
{
    FlatStats s = catalog.stats();

    if (s.empty) {
        ui->leAreaMin->setPlaceholderText("від");
        ui->leAreaMax->setPlaceholderText("до");
        ui->lePriceMin->setPlaceholderText("від");
        ui->lePriceMax->setPlaceholderText("до");
        ui->leMaxPerM2->setPlaceholderText("Ціна за м² до");
        ui->leAreaMin->setToolTip("Мінімальна площа, м². Порожнє поле означає без обмеження");
        ui->leAreaMax->setToolTip("Максимальна площа, м². Порожнє поле означає без обмеження");
        ui->lePriceMin->setToolTip("Мінімальна оренда, ₴ на місяць. Порожнє поле означає без обмеження");
        ui->lePriceMax->setToolTip("Максимальна оренда, ₴ на місяць. Порожнє поле означає без обмеження");
        ui->leMaxPerM2->setToolTip("Максимальна ціна за м² для квартир з меблями. Порожнє поле означає без обмеження");
        return;
    }

    QString areaRange = QString("У каталозі площа від %1 до %2 м². Порожнє поле означає без обмеження")
                            .arg(s.minArea, 0, 'f', 1).arg(s.maxArea, 0, 'f', 1);
    QString rentRange = QString("У каталозі оренда від %1 до %2 ₴. Порожнє поле означає без обмеження")
                            .arg(s.minRent, 0, 'f', 0).arg(s.maxRent, 0, 'f', 0);
    QString perM2Range = QString("Найбільша ціна за м² у каталозі серед квартир з меблями: %1 ₴. Порожнє поле означає без обмеження")
                             .arg(s.maxPricePerM2, 0, 'f', 0);

    ui->leAreaMin->setPlaceholderText(QString("від %1").arg(s.minArea, 0, 'f', 1));
    ui->leAreaMax->setPlaceholderText(QString("до %1").arg(s.maxArea, 0, 'f', 1));
    ui->lePriceMin->setPlaceholderText(QString("від %1").arg(s.minRent, 0, 'f', 0));
    ui->lePriceMax->setPlaceholderText(QString("до %1").arg(s.maxRent, 0, 'f', 0));
    ui->leMaxPerM2->setPlaceholderText(QString("до %1").arg(s.maxPricePerM2, 0, 'f', 0));

    ui->leAreaMin->setToolTip(areaRange);
    ui->leAreaMax->setToolTip(areaRange);
    ui->lePriceMin->setToolTip(rentRange);
    ui->lePriceMax->setToolTip(rentRange);
    ui->leMaxPerM2->setToolTip(perM2Range);
}

FlatFilter MainWindow::readFilter() const
{
    FlatFilter filter;

    filter.useDistrict = ui->chkUseDistrict->isChecked();
    filter.district = ui->cbDistrict->currentText().trimmed();

    filter.useRooms = ui->chkUseRooms->isChecked();
    filter.rooms = ui->spRooms->value();

    filter.useArea = ui->chkUseArea->isChecked();
    if (filter.useArea) {
        filter.minArea = parseNumber(ui->leAreaMin->text(), 0.0, "Площа від");
        filter.maxArea = parseNumber(ui->leAreaMax->text(), FlatFilter::NoLimit, "Площа до");
    }

    filter.usePrice = ui->chkUsePrice->isChecked();
    if (filter.usePrice) {
        filter.minRent = parseNumber(ui->lePriceMin->text(), 0.0, "Оренда від");
        filter.maxRent = parseNumber(ui->lePriceMax->text(), FlatFilter::NoLimit, "Оренда до");
    }

    filter.useFurniture = ui->chkFurniture->isChecked();
    if (filter.useFurniture)
        filter.maxPricePerM2 = parseNumber(ui->leMaxPerM2->text(), FlatFilter::NoLimit, "Ціна за м² до");

    return filter;
}

void MainWindow::applyFilters()
{
    try {
        QVector<Flat> result = catalog.find(readFilter());
        showFlats(result);
        logMessage(QString("Знайдено квартир: %1").arg(result.size()));
    }
    catch (const CError &e) {
        QMessageBox::warning(this, "Некоректний фільтр", e.getMessage());
    }
}

void MainWindow::resetFilters()
{
    ui->chkUseDistrict->setChecked(false);
    ui->chkUseRooms->setChecked(false);
    ui->chkUseArea->setChecked(false);
    ui->chkUsePrice->setChecked(false);
    ui->chkFurniture->setChecked(false);

    ui->spRooms->setValue(FlatLimits::MinRooms);
    ui->leAreaMin->clear();
    ui->leAreaMax->clear();
    ui->lePriceMin->clear();
    ui->lePriceMax->clear();
    ui->leMaxPerM2->clear();

    showFlats(catalog.all());
    logMessage("Фільтри скинуто");
}

void MainWindow::clearFlatsGrid()
{
    QLayoutItem *item;
    while ((item = ui->flatsGrid->takeAt(0)) != nullptr) {
        if (QWidget *w = item->widget()) {
            w->hide();
            w->deleteLater();
        }
        delete item;
    }
}

void MainWindow::addCardToGrid(const Flat &flat, int row, int column, int order)
{
    FlatCard *card = new FlatCard(flat);
    connect(card, &FlatCard::editRequested, this, &MainWindow::editFlat);
    connect(card, &FlatCard::deleteRequested, this, &MainWindow::deleteFlat);

    ui->flatsGrid->addWidget(card, row, column);
    card->playAppearAnimation(qMin(order * 40, 400));
}

void MainWindow::showFlats(const QVector<Flat> &list)
{
    currentList = list;
    clearFlatsGrid();

    const int columns = 3;
    for (int i = 0; i < list.size(); ++i)
        addCardToGrid(list[i], i / columns, i % columns, i);

    int rows = (list.size() + columns - 1) / columns;
    ui->flatsGrid->addItem(
        new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), rows, 0);
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
    int order = 0;
    for (auto it = groups.constBegin(); it != groups.constEnd(); ++it) {
        const QVector<Flat> &group = it.value();
        QString title = QString("%1 кімн. · %2 шт.").arg(it.key()).arg(group.size());
        ui->flatsGrid->addWidget(createGroupHeader(title), row, 0, 1, columns);
        ++row;

        for (int i = 0; i < group.size(); ++i) {
            addCardToGrid(group[i], row + i / columns, i % columns, order++);
            currentList.push_back(group[i]);
        }
        row += (group.size() + columns - 1) / columns;
    }
    ui->flatsGrid->addItem(
        new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), row, 0);
}

void MainWindow::editFlat(const Flat &flat)
{
    AddFlatDialog dialog(this);
    dialog.setFlat(flat);
    if (dialog.exec() != QDialog::Accepted)
        return;

    try {
        catalog.replace(flat, dialog.getFlat());
    }
    catch (const CError &e) {
        QMessageBox::warning(this, "Помилка збереження", e.getMessage());
        return;
    }

    refreshAll();
    logMessage("Квартиру змінено");
}

void MainWindow::deleteFlat(const Flat &flat)
{
    QMessageBox::StandardButton answer = QMessageBox::question(
        this, "Видалення", "Видалити квартиру за адресою " + flat.getAddress() + "?");
    if (answer != QMessageBox::Yes)
        return;

    try {
        catalog.remove(flat);
    }
    catch (const CError &e) {
        QMessageBox::warning(this, "Помилка збереження", e.getMessage());
        return;
    }

    refreshAll();
    logMessage("Квартиру видалено");
}

void MainWindow::showMinMaxReport()
{
    QMap<QString, QPair<Flat, Flat>> report = catalog.minMaxRentByDistrict();
    if (report.isEmpty()) {
        logMessage("Каталог порожній");
        return;
    }

    for (auto it = report.constBegin(); it != report.constEnd(); ++it) {
        const Flat &minFlat = it.value().first;
        const Flat &maxFlat = it.value().second;
        logMessage(QString("%1: мін. %2 грн (%3), макс. %4 грн (%5)")
                       .arg(it.key())
                       .arg(minFlat.getRent(), 0, 'f', 0)
                       .arg(minFlat.getAddress())
                       .arg(maxFlat.getRent(), 0, 'f', 0)
                       .arg(maxFlat.getAddress()));
    }
}

void MainWindow::showGroupReport()
{
    QMap<int, QVector<Flat>> groups = catalog.groupByRooms();
    showGroupedFlats(groups);
    logMessage(QString("Групування за кімнатами: груп %1").arg(groups.size()));
}

void MainWindow::logMessage(const QString &text)
{
    QString time = QTime::currentTime().toString("HH:mm");
    ui->logView->append("[" + time + "] > " + text);
    ui->statusbar->showMessage(text, 5000);
}