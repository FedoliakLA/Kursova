#include "addflatdialog.h"
#include "ui_addflatdialog.h"
#include "cerror.h"

#include <QRegularExpression>
#include <QRegularExpressionValidator>

AddFlatDialog::AddFlatDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddFlatDialog)
{
    ui->setupUi(this);

    ui->spRooms->setRange(FlatLimits::MinRooms, FlatLimits::MaxRooms);
    ui->spTotalFloors->setRange(FlatLimits::MinFloor, FlatLimits::MaxFloors);
    ui->spFloor->setRange(FlatLimits::MinFloor, ui->spTotalFloors->value());

    QRegularExpression areaPattern(R"(^\d{0,4}([.,]\d{0,2})?$)");
    QRegularExpression rentPattern(R"(^\d{0,7}([.,]\d{0,2})?$)");
    ui->leArea->setValidator(new QRegularExpressionValidator(areaPattern, this));
    ui->leRent->setValidator(new QRegularExpressionValidator(rentPattern, this));
}

AddFlatDialog::~AddFlatDialog()
{
    delete ui;
}

double AddFlatDialog::parseNumber(const QString &text, bool *ok)
{
    return text.trimmed().replace(',', '.').toDouble(ok);
}

Flat AddFlatDialog::getFlat() const
{
    return Flat(ui->leAddress->text().trimmed(),
                ui->leDistrict->text().trimmed(),
                ui->spRooms->value(),
                parseNumber(ui->leArea->text()),
                ui->spFloor->value(),
                ui->spTotalFloors->value(),
                parseNumber(ui->leRent->text()),
                ui->chkFurniture->isChecked());
}

void AddFlatDialog::setFlat(const Flat &flat)
{
    ui->leAddress->setText(flat.getAddress());
    ui->leDistrict->setText(flat.getDistrict());
    ui->spRooms->setValue(flat.getRooms());
    ui->leArea->setText(QString::number(flat.getArea(), 'g', 10));
    ui->spTotalFloors->setValue(flat.getTotalFloors());
    ui->spFloor->setValue(flat.getFloor());
    ui->leRent->setText(QString::number(flat.getRent(), 'g', 10));
    ui->chkFurniture->setChecked(flat.getHasFurniture());

    setWindowTitle("Редагувати квартиру");
    ui->lblDialogTitle->setText("Редагувати квартиру");
    ui->btnSave->setText("Зберегти");
}

void AddFlatDialog::validateInput() const
{
    bool ok = false;

    parseNumber(ui->leArea->text(), &ok);
    if (!ok)
        throw CError("Площа має бути числом, наприклад 53.5");

    parseNumber(ui->leRent->text(), &ok);
    if (!ok)
        throw CError("Оренда має бути числом, наприклад 12000");

    getFlat().validate();
}

void AddFlatDialog::showError(const QString &text)
{
    ui->lblError->setText(text);
    ui->lblError->setVisible(true);
}

void AddFlatDialog::on_btnSave_clicked()
{
    try {
        validateInput();
        accept();
    }
    catch (const CError &e) {
        showError(e.getMessage());
    }
}