#ifndef QTBT_H
#define QTBT_H

#include <QDebug>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QProcess>
#include <QTextStream>
#include <QThread>
#include <QTimer>
#include <QWidget>

class btScanThread : public QThread
{
    Q_OBJECT
    void run();
signals:
    void resultReady(const QStringList &s);
};

class qtBT : public QListWidget
{
    Q_OBJECT

public:
    qtBT(QWidget *parent = nullptr, QLabel *label = nullptr, QPushButton *btn = nullptr, bool on = false);
    ~qtBT();
    int foundCount;
    QStringList btList;
    bool isOn();
    void turnOn();
    void turnOff();
    void updateConnectState();

public slots:
    void on_btnClicked();
    void on_itemClicked(QListWidgetItem *item);
    void handleResults(const QStringList &list);
private:
    QLabel *text;
    QPushButton *switchBtn;
    btScanThread *bsThread;
};

#endif // QTBT_H
