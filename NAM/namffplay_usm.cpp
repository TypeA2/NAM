#include "nierautomatamediautil.h"

void NieRAutomataMediaUtil::usm_playback(){
    while(formRight->rowCount() > 4){
        formRight->removeRow(4);
    }
    usmPlayFFplay = new QPushButton;
    extraFfplayFlags = new QLineEdit;
    if(!extraFfplayFlagsPersistent.isEmpty()){
        extraFfplayFlags->setText(extraFfplayFlagsPersistent);
    }
    usmPlayFFplay->setText("Play with FFplay");
    connect(usmPlayFFplay, SIGNAL(released()), this, SLOT(play_with_ffplay()));
    connect(extraFfplayFlags, SIGNAL(textChanged(const QString &)), this, SLOT(extra_ffplay_flags_changed(const QString &)));

    formRight->addRow("Extra FFplay flags: ", extraFfplayFlags);
    formRight->addRow("", usmPlayFFplay);
}

void NieRAutomataMediaUtil::play_with_ffplay(){
    ffplay = new QProcess;
    connect(ffplay, SIGNAL(finished(int)), this, SLOT(ffplay_done()));
    connect(ffplay, SIGNAL(started()), this, SLOT(ffplay_started()));
    QString command = "\"" + QCoreApplication::applicationDirPath() + "/ffmpeg/bin/ffplay.exe\" -f mpegvideo -hide_banner -autoexit ";
    command.append(extraFfplayFlags->text());
    command.append(" -i \"" + info->absoluteFilePath() + "\" ");
    ffplay->start(command);
}

void NieRAutomataMediaUtil::usm_convert(){
    while(formRight->rowCount() > 4){
        formRight->removeRow(4);
    }

    convertBtn = new QPushButton;
    destinationSelect = new QPushButton;
    outputPath = new QLineEdit;
    outputFileName = new QLineEdit;
    fileNameHint = new QLabel;
    outputBitrate = new QLineEdit;
    extraFfmpegFlags = new QLineEdit;

    convertBtn->setText("Convert");
    convertBtn->setMaximumWidth(160);
    desc->setText("MPEG-based file used for storing cutscene video.");
    destinationSelect->setText("Select output directory");
    destinationSelect->setMaximumWidth(160);

    connect(destinationSelect, SIGNAL(released()), this, SLOT(proc_output_destination()));
    connect(outputPath, SIGNAL(textChanged(const QString &)), this, SLOT(output_path_changed(const QString &)));
    connect(outputBitrate, SIGNAL(textChanged(const QString &)), this, SLOT(output_bitrate_changed(const QString &)));
    connect(extraFfmpegFlags, SIGNAL(textChanged(const QString &)), this, SLOT(extra_ffmpeg_flags_changed(const QString &)));
    connect(convertBtn, SIGNAL(released()), this, SLOT(convert_ffmpeg()));

    if(!outputPathPersistent.isEmpty()){
        outputPath->setText(outputPathPersistent);
    }else{
        outputPath->setText(info->absolutePath());
    }
    if(!outputBitratePersistent.isEmpty()){
        outputBitrate->setText(outputBitratePersistent);
    }else{
        outputBitrate->setText("50M");
    }
    if(!extraFfmpegFlagsPersistent.isEmpty()){
        extraFfmpegFlags->setText(extraFfmpegFlagsPersistent);
    }
    outputFileName->setText(info->baseName() + ".mp4");
    fileNameHint->setText("<i>Changing file extension also changes file type</i>");

    formRight->addRow("File conversion:", new QLabel("<i>FFmpeg</i>"));
    formRight->addRow("Output directory:", outputPath);
    formRight->addRow("Select directory:", destinationSelect);
    formRight->addRow("Output filename:", outputFileName);
    formRight->addRow("", fileNameHint);
    formRight->addRow("Output bitrate:", outputBitrate);
    formRight->addRow("Extra FFmpeg flags", extraFfmpegFlags);
    formRight->addRow("", convertBtn);
}

void NieRAutomataMediaUtil::extra_ffplay_flags_changed(const QString &text){
    extraFfplayFlagsPersistent = text;
}

void NieRAutomataMediaUtil::ffplay_started(){
    usmPlayFFplay->setEnabled(false);
}

void NieRAutomataMediaUtil::ffplay_done(){
    usmPlayFFplay->setEnabled(true);
}
