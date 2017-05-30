#include "nierautomatamediautil.h"
#include <QApplication>

int main(int argc, char *argv[]){
    QApplication app(argc, argv);
    NieRAutomataMediaUtil nmd;
    nmd.show();
    return app.exec();
}
