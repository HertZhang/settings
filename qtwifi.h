#ifndef QTWIFI_H
#define QTWIFI_H

#include <QInputDialog>
#include <QLabel>
#include <QListWidget>
#include <QTimer>

class qtWifi : public QListWidget
{
    Q_OBJECT

public:
    qtWifi(QWidget *parent = nullptr, QLabel *label = nullptr, QPushButton *btn = nullptr, bool on = false);
    ~qtWifi();
    int cnt;
    bool isOn();
    void turnOn();
    void turnOff();
    void updateConnectState();
public slots:
    void scan();
    void on_btnClicked();
    void on_itemClicked(QListWidgetItem *item);

private:
    QTimer findTimer;
    QTimer updateTimer;
    QLabel *text;
    QPushButton *switchBtn;
};

#endif // QTWIFI_H
