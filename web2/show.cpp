#include "show.h"
#include "qevent.h"

Show::Show(QWidget *parent)
    : QWidget{parent}
{
    init();
}

Show::~Show()
{
    delete scene;

}
void Show::setPixmap(QString path){
    this->pixmap=QPixmap(path);
    if(this->imageItem!=nullptr){
        scene->removeItem(imageItem);
        delete imageItem;
    }
    this->imageItem=new ImageWidget(&pixmap);
    this->scene->addItem(imageItem);
    imageItem->setQGraphicsViewWH(w,h);
}
void Show::removePixmap(){
    if(this->imageItem!=nullptr){
        qDebug()<<"Show::removePixmap: imageItem!=nullptr";
        scene->removeItem(imageItem);
        delete imageItem;
        imageItem=nullptr;
    }
    this->pixmap=QPixmap("../web2/resource/EMP.png");
    this->imageItem=new ImageWidget(&pixmap);
    this->scene->addItem(imageItem);
    imageItem->setQGraphicsViewWH(w,h);
}
void Show::closeEvent(QCloseEvent*e){
    this->hide();

    emit(out());
}
inline void Show::init(){
    scene=new QGraphicsScene;
    w=this->width();
    h=this->height();
    scene->setSceneRect(QRectF(-w/2,-h/2,w,h));
    view=new QGraphicsView(this);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setRenderHint(QPainter::Antialiasing);
    view->setRenderHint(QPainter::SmoothPixmapTransform);
    view->setScene(scene);
    view->setSceneRect(QRectF(-w/2,-h/2,w,h));
    view->move(0,0);
    view->show();
    view->setFocus();
    view->setMouseTracking(true);
}

