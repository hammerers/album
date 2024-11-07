#ifndef IMAGEGROUP_H
#define IMAGEGROUP_H
#include<QImage>
#include <QAbstractItemModel>
#include<QPixmap>
#include<QStringList>
#include"mylistwidgetitem.h"
#include<QListWidgetItem>
#include<QFile>
#include<QFileInfo>
#include<QDir>
#include <QtConcurrent/QtConcurrent>
#include<QFileDialog>

class ImageGroup : public QObject
{
    Q_OBJECT

public:
    QStringList* get_all_images(){return &all_images;}
    QStringList* gey_new_images(){return &new_images;}
   ImageGroup(QObject *parent = nullptr);
   QStringList all_images;
   QStringList new_images;
   QStringList all_tree;
   QStringList new_tree;
   QStringList all_dirs;
   QString current_path="../web2/resource/datas";
   QStringList current_paths;
   QString path;

   void init();//初始化
   void show_all_images();//发送信号让mainwindow的list_widget显示文件夹和图片
   void show_list_widget();//读取dir_config,config文件导入图片和文件夹大炮all_images,all_dirs
   void show_tree_widget();//读取文件夹信息,和发送信号让mainwindow的tree_widget显示文件夹和图片,相比list,合并了以上两个方法

   void add_images(QStringList&);//这三个方法处理批量导入图片
   void process_images(QStringList&);
   void create_preview(QString path,QString base_name,QString dir_path,QString suffix);//创建缩略图

   void add_tree_widget();
    void delete_dirs(const QString &);//初始化时调用对全部文件夹扫盘，处理它们的delete_config
   void delete_images_text(QStringList&);//在delete_config写入需要删除的文件
   void delete_entire_dir(QString);//扫盘当中delete_config提取到应删除的路径进行完全删除，不用理会当中的delete_config

   QString get_image_name(QString);
   QString get_dir_path(QString);//返回dir的上一级

   void create_folder(QString);//创建文件夹及其三个config文件
   bool is_image(QString);
   bool is_dir(QString);
   bool is_my_dir(QString);//与前两个不同，由于图片也会包一层文件夹，所以判断是否为文件夹关键在有没有config文档

   void set_config(QString,QStringList&);//处理config，第一个参数是决定dir_config还是config以及在哪个具体文件夹，第二个参数是需要去除的路径
   void set_config_and_replace(QString,QString,QString);
   void set_current_path(QString);//设置当前路径

   void clear_inf();
   QString get_current_path();
   public slots:
   void change_file_name(const QString &cur_preview_path, const QString &target_path, bool is_dir);//改名字

   void delete_dirs_reflush(const QString&);
signals:
   void sendImage(QString,bool);//发送信号让listwidget添加item，第一个是路径，第二个判断为文件夹还是图片
    void sendTreeStruct(QString dir_path,QStringList dir_files,QString dirname,bool dir_tag);//发送信号让treewidget添加item
   void reflush();
};

#endif // IMAGEGROUP_H
