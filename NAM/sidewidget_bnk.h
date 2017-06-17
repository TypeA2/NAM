#ifndef SIDEWIDGET_BNK_H
#define SIDEWIDGET_BNK_H

#include <QTreeWidget>

#include "nierautomatamediautil.h"
#include "namconversion.h"
#include "convertdialog.h"

class SideWidget_BNK : public QWidget{
    Q_OBJECT
public:
    enum SectionFlag {
        BKHD = 0x0,
        DIDX = 0x1,
        HIRC = 0x2
    };

    explicit SideWidget_BNK(QFileInfo *fileInfo, NieRAutomataMediaUtil *parent);
    ~SideWidget_BNK();

    QFileInfo *info;

    NieRAutomataMediaUtil *initiator;

    QFormLayout *form;

    QComboBox *fileContents;

    static quint32 getBnkFileCount(QByteArray blob);
    static QString int2hex(quint32 num);
    static QString int2hex(quint8 num);
private:
    QByteArray fileBlob;

    quint32 pointer;

    QTreeWidgetItem *makeTopLevelItem(QTreeWidget *tree, SectionFlag section);

    QList<QTreeWidgetItem*> makeTreeChildren(QTreeWidgetItem *parent, SectionFlag section);

    QTreeWidgetItem *makeTreeChild(QTreeWidgetItem *parent, SectionFlag section, int index);

    QTreeWidgetItem *makeObjectTree(QTreeWidgetItem *parent);

    quint32 getSectionIndex(SectionFlag section, int index);

    quint8 getByte(int index);

    quint32 getInt(quint32 index);

private slots:
    void convertButtonSlot();
    void infoButtonSlot(bool init = false);
    void startConversion(QStringList commands, QStringList outputInfo);

};

#endif // SIDEWIDGET_BNK_H
