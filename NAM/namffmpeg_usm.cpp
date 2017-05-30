void NieRAutomataMediaUtil::convert_ffmpeg(){
    if(outputPath->text().isEmpty()){
        QMessageBox::warning(this, "ffmpeg conversion", "Output path not specified.");
        return;
    }else if(outputFileName->text().isEmpty()){
        QMessageBox::warning(this, "ffmpeg conversion", "Output filename not specified");
        return;
    }else if(outputBitrate->text().isEmpty()){
        QMessageBox::warning(this, "ffmpeg conversion", "Output bitrate not specified.");
        return;
    }
    outfile = outputPath->text() + "/" + outputFileName->text();
    if(QFile::exists(outfile)){
        if(QMessageBox::question(this, "ffmpeg conversion", "Specified output file already exists. Overwrite?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::No){
            return;
        }else{
            QFile::remove(outfile);
            while(QFile::exists(outfile));
        }
    }
    ffmpeg = new QProcess;
    QString command = "\"" + QCoreApplication::applicationDirPath() + "/tools/ffmpeg.exe\" -f mpegvideo -hide_banner -i ";
    command.append("\"" + info->absoluteFilePath() + "\" ");
    command.append(extraFfmpegFlags->text());
    command.append(" -b:v " + outputBitrate->text() + " ");
    command.append("\"" + outfile + "\"");
    ffmpeg_stdoutstr.append(command + "\n");

    connect(ffmpeg, SIGNAL(readyReadStandardOutput()), this, SLOT(ffmpeg_readyOut()));
    connect(ffmpeg, SIGNAL(finished(int)), this, SLOT(ffmpeg_done()));
    connect(ffmpeg, SIGNAL(started()), this, SLOT(ffmpeg_started()));

    ffmpeg->setProcessChannelMode(QProcess::MergedChannels);

    ffmpeg_out = new QDialog;
    ffmpeg_stdout = new QTextEdit;
    openOutFile = new QPushButton;
    openOutDir = new QPushButton;
    saveLog = new QPushButton;
    closeStdout = new QPushButton;
    ffmpeg_out_layout = new QFormLayout;
    ffmpeg_out_controls = new QGridLayout;

    openOutFile->setText("Open output file");
    openOutDir->setText("Open output directory");
    saveLog->setText("Save output log");
    closeStdout->setText("Close");

    connect(openOutFile, SIGNAL(released()), this, SLOT(open_out_file()));
    connect(openOutDir, SIGNAL(released()), this, SLOT(open_out_dir()));
    connect(saveLog, SIGNAL(released()), this, SLOT(save_ffmpeg_log()));
    connect(closeStdout, SIGNAL(released()), this, SLOT(close_stdout_widget()));

    ffmpeg_out_controls->addWidget(openOutFile, 0, 0, 1, 1);
    ffmpeg_out_controls->addWidget(openOutDir, 0, 1, 1, 1);
    ffmpeg_out_controls->addWidget(saveLog, 1, 0, 1, 1);
    ffmpeg_out_controls->addWidget(closeStdout, 1, 1, 1, 1);

    ffmpeg_stdout->setFixedHeight(320);
    ffmpeg_stdout->setReadOnly(true);
    ffmpeg_out_layout->addRow(ffmpeg_stdout);
    ffmpeg_out_layout->addRow(ffmpeg_out_controls);
    ffmpeg_out->setLayout(ffmpeg_out_layout);
    ffmpeg_out->setModal(true);
    ffmpeg_out->setFixedSize(512, 400);
    ffmpeg_out->setWindowFlags(ffmpeg_out->windowFlags() & ~Qt::WindowCloseButtonHint);
    ffmpeg_out->setWindowTitle("ffmpeg conversion");
    ffmpeg_out->show();

    ffmpeg->start(command);
}

void NieRAutomataMediaUtil::ffmpeg_started(){
    openOutFile->setEnabled(false);
    openOutDir->setEnabled(false);
    saveLog->setEnabled(false);
    closeStdout->setEnabled(false);
}

void NieRAutomataMediaUtil::ffmpeg_readyOut(){
    ffmpeg_stdoutstr.append(ffmpeg->readAllStandardOutput());
    ffmpeg_stdout->setText(ffmpeg_stdoutstr);
    ffmpeg_stdout->verticalScrollBar()->setValue(ffmpeg_stdout->verticalScrollBar()->maximum());
}

void NieRAutomataMediaUtil::ffmpeg_done(){
    if(QFile::exists(outputPath->text() + "/" + outputFileName->text())){
        ffmpeg_stdoutstr.append("ffmpeg finished.");
    }else{
        ffmpeg_stdoutstr.append("Failure");
    }
    ffmpeg_stdout->setText(ffmpeg_stdoutstr);
    ffmpeg_stdout->verticalScrollBar()->setValue(ffmpeg_stdout->verticalScrollBar()->maximum());
    openOutFile->setEnabled(true);
    openOutDir->setEnabled(true);
    saveLog->setEnabled(true);
    closeStdout->setEnabled(true);
}

void NieRAutomataMediaUtil::save_ffmpeg_log(){
    QString ffmpeg_log;
    if(!outputPathPersistent.isEmpty()){
        ffmpeg_log = QFileDialog::getSaveFileName(this, "Save log file to", outputPathPersistent + "/ffmpeg_log_" + QDateTime::currentDateTime().toString("dd_MM_yyyy_HH_mm_ss") + ".txt", "All files (*.*)");
    }else{
        ffmpeg_log = QFileDialog::getSaveFileName(this, "Save log file to", QCoreApplication::applicationDirPath() + "/ffmpeg_log_" + QDateTime::currentDateTime().toString("dd_MM_yyyy_HH_mm_ss") + ".txt", "All files (*.*)");
    }
    QFile out(ffmpeg_log);
    if(out.open(QIODevice::WriteOnly)){
        QTextStream stream(&out);
        stream << ffmpeg_stdoutstr;
    }
}

void NieRAutomataMediaUtil::output_path_changed(const QString &text){
    outputPathPersistent = text;
}

void NieRAutomataMediaUtil::output_bitrate_changed(const QString &text){
    outputBitratePersistent = text;
}

void NieRAutomataMediaUtil::extra_ffmpeg_flags_changed(const QString &text){
    extraFfmpegFlagsPersistent = text;
}

void NieRAutomataMediaUtil::open_out_dir(){
    QDesktopServices::openUrl(outputPath->text());
}

void NieRAutomataMediaUtil::open_out_file(){
    QDesktopServices::openUrl(outfile);
}

void NieRAutomataMediaUtil::close_stdout_widget(){
    ffmpeg_out->close();
}

void NieRAutomataMediaUtil::proc_output_destination(){
    outputDir = QFileDialog::getExistingDirectory(this, "Select output directory", dir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    outputPath->setText(outputDir);
}
