#ifndef ADDFLATDIALOG_H
#define ADDFLATDIALOG_H

#include <QDialog>
#include "flat.h"

namespace Ui {
class AddFlatDialog;
}

class AddFlatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddFlatDialog(QWidget *parent = nullptr);
    ~AddFlatDialog();

    Flat getFlat() const;

private slots:
    void on_btnSave_clicked();

private:
    Ui::AddFlatDialog *ui;

    bool validateInput();
    void showError(const QString &text);
};

#endif // ADDFLATDIALOG_H