#include "imagewidget.h"
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QPointF>
#include <QGraphicsSceneDragDropEvent>
#include <QDrag>
#include <math.h>
ImageWidget::ImageWidget(QPixmap *pixmap)
{
    m_pix = *pixmap;
    setAcceptDrops(true);
    m_scaleValue = 0;
    m_scaleDafault = 0;
    m_isMove = false;
}
QRectF ImageWidget::boundingRect() const{
    return QRectF(-m_pix.width() / 2, -m_pix.height() / 2,
                  m_pix.width(), m_pix.height());
}
void ImageWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                        QWidget *){
    painter->drawPixmap(-m_pix.width() / 2, -m_pix.height() / 2, m_pix);
}
void ImageWidget::mousePressEvent(QGraphicsSceneMouseEvent *event){
    if(event->button()== Qt::LeftButton){
        m_startPos = event->pos();
        m_isMove = true;
    }
    else if(event->button() == Qt::RightButton){
        ResetItemPos();
    }
}
void ImageWidget::mouseMoveEvent(QGraphicsSceneMouseEvent *event){
    if(m_isMove){
        QPointF point = (event->pos() - m_startPos)*m_scaleValue;
        moveBy(point.x(), point.y());
    }
}
void ImageWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent *){
    m_isMove = false;
}

void ImageWidget::wheelEvent(QGraphicsSceneWheelEvent *event){
    if((event->delta() > 0)&&(m_scaleValue >= 50)){
        return;
    }
    else if((event->delta() < 0)&&(m_scaleValue <= m_scaleDafault)){
        ResetItemPos();
    }else{
        qreal qrealOriginScale = m_scaleValue;
        if(event->delta() > 0){
            m_scaleValue*=1.1;
        }else{
            m_scaleValue*=0.9;
        }
        setScale(m_scaleValue);
        if(event->delta() > 0){
            moveBy(-event->pos().x()*qrealOriginScale*0.1, -event->pos().y()*qrealOriginScale*0.1);
        }else{
            moveBy(event->pos().x()*qrealOriginScale*0.1, event->pos().y()*qrealOriginScale*0.1);
        }
        //qDebug()<<event->pos();
    }
}

void ImageWidget::setQGraphicsViewWH(int nwidth, int nheight){
    int nImgWidth = m_pix.width();
    int nImgHeight = m_pix.height();
    qreal temp1 = nwidth*1.0/nImgWidth;
    qreal temp2 = nheight*1.0/nImgHeight;
    if(temp1>temp2){//选取比例更大的那个
        m_scaleDafault = temp2;
    }else{
        m_scaleDafault = temp1;
    }
    setScale(m_scaleDafault);
    m_scaleValue = m_scaleDafault;
}

void ImageWidget::ResetItemPos(){
    m_scaleValue = m_scaleDafault;
    setScale(m_scaleDafault);
    setPos(0,0);
}

qreal ImageWidget::getScaleValue() const{
    return m_scaleValue;
}
