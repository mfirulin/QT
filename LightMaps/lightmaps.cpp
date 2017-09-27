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
//     QPalette palette;
//     palette.setColor(QPalette::WindowText, QColor(Qt::darkGray).darker(150));
//     coordinateLabel->setPalette(palette);
    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addStretch();
    vLayout->addWidget(coordinateLabel);
        
    scaleTextLabel = new QLabel;
    scaleImageLabel = new QLabel;
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(scaleTextLabel);
    hLayout->addWidget(scaleImageLabel);

    QVBoxLayout *vLayout2 = new QVBoxLayout;
    vLayout2->addWidget(zoomSlider);
    vLayout2->setAlignment(zoomSlider, Qt::AlignRight);
    vLayout2->addLayout(hLayout);    
        
    QHBoxLayout *hLayout2 = new QHBoxLayout;
    hLayout2->setMargin(10);
    hLayout2->addLayout(vLayout);
    hLayout2->addStretch();
    hLayout2->addLayout(vLayout2);

    this->setLayout(hLayout2);

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
    displayScale();
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
    p.setFont(QFont("Helvetica", 8, QFont::StyleNormal));
    p.drawText(rect(),  Qt::AlignBottom | Qt::AlignRight | Qt::TextWordWrap,
               "OpenStreetMap.org");
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
        QApplication::setOverrideCursor(Qt::SizeAllCursor);
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
    QApplication::restoreOverrideCursor();
    update();
}

void LightMaps::wheelEvent(QWheelEvent *event)
{
    if(event->delta() > 0 && m_map->zoom < ZOOM_MAX)
        ++m_map->zoom;
    else if(event->delta() < 0 && m_map->zoom > ZOOM_MIN)
        --m_map->zoom;
    else
        return;

    zoomSlider->setValue(m_map->zoom);
    m_map->invalidate();
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

void LightMaps::displayScale()
{
    qreal zn = static_cast<qreal>(1 << m_map->zoom);
    qreal latitude = m_map->latitude / 100;
    qreal resolution = 156543.03/*meters/pixel on equator*/ * cos(latitude) / zn;
//    qreal scl = physicalDpiX() * 39.37/*inches/meter*/ * resolution;

    qreal dist;
    switch(m_map->zoom) {
    case 19: dist = 10; break;
    case 18: dist = 20; break;
    case 17: dist = 40; break;
    case 16: dist = 80; break;
    case 15: dist = 150; break;
    case 14: dist = 350; break;
    case 13: dist = 700; break;
    case 12: dist = 1500; break;
    case 11: dist = 2500; break;
    case 10: dist = 5000; break;
    case 9: dist = 10000; break;
    case 8: dist = 20000; break;
    case 7: dist = 40000; break;
    case 6: dist = 100000; break;
    case 5: dist = 150000; break;
    case 4: dist = 350000; break;
    case 3: dist = 700000; break;
    case 2: dist = 1500000; break;
    case 1: dist = 2500000; break;
    case 0: dist = 5000000; break;
    default: dist = 0;
    }

    int scale = qRound(dist / resolution);

    QPicture pic;
    QPainter painter(&pic);
    //QPen pen(QColor(65, 168, 234)); // Blue color similar to the slider.
    QPen pen(Qt::black);
    pen.setWidth(2);
    painter.setPen(pen);
    painter.drawLine(0, 2, scale, 2); // draw: ------
    painter.drawLine(0, 0, 0, 4); // draw: |-
    painter.drawLine(scale, 0, scale, 4); // draw: -|
    painter.end();

    QString suffix;
    if(dist > 1000) {
        dist /= 1000;
        suffix = tr("km");
    }
    else
        suffix = tr("m");

    QString s = QString::number(dist) + " " + suffix;
    scaleTextLabel->setText(s);
    scaleImageLabel->setPicture(pic);
}
