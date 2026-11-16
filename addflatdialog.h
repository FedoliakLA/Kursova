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
    void setFlat(const Flat &flat);

private slots:
    void on_btnSave_clicked();

private:
    Ui::AddFlatDialog *ui;

    static double parseNumber(const QString &text, bool *ok = nullptr);
    void validateInput() const;
    void showError(const QString &text);
};

#endif // ADDFLATDIALOG_H