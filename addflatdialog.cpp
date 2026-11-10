#include "addflatdialog.h"
#include "ui_addflatdialog.h"

AddFlatDialog::AddFlatDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddFlatDialog)
{
    ui->setupUi(this);
}

AddFlatDialog::~AddFlatDialog()
{
    delete ui;
}

Flat AddFlatDialog::getFlat() const
{
    return Flat();
}

bool AddFlatDialog::validateInput()
{
    return false;
}

void AddFlatDialog::showError(const QString &text)
{
    Q_UNUSED(text);
}

void AddFlatDialog::on_btnSave_clicked()
{
}