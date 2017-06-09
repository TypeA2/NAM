#include "sidewidget_empty.h"

SideWidget_Empty::SideWidget_Empty() : QWidget(){
    QFormLayout *form = new QFormLayout;

    form->addRow("Filename:", new QLabel("<no file selected>"));
    form->addRow("File type:", new QLabel("<no file selected>"));
    form->addRow("File description:", new QLabel("<no file selected>"));

    this->setLayout(form);
}
