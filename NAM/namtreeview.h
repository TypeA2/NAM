#ifndef NAMTREEVIEW_H
#define NAMTREEVIEW_H

#include <QTreeView>

class NAMTreeView : public QTreeView{
Q_OBJECT

public:
    NAMTreeView();

protected:
    virtual void keyPressEvent(QKeyEvent *event);
};

#endif // NAMTREEVIEW_H
