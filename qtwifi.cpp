
#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QListWidgetItem>
#include <QProcess>
#include <QTextStream>
#include "qtkeyboard.h"
#include "qtinputdialog.h"
#include "qtwifi.h"

qtWifi::qtWifi(QWidget *parent, QLabel *label, QPushButton *btn, bool on): cnt(0)
{
    const QRect availableGeometry = QApplication::desktop()->availableGeometry(parent);
    resize(availableGeometry.width(), availableGeometry.height());

    QFont font;
    font.setPixelSize(availableGeometry.height()/20);

    if(label){
        text = label;
        updateConnectState();
        text->setVisible(true);
    }
    if(btn){
        switchBtn = btn;
        switchBtn->setCheckable(true);
        switchBtn->setVisible(true);
        switchBtn->setStyleSheet("QPushButton{background-color:green;}");
        switchBtn->setStyleSheet("QPushButton:checked{background-color:red;}");
        if (on){
            switchBtn->setChecked(true);
            switchBtn->setText("on");
        } else {
            switchBtn->setChecked(false);
            switchBtn->setText("off");
        }
        connect(switchBtn, SIGNAL(clicked(bool)), this, SLOT(on_btnClicked()));
    }
    setObjectName("WiFi");
    setFont(font);
    wifiThread = new wifiScanThread();
    connect(this, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(on_itemClicked(QListWidgetItem *)));
    connect(wifiThread, SIGNAL(resultReady(QStringList)), this, SLOT(handleResults(QStringList)));
    connect(wifiThread, SIGNAL(finished()), wifiThread, SLOT(deleteLater()));
    show();
    if(on)
        turnOn();
}

qtWifi::~qtWifi()
{
    QKeyBoard *qkb = QKeyBoard::getInstance();
    delete qkb;
    inputDialog *dialog = inputDialog::getInstance(this);
    delete dialog;
    if(wifiThread){
        if(wifiThread->isRunning()){
            wifiThread->terminate();
            wifiThread->wait();
        }
        delete wifiThread;
        wifiThread = nullptr;
    }
    if(switchBtn){
        switchBtn->setVisible(false);
    }
    if(text){
        text->setVisible(false);
    }
}

bool qtWifi::isOn()
{
    if(switchBtn){
        if (! switchBtn->text().compare("on")){
            return true;
        } else {
            return false;
        }
    }
    return false;
}

void qtWifi::turnOn()
{
    if(QFile::exists("/userdata")){
        QProcess p;
        p.start("ifconfig wlan0 up");
        p.waitForStarted();
        p.waitForFinished();
        wifiThread->start();
    } else {
        QStringList list;
        list << "wifi1" << "wifi2" << "wifi3" << "wifi4" << "wifi5" << "wifi6" << "wifi7";
        addItems(list);
    }
}

void qtWifi::turnOff()
{
    if(QFile::exists("/userdata")){
        QProcess p;
        p.start("ifconfig wlan0 down");
        p.waitForStarted();
        p.waitForFinished();
    }
    clear();
}

void qtWifi::updateConnectState()
{
    QProcess p;

    p.start("wpa_cli -i wlan0 status");
    p.waitForStarted();
    p.waitForFinished();
    QTextStream tt(p.readAllStandardOutput());
    QString ll, ssid;
    bool found = false;
    do{
        ll = tt.readLine();

        if(!ll.compare("wpa_state=COMPLETED")){
            found = true;
        }
        if(!ll.mid(0, 5).compare("ssid=")){
            ssid = ll.mid(5, ll.count());
        }
    }while (! ll.isNull());

    if(found){
        text->setText(ssid + " Connected");
    }else {
        text->setText("No Wifi Connected");
    }
}

void qtWifi::on_btnClicked()
{
    if(switchBtn){
        if (! switchBtn->text().compare("on")){
            switchBtn->setText("off");
            turnOff();
        } else if (! switchBtn->text().compare("off")){
            switchBtn->setText("on");
            turnOn();
        }
    }
}

void qtWifi::on_itemClicked(QListWidgetItem *item)
{
    QKeyBoard::getInstance();
    inputDialog *dialog = inputDialog::getInstance(this);

    dialog->setText("Connect", "Cancel", "Password of " + item->text());
    int result = dialog->exec();
    if(result){
        QString str = dialog->getEditText();
        QProcess p;
        QStringList arguments;

        QString ss = "wifi_start.sh " + item->text() + " " + str;
        qDebug() << ss;
        p.start(ss);
//        arguments << "-c" << "wifi_start.sh " + item->text() + " " + str;
//        p.start("sh", arguments);
        p.waitForStarted();
        p.waitForFinished();
        updateConnectState();
    }
}

void qtWifi::handleResults(const QStringList &list)
{
    clear();
    addItems(list);
    updateConnectState();
}

void wifiScanThread::run() {
    QProcess p;
    while(1){
        QStringList arguments;
        arguments << "-c" << "iw dev wlan0 scan | grep SSID";
        p.start("sh", arguments);
        p.waitForStarted();
        p.waitForFinished();
        QTextStream result(p.readAllStandardOutput());
        QString line;
        QStringList list, sl;
        do{
            line = result.readLine();
            line = line.mid(7);
//            qDebug() << line;
            list << line;
        }while (! line.isNull());
        emit resultReady(list);
        sleep(10000);
    }
}
