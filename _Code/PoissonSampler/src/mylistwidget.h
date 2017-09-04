#ifndef MYLISTWIDGET_H
#define MYLISTWIDGET_H


#include <QListWidget>
#include <QListWidgetItem>
#include "vertex.h"
#include "face.h"
#include "halfedge.h"

class MyListWidget : public QListWidget
{
    Q_OBJECT
public:
    MyListWidget(QWidget *q);

public slots:
      void slot_addHalfEdge(QListWidgetItem *);
      void slot_addVertex(QListWidgetItem *);
      void slot_addFace(QListWidgetItem *);

      void slot_selectItem(QListWidgetItem *);
};

#endif // MYLISTWIDGET_H
