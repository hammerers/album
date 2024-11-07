

#include <QWidget>
#include <QPropertyAnimation>
#include <qlabel.h>
#include <qmovie.h>
#include<QVBoxLayout>
class MyListWidgetItem  : public QWidget
{
    Q_OBJECT

public:
    MyListWidgetItem(const QString& filePath, const QString& full_name,QWidget* parent = nullptr);
    ~MyListWidgetItem();
public:
    QLabel* label;
    QMovie* movie;
    QLabel* path_label;

};
