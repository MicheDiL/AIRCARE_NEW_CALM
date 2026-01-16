#include "toastmessage.h"

#include <QVBoxLayout>

toastmessage::toastmessage(const QString &message, QWidget *parent, int durationMs)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);

    label = new QLabel(message, this);
    label->setStyleSheet("QLabel { background-color: rgba(50, 50, 50, 200); color: white; "
                         "border-radius: 8px; padding: 10px; font-weight: bold; }");
    label->setAlignment(Qt::AlignCenter);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(label);

    adjustSize();

    // Posiziona il toast in alto centro rispetto al parent
    if (parent) {
        QPoint parentTopCenter = parent->geometry().topLeft() + QPoint(parent->width()/2 - width()/2, 20);
        move(parentTopCenter);
    }

    // Timer per chiudere automaticamente
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, &toastmessage::close);
    timer->start(durationMs);

    show();
}
