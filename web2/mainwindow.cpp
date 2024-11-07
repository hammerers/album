#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"path.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
    this->setWindowTitle("images");
    ui->tree_widget->setHeaderLabel(PATH::BASE_PATH);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{


    //初始化展示图片的widget
    show_widget=new Show();

    pixmap=new QPixmap(PATH::EMP);

    ui->sort_box->addItem("正序");
    ui->sort_box->addItem("倒序");
    ui->flush_btn->setIcon(QIcon(PATH::FLUSH));

    ui->image_list_widget->setIconSize(QSize(125, 125));
    ui->image_list_widget->setViewMode(QListView::IconMode);
    ui->image_list_widget->setFlow(QListView::LeftToRight);
    ui->image_list_widget->setResizeMode(QListView::Adjust);
    ui->image_list_widget->setMovement(QListView::Static);
    ui->image_list_widget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->image_list_widget->setContextMenuPolicy(Qt::CustomContextMenu);
    //美化
    ui->image_list_widget->setStyleSheet(R"(
    QListWidget {
        border-radius: 10px; /* 设置圆角半径 */
        border: 2px solid #3b3b3b; /* 设置边框 */
        background-color: #f5f5f5; /* 设置背景颜色 */
    }

    QListWidget::Item:hover {
        background: lightblue; /* 鼠标悬停时的背景色 */
    }

    QListWidget::item:selected {
        background: lightgreen; /* 选中项的背景色 */
    }

    QScrollBar:vertical {
        border: 2px solid grey;
        background: #f1f1f1;
        width: 15px;
        margin: 22px 0 22px 0;
        border-radius: 10px; /* 为滚动条容器添加圆角 */
    }

    QScrollBar::handle:vertical {
        background: #888; /* 滑块的颜色 */
        min-height: 0px;
        border-radius: 10px; /* 设置滑块的圆角 */
    }

    QScrollBar::add-line:vertical {
        border: 2px solid grey;
        background: #32CC99;
        height: 20px;
        subcontrol-position: bottom;
        subcontrol-origin: margin;
        border-radius: 10px; /* 添加圆角 */
    }

    QScrollBar::sub-line:vertical {
        border: 2px solid grey;
        background: #32CC99;
        height: 20px;
        subcontrol-position: top;
        subcontrol-origin: margin;
        border-radius: 10px; /* 添加圆角 */
    }

    QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical {
        border: 2px solid grey;
        width: 3px;
        height: 3px;
        background: white;
        border-radius: 10px; /* 设置箭头部分的圆角 */
    }

    QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
        background: none;
    }
)"
);

    //创建文件夹填写名字
    dir_name_widget=new DirNameWidget();
    dir_name_widget->hide();
    //处理文件路径，裁剪图片类
    image_group=new ImageGroup(this);
    //初始化右键菜单栏
    menu=new QMenu();
    delete_action=menu->addAction(tr("delete"));
    folder_action=menu->addAction(tr("folder"));
    save_action=menu->addAction(tr("save"));
    //连接槽函数
    connect(image_group,&ImageGroup::reflush,this,&MainWindow::flush);
    connect(dir_name_widget,&DirNameWidget::sendDirName,this,&MainWindow::create_folder);
    connect(folder_action,&QAction::triggered,this,&MainWindow::show_dir_name_widget);
    connect(delete_action,&QAction::triggered,this,&MainWindow::delete_images);
    connect(image_group,&ImageGroup::sendImage,this,&MainWindow::add_preview,Qt::QueuedConnection);
    connect(ui->image_list_widget,&QListWidget::itemClicked,this,&MainWindow::set_view_image);
    connect(ui->image_list_widget,&QListWidget::itemDoubleClicked,this,&MainWindow::view_image);
    connect(ui->actionOpen,&QAction::triggered,this,&MainWindow::add_image);
    connect(ui->line_edit,&QLineEdit::editingFinished,this,&MainWindow::change_file_name);
    connect(ui->filter_edit,&QLineEdit::returnPressed,this,&MainWindow::filter_image);
    connect(ui->sort_box,&QComboBox::currentIndexChanged,this,&MainWindow::sort_change);
    connect(ui->flush_btn,&QPushButton::clicked,this,&MainWindow::flush);
    connect(image_group,&ImageGroup::sendTreeStruct,this,&MainWindow::add_tree_struct);
    connect(ui->tree_widget,&QTreeWidget::itemDoubleClicked,this,&MainWindow::view_image_by_tree);
    connect(ui->left_btn,&QPushButton::clicked,this,&MainWindow::back);
    connect(save_action,&QAction::triggered,this,&MainWindow::save_images);
    //连接槽函数后再进行初始化以免构造函数发送信号时没有槽函数连接导致信号丢失
    image_group->init();
    //读取config.txt文件获取所有的图片
    image_group->show_all_images();
    //安装listwidget过滤器
    ui->image_list_widget->installEventFilter(this);
    //常驻滚动条
    ui->scrollArea->setWidgetResizable(2);
    sort_change();
}
//单击图片时在右上方显示图片缩略图
void MainWindow::set_view_image(QListWidgetItem* item)
{
        view_image_path=item->data(Qt::UserRole).toString();
        preview_image_path=item->data(Qt::UserRole+1).toString();
        int index=item->data(Qt::UserRole+5).toInt();
        if(ui->view_label->movie()){
            QMovie *movie=ui->view_label->movie();
            ui->view_label->setMovie(nullptr);
            movie->stop();
            delete movie;
        }
        QMovie*movie=new QMovie(preview_image_path);
        ui->view_label->setScaledContents(true);
        QString image_name=image_group->get_image_name(view_image_path);
        cur_preview_path=view_image_path;//..web2/resource/datas/data/a/a.png
        if(index==0){
            image_name=item->data(Qt::UserRole+3).toString();//为了显示的名字不是文件夹的那个统一的路径而是文件夹的名字
            cur_preview_path=item->data(Qt::UserRole+4).toString()+"/"+item->data(Qt::UserRole+3).toString();//..web2/resource/datas/data/a
        }
        if(movie->isValid()){
            ui->view_label->setMovie(movie);
            movie->start();
            ui->line_edit->setText(image_name);

        }else{
            delete movie;
            QPixmap pixmap(view_image_path);
            if(pixmap.isNull()){
                qDebug()<<"MainWindow::set_view_image: image load error";
            }else{
                ui->view_label->setPixmap(pixmap.scaled(200,200,Qt::KeepAspectRatio, Qt::SmoothTransformation));

                ui->line_edit->setText(image_name);
            }
        }

}

void MainWindow::view_image(QListWidgetItem* item)
{

    int index=item->data(Qt::UserRole+5).toInt();
    if(index==1){//双击图片或单击预览按钮，显示show页面进行图片查看
        if(view_image_path!=""){
            show_widget->setPixmap(view_image_path);
            show_widget->show();
        }else{
            show_widget->setPixmap(PATH::EMP);
            qDebug()<<"MainWindow::view_image: currrent view_image_path is empty";
        }
    }else if(index==0){//如果是文件夹则进入文件夹
        QString dir_name=item->data(Qt::UserRole+3).toString();
        tree_label_paths.push_back(dir_name);
        QString new_path=PATH::BASE_PATH;
        for(auto path:tree_label_paths){
            new_path+="/"+path;
        }
        qDebug()<<"MainWindow::view_image: new_path: "<<new_path;
        ui->tree_widget->setHeaderLabel(new_path);
        image_group->set_current_path(new_path);
        ui->image_list_widget->clear();
        ui->tree_widget->clear();
        clear_show_widget();
        image_group->show_list_widget();
        image_group->show_all_images();
        image_group->show_tree_widget();
}

}
//左上方批量添加图片
void MainWindow::add_image()
{

    QStringList filenames=QFileDialog::getOpenFileNames(this,tr("images(*.png *jpeg *bmp)"));
    for(auto path:filenames){
        QFileInfo file_info(path);
        QString base_name=file_info.baseName();
        QString dir_path=file_info.dir().path();
        qDebug()<<"MainWindow::add_image: "<<path<<base_name<<dir_path;

    }
    image_group->add_images(filenames);
    Sleep(250);
    image_group->add_tree_widget();
}
//为listwidget中添加图片缩略图
void MainWindow::add_preview(QString path,bool is_file)
{
    if(is_file){
        qDebug()<<"MainWindow::add_preview: file exec";
        QFileInfo file_info(path);
        QString dir=file_info.dir().path();
        QString name=file_info.baseName();
        QString full_name=file_info.fileName();
        QString preview_path=dir+"/"+"_preview.gif";
        if(!QFile::exists(preview_path)){
            preview_path=dir+"/"+"_preview.jpg";
        }
        QListWidgetItem * item=new QListWidgetItem(full_name);
        MyListWidgetItem* item_widget = new MyListWidgetItem(preview_path,full_name);
        item->setForeground(QBrush(Qt::transparent));
        item->setSizeHint(item_widget->sizeHint());
        item->setData(Qt::UserRole,path);//原图地址../web2/resource/a/a.jpg
        item->setData(Qt::UserRole+1,preview_path);//缩略图地址../web2/resource/a/_preview.jpg
        item->setData(Qt::UserRole+2,full_name);//带后缀的名字，a.jpg
        item->setData(Qt::UserRole+3,name);//不带后缀的名字 a
        item->setData(Qt::UserRole+4,dir);//文件夹地址../web2/resource
        item->setData(Qt::UserRole+5,1);//1为文件0为文件夹
        //item->setData(Qt::UserRole+6,)//文件id
        ui->image_list_widget->addItem(item);
        ui->image_list_widget->setItemWidget(item,item_widget);
    }else{//文件夹
        qDebug()<<"MainWindow::add_preview: dir exec";
        QFileInfo file_info(path);
        QString name=file_info.baseName();
        QString dir=file_info.dir().path();
        QListWidgetItem * item=new QListWidgetItem(name);
        MyListWidgetItem* item_widget = new MyListWidgetItem(PATH::DIR_ICON,name);
        item->setForeground(QBrush(Qt::transparent));
        item->setSizeHint(item_widget->sizeHint());
        item->setData(Qt::UserRole,PATH::DIR_ICON);//原图地址../web2/resource/a/a.jpg
        item->setData(Qt::UserRole+3,name);//单纯的文件夹名字a
        item->setData(Qt::UserRole+1,PATH::DIR_ICON);//缩略图地址../web2/resource/a/_preview.jpg
        item->setData(Qt::UserRole+4,dir);//文件夹地址../web2/resource
        item->setData(Qt::UserRole+5,0);//1为文件0为文件夹
        //item->setData(Qt::UserRole+6,)//文件夹id
        ui->image_list_widget->addItem(item);
        ui->image_list_widget->setItemWidget(item,item_widget);
    }




}
//dir_path->../web2/resource/data
//dir_name->a
//dir_files->a.jpg,a_preview.jpg
void MainWindow::add_tree_struct(QString dir_path, QStringList dir_files,QString dir_name,bool dir_tag)//
{
    qDebug()<<"MainWindow::add_tree_struct: exec";
    if(dir_tag){
        QTreeWidgetItem* dir_item=new QTreeWidgetItem(QStringList()<<dir_name);
        ui->tree_widget->addTopLevelItem(dir_item);
        dir_item->setData(0,Qt::UserRole,dir_path);//文件夹地址
        dir_item->setData(0,Qt::UserRole+4,dir_name);//文件夹名字
        dir_item->setData(0,Qt::UserRole+5,0);//1文件0文件夹
    }else{
        QTreeWidgetItem* dir_item=new QTreeWidgetItem(QStringList()<<dir_name);
        ui->tree_widget->addTopLevelItem(dir_item);
        dir_item->setData(0,Qt::UserRole,dir_path);//文件夹地址
        dir_item->setData(0,Qt::UserRole+4,dir_name);//文件夹名字
        dir_item->setData(0,Qt::UserRole+5,1);//1文件0文件夹
        for(auto file:dir_files){
            QTreeWidgetItem* item=new QTreeWidgetItem(QStringList()<<file);
            item->setData(0,Qt::UserRole,dir_path);//文件夹地址../web2/resource
            item->setData(0,Qt::UserRole+1,file);//文件夹中的文件名字a.jpg
            item->setData(0,Qt::UserRole+4,dir_name);//文件夹名字a
            item->setData(0,Qt::UserRole+5,1);//1文件0文件夹
            dir_item->addChild(item);
        }
    }

}
//修改图片名称
void MainWindow::change_file_name()
{
    QString target_path=ui->line_edit->text();
    if(target_path=="Empty"){
        return;
    }
    //emit(ui->flush_btn->clicked());
    //QCoreApplication::processEvents();
    ui->image_list_widget->clear();
    ui->tree_widget->clear();
    clear_show_widget();
    Sleep(250);
    qDebug()<<"MainWindow::change_file_name: changed name: "<<target_path;

    bool is_dir=image_group->is_dir(cur_preview_path);
    if(is_dir){
        target_path=image_group->get_current_path()+"/"+target_path;
        qDebug()<<"MainWindow::change_file_name: origin_path: "<<cur_preview_path;
        qDebug()<<"MainWindow::change_file_name: target_path: "<<target_path;
        image_group->change_file_name(cur_preview_path,target_path,true);
    }else{
        target_path=image_group->get_current_path()+"/"+QFileInfo(target_path).baseName();
        qDebug()<<"MainWindow::change_file_name: origin_path: "<<cur_preview_path;
        qDebug()<<"MainWindow::change_file_name: target_path: "<<target_path;
        QMetaObject::invokeMethod(image_group, "change_file_name", Qt::QueuedConnection,Q_ARG(QString, cur_preview_path), Q_ARG(QString, target_path),Q_ARG(bool,false));
        //image_group->change_file_name(cur_preview_path,target_path,false);
    }
}
//搜索名称相同的图片进行高亮显示
void MainWindow::filter_image()
{
    QString path=ui->filter_edit->text();
    int count=ui->image_list_widget->count();
    for(int i=0;i<count;i++){
        auto item=ui->image_list_widget->item(i);
        if(item->data(Qt::UserRole+2).toString()==path){
            item->setSelected(true);
        }else{
            item->setSelected(false);
        }
    }
}

void MainWindow::sort_change()
{
    int mode=ui->sort_box->currentText()=="正序"?1:0;
        qDebug()<<"MainWindow::sort_change mode is :"<<mode;
    if(mode){
        ui->image_list_widget->sortItems(Qt::AscendingOrder);
    }else{

        ui->image_list_widget->sortItems(Qt::DescendingOrder);
    }


}

void MainWindow::flush()
{
    qDebug()<<"MainWindow::flush";
    sort_change();
    ui->image_list_widget->clear();
    ui->tree_widget->clear();
    clear_show_widget();
    image_group->show_list_widget();
    image_group->show_all_images();
    image_group->show_tree_widget();
    int count=ui->image_list_widget->count();
}

void MainWindow::view_image_by_tree()
{
    auto item=ui->tree_widget->currentItem();
    int index=item->data(0,Qt::UserRole+5).toInt();
    qDebug()<<index<<"|||||||||||||||||||||";
    if(index==1){
        if(item->parent()==nullptr){
            qDebug()<<"MainWindow::view_image_by_tree: top_item return;";
            return;
        }
        QString dir_path=item->data(0,Qt::UserRole).toString();
        QString dir_name=item->data(0,Qt::UserRole+4).toString();
        QString file_name=item->data(0,Qt::UserRole+1).toString();
        QFile file(dir_path+"/"+dir_name+"/"+file_name);
        if(!file.exists()){
            qDebug()<<"MainWindow::view_image_by_tree: file no exist";
            qDebug()<<"MainWindow::view_image_by_tree: current path: "<<QString(dir_path+"/"+dir_name+"/"+file_name);
            file.close();
            return ;
        }
        file.close();
        view_image_path=dir_path+"/"+dir_name+"/"+file_name;
        qDebug()<<dir_path<<" "<<dir_name<<" "<<file_name;
        QListWidgetItem*target_item=find_image_item(view_image_path);
        if(target_item==nullptr){
            qDebug()<<"MainWindow::view_image_by_tree: item is nullptr, path is: "<<view_image_path;
            return ;
        }
        view_image(target_item);
    }else if(index==0){
        QString dir_name=item->data(0,Qt::UserRole+4).toString();
        tree_label_paths.push_back(dir_name);
        QString new_path=PATH::BASE_PATH;
        for(auto path:tree_label_paths){
            new_path+="/"+path;
        }
        qDebug()<<"MainWindow::view_image_by_tree: new_path: "<<new_path;
        ui->tree_widget->setHeaderLabel(new_path);
        image_group->set_current_path(new_path);
        ui->image_list_widget->clear();
        ui->tree_widget->clear();
        image_group->show_list_widget();
        image_group->show_all_images();
        image_group->show_tree_widget();
    }





}

void MainWindow::create_folder(QString name)
{
    image_group->create_folder(name);
    add_tree_struct(image_group->get_current_path(),QStringList(),name,true);
}

void MainWindow::show_dir_name_widget()
{
    dir_name_widget->show();
}

void MainWindow::back()
{
    QString cur_path=image_group->get_current_path();
    if(cur_path==PATH::BASE_PATH){
        qDebug()<<"MainWindow::back: there is the base path ,you can't back";
        return;
    }
    QString new_path=image_group->get_dir_path(cur_path);
    qDebug()<<"MainWindow::back: cur_path: "<<cur_path;
    qDebug()<<"MainWindow::back: new_path: "<<new_path;
    image_group->set_current_path(new_path);
    ui->image_list_widget->clear();
    ui->tree_widget->clear();
    image_group->show_list_widget();
    image_group->show_all_images();
    image_group->show_tree_widget();
    ui->tree_widget->setHeaderLabel(new_path);
    tree_label_paths.pop_back();

    if(show_widget!=nullptr){
        delete show_widget;
        show_widget=nullptr;
    }
    show_widget=new Show();
    ui->line_edit->setText("Empty");
    ui->view_label->clear();
}

void MainWindow::clear_show_widget()
{
    if(show_widget!=nullptr){
        delete show_widget;
        show_widget=nullptr;
    }
    show_widget=new Show();
    ui->line_edit->setText("Empty");
    QMovie* movie=ui->view_label->movie();
    ui->view_label->clear();
    delete movie;
}

void MainWindow::save_images()
{
    QString folder_path = QFileDialog::getExistingDirectory(nullptr, "Select Folder", "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(!folder_path.isEmpty()){
        QList<QListWidgetItem*>items=ui->image_list_widget->selectedItems();
        if(items.size()==0){
            return;
        }
        QStringList paths;
        for(auto item:items){
            QString path=item->data(Qt::UserRole).toString();
            if(path==PATH::DIR_ICON){
                qDebug()<<"MainWindow::save_images: unable to save dir, continue this dir";
                continue;
            }
            QString name=item->data(Qt::UserRole+2).toString();
            QImage image(path);
            if(!image.isNull()){
                image.save(folder_path+"/"+name);
            }
        }

    }

}
//事件过滤器
bool MainWindow:: eventFilter(QObject *obj, QEvent *event){

    return QWidget::eventFilter(obj, event);
}
//鼠标事件
void MainWindow::mousePressEvent(QMouseEvent*e){
    if(e->button()==Qt::RightButton){
        //右击鼠标显示悬浮菜单栏
        menu->exec(QCursor::pos());
    }
}

QListWidgetItem* MainWindow::find_image_item(QString path)
{
    for(int i=0;i<ui->image_list_widget->count();i++){
        bool tag_1=ui->image_list_widget->item(i)->data(Qt::UserRole)==path;//匹配原图
        bool tag_2=ui->image_list_widget->item(i)->data(Qt::UserRole+1)==path;//匹配缩略图
        if(tag_1||tag_2){
            return ui->image_list_widget->item(i);
        }
    }
    return nullptr;
}

//批量删除图片
void MainWindow::delete_images()
{
    view_image_path="";
    preview_image_path="";
    ui->view_label->clear();

    QList<QListWidgetItem*>selected_images=ui->image_list_widget->selectedItems();
    QStringList images_path;
    if(selected_images.size()==0){
        qDebug()<<"MainWindow::delete_images: select one or more images";
        return;
    }
    for(QListWidgetItem* item :selected_images){
        //删除listwidget
        QString path=item->data(Qt::UserRole).toString();
        bool is_dir=path==PATH::DIR_ICON?true:false;
        qDebug()<<"MainWindow::delete_images selected image: "<<path;
        if(is_dir){
            path=item->data(Qt::UserRole+4).toString()+"/"+item->data(Qt::UserRole+3).toString();
            images_path.push_back(path);
        }else{
            images_path.push_back(path);
        }
        QWidget* item_widget = ui->image_list_widget->itemWidget(item);
        MyListWidgetItem* my_item_widget=qobject_cast<MyListWidgetItem*>(item_widget);
        ui->image_list_widget->removeItemWidget(item);

        //删除treewidget
        QString name=item->data(Qt::UserRole+3).toString();
        qDebug()<<"MainWindow::delete_images tree name: "<<name;
        auto tree_items=ui->tree_widget->findItems(name,Qt::MatchContains | Qt::MatchRecursive);
        for (auto& item : tree_items) {
            QTreeWidgetItem *parent = item->parent();
            if (parent) {
                parent->removeChild(item);
            } else {
                delete ui->tree_widget->takeTopLevelItem(ui->tree_widget->indexOfTopLevelItem(item));
                break;
            }
        }


        if(item!=nullptr){
            delete item;
            item=nullptr;
        }
        my_item_widget->label->clear();
        my_item_widget->movie->stop();
        if(my_item_widget!=nullptr){
            delete my_item_widget;
            my_item_widget=nullptr;
        }


    }
    if(show_widget!=nullptr){
        delete show_widget;
        show_widget=nullptr;
    }
    show_widget=new Show();
    ui->line_edit->setText("Empty");
    image_group->delete_images_text(images_path);
    qDebug()<<"MainWindow::delete_images: current image_list_widget count: "<<ui->image_list_widget->count();
    qDebug()<<"MainWindow::delete_images: current image_group->all_images count: "<<image_group->all_images.count();

}
