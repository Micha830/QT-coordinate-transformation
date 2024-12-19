#include "polarchartview.h"

PolarChartView::PolarChartView(QWidget *parent) :
    QWidget(parent),
    polarchart(new QPolarChart ()),
    polarChartView(new QChartView(polarchart, this)),
    angularAxis(new QValueAxis(this)),
    radialAxis(new QValueAxis(this)),
    scatterSeries(new QScatterSeries(this)),
    splineSeries(new QSplineSeries(this))

{
    // Layout setzen
    QGridLayout *layout = new QGridLayout(this);

    m_comboBox = new QComboBox;
    m_checkSplineSerie = new QCheckBox("Spline Serie");
    m_checkScatterSerie = new QCheckBox("Scatter Serie");
    m_slider = new QSlider(Qt::Horizontal, this);

    layout->addWidget(m_slider, 0, 0, 1, 0);
    layout->addWidget(m_comboBox, 1, 0, 1, 0);
    layout->addWidget(m_checkSplineSerie, 2, 0);
    layout->addWidget(m_checkScatterSerie, 2, 1);
    layout->addWidget(polarChartView, 3, 0, 1, 0);

    setLayout(layout);

    m_checkSplineSerie->setCheckState(Qt::Checked);
    m_checkScatterSerie->setCheckState(Qt::Checked);

    polarChartView->setRenderHint(QPainter::Antialiasing);

    angularAxis->setTickCount(9);
    angularAxis->setLabelFormat("%.1f");
    angularAxis->setShadesVisible(true);
    angularAxis->setShadesBrush(QBrush(QColor(249, 249, 255)));

    radialAxis->setTickCount(9);
    radialAxis->setLabelFormat("%d");

    scatterSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    scatterSeries->setMarkerSize(10.0);

  //  polarchart = new QPolarChart ();
    polarchart->addSeries(scatterSeries);
    polarchart->addSeries(splineSeries);
    polarchart->setTitle("Cavern Measurements");

    connect(m_comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePolarChart()));
    connect(m_checkSplineSerie, SIGNAL(clicked(bool)), this, SLOT(updatePolarChart()));
    connect(m_checkScatterSerie, SIGNAL(clicked(bool)), this, SLOT(updatePolarChart()));
    connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(updateSliderMove(int)));
}


void PolarChartView::updatePolarChart()
{
    comboBoxCurrentIndex = m_comboBox->currentIndex();

    bool fixRadialMax = true;

    float ang, rad, radialMin = 0, radialMax = 10;
    qreal ang1, rad1;

    const qreal angularMin = 0;
    const qreal angularMax = 360;

    scatterSeries->clear();
    splineSeries->clear();


    for(int i=0; i < scan_datalist.at(comboBoxCurrentIndex).polarcordinate.size(); i++){

        ang = i* 360/scan_datalist.at(comboBoxCurrentIndex).polarcordinate.size();
        //rad = m_datalist.at(ui->comboBox_visudepth->currentIndex()).polarcordinate.at(i);
        rad = scan_datalist.at(comboBoxCurrentIndex).polarcordinatewithouttilt.at(i);

        ang1 = static_cast<qreal>(ang);
        rad1 = static_cast<qreal>(rad);

        if(m_checkScatterSerie->isChecked())scatterSeries->append(ang1, rad1);
        if(m_checkSplineSerie->isChecked())splineSeries->append(ang1, rad1);

        if(radialMax < rad1)radialMax = rad1;
    }

    rad = scan_datalist.at(comboBoxCurrentIndex).polarcordinatewithouttilt.at(0);
    rad1 = static_cast<qreal>(rad);
    //qDebug() << "abgeschlossen " << rad1 << rad;
    if(m_checkSplineSerie->isChecked())splineSeries->append(360, rad1);


    if(fixRadialMax)radialMax = 20;
    else radialMax+=10;

    polarchart->setTitle("Polar-Diagramm Z: " + QString::number(scan_datalist.at(comboBoxCurrentIndex).z));

    polarchart->addAxis(angularAxis, QPolarChart::PolarOrientationAngular);
    polarchart->addAxis(radialAxis, QPolarChart::PolarOrientationRadial);

    scatterSeries->attachAxis(radialAxis);
    scatterSeries->attachAxis(angularAxis);
    splineSeries->attachAxis(radialAxis);
    splineSeries->attachAxis(angularAxis);

    //radialAxis.
    radialAxis->setRange(radialMin, radialMax);
    // radialAxis->setRange(radialMin, radrange*1.2);
    angularAxis->setRange(angularMin, angularMax);

    polarchart->legend()->hide();
    polarchart->update();

    emit tableUpdated(comboBoxCurrentIndex);

}

void PolarChartView::setScanData(const QList<XmlScanData> &data){

    scan_datalist = data;
    stringlistdepth.clear();

    qDebug() << "neuer Durchlauf polar : " << scan_datalist.size() << " " << data.size();

    for(int i=0; i < scan_datalist.size(); i++){

        stringlistdepth.append("Pos.: " + QString::number(scan_datalist.at(i).scanpos)
                               + ";  Depth: " + QString::number(scan_datalist.at(i).z)
                               + ";  Tilt: " + QString::number(scan_datalist.at(i).tilt));
    }

    m_comboBox->blockSignals(true);
    m_comboBox->clear();
    m_comboBox->addItems(stringlistdepth);
    // m_comboBox->setCurrentIndex(stringlistdepth.size()/2);
    for (int i = 0 ; i < m_comboBox->count() ; ++i) {
        m_comboBox->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);
    }
    m_comboBox->blockSignals(false);

  //  m_slider->setTickInterval(stringlistdepth.size());
    m_slider->blockSignals(true);
    m_slider->setSingleStep(1);
    m_slider->setRange(0, stringlistdepth.size()-1);
    m_slider->blockSignals(false);

    updatePolarChart();

}

void PolarChartView::updateSliderMove(int pos){

   // qDebug() << "move " << m_slider->sliderPosition() << " " << pos;
    m_comboBox->setCurrentIndex(pos);
}
