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
#include <QSettings>

#include "namtreeview.h"
#include "sidewidget_empty.h"

class NieRAutomataMediaUtil : public QWidget{
    Q_OBJECT

public:
    NieRAutomataMediaUtil(QWidget *parent = 0);
    ~NieRAutomataMediaUtil();

    QString optsGamePath;
    QString optsOutputName;
    QString optsFFmpegFlags;
    QString optsDefaultOutputDirectory;

private:
    QBoxLayout *outer;
    QFileSystemModel *filemodel;
    NAMTreeView *fileview;

    QMenu *fileMenu;

    QMenuBar *menuBar;

    QAction *openFolderAct;
    QAction *exitProgram;
    QAction *optionsDialogAct;

    QFileInfo *info;

    QDialog *optionsDialog;

    QLineEdit *optionsDefaultGamePath;
    QLineEdit *optionsDefaultOutputName;
    QLineEdit *optionsDefaultFFmpegFlags;
    QLineEdit *optionsDefaultOutputDirectory;

    void reset_outer();

private slots:
    void open();
    void options();
    void fileSelect(const QModelIndex &index);
    void selectDefaultGamePathSlot();
    void selectDefaultOutputDirectorySlot();
    void optionsSaveAndCloseSlot();

};

#endif // NIERAUTOMATAMEDIAUTIL_H
