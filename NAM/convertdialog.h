#ifndef CONVERTDIALOG_H
#define CONVERTDIALOG_H

#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QCoreApplication>

#include "sidewidget_wsp.h"
#include "sidewidget_bnk.h"

class ConvertDialog : public QDialog{
    Q_OBJECT
signals:
    void startConversionSignal(QStringList commands, QStringList outputInfo);

public:
    ConvertDialog(QComboBox *currentFileContents, NieRAutomataMediaUtil *initiator, QFileInfo *info);
    ~ConvertDialog();

    QComboBox *currentFileContents;
    NieRAutomataMediaUtil *initiator;
    QFileInfo *info;


private:
    QLineEdit *destinationDirectory;
    QLineEdit *destinationFilename;
    QLineEdit *ffmpegFlags;

    void layoutAudioWwise();

private slots:
    void destinationDirectorySelected();
    void startConversion();
    void closeConvertDialog();
};

#endif // CONVERTDIALOG_H
