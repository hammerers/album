#ifndef SHOW_H
#define SHOW_H

#include <QObject>
#include <QWidget>
#include<QPushButton>
#include<QLabel>
#include<QWheelEvent>
#include<QGraphicsPixmapItem>
#include<QGraphicsView>
#include<QGraphicsScene>
#include <QApplication>
#include <QMouseEvent>
#include <QGraphicsSceneWheelEvent>
#include<imagewidget.h>
class Show : public QWidget
{
    Q_OBJECT
public:
    explicit Show(QWidget *parent = nullptr);
    ~Show();
    QPoint lastMousePos;
    QPixmap pixmap;
    QGraphicsScene* scene;
    QGraphicsView* view;
    ImageWidget* imageItem=nullptr;
    float scale=1.0;
    int mouse_x;
    int mouse_y;
    int h;
    int w;
    void closeEvent(QCloseEvent *event);
    void setPixmap(QString path);
    inline void init();
    void removePixmap();
signals:
    void out();
};

#endif // SHOW_H
