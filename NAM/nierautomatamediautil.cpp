#include "nierautomatamediautil.h"

#include "sidewidget_wsp.h"
#include "sidewidget_bnk.h"

NieRAutomataMediaUtil::NieRAutomataMediaUtil(QWidget *parent) : QWidget(parent){
    outer = new QBoxLayout(QBoxLayout::LeftToRight);

    filemodel = new QFileSystemModel;
    fileview = new NAMTreeView;
    menuBar = new QMenuBar;
    fileMenu = new QMenu;
    openFolderAct = new QAction("Open");
    exitProgram = new QAction("Exit");
    optionsDialogAct = new QAction("Options");

    QSettings settings(QCoreApplication::applicationDirPath() + "/settings.ini", QSettings::IniFormat);

    if(!settings.allKeys().contains("defaultGamePath") ||
            settings.value("defaultGamePath").toString().isEmpty() ||
            !QDir(settings.value("defaultGamePath").toString()).exists()) settings.setValue("defaultGamePath", "C:/Program Files (x86)/Steam/SteamApps/common/NieRAutomata");

    if(!settings.allKeys().contains("defaultOutputName") ||
            settings.value("defaultOutputName").toString().isEmpty() ||
            settings.value("defaultOutputName").toString().count("%1") < 1 ||
            settings.value("defaultOutputName").toString().count("%2") < 1 ||
            settings.value("defaultOutputName").toString().count(".%3") < 1) settings.setValue("defaultOutputName", "[%1]%2.%3");

    if(!settings.allKeys().contains("defaultFFmpegFlags")) settings.setValue("defaultFFmpegFlags", "-hide_banner");

    if(!settings.allKeys().contains("defaultOutputDir") ||
            settings.value("defaultOutputDir").toString().isEmpty() ||
            !QDir(settings.value("defaultOutputDir").toString()).exists()) settings.setValue("defaultOutputDir", QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));

    this->optsGamePath = settings.value("defaultGamePath", "C:/Program Files (x86)/Steam/SteamApps/common/NieRAutomata").toString();
    this->optsOutputName = settings.value("defaultOutputName", "[%1]%2.%3").toString();
    this->optsFFmpegFlags = settings.value("defaultFFmpegFlags", "-hide_banner").toString();
    this->optsDefaultOutputDirectory = settings.value("defaultOutputDir", QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)).toString();

    QStringList fileFilters;

    //fileFilters << "*.cpk" << "*.usm" << "*.wai" << "*.bnk" << "*.wem" << "*.wsp";
    fileFilters << "*.wsp" << "*.bnk";

    filemodel->setRootPath("C:/");

    filemodel->setNameFilters(fileFilters);

    filemodel->setNameFilterDisables(false);

    fileview->setModel(filemodel);

    fileview->setRootIndex(filemodel->index(this->optsGamePath));

    fileview->setColumnWidth(0, 300);
    fileview->setColumnWidth(1, 70);
    fileview->setColumnWidth(2, 70);

    fileview->hideColumn(3);

    fileview->setFixedWidth(480);

    fileview->setSortingEnabled(true);

    fileview->setSelectionMode(QAbstractItemView::SingleSelection);

    openFolderAct->setShortcuts(QKeySequence::Open);
    openFolderAct->setStatusTip("Select the game directory");

    exitProgram->setShortcuts(QKeySequence::Quit);
    exitProgram->setStatusTip("Quit");

    this->optionsDialogAct->setStatusTip("Options");

    connect(openFolderAct, &QAction::triggered, this, &NieRAutomataMediaUtil::open);
    connect(exitProgram, &QAction::triggered, this, &QWidget::close);
    connect(this->optionsDialogAct, &QAction::triggered, this, &NieRAutomataMediaUtil::options);

    connect(fileview, SIGNAL(clicked(const QModelIndex&)), this, SLOT(fileSelect(const QModelIndex &)));

    fileMenu->setTitle("File");

    fileMenu->addAction(openFolderAct);
    this->fileMenu->addAction(this->optionsDialogAct);

    fileMenu->addSeparator();

    fileMenu->addAction(exitProgram);

    menuBar->addMenu(fileMenu);

    outer->addWidget(fileview);
    outer->addWidget(new SideWidget_Empty);

    outer->setMenuBar(menuBar);

    this->setLayout(outer);
    this->setWindowTitle("NieR:Automata™ Media Tool");
    this->setFixedSize(960, 512);
}

void NieRAutomataMediaUtil::open(){
    QString dir = QFileDialog::getExistingDirectory(this, "Select game directory", this->optsGamePath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if(!dir.isEmpty()) fileview->setRootIndex(filemodel->index(dir));

    this->fileSelect(this->fileview->currentIndex());
}

void NieRAutomataMediaUtil::options(){
    optionsDialog = new QDialog(this);

    QFormLayout *optionsLayout = new QFormLayout;

    QPushButton *selectDefaultGamePath = new QPushButton("Select directory");
    QPushButton *selectDefaultOutputDirectory = new QPushButton("Select directory");
    QPushButton *optionsSaveAndClose = new QPushButton("Save and close");

    this->optionsDefaultGamePath = new QLineEdit(this->optsGamePath);
    this->optionsDefaultOutputDirectory = new QLineEdit(this->optsDefaultOutputDirectory);
    this->optionsDefaultOutputName = new QLineEdit(this->optsOutputName);
    this->optionsDefaultFFmpegFlags = new QLineEdit(this->optsFFmpegFlags);

    selectDefaultGamePath->setFixedWidth(128);
    selectDefaultOutputDirectory->setFixedWidth(128);
    optionsSaveAndClose->setFixedWidth(128);

    optionsSaveAndClose->setDefault(true);

    connect(selectDefaultGamePath, SIGNAL(released()), this, SLOT(selectDefaultGamePathSlot()));
    connect(selectDefaultOutputDirectory, SIGNAL(released()), this, SLOT(selectDefaultOutputDirectorySlot()));
    connect(optionsSaveAndClose, SIGNAL(released()), this, SLOT(optionsSaveAndCloseSlot()));

    optionsLayout->addRow("Game path:", this->optionsDefaultGamePath);
    optionsLayout->addRow("", selectDefaultGamePath);
    optionsLayout->addRow("Output directory:", this->optionsDefaultOutputDirectory);
    optionsLayout->addRow("", selectDefaultOutputDirectory);
    optionsLayout->addRow("Output filename format:", this->optionsDefaultOutputName);
    optionsLayout->addRow("", new QLabel("%1: file numbering, %2: source filename, .%3: file extension."));
    optionsLayout->addRow("FFmpeg flags:", this->optionsDefaultFFmpegFlags);
    optionsLayout->addRow("", optionsSaveAndClose);

    this->optionsDialog->setLayout(optionsLayout);
    this->optionsDialog->setModal(true);
    this->optionsDialog->setWindowTitle("Options");
    this->optionsDialog->setFixedWidth(512);
    this->optionsDialog->setWindowFlags(optionsDialog->windowFlags() & ~Qt::WindowCloseButtonHint);

    this->optionsDialog->show();
}

void NieRAutomataMediaUtil::selectDefaultOutputDirectorySlot(){
    QString dir = QFileDialog::getExistingDirectory(this, "Select output directory", "C:/", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if(!dir.isEmpty()) this->optionsDefaultOutputDirectory->setText(dir);
}

void NieRAutomataMediaUtil::selectDefaultGamePathSlot(){
    QString dir = QFileDialog::getExistingDirectory(this, "Select game directory", this->optsGamePath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if(!dir.isEmpty()) this->optionsDefaultGamePath->setText(dir);
}

void NieRAutomataMediaUtil::optionsSaveAndCloseSlot(){
    QString currentModelPath = QFileInfo(this->filemodel->filePath(this->fileview->currentIndex())).absolutePath();

    QString defaultGamePath;

    if(!this->optionsDefaultGamePath->text().isEmpty()){
        if(QDir(this->optionsDefaultGamePath->text()).exists()){
            defaultGamePath = this->optionsDefaultGamePath->text();
        }else{
            QMessageBox::warning(this, "Options", "Specified directory does not exist.");

            return;
        }
    }

    QString defaultOutputDir;

    if(!this->optionsDefaultOutputDirectory->text().isEmpty()){
        if(QDir(this->optionsDefaultOutputDirectory->text()).exists()){
            defaultOutputDir = this->optionsDefaultOutputDirectory->text();
        }else{
            QMessageBox::warning(this, "Options", "Specified output directory does not exist.");

            return;
        }
    }

    QString defaultOutputName;

    if(this->optionsDefaultOutputName->text().count("%1") < 1){
        QMessageBox::warning(this, "Options", "%1 is missing in output filename format.");

        return;
    }else if(this->optionsDefaultOutputName->text().count("%2") < 1){
        QMessageBox::warning(this, "Options", "%2 is missing in output filename format.");

        return;
    }else if(this->optionsDefaultOutputName->text().count(".%3") < 1){
        QMessageBox::warning(this, "Options", ".%3 is missing in output filename format. (dot is also required)");

        return;
    }else defaultOutputName = this->optionsDefaultOutputName->text();

    QSettings settings(QCoreApplication::applicationDirPath() + "/settings.ini", QSettings::IniFormat);

    settings.setValue("defaultGamePath", defaultGamePath);
    settings.setValue("defaultOutputName", defaultOutputName);
    settings.setValue("defaultFFmpegFlags", this->optionsDefaultFFmpegFlags->text());
    settings.setValue("defaultOutputDir", defaultOutputDir);

    this->optsGamePath = settings.value("defaultGamePath", "C:/Program Files (x86)/Steam/SteamApps/common/NieRAutomata").toString();
    this->optsOutputName = settings.value("defaultOutputName", "[%1]%2.%3").toString();
    this->optsFFmpegFlags = settings.value("defaultFFmpegFlags", "-hide_banner").toString();
    this->optsDefaultOutputDirectory = settings.value("defaultOutputDir", QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)).toString();

    if(currentModelPath != this->optsGamePath){
        fileview->setRootIndex(filemodel->index(this->optsGamePath));

        fileSelect(this->fileview->currentIndex());
    }

    this->optionsDialog->close();
}

void NieRAutomataMediaUtil::fileSelect(const QModelIndex &index){
    if(this->outer->itemAt(1)->widget()->metaObject()->className() == QString("SideWidget_BNK")){
       delete this->outer->takeAt(1)->widget();
    }

    this->reset_outer();

    info = new QFileInfo(filemodel->fileInfo(index));

    if(!info->isFile()){
        this->outer->addWidget(new SideWidget_Empty, Qt::AlignLeft);
    }else if(info->suffix() == "wsp"){
        this->outer->addWidget(new SideWidget_WSP(info, this), Qt::AlignLeft);
    }else if(info->suffix() == "bnk"){
        this->outer->addWidget(new SideWidget_BNK(info, this), Qt::AlignLeft);
    }else{
        QFormLayout *form = new QFormLayout;
        QWidget *sideWidget = new QWidget;

        form->addRow("Filename:", new QLabel(info->fileName()));
        form->addRow("File type:", new QLabel(info->suffix()));
        form->addRow("File description:", new QLabel("File type not implemented (yet)."));

        sideWidget->setLayout(form);

        this->outer->addWidget(sideWidget);
    }
}

void NieRAutomataMediaUtil::reset_outer(){
    while(this->outer->count() > 1){
        this->outer->itemAt(1)->widget()->setParent(NULL);

        delete outer->itemAt(1);
    }
}

NieRAutomataMediaUtil::~NieRAutomataMediaUtil(){
    if(QDir(QCoreApplication::applicationDirPath() + "/Temp").exists()){
        QDir temp(QCoreApplication::applicationDirPath() + "/Temp");

        foreach(QString f, temp.entryList()){
            temp.remove(f);
        }
    }
}
