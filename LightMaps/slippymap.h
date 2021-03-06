#ifndef SLIPPYMAP_H
#define SLIPPYMAP_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QPixmap>
#include <QUrl>

class QNetworkReply;
class QPainter;

class SlippyMap : public QObject
{
    Q_OBJECT
public:
    explicit SlippyMap(int, int, int, qreal, qreal, QObject *parent = nullptr);

    void invalidate();
    void render(QPainter *p, const QRect &rect);
    void pan(const QPoint &delta);
    QPointF coordinateFromPoint(const QPoint &pos);
    QPointF worldCoordinate(qreal lat, qreal lng);
    QPoint pixelCoordinate(qreal lat, qreal lng);

    int width;
    int height;
    int zoom;
    qreal latitude;
    qreal longitude;

signals:
    void updated(const QRect &rect);

private slots:
    void handleNetworkData(QNetworkReply *reply);
    void download();

protected:
    QRect tileRect(const QPoint &tp);

private:
    QPoint m_offset;
    QRect m_tilesRect;
    QPixmap m_emptyTile;
    QHash<QPoint, QPixmap> m_tilePixmaps;
    QNetworkAccessManager m_manager;
    QUrl m_url;
};

#endif // SLIPPYMAP_H
