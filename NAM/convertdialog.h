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

class ConvertDialog : public QDialog{
    Q_OBJECT
signals:
    void startConversionSignal(QStringList commands, QStringList outputInfo);

public:
    ConvertDialog(int *type, SideWidget_WSP *parent);
    ~ConvertDialog();

    SideWidget_WSP *initiator;


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
