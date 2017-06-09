#ifndef NAMCONVERSION_H
#define NAMCONVERSION_H

#include <QDialog>
#include <QTextEdit>
#include <QProcess>
#include <QGridLayout>
#include <QDesktopServices>
#include <QScrollBar>
#include <QFileDialog>
#include <QDateTime>
#include <QMessageBox>
#include <QCoreApplication>

#include "sidewidget_wsp.h"

class NAMConversion : public QDialog{
    Q_OBJECT

public:
    NAMConversion(SideWidget_WSP *parent, QStringList &commands, QStringList &outputInfo, bool m = false);
    ~NAMConversion();

    void start();

private:
    SideWidget_WSP *initiator;
    QStringList commandList;
    QStringList outputFileInfo;
    QStringList commandQueue;

    QProcess *firstConversion;
    QProcess *secondConversion;

    QList<QProcess*> multiConversions;

    bool isSecondProcess;
    bool multi;

    int currentMultiIndex;

    QString outputString;

    QTextEdit *output;

    QPushButton *openDestinationFile;
    QPushButton *openDestinationDirectory;
    QPushButton *killCurrentProcess;
    QPushButton *saveOutputLogToFile;
    QPushButton *closeOutputDialog;

private slots:
    void openDestinationFileSlot();
    void openDestinationDirectorySlot();
    void killCurrentProcessSlot();
    void saveOutputLogToFileSlot();
    void closeOutputDialogSlot();
    void conversionReadyReadStandardOutputSlot();
    void conversionFinishedSlot();
    void multiConversionsPartFinishedSlot();
};
#endif // NAMCONVERSION_H
