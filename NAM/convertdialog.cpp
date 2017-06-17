#include "convertdialog.h"
#include "namconversion.h"

ConvertDialog::ConvertDialog(QComboBox *currentFileContents, NieRAutomataMediaUtil *initiator, QFileInfo *info) : QDialog(){
    this->currentFileContents = currentFileContents;
    this->initiator = initiator;
    this->info = info;

    this->setModal(true);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowCloseButtonHint);
    this->setWindowTitle("File conversion");
    this->setFixedWidth(512);

    layoutAudioWwise();
}

void ConvertDialog::layoutAudioWwise(){
    QFormLayout *form = new QFormLayout;
    QHBoxLayout *startCloseButtons = new QHBoxLayout;
    QPushButton *selectDestinationDirectory = new QPushButton("Select output directory");
    QLabel *fileTypeHint = new QLabel("<i>Changing file extension also changes file type</i>");
    QPushButton *startConversionButton = new QPushButton("Start conversion");
    QPushButton *closeConvertDialogButton = new QPushButton("Close");

    selectDestinationDirectory->setMaximumWidth(160);

    connect(selectDestinationDirectory, SIGNAL(released()), this, SLOT(destinationDirectorySelected()));
    connect(startConversionButton, SIGNAL(released()), this, SLOT(startConversion()));
    connect(closeConvertDialogButton, SIGNAL(released()), this, SLOT(closeConvertDialog()));

    startCloseButtons->addWidget(startConversionButton);
    startCloseButtons->addWidget(closeConvertDialogButton);

    if(this->currentFileContents->currentIndex() == 0){
        destinationDirectory = new QLineEdit(this->initiator->optsDefaultOutputDirectory);
        destinationFilename = new QLineEdit(this->initiator->optsOutputName.arg("%1", this->info->baseName(), "wav"));
        ffmpegFlags = new QLineEdit(this->initiator->optsFFmpegFlags);

        fileTypeHint->setText("<i>Changing file extension also changes file type, %1 marks file numbering</i>");

        form->addRow("Destination directory:", destinationDirectory);
        form->addRow("Select directory:", selectDestinationDirectory);
        form->addRow("Destination file pattern:", destinationFilename);
        form->addRow("", fileTypeHint);
        form->addRow("FFmpeg flags:", ffmpegFlags);
        form->addRow("", startCloseButtons);
    }else{
        destinationDirectory = new QLineEdit(this->initiator->optsDefaultOutputDirectory);
        destinationFilename = new QLineEdit(this->initiator->optsOutputName.arg(QString::number(this->currentFileContents->currentIndex() - 1), this->info->baseName(), "wav"));
        ffmpegFlags = new QLineEdit(this->initiator->optsFFmpegFlags);

        form->addRow("Destination directory:", destinationDirectory);
        form->addRow("Select directory:", selectDestinationDirectory);
        form->addRow("Destination filename:", destinationFilename);
        form->addRow("", fileTypeHint);
        form->addRow("FFmpeg flags:", ffmpegFlags);
        form->addRow("", startCloseButtons);
    }

    this->setLayout(form);
}

void ConvertDialog::destinationDirectorySelected(){
    QString directory = QFileDialog::getExistingDirectory(this, "Select destination directory", this->info->absolutePath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if(!directory.isEmpty()) destinationDirectory->setText(directory);
}

void ConvertDialog::startConversion(){
    if(destinationDirectory->text().isEmpty()){
        QMessageBox::warning(this, "File conversion", "No destination directory specified.");

        return;
    }

    if(destinationFilename->text().isEmpty()){
        QMessageBox::warning(this, "File conversion", "No destination file specified.");

        return;
    }

    if(!QDir(destinationDirectory->text()).exists()){
        if(QMessageBox::question(this, "File conversion", "Destination directory doesn't exist. Create it?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes){
            QDir().mkpath(destinationDirectory->text());
        }else{
            return;
        }
    }

    if(!QDir(QCoreApplication::applicationDirPath() + "/Temp").exists()){
        QDir().mkpath(QCoreApplication::applicationDirPath() + "/Temp");

        while(!QDir().exists(QCoreApplication::applicationDirPath() + "/Temp"));
    }

    if(this->currentFileContents->currentIndex() == 0){
        int count = this->currentFileContents->count() - 1;

        QStringList commands;
        QStringList outputInfo;

        if(this->destinationFilename->text().count("%1") < 1){
            QMessageBox::warning(this, "File conversion", "File numbering not specified.");

            return;
        }

        for(int i = 0; i < count; i++){
            if(QFile::exists(this->destinationDirectory->text() + "/" + this->destinationFilename->text().arg(i))){
                if(QMessageBox::question(this, "File conversion", "File already exists: " + this->destinationFilename->text().arg(i) + ". Overwrite?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes){
                    QFile::remove(this->destinationDirectory->text() + "/" + this->destinationFilename->text().arg(i));

                    while(QFile::exists(this->destinationDirectory->text() + "/" + this->destinationFilename->text().arg(i)));
                }
            }

            QString ww2oggcommand = "\"" + QCoreApplication::applicationDirPath() + "/ww2ogg/ww2ogg.exe\" ";
            ww2oggcommand.append("--pcb \""+ QCoreApplication::applicationDirPath() + "/ww2ogg/packed_codebooks_aoTuV_603.bin\" ");
            ww2oggcommand.append("\"" + QCoreApplication::applicationDirPath() + "/Temp/" + this->destinationFilename->text().arg(i) + ".ogg\" ");
            ww2oggcommand.append("-o \"" + QCoreApplication::applicationDirPath() + "/Temp/" + this->destinationFilename->text().arg(i) + "_ww2ogg.ogg\" ");

            QString ffmpegcommand = "\"" + QCoreApplication::applicationDirPath() + "/ffmpeg/bin/ffmpeg.exe\" ";
            ffmpegcommand.append(ffmpegFlags->text() + " ");
            ffmpegcommand.append("-i \"" + QCoreApplication::applicationDirPath() + "/Temp/" + this->destinationFilename->text().arg(i) + "_ww2ogg.ogg\" ");
            ffmpegcommand.append("\"" + this->destinationDirectory->text() + "/" + this->destinationFilename->text().arg(i) + "\"");

            commands << ww2oggcommand + "\n" + ffmpegcommand;

            outputInfo << this->destinationDirectory->text() + "\n" + this->destinationFilename->text().arg(i);
        }

        emit startConversionSignal(commands, outputInfo);
    }else{
        if(QFile::exists(destinationDirectory->text() + "/" + destinationFilename->text())){
            if(QMessageBox::question(this, "File conversion", "Destination file already exists. Overwrite?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes){
                QFile::remove(destinationDirectory->text() + "/" + destinationFilename->text());

                while(QFile::exists(destinationDirectory->text() + "/" + destinationFilename->text()));
            }else{
                return;
            }
        }

        QString ww2oggcommand = "\"" + QCoreApplication::applicationDirPath() + "/ww2ogg/ww2ogg.exe\" ";
        ww2oggcommand.append("--pcb \""+ QCoreApplication::applicationDirPath() + "/ww2ogg/packed_codebooks_aoTuV_603.bin\" ");
        ww2oggcommand.append("\"" + QCoreApplication::applicationDirPath() + "/Temp/" + this->destinationFilename->text() + ".ogg\" ");
        ww2oggcommand.append("-o \"" + QCoreApplication::applicationDirPath() + "/Temp/" + this->destinationFilename->text() + "_ww2ogg.ogg\"");

        QString ffmpegcommand = "\"" + QCoreApplication::applicationDirPath() + "/ffmpeg/bin/ffmpeg.exe\" ";
        ffmpegcommand.append(ffmpegFlags->text() + " ");
        ffmpegcommand.append("-i \"" + QCoreApplication::applicationDirPath() + "/Temp/" + this->destinationFilename->text() + "_ww2ogg.ogg\" ");
        ffmpegcommand.append("\"" + this->destinationDirectory->text() + "/" + this->destinationFilename->text() + "\"");

        QStringList commands;
        QStringList outputInfo;

        commands << ww2oggcommand;
        commands << ffmpegcommand;

        outputInfo << this->destinationDirectory->text();
        outputInfo << this->destinationFilename->text();

        emit startConversionSignal(commands, outputInfo);
    }

    this->close();
}

void ConvertDialog::closeConvertDialog(){
    this->close();
}

ConvertDialog::~ConvertDialog(){

}
