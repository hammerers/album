#include "dirnamewidget.h"

DirNameWidget::DirNameWidget(QWidget*parent)
{
    setWindowTitle("folder name");
    this->setFixedSize(QSize(300,200));
    dir_name=new QLineEdit(this);
    connect(dir_name,&QLineEdit::editingFinished,this,&DirNameWidget::return_dir_name);
    dir_name->resize(100,50);
    dir_name->move(100,75);

}

void DirNameWidget::return_dir_name()
{
    QString name=dir_name->text();
    emit(sendDirName(name));
    qDebug()<<"DirNameWidget::return_dir_name: "<<name;
    this->hide();
}
