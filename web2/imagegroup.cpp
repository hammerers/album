#include "imagegroup.h"
ImageGroup::ImageGroup(QObject *parent)
    : QObject(parent)
{
}
//遍历all_images，发送信号展示图片
void ImageGroup::show_all_images()
{
    for(auto path:all_images){
        emit(sendImage(path,true));//true为图，false为文件夹
    }
    for(auto path:all_dirs){
        emit(sendImage(path,false));
    }

}
//创建图片到treewidget，listwidget以及创建缩略图如下 add_images->process_images->create_preview
//add_images
//process_images处理路径字符串
//create_preview创建缩略图
void ImageGroup::add_images(QStringList &filenames)
{
    new_images.clear();
    if(filenames.size()==0){return;}
    new_images=filenames;
    process_images(new_images);
    all_images.reserve(all_images.size() + new_images.size());
}
void ImageGroup::process_images(QStringList&filenames)
{
    QFile file(current_path+"/"+"config.txt");

    if(!file.open(QIODevice::Append | QIODevice::ReadWrite | QIODevice::Text)){
        qDebug()<<"ImageGroup::process_images: error to open file";
        return ;
    }
    QStringList invalid_paths;
    QTextStream out(&file);
    for(QString path:filenames){
        QFileInfo file_check(path);
        QString basename=file_check.baseName();
        QString dirname=current_path+"/"+basename;
        if(QDir(dirname).exists()){//筛选已经创建过不能再次创建的
            qDebug()<<"ImageGroup::process_images: this dir has existed :"<<dirname;
            invalid_paths.append(path);
            continue;
        }
        QString suffix=file_check.suffix();
        qDebug()<<"ImageGroup::process_images: "<<dirname;
        if(!QDir().mkdir(dirname)){
            qDebug()<<"ImageGroup::process_images: fail to create dir";
            continue;
        }
        QString new_file_path=dirname+"/"+file_check.fileName();
        all_images.append(new_file_path);//添加图片路径到all_images
        if(!QFile().copy(path,new_file_path)){
            qDebug()<<"ImageGroup::process_images: error to copy file";
        }
        create_preview(path,basename,dirname,suffix);
        out<<"\n"<<new_file_path;
    }
    for(QString path:invalid_paths){
        int index=new_images.indexOf(path);
        if(index==-1){
            qDebug()<<"ImageGroup::process_images: delete invalid_paths error,index==-1";
        }else{
            new_images.removeAt(index);
        }
    }
    file.close();
}

void ImageGroup::create_preview(QString path,QString base_name,QString dir_path,QString suffix)
{
    QtConcurrent::run([=]() {//线程处理
        QImage img(path);
        QSize target_size(125,125);
        qreal scale=qMax(target_size.width()*1.0/img.width(),target_size.height()*1.0/img.height());
        QImage scaled=img.scaled(img.width()*scale,img.height()*scale,Qt::KeepAspectRatio,Qt::SmoothTransformation);
        int x = (scaled.width() - target_size.width()) / 2;
        int y = (scaled.height() - target_size.height()) / 2;
        QImage cropped=scaled.copy(x, y, target_size.width(), target_size.height());
        cropped.save(dir_path + "/" + "_preview.jpg");
        emit (sendImage(dir_path + "/" + base_name + "."+suffix,true));
        qDebug()<<"ImageGroup::create_preview: "<<dir_path + "/" + base_name + ".jpg";
    });
}
void ImageGroup::add_tree_widget()
{
    QStringList tree_paths=new_images;//通过new_images获取
    for(auto path:tree_paths){
        QFileInfo file_check(path);
        QString basename=file_check.baseName();
        QString dirname=current_path+"/"+basename;
        QDir dir=QDir(dirname);
        if(!dir.exists()){
            qDebug()<<"ImageGroup::add_tree_widget :dir no exist";
            continue;
        }
        QStringList dir_files=dir.entryList(QDir::NoDotAndDotDot | QDir::Files);
        emit(sendTreeStruct(current_path,dir_files,basename,false));
    }
}

void ImageGroup::delete_dirs(const QString & path)
{
    qDebug()<<"ImageGroup::delete_dirs exec: "<<path;
        QFile file(path+"/"+"delete_config.txt");
        if(!file.open(QIODevice::ReadWrite|QIODevice::Text)){
            qDebug()<<"ImageGroup::delete_dirs: open file error";
            return ;
        }
        if(!file.exists()){
            qDebug()<<"ImageGroup::delete_dirs: file not exist: "<<path+"/"+"delete_config.txt";
            return ;
        }
        QString line;
        QTextStream in(&file);
        while(in.readLineInto(&line)){
            qDebug()<<"ImageGroup::delete_dirs: "<<line;
            if(!QDir().exists(line)){continue;}
            qDebug()<<"ImageGroup::delete_dirs: contains: "<<line;
            QFileInfo file_check(line);
            QString dir_path=file_check.path();
            if(file_check.isDir()){//由于图片的地址../web2/resource/a/a.png需要.path获取../web2/resource/a,文件夹地址为../web2/resource/a所以不需要.path
                dir_path=line;
            }
            delete_entire_dir(dir_path);
        }
        file.resize(0);
        file.close();
        QFile dir_file(path+"/"+"dir_config.txt");
        if(!dir_file.open(QIODevice::ReadWrite|QIODevice::Text)){
            qDebug()<<"ImageGroup::delete_dirs: open file error";
            return ;
        }
        if(!dir_file.exists()){
            qDebug()<<"ImageGroup::delete_dirs: dir_file not exist: "<<path+"/"+"dir_config.txt";
            return ;
        }
        QString dir_line;
        QTextStream dir_in(&dir_file);
        while(dir_in.readLineInto(&dir_line)){
            qDebug()<<"ImageGroup::delete_dirs: "<<dir_line;
            if(!QDir().exists(dir_line)){continue;}
            qDebug()<<"ImageGroup::delete_dirs: contains: "<<dir_line;
            delete_dirs(dir_line);

        }
        dir_file.close();

}

void ImageGroup::delete_dirs_reflush(const QString &path)
{
        delete_dirs(path);
        emit(reflush());
}



void ImageGroup::delete_images_text(QStringList &paths)//删除config关于被删除图片的记录，添加delete_config被删除的记录
{
    QFile delete_file(current_path+"/"+"delete_config.txt");
    if(!delete_file.exists()){
        qDebug()<<"ImageGroup::delete_images_text: file not exist";
        return ;
    }
    if(!delete_file.open(QIODevice::Append | QIODevice::Text)){
        qDebug()<<"ImageGroup::delete_images_text: open file error";
        return ;
    }
    QTextStream out_1(&delete_file);
    for(auto path:paths){
        if(all_dirs.contains(path)){//说明当前删除的是文件夹
            int index=all_dirs.indexOf(path);
                if(index==-1){
                    qDebug()<<"ImageGroup::delete_images_text: path:"<<path<<" is no found in all_dirs";
                }else{
                    all_dirs.removeAt(index);
                    qDebug()<<"ImageGroup::delete_images_text: success delete dir: "<<path;
                    out_1<<path<<"\n";//将此次删除图片路径写入delete_config
                }
        }else{//当前删除的是图片
            int index=all_images.indexOf(path);
            if(index==-1){
                qDebug()<<"ImageGroup::delete_images_text: path:"<<path<<" is no found in all_images";
            }else{
                all_images.removeAt(index);
                qDebug()<<"ImageGroup::delete_images_text: success delete image: "<<path;
                out_1<<path<<"\n";//将此次删除图片路径写入delete_config
            }
        }

    }
    delete_file.close();
    set_config(current_path+"/"+"config.txt",paths);
    set_config(current_path+"/"+"dir_config.txt",paths);
}

void ImageGroup::delete_entire_dir(QString path)
{
    QDir dir(path);
    if(!dir.exists()){
        qDebug()<<"delete_entire_dir: "<<path<<" no exist";
        return;
    }
    QStringList dir_list=dir.entryList(QDir::Dirs| QDir::NoDotAndDotDot);
    QStringList file_list=dir.entryList(QDir::Files| QDir::NoDotAndDotDot);
    for(auto p:file_list){
        QString file_path=dir.filePath(p);
        QFile delete_file(file_path);
        qDebug()<<"ImageGroup::detele_entire_dir:current delete this file: "<<file_path;
        if (delete_file.remove()) {
            qDebug() << "ImageGroup::detele_entire_dir: Successfully removed file";
        } else {
            qDebug() << "ImageGroup::detele_entire_dir: remove error:" << delete_file.errorString();
            qDebug() << "ImageGroup::detele_entire_dir: Failed to remove file";
        }
        delete_file.close();
    }
    for(QString& p:dir_list){
        QString dir_path=dir.filePath(p);
        qDebug()<<"last crush ::"<<dir_path;
        delete_entire_dir(dir_path);
    }
    if(QDir().rmdir(path)){
        qDebug()<<"ImageGroup::detele_images: remove dir success";

    }else{
        qDebug()<<"ImageGroup::detele_images: remove dir fail";
    }


}
void ImageGroup::set_config(QString path, QStringList &paths)//path为该文件夹的config地址，paths是需要去除的路径
{
    QFile file(path);
    if(!file.exists()){
        qDebug()<<"ImageGroup::set_config: file not exist";
        return ;
    }
    if(!file.open(QIODevice::ReadWrite|QIODevice::Text)){
        qDebug()<<"ImageGroup::set_config: open file error";
        return ;
    }
    QStringList valid_paths;
    QTextStream in(&file);
    QString line;
    while(in.readLineInto(&line)){
        if(!paths.contains(line)){
            valid_paths.push_back(line);
        }else{

        }
    }
    file.resize(0);//清空原先的记录
    QTextStream out_2(&file);
    for(auto path:valid_paths){//config写入不在此次删除活动的记录
        out_2<<path<<"\n";
    }
    file.close();
}

void ImageGroup::set_config_and_replace(QString target_path, QString path, QString origin_path)
{
    QFile file(path);
    if(!file.exists()){
        qDebug()<<"ImageGroup::set_config: file not exist";
        return ;
    }
    if(!file.open(QIODevice::ReadWrite|QIODevice::Text)){
        qDebug()<<"ImageGroup::set_config: open file error";
        return ;
    }
    QStringList valid_paths;
    QTextStream in(&file);
    QString line;
    bool origin_path_exist=false;
    while(in.readLineInto(&line)){
        if(origin_path!=line){
            valid_paths.push_back(line);
        }else{
            origin_path_exist=true;
        }
    }
    file.resize(0);//清空原先的记录
    QTextStream out_2(&file);
    for(auto path:valid_paths){//config写入不在此次删除活动的记录
        out_2<<path<<"\n";
    }
    if(origin_path_exist){
        out_2<<target_path<<"\n";
    }
    file.close();

}

//返回带后缀的文件名字/web2/resource/data/1/1.png->1.png
QString ImageGroup::get_image_name(QString path)
{
    QFileInfo file_check(path);
    QString image_name=file_check.fileName();
    return image_name;
}

QString ImageGroup::get_dir_path(QString path)
{
    QFileInfo file_info(path);
    if(!file_info.exists()){
        qDebug()<<"ImageGroup::get_dir_path: this file path is no exist: "<<path;
        return "";
    }
    qDebug()<<"ImageGroup::get_dir_path: return path: "<<file_info.absolutePath();
    return file_info.path();
}


//初始化
void ImageGroup::init()
{
    delete_dirs(current_path);//启用前删除delete_config中需要删除的文件
    show_list_widget();//初始化图片列表
    show_tree_widget();//初始化树结构,treeWidget与listWidget初始化不放在同一个地方避免混淆


    for(auto path:all_images){
        qDebug()<<"ImageGroup::init: all_images: "<<path;
    }
    qDebug()<<"ImageGroup::init: all_images size: "<<all_images.count();
    for(auto path:all_tree){
        qDebug()<<"ImageGroup::init: all_tree: "<<path;
    }
    qDebug()<<"ImageGroup::init: all_tree size: "<<all_tree.count();
    for(auto path:all_dirs){
        qDebug()<<"ImageGroup::init: all_dirs: "<<path;
    }
    qDebug()<<"ImageGroup::init: all_dirs size: "<<all_dirs.count();
}

void ImageGroup::show_tree_widget()
{
    QDir dir=QDir(current_path);
    if(!dir.exists()){
        qDebug()<<"ImageGroup::show_tree_widget: dir no exist";
        return;
    }

    QStringList dir_list=dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs);
    for(auto path:dir_list){
        qDebug()<<"ImageGroup::show_tree_widget path: "<<path;
        QString full_path=QString(current_path+"/"+path);
        bool dir_tag=is_my_dir(full_path);
        QDir sub_dir=QDir(full_path);
        if(!sub_dir.exists()){
            qDebug()<<"ImageGroup::show_tree_widget: sub_dir no exist";
            continue;
        }
        all_tree.push_back(full_path);
        QStringList sub_dir_list=sub_dir.entryList(QDir::NoDotAndDotDot | QDir::Files);
        emit(sendTreeStruct(current_path,sub_dir_list,path,dir_tag));

    }
}

void ImageGroup::create_folder(QString name)
{

    QString path=current_path+"/"+name;
    qDebug()<<"ImageGroup::create_folder path: "<<path;
    QDir dir(path);
    if(!dir.exists()){
        QFile file(current_path+"/"+"dir_config.txt");
        if(!file.exists()){
            qDebug()<<"ImageGroup::create_folder: dir_config.txt is not exist";
            return;
        }
        if(!file.open(QIODevice::Append | QIODevice::Text)){
            qDebug()<<"ImageGroup::create_folder: open file error";
            return ;
        }
        //将该文件夹写入dir_config
        QTextStream out(&file);
        out<<path<<"\n";
        file.close();
        all_dirs.push_back(path);
        //创建文件夹
        if(!QDir().mkdir(path)){
            qDebug()<<"ImageGroup::create_folder:fail to create dir";
            return;
        }
        //创建三个config文件
        QString config_path=path+"/"+"config.txt";
        QString delete_config_path=path+"/"+"delete_config.txt";
        QString dir_config_path=path+"/"+"dir_config.txt";
        QFile cfg(config_path);
        QFile delete_cfg(delete_config_path);
        QFile dir_cfg(dir_config_path);
        if(!cfg.open(QIODevice::Append | QIODevice::Text)){
            qDebug()<<"ImageGroup::create_folder: open cfg";
            return ;
        }
        if(!delete_cfg.open(QIODevice::Append | QIODevice::Text)){
            qDebug()<<"ImageGroup::create_folder: open delete_cfg";
            return ;
        }
        if(!dir_cfg.open(QIODevice::Append | QIODevice::Text)){
            qDebug()<<"ImageGroup::create_folder: open dir_cfg";
            return ;
        }
        cfg.close();
        delete_cfg.close();
        dir_cfg.close();
        emit(sendImage(path,false));

    }else{
        qDebug()<<"ImageGroup::create_folder: dir "+path+" has existed";//这个名字的文件夹已经存在
        return;
    }
}

bool ImageGroup::is_image(QString path)
{
    return all_images.contains(path)?true:false;
}

bool ImageGroup::is_dir(QString path)
{
    return all_dirs.contains(path);
}

bool ImageGroup::is_my_dir(QString path)
{
    QFileInfo file_check(path+"/"+"config.txt");
    if(!file_check.exists()){
        qDebug()<<"ImageGroup::is_my_dir :"<<path<<" is a image";
        return false;
    }
    qDebug()<<"ImageGroup::is_my_dir :"<<path<<" is a dir";
    return true;

}



void ImageGroup::show_list_widget()
{
    //读取config文件中关于图片的路径信息
    QFile file(current_path+"/"+"config.txt");
    if(!file.exists()){
        qDebug()<<"ImageGroup::show_list_widget: file not exist";
        return ;
    }
    if(!file.open(QIODevice::ReadWrite |  QIODevice::Text)){
        qDebug()<<"ImageGroup::show_list_widget: open file error";
        return ;
    }
    QString line;
    QTextStream in(&file);
    QStringList paths;
    while(in.readLineInto(&line)){
        qDebug()<<"ImageGroup::show_list_widget: "<<line;
        paths.append(line);
    }
    file.close();
    paths.sort();

    //读取dir_config关于文件夹的路径信息
    QFile dir_file(current_path+"/"+"dir_config.txt");
    if(!dir_file.exists()){
        qDebug()<<"ImageGroup::show_list_widget: dir_file not exist";
        return ;
    }
    if(!dir_file.open(QIODevice::ReadWrite |  QIODevice::Text)){
        qDebug()<<"ImageGroup::show_list_widget: open dir_file error";
        return ;
    }
    QStringList dir_paths;
    line="";
    QTextStream in_2(&dir_file);
    while(in_2.readLineInto(&line)){
        qDebug()<<"ImageGroup::show_list_widget: dir "<<line;
        dir_paths.append(line);
    }
    dir_file.close();
    new_images.clear();
    all_images.clear();
    all_dirs.clear();
    //将从config读到的图片路径信息装填进all_images
    for(auto path:paths){
        QFileInfo checkFile(path);
        if(checkFile.isFile()&&checkFile.exists()){
//            QString previewPath_jpg = checkFile.absoluteDir().absolutePath() + "/" + checkFile.completeBaseName() + "_preview.jpg";
//            QString previewPath_gif = checkFile.absoluteDir().absolutePath() + "/" + checkFile.completeBaseName() + "_preview.gif";
            QString previewPath_jpg = checkFile.absoluteDir().absolutePath() + "/" + "_preview.jpg";
            QString previewPath_gif = checkFile.absoluteDir().absolutePath() + "/" + "_preview.gif";
            if (!QFile::exists(previewPath_jpg) && !QFile::exists(previewPath_gif)) {
                    qDebug()<<"ImageGroup::show_list_widget: no preview";
                    create_preview(path, checkFile.baseName(), checkFile.dir().path(),checkFile.suffix());
            }
            new_images.append(path);
            all_images.append(path);
        }
    }
    //将dir_config读到的文件夹路径装填进all_dirs
    for(auto path:dir_paths){
        QFileInfo checkFile(path);
        if(checkFile.isDir()&&checkFile.exists()){
            all_dirs.push_back(path);
        }else{
            qDebug()<<"ImageGroup::show_list_widget: no is dir: "<<path;
        }
    }
    qDebug()<<"ImageGroup::show_list_widget: current all_images size: "<<all_images.count();

}

void ImageGroup::set_current_path(QString new_current_path)
{
    qDebug()<<"ImageGroup::set_current_path: path: "<<new_current_path;
    current_paths.push_back(current_path);
    current_path=new_current_path;
}

void ImageGroup::clear_inf()
{
    all_images.clear();
    all_dirs.clear();
    all_tree.clear();
}


void ImageGroup::change_file_name(const QString& origin_path,const QString& target_path,bool is_dir)
{
    if(!QFileInfo(origin_path).exists()){
        qDebug()<<"ImageGroup::change_file_name: no exist: "<<origin_path;
    }
    if(is_dir){
        QDir dir;
        if(dir.rename(origin_path,target_path)){
            set_config_and_replace(target_path,current_path+"/"+"dir_config.txt",origin_path);//origin_path:  "../web2/resource/datas/data/2"
            set_config_and_replace(target_path,current_path+"/"+"delete_config.txt",origin_path);//target_path:  "../web2/resource/datas/data/3"
        }else{
            qDebug()<<"ImageGroup::change_image_name: dir fail to rename";
            return;
        }
        int index=all_dirs.indexOf(origin_path);
        all_dirs.removeAt(index);
        emit(reflush());
    }else{
        QFileInfo old_file(origin_path);//origin_path:  "../web2/resource/datas/data/2/2.png"
        QString suffix=old_file.suffix();//target_path:  "../web2/resource/datas/data/6"
        QString old_file_name=old_file.baseName();
        QString old_file_path=old_file.path();
        qDebug()<<old_file_name<<" "<<suffix<<" "<<old_file_path;
        QFileInfo new_file(target_path);
        QString new_file_name=new_file.baseName();
        QDir old_dir(old_file_path);
        QStringList sub_files=old_dir.entryList(QDir::Files|QDir::NoDotAndDotDot);
        for(auto path:sub_files){
            if(path==(old_file_name+"."+suffix)){
                qDebug()<<path;
                QDir().rename(old_file_path+"/"+path,old_file_path+"/"+new_file_name+"."+suffix);
            }
        }
        qDebug()<<"ImageGroup::change_file_name: old_file_path: "<<old_file_path;
        qDebug()<<"ImageGroup::change_file_name: target_file_path: "<<target_path;
        QString ab_old_path=QDir(old_file_path).absolutePath();
        QString ab_new_path=QFileInfo(ab_old_path).path()+"/"+new_file_name;
        qDebug()<<ab_new_path<<" "<<ab_old_path;
        if(!QDir(ab_old_path).rename(ab_old_path,ab_new_path)){
            qDebug()<<"ImageGroup::change_file_name: fail to rename dir :"<<QFile().errorString()<<" |||||||||||||||||||||||||||||||||||||||||";
        }else{
            qDebug()<<"yes|||||||||||||||||||||||";
        }
        int index=all_images.indexOf(origin_path);
        all_images.removeAt(index);
        QString new_path=QFileInfo(QFileInfo(origin_path).path()).path()+"/"+new_file_name+"/"+new_file_name+"."+suffix;
        qDebug()<<"ImageGroup::change_file_name: new_path: "<<new_path;
        set_config_and_replace(new_path,current_path+"/"+"config.txt",origin_path);
        set_config_and_replace(new_path,current_path+"/"+"delete_config.txt",origin_path);
        emit(reflush());
    }


}

QString ImageGroup::get_current_path()
{
    return current_path;
}





