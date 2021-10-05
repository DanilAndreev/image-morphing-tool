#include "Viewport/Viewport.h"

#include "Stroke/StrokeDrawer.h"

Viewport::Viewport(QWidget *parent) : QWidget(parent) {
}

void Viewport::drawLines(QPainter &painter) {
    QPen pen(Qt::black, 2, Qt::SolidLine);
    StrokeDrawer drawer{};
    for (const Stroke &stroke : this->strokes) {
        drawer.bind(&stroke);
        drawer.draw(painter);
    }
    if (!this->currentStroke.empty()) {
        Stroke current = Stroke{this->currentStroke};
        drawer.bind(&current);
        drawer.draw(painter);
    }
    drawer.unbind();
}

void Viewport::paintEvent(QPaintEvent *event) noexcept {
    QPainter painter(this);
    drawLines(painter);
    QWidget::paintEvent(event);
}

void Viewport::mouseMoveEvent(QMouseEvent *event) noexcept {
    if (event->buttons().testFlag(Qt::MouseButton::LeftButton)) {
        this->currentStroke.push_back(event->pos());
        this->repaint();
    }
    QWidget::mouseMoveEvent(event);
}

void Viewport::mousePressEvent(QMouseEvent *event) noexcept {
    if (event->button() == Qt::MouseButton::LeftButton) {
        this->currentStroke.clear();
    }
    QWidget::mousePressEvent(event);
}

void Viewport::mouseReleaseEvent(QMouseEvent *event) noexcept {
    if (event->button() == Qt::LeftButton && !this->currentStroke.empty()) {
        this->strokes.emplace_back(Stroke{this->currentStroke});
        this->repaint();
    }
    QWidget::mouseReleaseEvent(event);
}
