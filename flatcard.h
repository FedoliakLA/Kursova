#ifndef FLATCARD_H
#define FLATCARD_H

#include <QFrame>
#include "flat.h"

class QContextMenuEvent;

class FlatCard : public QFrame
{
    Q_OBJECT

private:
    Flat flat;

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

public:
    explicit FlatCard(const Flat &flat, QWidget *parent = nullptr);
    ~FlatCard() override = default;

    const Flat &getFlat() const;
    void playAppearAnimation(int delayMs);

signals:
    void editRequested(const Flat &flat);
    void deleteRequested(const Flat &flat);
};

#endif // FLATCARD_H
