#include <math.h>

#include <QtWidgets>
#include <QtNetwork>
#include "slippymap.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

uint qHash(const QPoint& p)
{
    return p.x() * 17 ^ p.y();
}

// tile size in pixels
const int tdim = 256;

QPointF tileForCoordinate(qreal lat, qreal lng, int zoom)
{
    qreal zn = static_cast<qreal>(1 << zoom);
    qreal tx = (lng + 180.0) / 360.0;
    qreal ty = (1.0 - log(tan(lat * M_PI / 180.0) +
                          1.0 / cos(lat * M_PI / 180.0)) / M_PI) / 2.0;
    return QPointF(tx * zn, ty * zn);
}

qreal longitudeFromTile(qreal tx, int zoom)
{
    qreal zn = static_cast<qreal>(1 << zoom);
    qreal lat = tx / zn * 360.0 - 180.0;
    return lat;
}

qreal latitudeFromTile(qreal ty, int zoom)
{
    qreal zn = static_cast<qreal>(1 << zoom);
    qreal n = M_PI - 2 * M_PI * ty / zn;
    qreal lng = 180.0 / M_PI * atan(0.5 * (exp(n) - exp(-n)));
    return lng;
}

SlippyMap::SlippyMap(int w, int h, int z, qreal ltd, qreal lng, QObject *parent) :
    QObject(parent),
    width(w),
    height(h),
    zoom(z),
    latitude(ltd),
    longitude(lng)
{
    m_emptyTile = QPixmap(tdim, tdim);
    m_emptyTile.fill(Qt::lightGray);

    QNetworkDiskCache *cache = new QNetworkDiskCache;
    cache->setCacheDirectory(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
    m_manager.setCache(cache);
    connect(&m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleNetworkData(QNetworkReply*)));
}

void SlippyMap::invalidate()
{
    if (width <= 0 || height <= 0)
        return;

    QPointF ct = tileForCoordinate(latitude, longitude, zoom);
    qreal tx = ct.x();
    qreal ty = ct.y();

    // top-left corner of the center tile
    int xp = width / 2 - (tx - floor(tx)) * tdim;
    int yp = height / 2 - (ty - floor(ty)) * tdim;

    // first tile vertical and horizontal
    int xa = (xp + tdim - 1) / tdim;
    int ya = (yp + tdim - 1) / tdim;
    int xs = static_cast<int>(tx) - xa;
    int ys = static_cast<int>(ty) - ya;

    // offset for top-left tile
    m_offset = QPoint(xp - xa * tdim, yp - ya * tdim);

    // last tile vertical and horizontal
    int xe = static_cast<int>(tx) + (width - xp - 1) / tdim;
    int ye = static_cast<int>(ty) + (height - yp - 1) / tdim;

    // build a rect
    m_tilesRect = QRect(xs, ys, xe - xs + 1, ye - ys + 1);

    if (m_url.isEmpty())
        download();

    emit updated(QRect(0, 0, width, height));
}

void SlippyMap::render(QPainter *p, const QRect &rect)
{
    for (int x = 0; x <= m_tilesRect.width(); ++x)
        for (int y = 0; y <= m_tilesRect.height(); ++y) {
            QPoint tp(x + m_tilesRect.left(), y + m_tilesRect.top());
            QRect box = tileRect(tp);
            if (rect.intersects(box)) {
                if (m_tilePixmaps.contains(tp))
                    p->drawPixmap(box, m_tilePixmaps.value(tp));
                else
                    p->drawPixmap(box, m_emptyTile);
            }
        }
}

void SlippyMap::pan(const QPoint &delta)
{
    QPointF dx = QPointF(delta) / qreal(tdim);
    QPointF center = tileForCoordinate(latitude, longitude, zoom) - dx;
    latitude = latitudeFromTile(center.y(), zoom);
    longitude = longitudeFromTile(center.x(), zoom);
    invalidate();
}

QPointF SlippyMap::coordinateFromPoint(const QPoint &pos)
{
    QPoint center(width / 2, height / 2);
    QPoint delta = center - pos;
    QPointF dx = QPointF(delta) / qreal(tdim);
    QPointF p = tileForCoordinate(latitude, longitude, zoom) - dx;
    qreal latitude = latitudeFromTile(p.y(), zoom);
    qreal longitude = longitudeFromTile(p.x(), zoom);
    p.setX(latitude);
    p.setY(longitude);
    return p;
}

QPointF SlippyMap::worldCoordinate(qreal lat, qreal lng)
{
    qreal sinY = sin(lat * M_PI / 180);
    sinY = qMin(qMax(sinY, -0.9999), 0.9999);
    qreal x = tdim * (0.5 + lng / 360);
    qreal y = tdim * (0.5 - log((1 + sinY) / (1 - sinY)) / (4 * M_PI));
    return QPointF(x, y);
}

QPoint SlippyMap::pixelCoordinate(qreal lat, qreal lng)
{
    QPointF pf = worldCoordinate(lat, lng);
    return QPoint(qFloor(pf.x() * (1 << zoom)), qFloor(pf.y() * (1 << zoom)));
}

void SlippyMap::handleNetworkData(QNetworkReply *reply)
{
    QImage img;
    QPoint tp = reply->request().attribute(QNetworkRequest::User).toPoint();
    QUrl url = reply->url();
    if (!reply->error())
        if (!img.load(reply, 0))
            img = QImage();
    reply->deleteLater();
    m_tilePixmaps[tp] = QPixmap::fromImage(img);
    if (img.isNull())
        m_tilePixmaps[tp] = m_emptyTile;
    emit updated(tileRect(tp));

    // purge unused spaces
    QRect bound = m_tilesRect.adjusted(-2, -2, 2, 2);
    foreach(QPoint tp, m_tilePixmaps.keys())
    if (!bound.contains(tp))
        m_tilePixmaps.remove(tp);

    download();
}

void SlippyMap::download()
{
    QPoint grab(0, 0);
    for (int x = 0; x <= m_tilesRect.width(); ++x)
        for (int y = 0; y <= m_tilesRect.height(); ++y) {
            QPoint tp = m_tilesRect.topLeft() + QPoint(x, y);
            if (!m_tilePixmaps.contains(tp)) {
                grab = tp;
                break;
            }
        }
    if (grab == QPoint(0, 0)) {
        m_url = QUrl();
        return;
    }

    // Open Street Map
    //QString path = "http://tile.openstreetmap.org/%1/%2/%3.png";
    // Google Maps. lyrs parameter: y = hybrid, s = satelite, t = train, m = map
    //QString path = "http://mt1.google.com/vt/lyrs=m&x=%2&y=%3&z=%1";
    // Yandex Maps
    //QString path = "http://vec01.maps.yandex.net/tiles?l=map&x=%2&y=%3&z=%1";
    // Internal map. Yandex API tiles.
    //QString path = "http://localhost:81/map/%1/tile-%2-%3.jpg";
    // Internal map. MapTiler tiles.
    //QString path = "http://localhost:81/map/%1/%2/%3.png";
    QString path = "http://tile.openstreetmap.org/%1/%2/%3.png";
    m_url = QUrl(path.arg(zoom).arg(grab.x()).arg(grab.y()));
    QNetworkRequest request;
    request.setUrl(m_url);
    request.setRawHeader("User-Agent", "Nokia (Qt) Graphics Dojo 1.0");
    request.setAttribute(QNetworkRequest::User, QVariant(grab));
    m_manager.get(request);
}

QRect SlippyMap::tileRect(const QPoint &tp)
{
    QPoint t = tp - m_tilesRect.topLeft();
    int x = t.x() * tdim + m_offset.x();
    int y = t.y() * tdim + m_offset.y();
    return QRect(x, y, tdim, tdim);
}
