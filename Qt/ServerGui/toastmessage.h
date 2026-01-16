/*
 Author: Michele Di Lucchio
        23/05/25
    Classe helper ToastMessage (una piccola finestra popup)
*/
#ifndef TOASTMESSAGE_H
#define TOASTMESSAGE_H

#include <QWidget>
#include <QTimer>
#include <QLabel>

class toastmessage : public QWidget
{
    Q_OBJECT
public:
    explicit toastmessage(const QString &message, QWidget *parent = nullptr, int durationMs = 3000);

private:
    QLabel *label;
    QTimer *timer;
};

#endif // TOASTMESSAGE_H
