#include "sidewidget_wsp.h"
#include "convertdialog.h"
#include "namconversion.h"

SideWidget_WSP::SideWidget_WSP(QFileInfo *fileInfo, NieRAutomataMediaUtil *parent) : QWidget(){
    this->info = fileInfo;
    this->initiator = parent;

    form = new QFormLayout;

    QFile file(info->filePath());

    if(!file.open(QIODevice::ReadOnly)){
        scanFileButton->setText("File not found");
    }else{
        currentFileBlob = file.read(file.size());

        file.close();
    }

    if(currentFileBlob.count("RIFF") < 1){
        while(form->rowCount() > 3){
            form->removeRow(3);
        }

        form->addRow("File contents:", new QLabel("<format not recognised>"));
    }

    int count = currentFileBlob.count("RIFF");

    for(int i = 0; i < count; i++){
        QByteArray temp = currentFileBlob.mid(0, currentFileBlob.indexOf("RIFF", 1));

        currentFileBlobs.append(temp);

        currentFileBlob.remove(0, currentFileBlob.indexOf("RIFF", 1));
    }

    currentFileContents = new QComboBox;

    currentFileContents->addItem("<all>");

    currentFileContents->setMaximumWidth(216);

    while(form->rowCount() > 3){
        form->removeRow(3);
    }

    for(int i = 0; i < currentFileBlobs.length(); i++){
        currentFileContents->addItem("[" + QString::number(i) + "]" + info->baseName() + ".ogg");
    }

    QLabel *fileName = new QLabel(info->fileName());
    QLabel *fileType = new QLabel(info->suffix());
    QLabel *fileDescription = new QLabel("Binary file containing multiple Audiokinetic Wwise RIFF/RIFX Vorbis audio streams.");

    fileDescription->setWordWrap(true);

    QPushButton *convertButton = new QPushButton("Convert");

    convertButton->setMaximumWidth(160);

    connect(convertButton, SIGNAL(released()), this, SLOT(convertCurrentSelection()));

    form->addRow("Filename:", fileName);
    form->addRow("File type:", fileType);
    form->addRow("File description:", fileDescription);
    form->addRow("File contents:", currentFileContents);
    form->addRow("", convertButton);

    this->setLayout(form);
}

void SideWidget_WSP::convertCurrentSelection(){
    ConvertDialog *win = new ConvertDialog(0, this);

    connect(win, SIGNAL(startConversionSignal(QStringList, QStringList)), this, SLOT(startConversion(QStringList, QStringList)));

    win->show();
}

void SideWidget_WSP::startConversion(QStringList commands, QStringList outputInfo){
    if(!QDir(QCoreApplication::applicationDirPath() + "/Temp").exists()){
        QDir().mkpath(QCoreApplication::applicationDirPath() + "/Temp");

        while(!QDir().exists(QCoreApplication::applicationDirPath() + "/Temp"));
    }

    if(commands[0].split("\n").size() == 1){
        NAMConversion *conversion = new NAMConversion(this, commands, outputInfo, false);
        conversion->start();
    }else{
        NAMConversion *conversion = new NAMConversion(this, commands, outputInfo, true);

        conversion->start();
    }
}

SideWidget_WSP::~SideWidget_WSP(){

}
