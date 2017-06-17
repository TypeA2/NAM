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
#include <QProgressBar>

#include "sidewidget_wsp.h"

class NAMConversion : public QDialog{
    Q_OBJECT

public:
    NAMConversion(QList<QByteArray> &currentFileBlobs, QComboBox *&currentFileContents, QStringList &commands, QStringList &outputInfo, bool m);
    ~NAMConversion();

    void start();

private:
    QList<QByteArray> currentFileBlobs;
    QComboBox *currentFileContents;

    SideWidget_WSP *initiator;

    QGridLayout *dialogControls;

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

    QProgressBar *multiConversionProgressBar;

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
