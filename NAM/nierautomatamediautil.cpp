#include "nierautomatamediautil.h"

#include "namfilehandler.h"
#include "sidewidget_wsp.h"

NieRAutomataMediaUtil::NieRAutomataMediaUtil(QWidget *parent) : QWidget(parent){
    outer = new QBoxLayout(QBoxLayout::LeftToRight);
    layout = new QGridLayout;
    filemodel = new QFileSystemModel;
    fileview = new NAMTreeView;
    menuBar = new QMenuBar;
    fileMenu = new QMenu;
    openFolderAct = new QAction("Open");
    exitProgram = new QAction("Exit");;

    QStringList fileFilters;

    fileFilters << "*.cpk" << "*.usm" << "*.wai" << "*.bnk" << "*.wem" << "*.wsp";

    filemodel->setRootPath("C:/Program Files (x86)/Steam/SteamApps/common/NieRAutomata");

    filemodel->setNameFilters(fileFilters);

    filemodel->setNameFilterDisables(false);

    fileview->setModel(filemodel);

    fileview->setRootIndex(filemodel->index("C:/Program Files (x86)/Steam/SteamApps/common/NieRAutomata"));
    //fileview->setRootIndex(filemodel->index("F:/Steam/SteamApps/common/NieRAutomata/data"));

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

    outer->addWidget(fileview);
    outer->addWidget(new SideWidget_Empty);

    outer->setMenuBar(menuBar);

    this->setLayout(outer);
    this->setWindowTitle("NieR:Automata™ Media Tool");
    this->setFixedSize(960, 512);
}

void NieRAutomataMediaUtil::open(){
    dir = QFileDialog::getExistingDirectory(this, "Select game directory", "C:/", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if(!dir.isEmpty()) fileview->setRootIndex(filemodel->index(dir));

    this->fileSelect(this->fileview->currentIndex());
}

void NieRAutomataMediaUtil::fileSelect(const QModelIndex &index){
    this->reset_outer();

    info = new QFileInfo(filemodel->fileInfo(index));

    if(!info->isFile()){
        this->outer->addWidget(new SideWidget_Empty, Qt::AlignLeft);
    }else if(info->suffix() == "wsp"){
        this->outer->addWidget(new SideWidget_WSP(info, this), Qt::AlignLeft);
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
