#ifndef DIRNAMEWIDGET_H
#define DIRNAMEWIDGET_H

#include <QWidget>
#include<QLineEdit>
class DirNameWidget:public QWidget
{
        Q_OBJECT
public:
    explicit DirNameWidget(QWidget*parent=nullptr);
    QLineEdit*dir_name;
public slots:
    void return_dir_name();
signals:
    void sendDirName(QString);
};

#endif // DIRNAMEWIDGET_H
