#ifndef SideWidget_WSP_H
#define SideWidget_WSP_H

#include <QWidget>
#include <QFileInfo>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QDebug>
#include <QFile>
#include <QComboBox>

#include "nierautomatamediautil.h"

class SideWidget_WSP : public QWidget{
    Q_OBJECT

public:
    explicit SideWidget_WSP(QFileInfo *fileInfo, NieRAutomataMediaUtil *parent);
    ~SideWidget_WSP();

    QFileInfo *info;

    QFormLayout *form;

    QComboBox *currentFileContents;

    QList<QByteArray> currentFileBlobs;

private:
    NieRAutomataMediaUtil *initiator;

    QPushButton *scanFileButton;
    QByteArray currentFileBlob;

private slots:
    void convertCurrentSelection();
    void startConversion(QStringList commands, QStringList outputInfo);
};

#endif // SideWidget_WSP_H
