#include "nierautomatamediautil.h"

#include "namfilehandler.h"

NieRAutomataMediaUtil::NieRAutomataMediaUtil(QWidget *parent)
    : QWidget(parent){
    QStringList fileFilters;

    dekaasvariabel = new QWidget(this);
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
    //fileview->setRootIndex(filemodel->index("C:/Program Files (x86)/Steam/SteamApps/common/NieRAutomata"));
    fileview->setRootIndex(filemodel->index("F:/Steam/SteamApps/common/NieRAutomata"));
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

    dekaasvariabel->setLayout(this->layout);
    outer->addWidget(dekaasvariabel);
    setLayout(outer);
    setWindowTitle("NieR:Automata™ Media Tool");
    setFixedSize(960, 512);
}

void NieRAutomataMediaUtil::open(){
    dir = QFileDialog::getExistingDirectory(this, "Select game directory", "C:\\", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    fileview->setRootIndex(filemodel->index(dir));
}

void NieRAutomataMediaUtil::fileSelect(const QModelIndex &index){
    while(formRight->rowCount() > 3){
        formRight->removeRow(3);
    }
    int type = 0;
    info = new QFileInfo(filemodel->fileInfo(index));
    if(info->isFile()){
        sFile->setText(info->fileName());
        sFileType->setText(info->suffix());
        desc->setText("File type not (yet) implemented.");
    }else{
        sFile->setText("<no file selected>");
        sFileType->setText("<no file selected>");
        desc->setText("<no file selected>");
    }
    if(info->suffix() == "usm"){
        type = 1;
    }
    switch(type){
        case 1:
            init_usm();
    }
}


NieRAutomataMediaUtil::~NieRAutomataMediaUtil(){

}
