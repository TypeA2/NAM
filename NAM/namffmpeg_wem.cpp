#include "nierautomatamediautil.h"

void NieRAutomataMediaUtil::layout_wem(){
    outputPath = new QLineEdit;
    destinationSelect = new QPushButton;
    outputFileName = new QLineEdit;
    fileNameHint = new QLabel;
    outputBitrate = new QLineEdit;
    extraFfmpegFlags = new QLineEdit;
    convertBtn = new QPushButton;
    if(!outputPathPersistent.isEmpty()){
        outputPath->setText(outputPathPersistent);
    }else{
        outputPath->setText(info->absolutePath());
    }

    destinationSelect->setText("Select output directory");
    outputFileName->setText(info->baseName() + ".ogg");
    fileNameHint->setText("<i>Changing file extension also changes file type</i>");
    if(!outputBitratePersistentAudio.isEmpty()){
        outputBitrate->setText(outputBitratePersistentAudio);
    }else{
        outputBitrate->setText("384k");
    }
    if(!extraFfmpegFlagsPersistentAudio.isEmpty()){
        extraFfmpegFlags->setText(extraFfmpegFlagsPersistentAudio);
    }
    convertBtn->setText("Convert");

    connect(destinationSelect, SIGNAL(released()), this, SLOT(proc_output_destination()));
    connect(outputPath, SIGNAL(textChanged(const QString &)), this, SLOT(output_path_changed(const QString &)));
    connect(outputBitrate, SIGNAL(textChanged(const QString &)), this, SLOT(output_bitrate_changed_audio(const QString &)));
    connect(extraFfmpegFlags, SIGNAL(textChanged(const QString &)), this, SLOT(extra_ffmpeg_flags_changed_audio(const QString &)));
    connect(convertBtn, SIGNAL(released()), this, SLOT(convert_wem_plain()));

    formRight->addRow("Output directory:", outputPath);
    formRight->addRow("Select directory:", destinationSelect);
    formRight->addRow("Output filename:", outputFileName);
    formRight->addRow("", fileNameHint);
    formRight->addRow("Output bitrate:", outputBitrate);
    formRight->addRow("Extra FFmpeg flags", extraFfmpegFlags);
    formRight->addRow("", convertBtn);
}

void NieRAutomataMediaUtil::convert_wem_plain(){
    if(outputPath->text().isEmpty()){
        QMessageBox::warning(this, "ww2ogg / ffmpeg conversion", "Output path not specified.");
        return;
    }else if(outputFileName->text().isEmpty()){
        QMessageBox::warning(this, "ww2ogg / ffmpeg conversion", "Output filename not specified");
        return;
    }else if(outputBitrate->text().isEmpty()){
        QMessageBox::warning(this, "ww2ogg / ffmpeg conversion", "Output bitrate not specified.");
        return;
    }
    if(!QDir(QCoreApplication::applicationDirPath() + "/Temp").exists()){
        QDir().mkdir(QCoreApplication::applicationDirPath() + "/Temp");
    }
    outfile = outputPath->text() + "/" + outputFileName->text();
    wemTempFile = QCoreApplication::applicationDirPath() + "/Temp/" + info->baseName() + ".ogg";

    if(QFile::exists(outfile)){
        if(QMessageBox::question(this, "ww2ogg / ffmpeg conversion", "Specified output file already exists. Overwrite?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::No){
            return;
        }else{
            QFile::remove(outfile);
            while(QFile::exists(outfile));
        }
    }

    wemConvertOut = new QDialog;
    wemConvertControls = new QGridLayout;
    wemOutLog = new QTextEdit;
    wemConvertLayout = new QFormLayout;
    ww2ogg = new QProcess;
    ffmpeg = new QProcess;

    wemOpenOutFile = new QPushButton;
    wemOpenOutDir = new QPushButton;
    wemSaveLog = new QPushButton;
    wemCloseOut = new QPushButton;
    wemKillProcess = new QPushButton;
    wemCurrentRunning = new QLabel;

    wemOpenOutFile->setText("Open output file");
    wemOpenOutDir->setText("Open output directory");
    wemSaveLog->setText("Save output log");
    wemCloseOut->setText("Close");
    wemKillProcess->setText("Kill none");
    wemCurrentRunning->setText("<i>none</i>");

    connect(wemOpenOutFile, SIGNAL(released()), this, SLOT(wem_open_out_file()));
    connect(wemOpenOutDir, SIGNAL(released()), this, SLOT(wem_open_out_dir()));
    connect(wemSaveLog, SIGNAL(released()), this, SLOT(wem_save_log()));
    connect(wemCloseOut, SIGNAL(released()), this, SLOT(wem_close_out()));
    connect(wemKillProcess, SIGNAL(released()), this, SLOT(wem_kill_process()));

    QString command = "\"" + QCoreApplication::applicationDirPath() + "/ww2ogg/ww2ogg.exe\" ";
    command.append("\"" + info->absoluteFilePath() + "\" ");
    command.append("--pcb \"" + QCoreApplication::applicationDirPath() + "/ww2ogg/packed_codebooks_aoTuV_603.bin\" ");
    command.append("-o \"" + wemTempFile + "\"");

    connect(ww2ogg, SIGNAL(readyReadStandardOutput()), this, SLOT(wem_convert_out()));
    connect(ww2ogg, SIGNAL(finished(int)), this, SLOT(wem_convert_ww2ogg_done()));
    connect(ww2ogg, SIGNAL(started()), this, SLOT(wem_convert_ww2ogg_started()));

    wemStdoutstr.append(command + "\n");
    ww2ogg->setProcessChannelMode(QProcess::MergedChannels);

    wemOutLog->setFixedHeight(384);
    wemOutLog->setReadOnly(true);

    wemConvertControls->addWidget(wemOpenOutFile, 0, 0, 1, 1);
    wemConvertControls->addWidget(wemOpenOutDir, 0, 1, 1, 1);
    wemConvertControls->addWidget(wemKillProcess, 1, 0, 1, 1);
    wemConvertControls->addWidget(wemSaveLog, 1, 1, 1, 1);
    wemConvertControls->addWidget(wemCurrentRunning, 2, 0, 1, 1, Qt::AlignCenter);
    wemConvertControls->addWidget(wemCloseOut, 2, 1, 1, 1);

    wemConvertLayout->addRow(wemOutLog);
    wemConvertLayout->addRow(wemConvertControls);

    wemConvertOut->setLayout(wemConvertLayout);
    wemConvertOut->setModal(true);
    wemConvertOut->setFixedWidth(512);
    wemConvertOut->setWindowFlags(wemConvertOut->windowFlags() & ~Qt::WindowCloseButtonHint);
    wemConvertOut->setWindowTitle("ww2ogg / ffmpeg conversion");
    wemConvertOut->show();

    ww2ogg->start(command);
}

void NieRAutomataMediaUtil::wem_kill_process(){
    disconnect(ww2ogg, 0, 0, 0);
    if(ww2ogg->state() == QProcess::Running){
        ww2ogg->close();
        while(ww2ogg->state() != QProcess::NotRunning);
    }
    QFile::remove(wemTempFile);
    while(QFile::exists(wemTempFile));

    disconnect(ffmpeg, 0, 0, 0);
    if(ffmpeg->state() == QProcess::Running){
        ffmpeg->close();
        while(ffmpeg->state() != QProcess::NotRunning);
    }
    QFile::remove(outfile);
    while(QFile::exists(wemTempFile));

    wem_killed();
}

void NieRAutomataMediaUtil::wem_killed(){
    if(wemKillProcess->text() == "Kill ww2ogg"){
        wemStdoutstr.append("ww2ogg killed");
    }else if(wemKillProcess->text() == "Kill ffmpeg"){
        wemStdoutstr.append("ffmpeg killed");
    }
    wemOutLog->setText(wemStdoutstr);
    wemOutLog->verticalScrollBar()->setValue(wemOutLog->verticalScrollBar()->maximum());
    wemOpenOutFile->setEnabled(false);
    wemOpenOutDir->setEnabled(true);
    wemKillProcess->setEnabled(false);
    wemSaveLog->setEnabled(true);
    wemCloseOut->setEnabled(true);
}

void NieRAutomataMediaUtil::wem_close_out(){
    wemConvertOut->close();
}

void NieRAutomataMediaUtil::wem_save_log(){
    QString wemConvertLog;
    if(!outputPathPersistent.isEmpty()){
        wemConvertLog = QFileDialog::getSaveFileName(this, "Save log file to", outputPathPersistent + "/ww2ogg_ffmpeg_log_" + QDateTime::currentDateTime().toString("dd_MM_yyyy_HH_mm_ss") + ".txt", "All files (*.*)");
    }else{
        wemConvertLog = QFileDialog::getSaveFileName(this, "Save log file to", QCoreApplication::applicationDirPath() + "/ww2ogg_ffmpeg_log_" + QDateTime::currentDateTime().toString("dd_MM_yyyy_HH_mm_ss") + ".txt", "All files (*.*)");
    }
    QFile out(wemConvertLog);
    if(out.open(QIODevice::WriteOnly)){
        QTextStream stream(&out);
        stream << wemStdoutstr;
    }
}

void NieRAutomataMediaUtil::wem_open_out_dir(){
    QDesktopServices::openUrl(outputPath->text());
}

void NieRAutomataMediaUtil::wem_open_out_file(){
    QDesktopServices::openUrl(outfile);
}

void NieRAutomataMediaUtil::wem_convert_out(){
   if(ww2ogg->state() == QProcess::Running){
       wemStdoutstr.append(ww2ogg->readAllStandardOutput());
   }else if(ffmpeg->state() == QProcess::Running){
       wemStdoutstr.append(ffmpeg->readAllStandardOutput());
   }

   wemOutLog->setText(wemStdoutstr);
   wemOutLog->verticalScrollBar()->setValue(wemOutLog->verticalScrollBar()->maximum());
}

void NieRAutomataMediaUtil::wem_convert_ww2ogg_started(){
    wemOpenOutFile->setEnabled(false);
    wemOpenOutDir->setEnabled(false);
    wemKillProcess->setEnabled(true);
    wemKillProcess->setText("Kill ww2ogg");
    wemSaveLog->setEnabled(false);
    wemCurrentRunning->setText("<i>ww2ogg</i>");
    wemCloseOut->setEnabled(false);
}

void NieRAutomataMediaUtil::wem_convert_ww2ogg_done(){
    disconnect(ww2ogg, 0, 0, 0);
    QString command = "\"" + QCoreApplication::applicationDirPath() + "/ffmpeg/bin/ffmpeg.exe\" ";
    command.append("-hide_banner -i \"" + wemTempFile + "\" \"" + outfile + "\"");

    wemStdoutstr.append("\n" + command + "\n");

    ffmpeg->setProcessChannelMode(QProcess::MergedChannels);

    connect(ffmpeg, SIGNAL(readyReadStandardOutput()), this, SLOT(wem_convert_out()));
    connect(ffmpeg, SIGNAL(started()), this, SLOT(wem_convert_ffmpeg_started()));
    connect(ffmpeg, SIGNAL(finished(int)), this, SLOT(wem_convert_ffmpeg_done()));

    ffmpeg->start(command);
}

void NieRAutomataMediaUtil::wem_convert_ffmpeg_started(){
    wemKillProcess->setText("Kill ffmpeg");
    wemCurrentRunning->setText("<i>ffmpeg</i>");
}

void NieRAutomataMediaUtil::wem_convert_ffmpeg_done(){
    if(QFile::exists(outfile)){
        wemStdoutstr.append("ffmpeg finished");
    }else{
        wemStdoutstr.append("output file not found");
    }
    wemOutLog->setText(wemStdoutstr);
    wemOutLog->verticalScrollBar()->setValue(wemOutLog->verticalScrollBar()->maximum());
    disconnect(ffmpeg, 0, 0, 0);
    wemOpenOutFile->setEnabled(true);
    wemOpenOutDir->setEnabled(true);
    wemKillProcess->setEnabled(false);
    wemKillProcess->setText("Kill none");
    wemSaveLog->setEnabled(true);
    wemCurrentRunning->setText("<i>none</i>");
    wemCloseOut->setEnabled(true);
}

void NieRAutomataMediaUtil::output_bitrate_changed_audio(const QString &text){
    outputBitratePersistentAudio = text;
}

void NieRAutomataMediaUtil::extra_ffmpeg_flags_changed_audio(const QString &text){
    extraFfmpegFlagsPersistentAudio = text;
}
