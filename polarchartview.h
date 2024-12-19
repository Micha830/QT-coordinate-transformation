#ifndef POLARCHARTVIEW_H
#define POLARCHARTVIEW_H

#include "calculation.h"

#include <QPolarChart>
#include <QChart>
#include <QScatterSeries>
#include <QSplineSeries>
#include <QValueAxis>
#include <QChartView>
#include <QValueAxis>
#include <QCategoryAxis>

#include <QComboBox>
#include <QCheckBox>
#include <QSlider>

#include <QGridLayout>
#include <QPen>
#include <QBrush>

class PolarChartView : public QWidget
{
    Q_OBJECT



public:
    explicit PolarChartView(QWidget *parent = nullptr);

    void setScanData(const QList<XmlScanData> &data);

private:

    int comboBoxCurrentIndex;

    QComboBox *m_comboBox;
    QCheckBox *m_checkSplineSerie, *m_checkScatterSerie;
    QSlider *m_slider;

    QPolarChart  *polarchart;
    QChartView *polarChartView;  // QChartView für die Anzeige des Diagramms

    QValueAxis *angularAxis;    // Achsen definieren
    QValueAxis *radialAxis;

    QScatterSeries *scatterSeries;
    QSplineSeries *splineSeries;

    QStringList stringlistdepth;
    QList<XmlScanData> scan_datalist;

private slots:
    void updatePolarChart();
    void updateSliderMove(int pos);

signals:
    void tableUpdated(const int &comboBoxCurrentIndex);

};

#endif // POLARCHARTVIEW_H
