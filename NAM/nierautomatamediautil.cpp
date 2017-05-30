#include "nierautomatamediautil.h"

NieRAutomataMediaUtil::NieRAutomataMediaUtil(QWidget *parent)
    : QWidget(parent){
    QStringList fileFilters;

    window = new QWidget(this);
    outer = new QBoxLayout(QBoxLayout::LeftToRight);
    layout = new QGridLayout;
    right = new QWidget;
    filemodel = new QFileSystemModel;
    fileview = new NAMTreeView;
    menuBar = new QMenuBar;
    fileMenu = new QMenu;
    openFolderAct = new QAction("Open");
    exitProgram = new QAction("Exit");;
    sFile = new QLabel;
    sFileType = new QLabel;
    formRight = new QFormLayout;
    desc = new QLabel;

    fileFilters << "*.cpk" << "*.usm" << "*.wai" << "*.bnk" << "*.wem" << "*.wsp";

    filemodel->setRootPath("C:/Program Files (x86)/Steam/SteamApps/common/NieRAutomata");
    filemodel->setNameFilters(fileFilters);
    filemodel->setNameFilterDisables(false);

    fileview->setModel(filemodel);
    fileview->setRootIndex(filemodel->index("C:/Program Files (x86)/Steam/SteamApps/common/NieRAutomata"));
    fileview->setColumnWidth(0, 400);
    fileview->setColumnWidth(1, 60);
    fileview->hideColumn(2);
    fileview->hideColumn(3);
    fileview->setFixedWidth(480);
    fileview->setAnimated(true);
    fileview->setSortingEnabled(true);
    fileview->setSelectionMode(QAbstractItemView::SingleSelection);

    dir = QString::fromUtf8("C:/");

    layout->setContentsMargins(16, 16, 16, 16);

    openFolderAct->setShortcuts(QKeySequence::Open);
    openFolderAct->setStatusTip("Select the game directory");

    exitProgram->setShortcuts(QKeySequence::Quit);
    exitProgram->setStatusTip("Quit");

    connect(openFolderAct, &QAction::triggered, this, &NieRAutomataMediaUtil::open);
    connect(exitProgram, &QAction::triggered, this, &QWidget::close);
    connect(fileview, SIGNAL(clicked(const QModelIndex&)), this, SLOT(fileSelect(const QModelIndex &)));

    fileMenu->setTitle("File");
    fileMenu->addAction(openFolderAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitProgram);
    menuBar->addMenu(fileMenu);

    sFile->setText("<no file selected>");
    sFileType->setText("<no file selected>");
    desc->setText("<no file selected>");
    formRight->addRow("Filename:", sFile);
    formRight->addRow("File type:", sFileType);
    formRight->addRow("File description:", desc);

    right->setLayout(formRight);

    outer->addWidget(fileview);
    outer->addWidget(right);
    outer->setMenuBar(menuBar);

    window->setLayout(this->layout);
    outer->addWidget(window);
    setLayout(outer);
    setWindowTitle("NieR:Automata™ Media Tool");
    setFixedSize(960, 512);
}

void NieRAutomataMediaUtil::open(){
    dir = QFileDialog::getExistingDirectory(this, "Select game directory", "C:\\", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    fileview->setRootIndex(filemodel->index(dir));
}

void NieRAutomataMediaUtil::fileSelect(const QModelIndex &index){
    int type = 0;
    info = new QFileInfo(filemodel->fileInfo(index));
    if(info->isFile()){
        sFile->setText(info->fileName());
        sFileType->setText(info->suffix());
    }else{
        sFile->setText("<no file selected>");
        sFileType->setText("<no file selected>");
    }

    if(info->suffix() == "usm"){
        type = 1;
    }
    switch(type){
        case 1:
            init_usm();
    }
}

void NieRAutomataMediaUtil::init_usm(){
    while(formRight->rowCount() > 3){
        formRight->removeRow(3);
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

    outputPath->setText(info->absolutePath());
    outputFileName->setText(info->baseName() + ".mp4");
    fileNameHint->setText("<i>Changing file extension also changes file type</i>");
    outputBitrate->setText("50M");
    formRight->addRow("File conversion:", new QLabel("<i>FFmpeg</i>"));
    formRight->addRow("Output directory:", outputPath);
    formRight->addRow("Select directory:", destinationSelect);
    formRight->addRow("Output filename:", outputFileName);
    formRight->addRow("", fileNameHint);
    formRight->addRow("Output bitrade:", outputBitrate);
    formRight->addRow("Extra FFmpeg flags", extraFfmpegFlags);
    formRight->addRow("", convertBtn);

    connect(convertBtn, SIGNAL(released()), this, SLOT(convert_ffmpeg()));
}

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
    QString outfile = outputPath->text() + "/" + outputFileName->text();
    if(QFile::exists(outfile)){
        if(QMessageBox::question(this, "ffmpeg conversion", "Specified output file already exists. Overwrite?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::No){
            return;
        }else{
            QFile::remove(outfile);
        }
    }
    ffmpeg = new QProcess;
    QString command = "\"" + QCoreApplication::applicationDirPath() + "/tools/ffmpeg.exe\" -f mpegvideo -hide_banner -i ";
    command.append("\"" + info->absoluteFilePath() + "\" ");
    command.append(extraFfmpegFlags->text());
    command.append(" -b:v " + outputBitrate->text() + " ");
    command.append("\"" + outfile + "\"");
    connect(ffmpeg, SIGNAL(readyReadStandardOutput()), this, SLOT(ffmpeg_readyOut()));
    connect(ffmpeg, SIGNAL(finished(int)), this, SLOT(ffmpeg_done()));

    ffmpeg->setProcessChannelMode(QProcess::MergedChannels);

    ffmpeg_out = new QDialog;
    ffmpeg_stdout = new QTextEdit;
    saveToLog = new QPushButton;
    closeStdout = new QPushButton;
    ffmpeg_out_layout = new QFormLayout;
    ffmpeg_stdout->setFixedHeight(320);
    ffmpeg_stdout->setReadOnly(true);
    ffmpeg_out_layout->addRow(ffmpeg_stdout);
    ffmpeg_out->setLayout(ffmpeg_out_layout);
    ffmpeg_out->setModal(true);
    ffmpeg_out->setFixedSize(512, 384);
    ffmpeg_out->setWindowTitle("ffmpeg conversion");
    ffmpeg_out->show();

    ffmpeg->start(command);
}

void NieRAutomataMediaUtil::ffmpeg_readyOut(){
    ffmpeg_stdoutstr.append(ffmpeg->readAllStandardOutput());
    ffmpeg_stdout->setText(ffmpeg_stdoutstr);
    ffmpeg_stdout->verticalScrollBar()->setValue(ffmpeg_stdout->verticalScrollBar()->maximum());
}

void NieRAutomataMediaUtil::ffmpeg_done(){
    if(QFile::exists(outputPath->text() + "/" + outputFileName->text())){
        ffmpeg_stdoutstr.append("Success.");
    }else{
        ffmpeg_stdoutstr.append("Failure");
    }
    ffmpeg_stdout->setText(ffmpeg_stdoutstr);
    ffmpeg_stdout->verticalScrollBar()->setValue(ffmpeg_stdout->verticalScrollBar()->maximum());
}

void NieRAutomataMediaUtil::proc_output_destination(){
    outputDir = QFileDialog::getExistingDirectory(this, "Select output directory", dir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    outputPath->setText(outputDir);
}

NieRAutomataMediaUtil::~NieRAutomataMediaUtil(){

}
