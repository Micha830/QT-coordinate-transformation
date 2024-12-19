#include "areachartview.h"
#include <QApplication>
#include <QRectF>

AreaChartView::AreaChartView(QWidget *parent) :
    QWidget(parent),
    areaChartView(new QChartView(this)),
    areachart(new QChart()),
    leftareaSeries(new QLineSeries(this)),
    rightareaSeries(new QLineSeries(this)),
    areaSeries(nullptr),
    lineSeriesRight(nullptr),
    lineSeriesLeft(nullptr)
{

    this->installEventFilter(this);

  //  QApplication::restoreOverrideCursor();
   // areaChartView = new QChartView(this);
    areaChartView->setChart(areachart);
    areaChartView->setRenderHint(QPainter::Antialiasing);
    areaChartView->setDragMode(QChartView::NoDrag);  // Setze DragMode
    areaChartView->setAttribute(Qt::WA_TransparentForMouseEvents);
    //areaChartView->setDragMode(QChartView::RubberBandDrag);  // Setze DragMode
    areaChartView->setMouseTracking(true);  // Aktiviere die Mausverfolgung
    setMouseTracking(true);  // Aktiviere die Mausverfolgung

    QGridLayout *layout = new QGridLayout(this);

    m_comboBox = new QComboBox;
    m_checkFrontView = new QCheckBox("set north view");
    m_checkBoxFullView = new QCheckBox("Full view");
    m_checkBoxPipe = new QCheckBox("Pipe");
    m_checkBoxLineSeries = new QCheckBox("Lines");
    m_slider = new QSlider(Qt::Horizontal, this);
    m_buttonResize = new QPushButton("Resize");
    m_labelMousePoint = new QLabel("Point: ");

    m_checkFixAxis = new QCheckBox("set fix X axis");
    m_lineEditFixAxis = new QLineEdit("20");

    layout->addWidget(m_slider, 0, 0, 1 , 4);
    layout->addWidget(m_comboBox, 1, 0, 1 , 4);
    layout->addWidget(m_checkFrontView, 2, 0, 1, 1);
    layout->addWidget(m_checkBoxFullView, 2, 1, 1, 1);
    layout->addWidget(m_checkBoxPipe, 2, 2, 1, 1);
    layout->addWidget(m_checkBoxLineSeries, 2, 3, 1, 1);

    layout->addWidget(m_buttonResize, 3, 0, 1, 1);
    layout->addWidget(m_checkFixAxis, 3, 2, 1, 1);
    layout->addWidget(m_lineEditFixAxis, 3, 3, 1, 1);
    layout->addWidget(areaChartView, 4, 0, 1, 4);
    layout->addWidget(m_labelMousePoint, 5, 0, 1, 4);

    setLayout(layout);

    m_labelMousePoint->setMaximumHeight(15);

    m_checkBoxLineSeries->setCheckState(Qt::Checked);
    m_checkFrontView->setCheckState(Qt::Checked);
    m_checkBoxFullView->setCheckState(Qt::Checked);
    m_checkFixAxis->setCheckState(Qt::Checked);


    areaSeries = new QAreaSeries(leftareaSeries, rightareaSeries);
    areachart->addSeries(areaSeries);
    areachart->setTitle("Measurements");


    QLinearGradient gradient(QPointF(1, 0), QPointF(0, 0));
    gradient.setColorAt(0.1, 0x6cc63c);
    gradient.setColorAt(1.0, 0x26f626);
    gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    areaSeries->setBrush(gradient);
    areaSeries->setOpacity(0.5);// Setze die Transparenz



    connect(m_comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateAreaChart()));
    connect(m_checkFrontView, SIGNAL(clicked(bool)), this, SLOT(updateAreaChart()));
    connect(m_checkBoxFullView, SIGNAL(clicked(bool)), this, SLOT(updateComboBox()));
    connect(m_checkBoxPipe, SIGNAL(clicked(bool)), this, SLOT(updateAreaChart()));
    connect(m_checkBoxLineSeries, SIGNAL(clicked(bool)), this, SLOT(updateAreaChart()));
    connect(m_checkFixAxis, SIGNAL(clicked(bool)), this, SLOT(updateAreaChart()));
    connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(updateSliderMove(int)));
    connect(m_lineEditFixAxis, SIGNAL(textChanged(QString)), this, SLOT(updateAreaChart()));
    connect(m_buttonResize, SIGNAL(clicked(bool)), this, SLOT(updateAreaChart()));
}

bool AreaChartView::eventFilter(QObject *obj, QEvent *event){
    if (event->type() == QEvent::MouseButtonDblClick) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::MiddleButton) {
            qDebug() << "Middle button double-click detected in event filter!";
          // areachart->resize();
            updateAreaChart();
            return true;  // Event wurde verarbeitet
        }
    }
    // Weiteres Eventverarbeiten dem Standard überlassen
    return QWidget::eventFilter(obj, event);
}


void AreaChartView::wheelEvent(QWheelEvent *event){
    if (event->angleDelta().y() > 0)
        areachart->zoomIn();  // Zoom-In beim Drehen des Mausrads nach oben
       // areaChartView->chart()->zoomIn();
    else
        areachart->zoomOut();  // Zoom-Out beim Drehen des Mausrads nach unten
   // areaChartView->chart()->zoomOut();
}


void AreaChartView::mousePressEvent(QMouseEvent *event){
qDebug() << "Left mouse pressed." << event->button();
    if (event->button() == Qt::MiddleButton) {
        lastMousePos = event->pos();  // Speichere die aktuelle Mausposition
        setCursor(Qt::ClosedHandCursor);  // Cursor wieder zurücksetzen
    }

    else if(event->button() == Qt::LeftButton) {
                   qDebug() << "right mouse button pressed.";


        rubberBandStart = areaChartView->mapFromParent(event->pos());
        qDebug() << "x ." << event->pos().x() << " y: "<< event->pos().y() << "rubberBandStart " << rubberBandStart ;

        rubberBandRectItem = new QGraphicsRectItem(QRectF(rubberBandStart, QSize()));

        QBrush brush(QColor(100, 150, 205, 70)); // 100 ist der Alpha-Wert für Transparenz (0-255)
        rubberBandRectItem->setBrush(brush);

        QPen pen(QColor(0, 100, 255)); // Blaue Farbe für den Rand
        pen.setWidth(1); // Breite des Rahmens
        rubberBandRectItem->setPen(pen);

        areaChartView->scene()->addItem(rubberBandRectItem);

    }

    else if(event->button() == Qt::RightButton) {

        //Menü
    }
    QWidget::mousePressEvent(event);  // Weiterleitung des Events
}

void AreaChartView::mouseMoveEvent(QMouseEvent *event){

    if (event->buttons() & Qt::MiddleButton) {
        qDebug() << "Mouse move detected with left button pressed." << event->pos();
        QPointF delta = event->pos() - lastMousePos;

        // Hole die Achsen
        QValueAxis *axisX = qobject_cast<QValueAxis*>(areachart->axisX());
        QValueAxis *axisY = qobject_cast<QValueAxis*>(areachart->axisY());

        if (axisX && axisY) {
            double xRange = axisX->max() - axisX->min();
            double yRange = axisY->max() - axisY->min();

            // Berechne die neuen Bereiche für die Achsen
            double dx = -delta.x() * (xRange / areachart->plotArea().width());
            double dy = delta.y() * (yRange / areachart->plotArea().height());

            axisX->setRange(axisX->min() + dx, axisX->max() + dx);
            axisY->setRange(axisY->min() + dy, axisY->max() + dy);
            lastMousePos = event->pos();  // Update die letzte Mausposition
        }
    }

    else if (event->buttons() & Qt::LeftButton && rubberBandRectItem) {


        QPointF rubberBandEnd = areaChartView->mapFromParent(event->pos());
        QRectF newRect(rubberBandStart, rubberBandEnd);
        rubberBandRectItem->setRect(newRect.normalized());
    }
}

void AreaChartView::mouseReleaseEvent(QMouseEvent *event){

    if(event->button() == Qt::MiddleButton){
        setCursor(Qt::ArrowCursor);  // Cursor wieder zurücksetzen
    }
    else if (event->button() == Qt::LeftButton && rubberBandRectItem) {

        // Hole den Bereich des Rechtecks
        QRectF zoomRect = rubberBandRectItem->rect();
        areaChartView->chart()->zoomIn(zoomRect);

        areaChartView->scene()->removeItem(rubberBandRectItem);
        delete rubberBandRectItem;
        rubberBandRectItem = nullptr;
    }
}


void AreaChartView::updateAreaChart()
{

    QList<QPair<QPointF, QPointF>> pointPairs;

    comboBoxCurrentIndex=m_comboBox->currentIndex();

    if(axisX)areachart->removeAxis(axisX);
    if(axisY)areachart->removeAxis(axisY);

    axisX = new QValueAxis();
    axisY = new QValueAxis();

    float xl, xr , yl, yr, xMin, xMax, yMin, yMax, zMin,zMax;

    xMin = -1*scan_datalist.at(0).polarcordinatewithouttilt.at(0);
    xMax = scan_datalist.at(0).polarcordinatewithouttilt.at(0);

    yMin = scan_datalist.at(0).cardcordinateZ.at(0);
    yMax = scan_datalist.at(0).cardcordinateZ.at(0);

    zMin = scan_datalist.at(0).z;
    zMax = scan_datalist.at(0).z;
    for(int i=0; i < scan_datalist.size(); i++){
        if (zMin > scan_datalist.at(i).z)zMin=scan_datalist.at(i).z;
        if (zMax < scan_datalist.at(i).z)zMax=scan_datalist.at(i).z;
    }

    for(int i=0; i < scan_datalist.size(); i++){

        if(!m_checkFrontView->isChecked()){
            xr = scan_datalist.at(i).polarcordinatewithouttilt.at(comboBoxCurrentIndex);
            if(m_checkBoxFullView->isChecked()){
                xl = scan_datalist.at(i).polarcordinatewithouttilt.at(comboBoxCurrentIndex + (scan_datalist.at(i).polarcordinatewithouttilt.size()/2));
                xl = (-1)*xl;
            }
            else{
                xl = 0;
            }

        }else{
            xr = scan_datalist.at(i).cardcordinateX.at(comboBoxCurrentIndex);
            if(m_checkBoxFullView->isChecked()){
                xl = scan_datalist.at(i).cardcordinateX.at(comboBoxCurrentIndex + (scan_datalist.at(i).polarcordinatewithouttilt.size()/2));
            }
            else{
                xl = 0;
            }
        }

        yr = scan_datalist.at(i).cardcordinateZ.at(comboBoxCurrentIndex);

        if(m_checkBoxFullView->isChecked()){
            yl = scan_datalist.at(i).cardcordinateZ.at(comboBoxCurrentIndex + (scan_datalist.at(i).polarcordinatewithouttilt.size()/2));
        }
        else{
            yl = scan_datalist.at(i).cardcordinateZ.at(comboBoxCurrentIndex);
        }

        if(xMin > xl )xMin = xl;
        if(xMax < xr )xMax = xr;
        if(yMin > yr )yMin = yr;
        if(yMax < yr )yMax = yr;

        pointPairs.append(qMakePair(QPointF(xl, yl), QPointF(xr, yr))); // ansonsten füge Punkte zum Area zusammen
    }

    pointPairs.append(qMakePair(QPointF(0, yMin), QPointF(0, yMin))); //zur Y-Achse verschließen
    pointPairs.append(qMakePair(QPointF(0, yMax), QPointF(0, yMax)));

    // Sortiere die Paare nach dem Y-Wert des oberen Punktes (absteigend)
    std::sort(pointPairs.begin(), pointPairs.end(), [](const QPair<QPointF, QPointF>& x, const QPair<QPointF, QPointF>& y) {
        return x.first.y() > y.first.y(); // Sortiert nach Y-Wert der oberen Serie
    });

    if (rightareaSeries != nullptr && !rightareaSeries->points().isEmpty())rightareaSeries->clear();
    if (leftareaSeries != nullptr && !leftareaSeries->points().isEmpty())leftareaSeries->clear();

    // Füge die sortierten Punkte wieder in die Serien ein
    for (const QPair<QPointF, QPointF>& pair : pointPairs) {

        if(m_checkBoxFullView->isChecked())leftareaSeries->append(pair.first); // Füge den entsprechenden unteren Punkt hinzu unte Bedindgung dass der Check Button geclickt isz
        else leftareaSeries->append(0, pair.first.y());

        rightareaSeries->append(pair.second); // Füge den oberen Punkt hinzu
    }

    // areaSeries = new QAreaSeries(rightareaSeries, leftareaSeries);
    areaSeries->setName(QString::number(comboBoxCurrentIndex));


  //  qDebug() << "xmin " << xMin << " xmax " << xMax << " xl " << xl << " xr " <<xr;
    if(m_checkFixAxis->isChecked()){

        if(m_checkBoxFullView->isChecked()){
           xMin = -1*m_lineEditFixAxis->text().toFloat() - 5;
           xMax = m_lineEditFixAxis->text().toFloat() + 5;
        }
        else{
            xMin = - 5;
            xMax = m_lineEditFixAxis->text().toFloat() + 5;

            if(xr < 0){
                xMin = -1*xMax;
                xMax = 5;
            }
        }
    }
    else{
        xMax = xMax + 5;
        if(m_checkBoxFullView->isChecked()){
            xMin = xMin - 5;
        }
        else{
            xMin =  -5;
            if(xr < 0){

                xMin = xr - 5;
                xMax = 5;
            }
        }
    }

    // Achsen erstellen und zum Chart hinzufügen
    axisX->setRange(xMin, xMax);
    axisY->setRange(yMin - 10, yMax +10);

    axisX->setTitleText("Breite");
    axisY->setTitleText("Tiefe");

    // Füge die Achsen zum Diagramm hinzu
    areachart->addAxis(axisX, Qt::AlignBottom);
    areachart->addAxis(axisY, Qt::AlignLeft);

    areaSeries->attachAxis(axisX);
    areaSeries->attachAxis(axisY);

    for (QLineSeries* series : lineSerieList) {
        // Entferne die Serie zuerst aus dem Chart
        areachart->removeSeries(series);
    }
    lineSerieList.clear();
    //areachart->removeAllSeries();
    if (lineSeriesRight != nullptr)lineSeriesRight->clear();
    if (lineSeriesLeft != nullptr)lineSeriesLeft->clear();

    if(m_checkBoxLineSeries->isChecked()){

        for(int i=0; i < scan_datalist.size(); i++){

            if(!m_checkFrontView->isChecked()){
                xr = scan_datalist.at(i).polarcordinatewithouttilt.at(comboBoxCurrentIndex);
                xl = scan_datalist.at(i).polarcordinatewithouttilt.at(comboBoxCurrentIndex + (scan_datalist.at(i).polarcordinatewithouttilt.size()/2));
                xl = (-1)*xl;
            }else{
                xr = scan_datalist.at(i).cardcordinateX.at(comboBoxCurrentIndex);
                xl = scan_datalist.at(i).cardcordinateX.at(comboBoxCurrentIndex + (scan_datalist.at(i).polarcordinatewithouttilt.size()/2));
            }

            yr = scan_datalist.at(i).cardcordinateZ.at(comboBoxCurrentIndex);
            yl = scan_datalist.at(i).cardcordinateZ.at(comboBoxCurrentIndex + (scan_datalist.at(i).polarcordinatewithouttilt.size()/2));

            QPen pen(Qt::gray); // Frabe
           // pen.setColor(QColor( 0xFF, 0xA0, 0x00));
            pen.setWidth(1);// Dicke Linie
            pen.setStyle(Qt::DashDotLine);//Linienstil

            lineSeriesRight = new QLineSeries();
            lineSeriesLeft = new QLineSeries();

            lineSerieList.append(lineSeriesRight); //Programm stürzt ab
            lineSerieList.append(lineSeriesLeft);

            lineSeriesRight->append(QPointF(0, scan_datalist.at(i).z));  // Nullpunkt oder ein definierter Ursprung
            lineSeriesRight->append(QPointF(xr, yr));
            lineSeriesRight->setName(QString::number(i));

            // Füge die Linie zum Chart hinzu
            areachart->addSeries(lineSeriesRight);
            lineSeriesRight->attachAxis(axisX);
            lineSeriesRight->attachAxis(axisY);
            lineSeriesRight->setPen(pen);

            if(m_checkBoxFullView->isChecked()){

                lineSeriesLeft->append(QPointF(0, scan_datalist.at(i).z));  // Nullpunkt oder ein definierter Ursprung
                lineSeriesLeft->append(QPointF(xl, yl));
                lineSeriesLeft->setName(QString::number(i));

                // Füge die Linie zum Chart hinzu
                areachart->addSeries(lineSeriesLeft);
                lineSeriesLeft->attachAxis(axisX);
                lineSeriesLeft->attachAxis(axisY);
                lineSeriesLeft->setPen(pen);

            }
        }
    }

    areachart->legend()->hide();
    areachart->update();
    // chartViewArea->setChart(areachart);
}

void AreaChartView::setScanData(const QList<XmlScanData> &data){

    scan_datalist = data;
    updateComboBox();
}

void AreaChartView::updateComboBox(){

    stringlistrad.clear();

    if(m_checkBoxFullView->isChecked()){
        for(int i=0; i < scan_datalist.at(1).polarcordinate.size()/2; i++){

            stringlistrad.append("Direction: " + QString::number(i* 360/scan_datalist.at(1).polarcordinate.size())  + "°");
        }
    }
    else{
        for(int i=0; i < scan_datalist.at(1).polarcordinate.size(); i++){

            stringlistrad.append("Direction: " + QString::number(i* 360/scan_datalist.at(1).polarcordinate.size())  + "°");
        }
    }


    m_comboBox->blockSignals(true);
    m_comboBox->clear();
    m_comboBox->addItems(stringlistrad);
    for (int i = 0 ; i < m_comboBox->count() ; ++i) {
        m_comboBox->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);
    }
    m_comboBox->blockSignals(false);

    m_slider->blockSignals(true);
    m_slider->setSingleStep(1);
    m_slider->setRange(0, stringlistrad.size()-1);
    m_slider->blockSignals(false);
    updateAreaChart();

}

void AreaChartView::updateSliderMove(int pos){

    m_comboBox->setCurrentIndex(pos);
}
