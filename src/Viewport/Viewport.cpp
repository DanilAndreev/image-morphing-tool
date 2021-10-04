#include "Viewport/Viewport.h"

#include "Stroke/StrokeDrawer.h"

Viewport::Viewport(QWidget *parent) : QWidget(parent) {
}

void Viewport::drawLines(QPainter &painter) {
    QPen pen(Qt::black, 2, Qt::SolidLine);

    StrokeDrawer drawer{};
    for (const Stroke& stroke : this->strokes) {
        drawer.bind(&stroke);
        drawer.draw(painter);
    }
    drawer.unbind();
}

void Viewport::paintEvent(QPaintEvent *event) {
    qDebug() << "Paint event";
    QPainter painter(this);
    drawLines(painter);
    QWidget::paintEvent(event);
}

void Viewport::mouseMoveEvent(QMouseEvent *event) {
    QWidget::mouseMoveEvent(event);
}
