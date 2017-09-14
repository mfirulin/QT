#ifndef LIGHTMAPS_H
#define LIGHTMAPS_H

#include <QWidget>

class SlippyMap;
class QSlider;
class QLabel;

class LightMaps : public QWidget
{
    Q_OBJECT
public:
    explicit LightMaps(QWidget *parent = nullptr);
    void setCenter(qreal lat, qreal lng);

public slots:
    void toggleNightMode();
    QPointF getCoordinate();
    void copyCoordinateToClipboard();

protected:
    void resizeEvent(QResizeEvent *);
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void displayScale();

private slots:
    void updateMap(const QRect &r);
    void zoomMap();

private:
    static const int ZOOM_MIN = 3;
    static const int ZOOM_MAX = 19;
    static const int ZOOM_DEFAULT = 10;
    SlippyMap *m_map;
    bool pressed;
    bool snapped;
    QPoint pressPos;
    QPoint dragPos;
    QPixmap zoomPixmap;
    QPixmap maskPixmap;
    bool invert;
    QSlider *zoomSlider;
    QLabel *coordinateLabel;
    QLabel *scaleTextLabel;
    QLabel *scaleImageLabel;
};

#endif // LIGHTMAPS_H
