/*
 * Copyright (c) Mecontronics Corp.
 * Author:         Xia Weiwen
 * Date:           2017/08/03
 * File Name:      mec_filedialog.cpp
 * Description:    In order to hide system files/directories from users,
 *                 use our own file dialog instead of the system provided dialog.
 * Note:           Most codes of the QG_FileDialog are copied to retain the functions.
*/

#include "mec_filedialog.h"
#include "ui_mec_filedialog.h"
#include <QDesktopWidget>
#include <QApplication>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QFileInfo>
#include <QDebug>

//MEC_FileDialog::MEC_FileDialog(QWidget *parent) :
//    QDialog(parent),
//    ui(new Ui::MEC_FileDialog)
//{
//    ui->setupUi(this);
//    initGUI();
//    setRootDirType(0);
//}

void MEC_FileDialog::getType(const QString filter)
{
    if (filter== fLff) {
        ftype = RS2::FormatLFF;
    } else if (filter == fCxf) {
        ftype = RS2::FormatCXF;
    } else if (filter == fDxfrw2007 || filter == fDxfrw) {
        ftype = RS2::FormatDXFRW;
    } else if (filter == fDxfrw2004) {
        ftype = RS2::FormatDXFRW2004;
    } else if (filter == fDxfrw2000) {
        ftype = RS2::FormatDXFRW2000;
    } else if (filter == fDxfrw14) {
        ftype = RS2::FormatDXFRW14;
    } else if (filter == fDxfrw12) {
        ftype = RS2::FormatDXFRW12;
#ifdef DWGSUPPORT
    } else if (filter == fDwg) {
        ftype = RS2::FormatDWG;
#endif
    } else if (filter == fJww) {
        ftype = RS2::FormatJWW;
    } else if (filter == fDxf1) {
        ftype = RS2::FormatDXF1;
    }
}

MEC_FileDialog::MEC_FileDialog(QWidget* parent, Qt::WindowFlags f, FileType type) :
    QDialog(parent),
    ui(new Ui::MEC_FileDialog)
{
    ui->setupUi(this);
    setWindowFlags(f);
    initGUI();
    setRootDirType(0);
    ftype= RS2::FormatDXFRW;

    fDxfrw2007 = tr("Drawing Exchange DXF 2007 %1").arg("(*.dxf)");
    fDxfrw2004 = tr("Drawing Exchange DXF 2004 %1").arg("(*.dxf)");
    fDxfrw2000 = tr("Drawing Exchange DXF 2000 %1").arg("(*.dxf)");
    fDxfrw14 = tr("Drawing Exchange DXF R14 %1").arg("(*.dxf)");
    fDxfrw12 = tr("Drawing Exchange DXF R12 %1").arg("(*.dxf)");
    fDxfrw = tr("Drawing Exchange %1").arg("(*.dxf)");

    fLff = tr("LFF Font %1").arg("(*.lff)");
#ifdef DWGSUPPORT
    fDwg = tr("dwg Drawing %1").arg("(*.dwg)");
#endif
    fCxf = tr("QCad Font %1").arg("(*.cxf)");
    fJww = tr("Jww Drawing %1").arg("(*.jww)");
    fDxf1 = tr("QCad 1.x file %1").arg("(*.dxf)");
    switch(type){
    case BlockFile:
        name=tr("Block", "block file");
        break;
    default:
        name=tr("Drawing", "drawing file");
    }
}

void MEC_FileDialog::setRootDirType(int type)
{
    if (type == 0)
    {
        if (QDir(QDir::homePath() + "/drill/testdata").exists())
        {
            rootDir = QDir::homePath() + "/drill/testdata/";
        }
        else if (QDir(QDir::homePath() + "/DRILL-mec/testdata").exists())
        {
            rootDir = QDir::homePath() + "/DRILL-mec/testdata/";
        }
        else if (QDir(QDir::homePath() + "/Documents/drill/drill/testdata").exists())
        {
            rootDir = QDir::homePath() + "/Documents/drill/drill/testdata/";
        }
        else if (QDir(QDir::homePath() + "/SAW-mec/testdata/").exists())
        {
            rootDir = QDir::homePath() + "/SAW-mec/testdata/";
        }
        else if (QDir(QDir::homePath() + "/SAW-mec/user_files/").exists())
        {
            rootDir = QDir::homePath() + "/SAW-mec/user_files/";
        }
        else if (QDir(QDir::homePath() + "/saw/testdata/").exists())
        {
            rootDir = QDir::homePath() + "/saw/testdata/";
        }
        else if (QDir(QDir::homePath() + "/saw/user_files/").exists())
        {
            rootDir = QDir::homePath() + "/saw/user_files/";
        }
        else
        {
            rootDir = QDir::homePath();
        }
    }
    else if (type == 1)
    {
        rootDir = QDir::homePath().replace("home", "media").append("/");
    }
}

void MEC_FileDialog::initGUI()
{
    // window
    setWindowFlags(Qt::WindowStaysOnTopHint |
                   Qt::Dialog);


    // Others
    ui->filesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->filesTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->filesTable->verticalHeader()->hide();
    ui->filesTable->setShowGrid(false);
    ui->filesTable->setColumnWidth(0, 320);
    ui->filesTable->setColumnWidth(1, 200);
    connect(ui->filesTable, SIGNAL(cellActivated(int,int)),
            this, SLOT(fileDoubleClicked(int,int)));
    connect(ui->filesTable, SIGNAL(cellClicked(int,int)),
            this, SLOT(fileClicked(int,int)));
    ui->btnOpenFolder->hide();
    ui->editFileName->setValidator(new QRegExpValidator(
                    QRegExp("[^/\\\\^,\\*\"\\[\\]\\:;\\|\\=]*")));
    move((qApp->desktop()->width() - width()) / 2,
         (qApp->desktop()->height() - height()) / 2);
    ui->btnRefresh->setFocus();
}

QString MEC_FileDialog::getExtension (RS2::FormatType type) const{
    switch (type) {
    case RS2::FormatLFF:
        return QString(".lff");
    case RS2::FormatJWW:
        return QString(".jww");
    case RS2::FormatCXF:
        return QString(".cxf");
#ifdef DWGSUPPORT
    case RS2::FormatDWG:
        return QString(".dwg");
#endif
    default:
        return QString(".dxf");
    }
}

QString MEC_FileDialog::getOpenFile(RS2::FormatType* type)
{
    Q_UNUSED(type)
    setAcceptMode(OpenFile);
    QString fn = "";
    QStringList filters;
    filters << "Drawing Exchange DXF (*.dxf *.DXF *.Dxf)"
            << "dwg Drawing (*.dwg *.DWG *.Dwg)";
//#ifdef DWGSUPPORT
//    filters << fDxfrw  << fDxf1 << fDwg << fLff << fCxf << fJww;
//#else
//    filters << fDxfrw  << fDxf1 << fLff << fCxf << fJww;
//#endif

    setWindowTitle(tr("Open %1").arg(name));
    setNameFilters(filters);
    ftype= RS2::FormatDXFRW;

    if (exec() == QDialog::Accepted)
    {
        QString fl = selectedFile();
        if (!fl.isEmpty())
        {
            fn = fl;
        }
        fn = QDir::toNativeSeparators( QFileInfo(fn).absoluteFilePath() );
        if (type)
        {
            QString filter = selectedNameFilter();
            if (filter.contains("dxf", Qt::CaseInsensitive))
            {
                getType(fDxfrw);
            }
            else if (filter.contains("dwg", Qt::CaseInsensitive))
            {
                getType(fDwg);
            }
            *type = ftype;
        }
    }

    return fn;
}

QString MEC_FileDialog::getSaveFile(RS2::FormatType* type)
{
    Q_UNUSED(type)
    // initialize dialog properties
    setWindowTitle(tr("Save %1 As").arg(name));
    setViewMode(All);
    setAcceptMode(SaveFile);
    // setup filters
    QStringList filters;
    filters << "Drawing Exchange DXF (*.dxf *.DXF *.Dxf)";
    setNameFilters(filters);

//#ifdef JWW_WRITE_SUPPORT
//    filters << fDxfrw2007 << fDxfrw2004 << fDxfrw2000 << fDxfrw14 << fDxfrw12 << fJww << fLff << fCxf;
//#else
//    filters << fDxfrw2007 << fDxfrw2004 << fDxfrw2000 << fDxfrw14 << fDxfrw12 << fLff << fCxf;
//#endif

    ftype = RS2::FormatDXFRW;

    // when defFilter is added the below should use the default extension.
    // generate an untitled name
    QString fn = tr("Untitled");
    qDebug() << "Save File: (default)" << rootDir + fn + getExtension( ftype );
    if(QFile::exists( rootDir + fn + getExtension( ftype ) ))
    {
        int fileCount = 1;
        while(QFile::exists( rootDir + fn + QString("%1").arg(fileCount)
                             + getExtension(ftype)) ||
              QFile::exists( rootDir + fn + QString("%1").arg(fileCount)
                             + getExtension(ftype).toUpper()))
            ++fileCount;
        fn += QString("%1").arg(fileCount);
    }
    setSelectedFile(fn);

    // only return non empty string when we have a complete, user approved, file name.
    if (exec()!=QDialog::Accepted)
        return QString("");

    QString fl = selectedFile();
    if (fl.isEmpty())
        return QString("");

    QFileInfo fi = QFileInfo( fl );
    fn = QDir::toNativeSeparators( fi.absoluteFilePath() );

    if (type)
    {
        QString filter = selectedNameFilter();
        if (filter.contains("dxf", Qt::CaseInsensitive))
        {
            getType(fDxfrw);
        }
        else if (filter.contains("dwg", Qt::CaseInsensitive))
        {
            getType(fDwg);
        }
        *type = ftype;
    }

    // append default extension:
    if (!fi.fileName().endsWith(".dxf", Qt::CaseInsensitive))
        fn += getExtension(ftype);

    return fn;
}

void MEC_FileDialog::setTipText(QString tip)
{
    ui->lblLookIn->setText(tip);
}

void MEC_FileDialog::setRootDir(QString root)
{
    if (root.endsWith("/"))
        rootDir = root;
    else
        rootDir = root.append("/");
}

void MEC_FileDialog::setViewMode(ViewMode mode)
{
    vMode = mode;
}

void MEC_FileDialog::setAcceptMode(AcceptMode mode)
{
    aMode = mode;
}

void MEC_FileDialog::setNameFilters(QStringList filters)
{
    nameFilters = filters;
    if (filters.isEmpty())
        nameFilters = QStringList() << "*.*";
//    ui->cbbFileType->setCurrentIndex(-1);
//    for (int i = ui->cbbFileType->count(); i > 0; --i)
//        ui->cbbFileType->removeItem(0);
//    foreach (QString filter, nameFilters)
//    {
//        ui->cbbFileType->addItem(filter);
//    }
//    ui->cbbFileType->setCurrentIndex(0);
}

void MEC_FileDialog::setSelectedFile(QString filename)
{
    filename.remove(QRegExp(".*/"));
//    filename.replace(QRegExp("/{2,}"), "/");
    selectedFilename = filename;
    ui->editFileName->setText(filename);
}

QString MEC_FileDialog::selectedFile()
{
    return selectedFilename;
}

QString MEC_FileDialog::selectedNameFilter()
{
    return ui->cbbFileType->currentText();
}

int MEC_FileDialog::exec()
{
//    qDebug() << "Root dir:" << rootDir;
    currentDir_s = rootDir;
    currentDir = QDir(currentDir_s);
    ui->cbbFileType->addItems(nameFilters);
    ui->cbbFileType->setCurrentIndex(0);
    ui->filesTable->setRowCount(0);
    listAllEntries();

    return QDialog::exec();
}

void MEC_FileDialog::showDirs(QStringList dirs) // 显示文件夹
{
    QIcon icon;
    if (currentDir_s == QDir::homePath().replace("home", "media").append("/"))
        icon = QIcon(":/ui/usb_48px.png");
    else
        icon = QIcon(":/ui/folder.png");
    if (currentDir_s != rootDir)
    {
        QTableWidgetItem *DirNameItem = new QTableWidgetItem(tr("返回"));
        DirNameItem->setFlags(DirNameItem->flags() ^ Qt::ItemIsEditable);
        DirNameItem->setIcon(QIcon(":/ui/file_backward.png"));
        int row = ui->filesTable->rowCount();
        ui->filesTable->insertRow(row);
        ui->filesTable->setRowHeight(row, 40);
        ui->filesTable->setItem(row, 0, DirNameItem);
        ui->filesTable->setItem(row, 1, new QTableWidgetItem);
    }
    for (int i = 0; i < dirs.size(); ++i)
    {
        QTableWidgetItem *DirNameItem = new QTableWidgetItem(dirs.at(i));
        DirNameItem->setIcon(icon);
        DirNameItem->setFlags(DirNameItem->flags() ^ Qt::ItemIsEditable);
        QTableWidgetItem *sizeItem = new QTableWidgetItem("----");
        sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        sizeItem->setFlags(sizeItem->flags() ^ Qt::ItemIsEditable);
        int row = ui->filesTable->rowCount();
        ui->filesTable->insertRow(row);
        ui->filesTable->setItem(row, 0, DirNameItem);
        ui->filesTable->setItem(row, 1, sizeItem);
    }
    ui->filesTable->scrollToTop();
}

void MEC_FileDialog::showFiles(QStringList files)
{
    ui->filesTable->setIconSize(QSize(32, 32));
    for (int i = 0; i < files.size(); ++i) {
        QFile file(currentDir.absoluteFilePath(files[i]));
        qint64 size = QFileInfo(file).size();

        QTableWidgetItem *fileNameItem = new QTableWidgetItem(files[i]);
        fileNameItem->setIcon(QIcon(":/ui/file.png"));
        fileNameItem->setFlags(fileNameItem->flags() ^ Qt::ItemIsEditable);
        QString sizeStr;
        if (size < 1024) // < 1K
        {
            sizeStr = tr("%1 B").arg(size);
        }
        else if (size < 1024 * 1024) // < 1M
        {
            sizeStr = tr("%1 KB").arg(size / 1024.0, 0, 'f', 1);
        }
        else if (size < 1024 * 1024 * 1024) // < 1G
        {
            sizeStr = tr("%1 MB").arg(size / (1024.0 * 1024.0), 0, 'f', 1);
        }
        else // > 1G
        {
            sizeStr = tr("%1 GB").arg(size / (1024.0 * 1024.0 * 1024.0), 0, 'f', 1);
        }
        QTableWidgetItem *sizeItem = new QTableWidgetItem(sizeStr);
        sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        sizeItem->setFlags(sizeItem->flags() ^ Qt::ItemIsEditable);

        int row = ui->filesTable->rowCount();
        ui->filesTable->insertRow(row);
        ui->filesTable->setItem(row, 0, fileNameItem);
        ui->filesTable->setItem(row, 1, sizeItem);
    }
    ui->filesTable->scrollToTop();
}

void MEC_FileDialog::listAllEntries()
{
    QStringList files, dirs;
    if (vMode ^ Dirs)
    {
        dirs = currentDir.entryList(QStringList(),
                                QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
        showDirs(dirs);
    }
    if (vMode ^ Files)
    {
        QStringList filters;
        if (nameFilters.size() > 0)
        {
            QString filter = ui->cbbFileType->currentText();
            filter.remove(QRegExp(".*\\(")).remove(QRegExp("\\).*"));
            if (filter.contains(" "))
            {
                filters << filter.split(" ");
            }
            else
            {
                filters << filter;
            }
        }
        else
            filters << "All(*.*)";
        files = currentDir.entryList(filters, QDir::Files);
        showFiles(files);
    }
}

void MEC_FileDialog::on_btnCancel_clicked()
{
    reject();
}

void MEC_FileDialog::on_btnOk_clicked()
{
    QString filename = ui->editFileName->text();
    // 检测文件重名和空文件名
    if (aMode == SaveFile)
    {
        QString suffix = ui->cbbFileType->currentText();
        suffix.remove(QRegExp(".*\\(")).remove(QRegExp("\\).*")).remove(" .*");
        QStringList filters;
        if (suffix.contains(" "))
        {
            filters << suffix.split(" ");
        }
        else
        {
            filters << suffix;
        }
        suffix = filters.at(0);
        suffix.remove("*");
        if (!filename.endsWith(suffix, Qt::CaseInsensitive))
            filename.append(suffix);
        if (QFile(currentDir_s + filename).exists())
        {
            QMessageBox qmsg(QMessageBox::Warning, tr("文件已存在"),
                             tr("同名文件已经存在，是否覆盖？"), QMessageBox::NoButton,
                             this, /*Qt::FramelessWindowHint |*/ Qt::WindowStaysOnTopHint
                             | Qt::Dialog);
            QPushButton* btnNo = qmsg.addButton(tr("取消"), QMessageBox::NoRole);
            qmsg.addButton(tr("覆盖"), QMessageBox::YesRole);
            qmsg.exec();
            if (qmsg.clickedButton() == btnNo)
                return;
        }
        if (filename.isEmpty() || filename.startsWith("."))
        {
            QMessageBox qmsg(QMessageBox::Warning, tr("无效文件名"),
                             tr("文件名不能为空，也不能以特殊字符开头。"),
                             QMessageBox::NoButton, this,
                             /*Qt::FramelessWindowHint |*/ Qt::WindowStaysOnTopHint
                             | Qt::Dialog);
            qmsg.addButton(tr("好的"), QMessageBox::YesRole);
            qmsg.exec();
            return;
        }
    }
    else if (aMode == OpenFile)
    {
        if (filename.isEmpty()) return;
    }
    selectedFilename = currentDir_s + filename;
    if (aMode == GetDir)
    {
        int row = ui->filesTable->currentRow();
        if (row < 0)
            selectedFilename = currentDir_s;
        else
            selectedFilename = currentDir_s
                    + ui->filesTable->item(row, 0)->text();
    }
    accept();
}

void MEC_FileDialog::on_btnRefresh_clicked()
{
    ui->filesTable->setRowCount(0);
    listAllEntries();
}

void MEC_FileDialog::fileDoubleClicked(int row, int)
{
    if (ui->filesTable->item(row, 1)->text() == "----") // subfolder
    {
        currentDir_s.append(ui->filesTable->item(row, 0)->text()).append("/");
        currentDir = QDir(currentDir_s);
        ui->filesTable->setRowCount(0);
        listAllEntries();
        showCurDirName();
        ui->btnOpenFolder->hide();
    }
    else
    {
        on_btnOk_clicked();
    }
}

void MEC_FileDialog::fileClicked(int row, int)
{
    if (ui->filesTable->item(row, 0)->text() == "返回")
    {
        QStringList tree = currentDir_s.split("/");
        tree.removeAll(""); // remove blank
        tree.removeLast(); // remove subfolder
        currentDir_s.clear();
        currentDir_s.append("/");
        foreach (QString folder, tree)
        {
            currentDir_s.append(folder).append("/");
        }
        currentDir = QDir(currentDir_s);
        ui->filesTable->setRowCount(0);
        listAllEntries();
        showCurDirName();
        return;
    }
    if (ui->filesTable->item(row, 1)->text() == "----") // subfolder
    {
        ui->btnOpenFolder->show();
    }
    else
    {
        ui->editFileName->setText(ui->filesTable->item(row, 0)->text());
        ui->btnOpenFolder->hide();
    }
}

void MEC_FileDialog::on_btnOpenFolder_clicked()
{
    int row = ui->filesTable->currentRow();
    if (row < 0)
        return;
    if (ui->filesTable->item(row, 1)->text() != "----")
        return;
    fileDoubleClicked(row, 0);
}

void MEC_FileDialog::on_cbbFileType_currentIndexChanged(const QString &arg1)
{
    Q_UNUSED(arg1)
    on_btnRefresh_clicked();
}

void MEC_FileDialog::showCurDirName()
{
    QString dire = currentDir_s;
    dire.remove(rootDir);
    if (dire.size() > 40)
    {
        dire.remove(0, dire.size() - 40);
        dire.prepend("...");
    }
    ui->lblDirectory->setText(dire);
}

void MEC_FileDialog::on_rbtnLocalDir_clicked(bool checked)
{
    if (checked)
    {
        setRootDirType(0);
        currentDir_s = rootDir;
        currentDir = QDir(currentDir_s);
        on_btnRefresh_clicked();
    }
}

void MEC_FileDialog::on_rbtnUSBStorage_clicked(bool checked)
{
    if (checked)
    {
        setRootDirType(1);
        currentDir_s = rootDir;
        currentDir = QDir(currentDir_s);
        on_btnRefresh_clicked();
    }
}


/**
 * Shows a dialog for choosing a file name. Saving the file is up to
 * the caller.
 *
 * @param type Will contain the file type that was chosen by the user.
 *             Can be NULL to be ignored.
 *
 * @return File name with path and extension to determine the file type
 *         or an empty string if the dialog was cancelled.
 */
QString MEC_FileDialog::getSaveFileName(QWidget* parent, RS2::FormatType* type)
{
    MEC_FileDialog dlg(parent, 0);
    return dlg.getSaveFile(type);
}

/**
 * Shows a dialog for choosing a file name. Opening the file is up to
 * the caller.
 *
 * @return File name with path and extension to determine the file type
 *         or an empty string if the dialog was cancelled.
 */
QString MEC_FileDialog::getOpenFileName(QWidget* parent, RS2::FormatType* type)
{
    MEC_FileDialog dlg(parent, 0);
    return dlg.getOpenFile(type);
}

// EOF
