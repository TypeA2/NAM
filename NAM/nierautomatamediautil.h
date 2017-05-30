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
#include <QDesktopServices>
#include <QTextStream>
#include <QDateTime>
#include <QComboBox>

#include "namtreeview.h"

class NieRAutomataMediaUtil : public QWidget{
    Q_OBJECT

public:
    NieRAutomataMediaUtil(QWidget *parent = 0);
    ~NieRAutomataMediaUtil();

private:
    QWidget *dekaasvariabel;
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
    QGridLayout *ffmpeg_out_controls;
    QPushButton *openOutFile;
    QPushButton *openOutDir;
    QPushButton *saveLog;
    QPushButton *closeStdout;
    QString outfile;

    QString outputPathPersistent;
    QString outputFileNamePersistent;
    QString outputBitratePersistent;
    QString extraFfmpegFlagsPersistent;

    QComboBox *convertOrPlay;
    QPushButton *usmPlayFFplay;
    QProcess *ffplay;
    QLineEdit *extraFfplayFlags;
    QString extraFfplayFlagsPersistent;

    void init_usm();
    void usm_convert();
    void usm_playback();

private slots:
    void open();
    void fileSelect(const QModelIndex &index);
    void proc_output_destination();
    void convert_ffmpeg();
    void ffmpeg_started();
    void ffmpeg_readyOut();
    void ffmpeg_done();
    void open_out_file();
    void open_out_dir();
    void save_ffmpeg_log();
    void close_stdout_widget();
    void output_path_changed(const QString &text);
    void output_bitrate_changed(const QString &text);
    void extra_ffmpeg_flags_changed(const QString &text);
    void convert_or_play_changed(int index);
    void play_with_ffplay();
    void ffplay_started();
    void ffplay_done();
    void extra_ffplay_flags_changed(const QString &text);
};

#endif // NIERAUTOMATAMEDIAUTIL_H
