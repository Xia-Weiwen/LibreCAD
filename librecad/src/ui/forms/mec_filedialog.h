/*
 * Copyright (c) Mecontronics Corp.
 * Author:         Xia Weiwen
 * Date:           2017/08/03
 * File Name:      mec_filedialog.h
 * Description:    In order to hide system files/directories from users,
 *                 use our own file dialog instead of the system provided dialog.
 * Note:           Most codes of the QG_FileDialog are copied to retain the functions.
*/

#ifndef MEC_FILEDIALOG_H
#define MEC_FILEDIALOG_H

#include <QDialog>
#include <QDir>
#include "rs.h"

namespace Ui {
class MEC_FileDialog;
}

class MEC_FileDialog : public QDialog
{
    Q_OBJECT
public:
    enum ViewMode
    {
        Files = 0x1,
        Dirs = 0x10,
        All = 0x11,
    };
    enum AcceptMode
    {
        OpenFile,
        SaveFile,
        GetDir,
    };
    enum FileType{
        DrawingFile=0,
        BlockFile=1
    };
    /**
*@ FileType, used to set dialog window title, currently, should be either "drawing" or "block"
**/
//    MEC_FileDialog(QWidget *parent = 0);
    MEC_FileDialog(QWidget *parent=0, Qt::WindowFlags f=0, FileType type = DrawingFile);

    QString getOpenFile(RS2::FormatType* type=nullptr);
    QString getSaveFile(RS2::FormatType* type=nullptr);
    void setTipText(QString tip);
    void setRootDir(QString root);
    void setViewMode(ViewMode mode);
    void setAcceptMode(AcceptMode mode);
    void setNameFilters(QStringList filters);
    void setSelectedFile(QString filename);
    QString selectedFile();
    QString selectedNameFilter();

    int exec();

    static QString getOpenFileName(QWidget* parent, RS2::FormatType* type=nullptr);
    static QString getSaveFileName(QWidget* parent, RS2::FormatType* type=nullptr);

private:
    Ui::MEC_FileDialog *ui;
    void getType(const QString filter);
    QString getExtension (RS2::FormatType type) const;
    RS2::FormatType ftype;
    QString fDxfrw2007;
    QString fDxfrw2004;
    QString fDxfrw2000;
    QString fDxfrw14;
    QString fDxfrw12;
    QString fDxfrw;
#ifdef DWGSUPPORT
    QString fDwg;
#endif
    QString fDxf1;
    QString fLff;
    QString fCxf;
    QString fJww;
    QString name;

    QDir currentDir;
    QString currentDir_s;
    QString rootDir;
    ViewMode vMode;
    AcceptMode aMode;
    QStringList nameFilters;
    QString selectedFilename;

    void initGUI();
    void setRootDirType(int type); // 0 local, 1 usb
    void showDirs(QStringList dirs);
    void showFiles(QStringList files);
    void listAllEntries();
    void showCurDirName();

private slots:
    void on_btnCancel_clicked();

    void on_btnOk_clicked();

    void on_btnRefresh_clicked();

    void fileDoubleClicked(int row, int);

    void fileClicked(int row, int);

    void on_btnOpenFolder_clicked();

    void on_cbbFileType_currentIndexChanged(const QString &arg1);
    void on_rbtnLocalDir_clicked(bool checked);
    void on_rbtnUSBStorage_clicked(bool checked);
};

#endif // MEC_FILEDIALOG_H
