#ifndef NIERAUTOMATAMEDIAUTIL_H
#define NIERAUTOMATAMEDIAUTIL_H

#include <QWidget>
#include <QGridLayout>
#include <QBoxLayout>
#include <QFormLayout>
#include <QFileSystemModel>
#include <QTreeView>
#include <QAction>
#include <QMenuBar>
#include <QFileDialog>
#include <QLabel>
#include <QFileInfo>
#include <QPushButton>
#include <QLineEdit>
#include <QProcess>
#include <QCoreApplication>
#include <QDebug>
#include <QMessageBox>
#include <QTextEdit>
#include <QScrollBar>

#include "namtreeview.h"

class NieRAutomataMediaUtil : public QWidget{
    Q_OBJECT

public:
    NieRAutomataMediaUtil(QWidget *parent = 0);
    ~NieRAutomataMediaUtil();

private:
    QWidget *window;
    QBoxLayout *outer;
    QGridLayout *layout;
    QFileSystemModel *filemodel;
    NAMTreeView *fileview;
    QWidget *right;
    QFormLayout *formRight;

    QMenu *fileMenu;
    QMenuBar *menuBar;
    QAction *openFolderAct;
    QAction *exitProgram;
    QString dir;

    QLabel *sFile;
    QLabel *sFileType;
    QLabel *desc;

    QFileInfo *info;
    QPushButton *convertBtn;
    QPushButton *destinationSelect;
    QString outputDir;
    QLineEdit *outputPath;
    QLineEdit *outputFileName;
    QLabel *fileNameHint;
    QLineEdit *outputBitrate;
    QLineEdit *extraFfmpegFlags;
    QProcess *ffmpeg;
    QDialog *ffmpeg_out;
    QFormLayout *ffmpeg_out_layout;
    QTextEdit *ffmpeg_stdout;
    QString ffmpeg_stdoutstr;
    QPushButton *saveToLog;
    QPushButton *closeStdout;

    void init_usm();

private slots:
    void open();
    void fileSelect(const QModelIndex &index);
    void proc_output_destination();
    void convert_ffmpeg();
    //void ffmpeg_started();
    void ffmpeg_readyOut();
    void ffmpeg_done();
};

#endif // NIERAUTOMATAMEDIAUTIL_H
