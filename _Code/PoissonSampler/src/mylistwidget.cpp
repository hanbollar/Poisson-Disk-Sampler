#include "mylistwidget.h"

MyListWidget::MyListWidget(QWidget* q) : QListWidget(q) {
    this->setFocusPolicy(Qt::ClickFocus);
}

void MyListWidget::slot_addHalfEdge(QListWidgetItem * it) {
    QListWidget::addItem(it);
}

void MyListWidget::slot_addVertex(QListWidgetItem * it) {
    QListWidget::addItem(it);
}

void MyListWidget::slot_addFace(QListWidgetItem * it) {
    QListWidget::addItem(it);
}

void MyListWidget::slot_selectItem(QListWidgetItem * it) {
    QListWidget::setCurrentItem(it);
}
