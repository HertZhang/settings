#ifndef QTBT_H
#define QTBT_H

#include <QWidget>

class qtBT : public QWidget
{
    Q_OBJECT

public:
    qtBT(QWidget *parent = 0);
    ~qtBT();
    int foundCount;
    QStringList btList;

public slots:

private:

};

#endif // QTBT_H
