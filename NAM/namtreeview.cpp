#include "namtreeview.h"

#include <QKeyEvent>
#include <QModelIndex>
#include <QAbstractItemModel>

NAMTreeView::NAMTreeView(){

}

void NAMTreeView::keyPressEvent(QKeyEvent *event){
    QTreeView::keyPressEvent(event);
    if(event->key() == Qt::Key_Down || event->key() == Qt::Key_Up || event->key() == Qt::Key_Left || event->key() == Qt::Key_Right || event->key() == Qt::Key_W || event->key() == Qt::Key_S || event->key() == Qt::Key_A || event->key() == Qt::Key_D){
        emit clicked(this->currentIndex());
    }
}
