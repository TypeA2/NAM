#include "namconversion.h"

NAMConversion::NAMConversion(QList<QByteArray> &currentFileBlobs, QComboBox *&currentFileContents, QStringList &commands, QStringList &outputInfo, bool m) : QDialog(){
    this->currentFileBlobs = currentFileBlobs;
    this->currentFileContents = currentFileContents;
    this->commandList = commands;
    this->outputFileInfo = outputInfo;
    this->multi = m;

    if(QDir(QCoreApplication::applicationDirPath() + "/Temp").exists()){
        QDir temp(QCoreApplication::applicationDirPath() + "/Temp");

        foreach(QString f, temp.entryList()){
            temp.remove(f);
        }
    }

    if(this->multi == false){
        QFile out(QCoreApplication::applicationDirPath() + "/Temp/" + outputInfo[1] + ".ogg");

        out.open(QIODevice::WriteOnly);

        out.write(this->currentFileBlobs[this->currentFileContents->currentIndex() - 1], (this->currentFileBlobs[this->currentFileContents->currentIndex() - 1].length()));

        out.close();

        this->outputString = commands[0] + "\n";

        this->firstConversion = new QProcess;

        this->firstConversion->setProcessChannelMode(QProcess::MergedChannels);

        connect(this->firstConversion, SIGNAL(readyReadStandardOutput()), this, SLOT(conversionReadyReadStandardOutputSlot()));
        connect(this->firstConversion, SIGNAL(finished(int)), this, SLOT(conversionFinishedSlot()));

        if(this->commandList.length() == 2){
            this->secondConversion = new QProcess;

            this->secondConversion->setProcessChannelMode(QProcess::MergedChannels);

            connect(this->secondConversion, SIGNAL(readyReadStandardOutput()), this, SLOT(conversionReadyReadStandardOutputSlot()));
            connect(this->secondConversion, SIGNAL(finished(int)), this, SLOT(conversionFinishedSlot()));
        }

        this->isSecondProcess = false;
    }else{
        foreach(QString command, this->commandList){
            this->commandQueue.append(command.split("\n")[0]);
            this->commandQueue.append(command.split("\n")[1]);
        }

        this->currentMultiIndex = 0;

        this->outputString = this->commandList[0].split("\n")[0] + "\n\n";

        for(int i = 0; i < this->outputFileInfo.length(); i++){
            QString file = this->outputFileInfo[i].split("\n")[1];

            QFile out(QCoreApplication::applicationDirPath() + "/Temp/" + file + ".ogg");

            out.open(QIODevice::WriteOnly);

            out.write(this->currentFileBlobs[i], (this->currentFileBlobs[i].length()));

            out.close();

            QProcess *newProcess1 = new QProcess;
            QProcess *newProcess2 = new QProcess;

            newProcess1->setProcessChannelMode(QProcess::MergedChannels);
            newProcess2->setProcessChannelMode(QProcess::MergedChannels);

            connect(newProcess1, SIGNAL(readyReadStandardOutput()), this, SLOT(conversionReadyReadStandardOutputSlot()));
            connect(newProcess2, SIGNAL(readyReadStandardOutput()), this, SLOT(conversionReadyReadStandardOutputSlot()));

            connect(newProcess1, SIGNAL(finished(int)), this, SLOT(multiConversionsPartFinishedSlot()));
            connect(newProcess2, SIGNAL(finished(int)), this, SLOT(multiConversionsPartFinishedSlot()));

            this->multiConversions.append(newProcess1);
            this->multiConversions.append(newProcess2);
        }

        disconnect(this->multiConversions.last(), 0, 0, 0);

        connect(this->multiConversions.last(), SIGNAL(finished(int)), this, SLOT(conversionFinishedSlot()));
    }

    this->output = new QTextEdit;
    this->output->setFixedHeight(320);
    this->output->setReadOnly(true);

    this->openDestinationFile = new QPushButton("Open destination file");
    this->openDestinationDirectory = new QPushButton("Open destination directory");
    this->killCurrentProcess = new QPushButton("Kill process");
    this->saveOutputLogToFile = new QPushButton("Save output log to file");
    this->closeOutputDialog = new QPushButton("Close");

    this->killCurrentProcess->setEnabled(false);

    connect(this->openDestinationFile, SIGNAL(released()), this, SLOT(openDestinationFileSlot()));
    connect(this->openDestinationDirectory, SIGNAL(released()), this, SLOT(openDestinationDirectorySlot()));
    connect(this->killCurrentProcess, SIGNAL(released()), this, SLOT(killCurrentProcessSlot()));
    connect(this->saveOutputLogToFile, SIGNAL(released()), this, SLOT(saveOutputLogToFileSlot()));
    connect(this->closeOutputDialog, SIGNAL(released()), this, SLOT(closeOutputDialogSlot()));

    this->dialogControls = new QGridLayout;
    dialogControls->addWidget(this->openDestinationFile, 1, 0, 1, 1);
    dialogControls->addWidget(this->openDestinationDirectory, 1, 1, 1, 1);
    dialogControls->addWidget(this->killCurrentProcess, 2, 0, 1, 1);
    dialogControls->addWidget(this->saveOutputLogToFile, 2, 1, 1, 1);
    dialogControls->addWidget(this->closeOutputDialog, 3, 0, 1, 2);

    QBoxLayout *dialogLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    dialogLayout->addWidget(this->output);
    dialogLayout->addLayout(this->dialogControls);

    this->setLayout(dialogLayout);
    this->setModal(true);
    this->setFixedWidth(512);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowCloseButtonHint);
    this->setWindowTitle("File conversion");
}

void NAMConversion::multiConversionsPartFinishedSlot(){
    this->currentMultiIndex += 1;

    this->multiConversions[this->currentMultiIndex]->start(this->commandQueue[this->currentMultiIndex]);

    this->outputString.append("\n" + this->commandQueue[this->currentMultiIndex] + "\n");

    if(this->currentMultiIndex % 2 == 0) this->outputString.append("\n");

    this->multiConversionProgressBar->setValue(this->currentMultiIndex);
}

void NAMConversion::conversionFinishedSlot(){
    if(this->multi == true){
        this->openDestinationDirectory->setEnabled(true);
        this->killCurrentProcess->setEnabled(false);
        this->saveOutputLogToFile->setEnabled(true);
        this->closeOutputDialog->setEnabled(true);

        if(QDir(QCoreApplication::applicationDirPath() + "/Temp").exists()){
            QDir temp(QCoreApplication::applicationDirPath() + "/Temp");

            foreach(QString f, temp.entryList()){
                temp.remove(f);
            }
        }

        this->multiConversionProgressBar->setValue(this->multiConversionProgressBar->maximum());
    }else if(this->commandList.length() == 1 || this->isSecondProcess == true){
        this->outputString.append(this->firstConversion->readAllStandardOutput());

        this->output->setText(this->outputString);

        this->output->verticalScrollBar()->setValue(this->output->verticalScrollBar()->maximum());

        QFile::remove(QCoreApplication::applicationDirPath() + "/Temp/" + this->outputFileInfo[1] + ".ogg");
        QFile::remove(QCoreApplication::applicationDirPath() + "/Temp/" + this->outputFileInfo[1] + "_ww2ogg.ogg");

        while(QFile::exists(QCoreApplication::applicationDirPath() + "/Temp/" + this->outputFileInfo[1] + ".ogg"));
        while(QFile::exists(QCoreApplication::applicationDirPath() + "/Temp/" + this->outputFileInfo[1] + "_ww2ogg.ogg"));

        this->openDestinationFile->setEnabled(true);
        this->openDestinationDirectory->setEnabled(true);
        this->killCurrentProcess->setEnabled(false);
        this->saveOutputLogToFile->setEnabled(true);
        this->closeOutputDialog->setEnabled(true);
    }else if(this->commandList.length() == 2){
        this->outputString.append(this->commandList[1] + "\n");

        this->isSecondProcess = true;

        this->secondConversion->start(this->commandList[1]);
    }
}

void NAMConversion::conversionReadyReadStandardOutputSlot(){
    if(this->multi == false){
        if(this->isSecondProcess == false){
            this->outputString.append(this->firstConversion->readAllStandardOutput());
        }else if(this->isSecondProcess == true){
            this->outputString.append(this->secondConversion->readAllStandardOutput());
        }

        this->output->setText(this->outputString);

        this->output->verticalScrollBar()->setValue(this->output->verticalScrollBar()->maximum());
    }else{
        this->outputString.append(this->multiConversions[this->currentMultiIndex]->readAllStandardOutput());

        this->output->setText(this->outputString);

        this->output->verticalScrollBar()->setValue(this->output->verticalScrollBar()->maximum());
    }
}

void NAMConversion::closeOutputDialogSlot(){
    if(QDir(QCoreApplication::applicationDirPath() + "/Temp").exists()){
        QDir temp(QCoreApplication::applicationDirPath() + "/Temp");

        foreach(QString f, temp.entryList()){
            temp.remove(f);
        }
    }

    this->close();
}

void NAMConversion::saveOutputLogToFileSlot(){
    QString outfile = QFileDialog::getSaveFileName(this, "Save log file", this->outputFileInfo[0] + "/NAM_log_" + QDateTime::currentDateTime().toString("dd_MM_yyyy_HH_mm_ss") + ".txt", "All files (*.*)");

    QFile out(outfile);

    QFile::remove(outfile);

    while(QFile::exists(outfile));

    out.open(QIODevice::WriteOnly);

    QTextStream stream(&out);

    stream << this->outputString;

}

void NAMConversion::killCurrentProcessSlot(){
    if(this->multi == false){
        if(this->isSecondProcess == false){
            disconnect(this->firstConversion, 0, 0, 0);

            if(this->firstConversion->state() != QProcess::NotRunning){
                this->firstConversion->close();

                while(this->firstConversion->state() != QProcess::NotRunning);
            }
        }else if(this->isSecondProcess == true){
            disconnect(this->secondConversion, 0, 0, 0);

            if(this->secondConversion->state() != QProcess::NotRunning){
                this->secondConversion->close();

                while(this->secondConversion->state() != QProcess::NotRunning);
            }
        }

        QFile::remove(QCoreApplication::applicationDirPath() + "/Temp/" + this->outputFileInfo[1] + ".ogg");
        QFile::remove(QCoreApplication::applicationDirPath() + "/Temp/" + this->outputFileInfo[1] + "_ww2ogg.ogg");
        QFile::remove(this->outputFileInfo[0] + "/" + this->outputFileInfo[1]);

        this->openDestinationFile->setEnabled(true);
    }else{
        disconnect(this->multiConversions[this->currentMultiIndex], 0, 0, 0);

        if(this->multiConversions[this->currentMultiIndex]->state() != QProcess::NotRunning){
            this->multiConversions[this->currentMultiIndex]->close();

            while(this->multiConversions[this->currentMultiIndex]->state() != QProcess::NotRunning);
        }

        foreach(QString outFile, this->outputFileInfo){
            QString filename = outFile.split("\n")[1];

            QFile::remove(QCoreApplication::applicationDirPath() + "/Temp/" + filename + ".ogg");
            QFile::remove(QCoreApplication::applicationDirPath() + "/Temp/" + filename + "_ww2ogg.ogg");

        }

        QFile::remove(this->outputFileInfo[this->currentMultiIndex].split("\n").join("/"));
    }

    this->openDestinationDirectory->setEnabled(true);
    this->killCurrentProcess->setEnabled(false);
    this->saveOutputLogToFile->setEnabled(true);
    this->closeOutputDialog->setEnabled(true);
}

void NAMConversion::openDestinationFileSlot(){
    if(this->multi == false){
        QDesktopServices::openUrl(QUrl::fromLocalFile(this->outputFileInfo[0] + "/" + this->outputFileInfo[1]));
    }else{
        QDesktopServices::openUrl(QUrl::fromLocalFile(this->outputFileInfo[0].split("\n").join("/")));
    }
}

void NAMConversion::openDestinationDirectorySlot(){
    if(this->multi == false){
        QDesktopServices::openUrl(QUrl::fromLocalFile(this->outputFileInfo[0]));
    }else{
        QDesktopServices::openUrl(QUrl::fromLocalFile(this->outputFileInfo[0].split("\n")[0]));
    }
}

void NAMConversion::start(){
    this->openDestinationFile->setEnabled(false);
    this->openDestinationDirectory->setEnabled(false);
    this->killCurrentProcess->setEnabled(true);
    this->saveOutputLogToFile->setEnabled(false);
    this->closeOutputDialog->setEnabled(false);

    this->show();

    if(this->multi == false){
        this->firstConversion->start(this->commandList[0]);
    }else{
        this->multiConversionProgressBar = new QProgressBar;

        this->multiConversionProgressBar->setRange(0, this->commandQueue.length());

        this->dialogControls->addWidget(this->multiConversionProgressBar, 4, 0, 1, 2);

        this->multiConversions[0]->start(this->commandQueue[0]);
    }
}

NAMConversion::~NAMConversion(){

}
