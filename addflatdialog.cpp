#include "addflatdialog.h"
#include "ui_addflatdialog.h"

AddFlatDialog::AddFlatDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddFlatDialog)
{
    ui->setupUi(this);
    for (int i = 1; i <= 10; i+=1) {
        ui->cbFloor->addItem(QString::number(i));
        ui->cbTotalFloors->addItem(QString::number(i));
    }
}

AddFlatDialog::~AddFlatDialog()
{
    delete ui;
}

Flat AddFlatDialog::getFlat() const
{
    QString address = ui->leAddress->text().trimmed();
    QString district = ui->leDistrict->text().trimmed();
    int rooms = ui->cbRooms->currentText().toInt();
    double area = ui->leArea->text().replace(',', '.').toDouble();
    int floor = ui->cbFloor->currentText().toInt();
    int totalFloors = ui->cbTotalFloors->currentText().toInt();
    double rent = ui->leRent->text().replace(',','.').toDouble();
    bool hasFurniture = ui->chkFurniture->isChecked();
    return Flat(address, district, rooms, area, floor, totalFloors, rent, hasFurniture);
}

bool AddFlatDialog::validateInput()
{
    return true;
}

void AddFlatDialog::showError(const QString &text)
{
    Q_UNUSED(text);
}

void AddFlatDialog::on_btnSave_clicked()
{
    if(validateInput()){
        accept();
    }

}