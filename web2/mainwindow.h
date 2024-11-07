

#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include<QLabel>
#include<QPixmap>
#include<QStackedWidget>
#include<QPushButton>
#include<QListWidget>
#include<QMovie>
#include<QAbstractNativeEventFilter>
#include<QMouseEvent>
#include"show.h"
#include<iostream>
#include<QFileDialog>
#include<QStringList>
#include"imagegroup.h"
#include<QMouseEvent>
#include<QListWidget>
#include<QLineEdit>
#include<QTreeWidget>
#include"dirnamewidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    Show*show_widget;
    ImageGroup*image_group;
    DirNameWidget*dir_name_widget;
    QString view_image_path="";
    QString preview_image_path="";
    QMenu* menu;
    QAction*delete_action;
    QAction*folder_action;
    QAction*save_action;
    QPixmap*pixmap=nullptr;
    QStringList tree_label_paths;
    QString e_mail="";
    QString cur_preview_path="";//标识当前右边缩略图的路径
    void init();//初始化
    bool eventFilter(QObject *obj, QEvent *event);
    void mousePressEvent(QMouseEvent*e);
    QListWidgetItem* find_image_item(QString);
public slots:
    void delete_images();//删除被选中的图片
    void set_view_image(QListWidgetItem* item);//单击右上角出现预览图片
    void view_image(QListWidgetItem* item);//双击(list_widget)查看图片
    void add_image();//通过本地文件上传文件到应用
    void add_preview(QString,bool is_file=true);//通过imagegroup的信号创建对应list结构的item
    void add_tree_struct(QString,QStringList,QString,bool);//通过imagegroup的信号创建对应树结构的item
    void change_file_name();//修改文件夹，图片名字
    void filter_image();//查找
    void sort_change();//排序
    void flush();//刷新
    void view_image_by_tree();//双击某个文件(tree_widget)后查看图片
    void create_folder(QString);//创建文件夹
    void show_dir_name_widget();//创建文件夹时输入文件夹名称
    void back();//点击按钮后返回到上一个界面
    void clear_show_widget();
    void save_images();//保存图片
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
