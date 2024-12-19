#ifndef AREACHARTVIEW_H
#define AREACHARTVIEW_H

#include "calculation.h"

#include <QCoreApplication>
#include <QAreaSeries>
#include <QChart>
#include <QLineSeries>
#include <QSplineSeries>
#include <QChartView>
#include <QValueAxis>

#include <QComboBox>
#include <QCheckBox>
#include <QSlider>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

#include <QGridLayout>
#include <QPen>
#include <QBrush>
#include <QGestureEvent>

class AreaChartView : public QWidget
{
    Q_OBJECT

public:
    explicit AreaChartView(QWidget *parent = nullptr);
    void setScanData(const QList<XmlScanData> &data);

private:
    int comboBoxCurrentIndex;
    QPoint lastMousePos;

    /************Zoom Rechteck************/
    QGraphicsRectItem *rubberBandRectItem = nullptr;  // Halte das Rechteck als Item
    bool isRubberBandActive;
    QRect rubberBandRect;
    QPointF rubberBandStart;
  //  QPointF rubberBandEnd;

    QComboBox *m_comboBox;
    QCheckBox *m_checkFrontView, *m_checkBoxPipe, *m_checkBoxLineSeries, *m_checkBoxFullView;
    QCheckBox *m_checkFixAxis;
    QSlider *m_slider;
    QLineEdit *m_lineEditFixAxis;
    QPushButton *m_buttonResize;
    QLabel *m_labelMousePoint;

    QChartView *areaChartView;  // QChartView für die Anzeige des Diagramms
    QChart *areachart;

    QValueAxis *axisX;    // Achsen definieren
    QValueAxis *axisY;

    QLineSeries *leftareaSeries; //Punkte Serien definieren
    QLineSeries *rightareaSeries;
    QAreaSeries *areaSeries;
    QLineSeries *lineSeriesRight;
    QLineSeries *lineSeriesLeft;
    QList<QLineSeries*> lineSerieList;

    QStringList stringlistrad;
    QList<XmlScanData> scan_datalist;



protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void updateAreaChart();
    void updateSliderMove(int pos);
    void updateComboBox();

};

#endif // AREACHARTVIEW_H
