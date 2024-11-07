#include "Mylistwidgetitem.h"

#include <qbitmap.h>
#include <qpainter.h>
#include <QVBoxLayout>

MyListWidgetItem::MyListWidgetItem(const QString& filePath,const QString& full_name, QWidget* parent) : QWidget(parent) {
    QVBoxLayout* layout = new QVBoxLayout(this);

    layout->setSpacing(5);
    label = new QLabel;
    movie = new QMovie(filePath);
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    label->setScaledContents(true);
    label->setMovie(movie);
    label->setFixedSize(150,150);
    movie->start();
    path_label=new QLabel;
    path_label->setFixedSize(150,20);
    path_label->setText(full_name);
    if (movie->isValid()) {
        label->setMovie(movie);
        movie->start();
    }
    else {
        QPixmap pixmap(filePath);
        label->setPixmap(pixmap);
    }
    layout->addWidget(label);
    layout->addWidget(path_label);
    layout->setAlignment(Qt::AlignCenter);
    this->setLayout(layout);
}


MyListWidgetItem::~MyListWidgetItem()
{
    delete label;
    delete movie;
    delete path_label;
}
