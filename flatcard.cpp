#include "flatcard.h"

#include <QContextMenuEvent>
#include <QGraphicsOpacityEffect>
#include <QLabel>
#include <QMenu>
#include <QPropertyAnimation>
#include <QTimer>
#include <QVBoxLayout>

FlatCard::FlatCard(const Flat &flat, QWidget *parent)
    : QFrame(parent)
    , flat(flat)
{
    setObjectName("flatCard");
    setFixedWidth(250);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum);
    setAttribute(Qt::WA_Hover);
    setToolTip("Права кнопка миші: редагувати або видалити");

    QVBoxLayout *layout = new QVBoxLayout(this);
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
}

const Flat &FlatCard::getFlat() const
{
    return flat;
}

void FlatCard::playAppearAnimation(int delayMs)
{
    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(this);
    effect->setOpacity(0.0);
    setGraphicsEffect(effect);

    QPropertyAnimation *fade = new QPropertyAnimation(effect, "opacity", this);
    fade->setDuration(350);
    fade->setStartValue(0.0);
    fade->setEndValue(1.0);
    fade->setEasingCurve(QEasingCurve::OutCubic);

    connect(fade, &QPropertyAnimation::finished, this, [this]() {
        setGraphicsEffect(nullptr);
    });

    QTimer::singleShot(delayMs, fade, [fade]() {
        fade->start(QAbstractAnimation::DeleteWhenStopped);
    });
}

void FlatCard::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    QAction *edit = menu.addAction("Редагувати");
    QAction *remove = menu.addAction("Видалити");

    QAction *chosen = menu.exec(event->globalPos());

    if (chosen == edit)
        QTimer::singleShot(0, this, [this]() { emit editRequested(flat); });
    else if (chosen == remove)
        QTimer::singleShot(0, this, [this]() { emit deleteRequested(flat); });
}
