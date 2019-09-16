
#include <QApplication>
#include <QDesktopWidget>
#include <QFile>
#include <QProcess>
#include "qtbt.h"

qtBT::qtBT(QWidget *parent, QLabel *label, QPushButton *btn, bool on)
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
    setObjectName("BT");
    setFont(font);
    bsThread = new btScanThread();
    connect(this, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(on_itemClicked(QListWidgetItem *)));
    connect(bsThread, SIGNAL(resultReady(QStringList)), this, SLOT(handleResults(QStringList)));
    connect(bsThread, SIGNAL(finished()), bsThread, SLOT(deleteLater()));
    show();
    if(on)
        turnOn();

}

qtBT::~qtBT()
{
    if(bsThread){
        if(bsThread->isRunning()){
            bsThread->terminate();
            bsThread->wait();
        }
        delete bsThread;
        bsThread = nullptr;
    }
    if(switchBtn){
        switchBtn->setVisible(false);
    }
    if(text){
        text->setVisible(false);
    }
}

bool qtBT::isOn()
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

void qtBT::turnOn()
{
    if(QFile::exists("/userdata") && QFile::exists("/usr/bin/bt_pcba_test")){
        QProcess p;
        p.start("/usr/bin/bt_pcba_test");
        p.waitForStarted();
        p.waitForFinished();
        p.start("hciconfig hci0 up");
        p.waitForStarted();
        p.waitForFinished();
        bsThread->start();
    } else {
        QStringList list;
        list << "bt1" << "bt2" << "bt3" << "bt4" << "bt5" << "bt6" << "bt7";
        addItems(list);
    }
}

void qtBT::turnOff()
{
    if(QFile::exists("/userdata")){
        QProcess p;
        p.start("hciconfig hci0 down");
        p.waitForStarted();
        p.waitForFinished();
    }
    clear();
}

void qtBT::updateConnectState()
{

}

void qtBT::on_btnClicked()
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

void qtBT::on_itemClicked(QListWidgetItem *item)
{
    qDebug() << item->text();
}

void qtBT::handleResults(const QStringList &list)
{
    clear();
    addItems(list);
    updateConnectState();
}
