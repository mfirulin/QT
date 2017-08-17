#include <QtCore>
#include <QtWidgets>
#include <QtNetwork>

#include <math.h>

#include "lightmaps.h"
#include "slippymap.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

LightMaps::LightMaps(QWidget *parent) :
    QWidget(parent),
    pressed(false),
    snapped(false),
    invert(false)
{
    m_map = new SlippyMap(400, 300, ZOOM_DEFAULT, 56.32698, 44.00567/*N.Novgorod*/, this);
    connect(m_map, SIGNAL(updated(QRect)), SLOT(updateMap(QRect)));

    zoomSlider = new QSlider(Qt::Vertical);
    zoomSlider->setRange(ZOOM_MIN, ZOOM_MAX);
    zoomSlider->setPageStep(1);
    zoomSlider->setValue(ZOOM_DEFAULT);
    connect(zoomSlider, &QSlider::valueChanged, this, &LightMaps::zoomMap);

    coordinateLabel = new QLabel;
    QPalette palette;
    palette.setColor(QPalette::WindowText, QColor(Qt::darkGray).darker(150));
    coordinateLabel->setPalette(palette);
    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addStretch();
    vLayout->addWidget(coordinateLabel);

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->setMargin(20);
    hLayout->addLayout(vLayout);
    hLayout->addStretch();
    hLayout->addWidget(zoomSlider);
    this->setLayout(hLayout);

    setMouseTracking(true);
}

void LightMaps::setCenter(qreal lat, qreal lng)
{
    m_map->latitude = lat;
    m_map->longitude = lng;
    m_map->invalidate();
}

void LightMaps::toggleNightMode()
{
    invert = !invert;
    update();
}

void LightMaps::updateMap(const QRect &r)
{
    update(r);
}

void LightMaps::zoomMap()
{
    m_map->zoom = zoomSlider->value();
    m_map->invalidate();
    update();
    this->setFocus();
}

void LightMaps::resizeEvent(QResizeEvent *)
{
    m_map->width = width();
    m_map->height = height();
    m_map->invalidate();
}

void LightMaps::paintEvent(QPaintEvent *event)
{
    QPainter p;
    p.begin(this);
    m_map->render(&p, event->rect());
    p.setPen(Qt::gray);
    p.drawText(rect(),  Qt::AlignBottom | Qt::AlignRight | Qt::TextWordWrap,
               "Map data OpenStreetMap.org");
    p.end();

    if (invert) {
        QPainter p(this);
        p.setCompositionMode(QPainter::CompositionMode_Difference);
        p.fillRect(event->rect(), Qt::white);
        p.end();
    }
}

void LightMaps::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton) {
        pressed = snapped = true;
        pressPos = dragPos = event->pos();
    }
    else if (event->buttons() == Qt::RightButton) {
        pressPos = event->pos();
    }
}

void LightMaps::mouseMoveEvent(QMouseEvent *event)
{
    if (!event->buttons()) {
        QPointF c = m_map->coordinateFromPoint(event->pos());
        coordinateLabel->setText(QString::number(c.x()) + " " + QString::number(c.y()));
        return;
    }

    if (!pressed || !snapped) {
        QPoint delta = event->pos() - pressPos;
        pressPos = event->pos();
        m_map->pan(delta);
        return;
    } else {
        const int threshold = 10;
        QPoint delta = event->pos() - pressPos;
        if (snapped) {
            snapped &= delta.x() < threshold;
            snapped &= delta.y() < threshold;
            snapped &= delta.x() > -threshold;
            snapped &= delta.y() > -threshold;
        }
    }
}

void LightMaps::mouseReleaseEvent(QMouseEvent *)
{
    pressed = false;
    update();
}

void LightMaps::wheelEvent(QWheelEvent *event)
{
    if(event->delta() > 0 && m_map->zoom < ZOOM_MAX)
        ++m_map->zoom;
    else if(event->delta() < 0 && m_map->zoom > ZOOM_MIN)
        --m_map->zoom;

    zoomSlider->setValue(m_map->zoom);
    m_map->invalidate();
    update();
    this->setFocus();
}

QPointF LightMaps::getCoordinate()
{
    return m_map->coordinateFromPoint(pressPos);
}

void LightMaps::copyCoordinateToClipboard()
{
    QPointF c = getCoordinate();
    QString s = QString::number(c.x()) + ", " + QString::number(c.y());
    QApplication::clipboard()->setText(s);
}

void LightMaps::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Left)
        m_map->pan(QPoint(20, 0));
    if (event->key() == Qt::Key_Right)
        m_map->pan(QPoint(-20, 0));
    if (event->key() == Qt::Key_Up)
        m_map->pan(QPoint(0, 20));
    if (event->key() == Qt::Key_Down)
        m_map->pan(QPoint(0, -20));
    if (event->key() == Qt::Key_Z || event->key() == Qt::Key_Select) {
        dragPos = QPoint(width() / 2, height() / 2);
    }
}
