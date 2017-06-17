#include "sidewidget_bnk.h"

SideWidget_BNK::SideWidget_BNK(QFileInfo *fileInfo, NieRAutomataMediaUtil *parent) : QWidget(){
    this->info = fileInfo;
    this->initiator = parent;

    form = new QFormLayout;

    QFile file(info->filePath());

    if(file.open(QIODevice::ReadOnly)){
        fileBlob = file.read(file.size());

        file.close();
    }

    if(!fileBlob.startsWith("BKHD")){
        while(form->rowCount() > 3) form->removeRow(3);

        form->addRow("", new QLabel("<invalid file format>"));

        this->setLayout(form);

        return;
    }

    if(fileBlob.indexOf("STMG") > 0){
        qDebug() << "Init.bnk";
    }else if(fileBlob.indexOf("DIDX") > 0){
        quint32 DIDX_length = SideWidget_BNK::getBnkFileCount(fileBlob);

        fileContents = new QComboBox;

        fileContents->addItem("<all>");

        fileContents->setMaximumWidth(216);

        for(unsigned int i = 0; i < DIDX_length / 12; i++){
            fileContents->addItem(this->initiator->optsOutputName.arg(QString::number(i), this->info->baseName(), "wem"));
        }

        QHBoxLayout *actions = new QHBoxLayout;

        QPushButton *convertButton = new QPushButton("Convert");
        QPushButton *infoButton = new QPushButton("File info (WIP)");

        connect(convertButton, SIGNAL(released()), this, SLOT(convertButtonSlot()));
        connect(infoButton, SIGNAL(released()), this, SLOT(infoButtonSlot()));

        actions->addWidget(convertButton);
        actions->addWidget(infoButton);

        form->addRow("Filename:", new QLabel(this->info->fileName()));
        form->addRow("File type:", new QLabel(this->info->suffix()));
        form->addRow("File description:", new QLabel("Wwise SoundBank, containing .wem files and additional information."));
        form->addRow("File contents:", fileContents);
        form->addRow("", actions);

    }else{
        while(form->rowCount() > 3) form->removeRow(3);

        form->addRow("", new QLabel("<invalid file format>"));
    }

    this->setLayout(form);
}

void SideWidget_BNK::infoButtonSlot(bool init){
    if(!init){
        QDialog *infoDialog = new QDialog(this->initiator);

        QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom);

        QTreeWidget *tree = new QTreeWidget;

        tree->setColumnCount(4);
        tree->setHeaderLabels(QStringList()  << "Value" << "Hex" << "Type" << "Description");
        tree->setColumnWidth(0, 256);
        tree->setColumnWidth(1, 96);
        tree->setColumnWidth(2, 48);

        tree->addTopLevelItem(makeTopLevelItem(tree, SideWidget_BNK::BKHD));
        tree->addTopLevelItem(makeTopLevelItem(tree, SideWidget_BNK::DIDX));
        tree->addTopLevelItem(makeTopLevelItem(tree, SideWidget_BNK::HIRC));

        layout->addWidget(tree);

        infoDialog->setLayout(layout);
        infoDialog->setFixedWidth(768);
        infoDialog->setModal(true);
        infoDialog->setWindowTitle("File info: " + this->info->fileName());
        infoDialog->resize(768, 384);

        infoDialog->show();
    }
}

QTreeWidgetItem *SideWidget_BNK::makeTopLevelItem(QTreeWidget *tree, SectionFlag section){
    QTreeWidgetItem *header = new QTreeWidgetItem(tree);

    switch(section){
        case SideWidget_BNK::BKHD:
            header->setText(3, "Wwise SoundBank Header.");

            header->addChildren(makeTreeChildren(header, section));

            header->setText(0, QString("BKHD [%1]").arg(header->childCount()));

            break;
        case SideWidget_BNK::DIDX:
            header->setText(3, "Data Index section.");

            header->addChildren(makeTreeChildren(header, section));

            header->setText(0, QString("DIDX [%1]").arg(header->childCount()));

            break;
        case SideWidget_BNK::HIRC:
            header->setText(3, "Contains all Wwise objects.");

            header->addChildren(makeTreeChildren(header, section));

            header->setText(0, QString("HIRC [%1]").arg(header->childCount()));
            break;
        default:
            header->setText(0, "Unknown");
    }

    return header;
}

QList<QTreeWidgetItem*> SideWidget_BNK::makeTreeChildren(QTreeWidgetItem *parent, SectionFlag section){
    QList<QTreeWidgetItem*> children;

    switch(section){
        case SideWidget_BNK::BKHD:
            for(quint32 i = 0; i < 2 + getSectionIndex(section, 1) / 4; i++)
                children.append(makeTreeChild(parent, section, i));

            break;
        case SideWidget_BNK::DIDX:
            children.append(makeTreeChild(parent, section, 0));
            children.append(makeTreeChild(parent, section, 1));
            children.append(makeTreeChild(parent, section, -1));

            for(quint32 i = 0; i < getSectionIndex(section, 1) / 12; i++)
                children.append(makeTreeChild(children[2], section, i + 2));

            break;

        case SideWidget_BNK::HIRC:
            for(int i = 0; i < 3; i++)
                children.append(makeTreeChild(parent, section, i));

            quint32 objectsLength = getSectionIndex(section, 2);

            QTreeWidgetItem *objects = new QTreeWidgetItem(parent);

            objects->setText(0, QString("Objects [%1]").arg(objectsLength));
            objects->setText(3, "Wwise objects.");

            int offset = fileBlob.indexOf("HIRC") + 12;

            pointer = (quint32)offset;

            for(quint32 i = 0; i < objectsLength; i++){
                objects->addChild(makeObjectTree(objects));
            }

            children.append(objects);

            break;
    }

    return children;
}

quint8 SideWidget_BNK::getByte(int index){
    quint8 value;

    QDataStream stream(fileBlob.mid(index, 1));

    stream.setByteOrder(QDataStream::LittleEndian);

    stream >> value;

    return value;
}

QTreeWidgetItem *SideWidget_BNK::makeObjectTree(QTreeWidgetItem *parent){
    QTreeWidgetItem *object = new QTreeWidgetItem(parent);

    quint8 type = getByte(pointer);

    quint32 length = getInt(pointer + 1);

    QList<QTreeWidgetItem*> children;

    QTreeWidgetItem *id = new QTreeWidgetItem(object);
    QTreeWidgetItem *lengthBytes = new QTreeWidgetItem(object);
    QTreeWidgetItem *objectId = new QTreeWidgetItem(object);

    id->setText(0, QString::number(type));
    id->setText(1, int2hex(type));
    id->setText(2, "uint8");

    lengthBytes->setText(0, QString::number(length));
    lengthBytes->setText(1, int2hex(length));
    lengthBytes->setText(2, "uint32");
    lengthBytes->setText(3, "Section length in bytes.");

    objectId->setText(0, QString::number(getInt(pointer + 5)));
    objectId->setText(1, int2hex(getInt(pointer + 5)));
    objectId->setText(2, "uint32");
    objectId->setText(3, "Object ID.");

    children.append(id);
    children.append(lengthBytes);
    children.append(objectId);

    QString section;

    switch(type){
        case 0x01:
            section = "Settings";

            break;
        case 0x02:{
            section = "Sound SFX / Voice";

            QTreeWidgetItem *byte0 = new QTreeWidgetItem(object);
            QTreeWidgetItem *byte1 = new QTreeWidgetItem(object);
            QTreeWidgetItem *byte2 = new QTreeWidgetItem(object);
            QTreeWidgetItem *byte3 = new QTreeWidgetItem(object);
            QTreeWidgetItem *streamed = new QTreeWidgetItem(object);
            QTreeWidgetItem *audioId = new QTreeWidgetItem(object);
            QTreeWidgetItem *sourceId = new QTreeWidgetItem(object);

            if(getInt(pointer + 10) == 0x00){
                QTreeWidgetItem *embedOffset = new QTreeWidgetItem(object);
                QTreeWidgetItem *embedLength = new QTreeWidgetItem(object);

                embedOffset->setText(0, QString::number(getInt(pointer + 22)));
                embedLength->setText(0, QString::number(getInt(pointer + 26)));

                embedOffset->setText(1, int2hex(getInt(pointer + 22)));
                embedLength->setText(1, int2hex(getInt(pointer + 26)));

                embedOffset->setText(2, "uint32");
                embedLength->setText(2, "uint32");

                embedOffset->setText(3, "Offset position of the embedded file.");
                embedLength->setText(3, "Length in bytes of the embedded file.");

                children.append(embedOffset);
                children.append(embedLength);

                pointer += 8;
            }

            QTreeWidgetItem *type = new QTreeWidgetItem(object);

            byte0->setText(0, QString::number(getByte(pointer + 6)));
            byte1->setText(0, QString::number(getByte(pointer + 7)));
            byte2->setText(0, QString::number(getByte(pointer + 8)));
            byte3->setText(0, QString::number(getByte(pointer + 9)));
            streamed->setText(0, QString::number(getInt(pointer + 10)));
            audioId->setText(0, QString::number(getInt(pointer + 14)));
            sourceId->setText(0, QString::number(getInt(pointer + 18)));
            type->setText(0, QString::number(getByte(pointer + 22)));

            byte0->setText(1, int2hex(getByte(pointer + 6)));
            byte1->setText(1, int2hex(getByte(pointer + 7)));
            byte2->setText(1, int2hex(getByte(pointer + 8)));
            byte3->setText(1, int2hex(getByte(pointer + 9)));
            streamed->setText(1, int2hex(getInt(pointer + 10)));
            audioId->setText(1, int2hex(getInt(pointer + 14)));
            sourceId->setText(1, int2hex(getInt(pointer + 18)));
            type->setText(1, int2hex(getByte(pointer + 22)));

            byte0->setText(2, "uint8");
            byte1->setText(2, "uint8");
            byte2->setText(2, "uint8");
            byte3->setText(2, "uint8");
            streamed->setText(2, "uint32");
            audioId->setText(2, "uint32");
            sourceId->setText(2, "uint32");
            type->setText(2, "uint8");

            byte0->setText(3, "Unknown byte.");
            byte1->setText(3, "Unknown byte.");
            byte2->setText(3, "Unknown byte.");
            byte3->setText(3, "Unknown byte.");

            switch(getInt(pointer + 10)){
                case 0x00:streamed->setText(3, "Audio is embedded in the SoundBank.");break;
                case 0x01:streamed->setText(3, "Audio is streamed.");break;
                case 0x02:streamed->setText(3, "Audio is streamed with zero latency (is prefetched).");break;
                default:streamed->setText(3, "Unknown value.");
            }

            audioId->setText(3, "ID of the audio file.");
            sourceId->setText(3, "ID of the source.");

            switch(getByte(pointer + 22)){
                case 0x00:type->setText(3, "Identifies this object as Sound SFX.");break;
                case 0x01:type->setText(3, "Identifies this object as Sound Voice.");break;
                default:type->setText(3, "Unknown value.");
            }

            children.append(byte0);
            children.append(byte1);
            children.append(byte2);
            children.append(byte3);
            children.append(streamed);
            children.append(audioId);
            children.append(sourceId);
            children.append(type);

            if(getInt(pointer + 2) == 0x00) pointer -= 8;

            }
            break;

        case 0x03:
            section = "Event Action";

            break;

        case 0x04:
            section = "Event";

            break;

        case 0x05:
            section = "Random or Sequence Container";

            break;

        case 0x06:
            section = "Switch Container";

            break;

        case 0x07:
            section = "Actor-Mixer";

            break;

        case 0x08:
            section = "Audio Bus";

            break;

        case 0x09:
            section = "Blend Container";

            break;

        case 0x0a:
            section = "Music Segment";

            break;

        case 0x0b:
            section = "Music Track";

            break;

        case 0x0c:
            section = "Music Switch Container";

            break;

        case 0x0d:
            section = "Music Playlist Container";

            break;

        case 0x0e:
            section = "Attenuation";

            break;

        case 0x0f:
            section = "Dialogue Event";

            break;

        case 0x10:
            section = "Motion Bus";

            break;

        case 0x11:
            section = "Motion FX";

            break;

        case 0x12:
            section = "Effect";

            break;

        case 0x14:
            section = "Auxiliary Bus";

            break;

        default:
            section = "Unknown";
    }

    id->setText(3, QString("Section identifier for %1.").arg(section));

    object->setText(0, section);

    foreach(QTreeWidgetItem *item, children){
        item->setFont(0, QFont("Monospace"));
        item->setFont(1, QFont("Monospace"));
        item->setFont(2, QFont("Monospace"));
    }

    object->addChildren(children);

    pointer += length + 5;

    return object;
}

QTreeWidgetItem *SideWidget_BNK::makeTreeChild(QTreeWidgetItem *parent, SectionFlag section, int index){
    QTreeWidgetItem *item = new QTreeWidgetItem(parent);

    quint32 value = getSectionIndex(section, index);

    item->setText(0, QString::number(value));
    item->setText(1, int2hex(value));
    item->setText(2, "uint32");

    item->setFont(0, QFont("Monospace"));
    item->setFont(1, QFont("Monospace"));
    item->setFont(2, QFont("Monospace"));

    switch(section){
        case SideWidget_BNK::BKHD:
            switch(index){
                case 0:
                    item->setText(2, "char[4]");
                    item->setText(3, "ASCII string \"BKHD\" to mark the Bank Header.");

                    return item;
                    break;
                case 1:
                    item->setText(3, "Length of the BKHD section in bytes (usually 24 bytes).");

                    return item;
                    break;

                case 2:
                    item->setText(3, "SoundBank version number.");

                    return item;
                    break;

                case 3:
                    item->setText(3, "SoundBank id.");

                    return item;
                    break;
                case 4:
                case 5:
                    item->setText(3, "Unknown / unused.");

                    return item;
                    break;

                default:
                    item->setText(3, "Unknown.");

                    return item;
            }

            break;
        case SideWidget_BNK::DIDX:{
            int j = (index - 2) * 3;
            if(index > 1){
                index = 2;
            }

            switch(index){
                case -1:
                    item->setText(0, "Files [" + QString::number(getSectionIndex(section, 1) / 12) + "]");
                    item->setText(1, "");
                    item->setText(2, "");
                    item->setText(3, "Embedded .wem files index.");

                    return item;
                    break;
                case 0:
                    item->setText(2, "char[4]");
                    item->setText(3, "ASCII string \"DIDX\" to mark the DIDX section.");

                    return item;
                    break;
                case 1:
                    item->setText(3, "Length of the DIDX section in bytes. (12 * number of embedded files)");

                    return item;
                    break;
                case 2:{
                    delete item;

                    quint32 fileId = getSectionIndex(section, j + 2);
                    quint32 dataOffset = getSectionIndex(section, j + 3);
                    quint32 fileLength = getSectionIndex(section, j + 4);

                    QTreeWidgetItem *fileItem = new QTreeWidgetItem(parent);

                    QTreeWidgetItem *fileIdItem = new QTreeWidgetItem(fileItem);
                    QTreeWidgetItem *dataOffsetItem = new QTreeWidgetItem(fileItem);
                    QTreeWidgetItem *fileLengthItem = new QTreeWidgetItem(fileItem);

                    fileItem->setText(0, QString::number(j / 3) + " [3]");

                    fileIdItem->setText(0, QString::number(fileId));
                    fileIdItem->setText(1, int2hex(fileId));
                    fileIdItem->setText(2, "uint32");
                    fileIdItem->setText(3, ".wem file id.");

                    dataOffsetItem->setText(0, QString::number(dataOffset));
                    dataOffsetItem->setText(1, int2hex(dataOffset));
                    dataOffsetItem->setText(2, "uint32");
                    dataOffsetItem->setText(3, "The file's offset from the start of the DATA section.");

                    fileLengthItem->setText(0, QString::number(fileLength));
                    fileLengthItem->setText(1, int2hex(fileLength));
                    fileLengthItem->setText(2, "uint32");
                    fileLengthItem->setText(3, "The file length in bytes.");

                    fileIdItem->setFont(1, QFont("Monospace"));
                    fileIdItem->setFont(2, QFont("Monospace"));

                    dataOffsetItem->setFont(1, QFont("Monospace"));
                    dataOffsetItem->setFont(2, QFont("Monospace"));

                    fileLengthItem->setFont(1, QFont("Monospace"));
                    fileLengthItem->setFont(2, QFont("Monospace"));

                    fileItem->addChild(fileIdItem);
                    fileItem->addChild(dataOffsetItem);
                    fileItem->addChild(fileLengthItem);

                    return fileItem;

                    }
                    break;
            }
            }
            break;
        case SideWidget_BNK::HIRC:{
            switch(index){
                case 0:
                    item->setText(2, "char[4]");
                    item->setText(3, "ASCII string \"HIRC\" to mark the HIRC section.");

                    return item;
                    break;
                case 1:
                    item->setText(3, "Length of the HIRC section in bytes.");

                    return item;
                    break;
                case 2:
                    item->setText(3, "Number of Wwise objects.");

                    return item;
                    break;
            }
            }
            break;
    }

    return item;
}

QString SideWidget_BNK::int2hex(quint8 num){
    QString string = QString::number(num, 16).toUpper();

    while(string.length() != 2) string.prepend("0");

    return string;
}

QString SideWidget_BNK::int2hex(quint32 num){
    QString string = QString::number(num, 16).toUpper();

    while(string.length() != 8) string.prepend("0");

    for(int i = 2; i <= string.size(); i += 3) string.insert(i, " ");

    return string;
}

quint32 SideWidget_BNK::getInt(quint32 index){
    quint32 value;

    QDataStream stream(fileBlob.mid(index, index + 4));

    stream.setByteOrder(QDataStream::LittleEndian);

    stream >> value;

    return value;
}

quint32 SideWidget_BNK::getSectionIndex(SectionFlag section, int index){
    quint32 value;

    switch(section){
        case SideWidget_BNK::BKHD:{
            QDataStream stream(fileBlob.mid(index * 4, index * 4 + 4));

            stream.setByteOrder(QDataStream::LittleEndian);

            stream >> value;
            }
            break;
        case SideWidget_BNK::DIDX:{
            int sectionIndex = fileBlob.indexOf("DIDX");

            QDataStream stream(fileBlob.mid(sectionIndex + index * 4, sectionIndex + index * 4 + 4));

            stream.setByteOrder(QDataStream::LittleEndian);

            stream >> value;
            }
            break;
        case SideWidget_BNK::HIRC:{
            int sectionIndex = fileBlob.indexOf("HIRC");

            QDataStream stream(fileBlob.mid(sectionIndex + index * 4, sectionIndex + index * 4 + 4));

            stream.setByteOrder(QDataStream::LittleEndian);

            stream >> value;
            }
    }

    return value;
}

void SideWidget_BNK::convertButtonSlot(){
    ConvertDialog *win = new ConvertDialog(this->fileContents, this->initiator, this->info);

    connect(win, SIGNAL(startConversionSignal(QStringList, QStringList)), this, SLOT(startConversion(QStringList, QStringList)));

    win->show();
}

void SideWidget_BNK::startConversion(QStringList commands, QStringList outputInfo){
    if(!QDir(QCoreApplication::applicationDirPath() + "/Temp").exists()){
        QDir().mkpath(QCoreApplication::applicationDirPath() + "/Temp");

        while(!QDir().exists(QCoreApplication::applicationDirPath() + "/Temp"));
    }
    // Hehehe~
    QByteArray iter(fileBlob);
    QList<QByteArray> fileContents;

    int count = (int)getSectionIndex(SideWidget_BNK::DIDX, 1) / 12;

    iter.remove(0, iter.indexOf("DATA") + 8);

    for(int i = 0; i < count; i++){

        QByteArray temp = iter.mid(0, iter.indexOf("RIFF", 1));

        fileContents.append(temp);

        iter.remove(0, iter.indexOf("RIFF", 1));
    }

    NAMConversion *conversion;

    if(commands[0].split("\n").size() == 1){
        conversion = new NAMConversion(fileContents, this->fileContents, commands, outputInfo, false);

        conversion->start();
    }else{
        conversion = new NAMConversion(fileContents, this->fileContents, commands, outputInfo, true);

        conversion->start();
    }
}

quint32 SideWidget_BNK::getBnkFileCount(QByteArray blob){
    int didx_index = blob.indexOf("DIDX");

    if(didx_index < 1){
        return didx_index;
    }else{
        QByteArray bytes = blob.mid(didx_index + 4, 4);

        QDataStream stream(bytes);

        stream.setByteOrder(QDataStream::LittleEndian);

        quint32 length;

        stream >> length;

        return length;
    }
}

SideWidget_BNK::~SideWidget_BNK(){

}
