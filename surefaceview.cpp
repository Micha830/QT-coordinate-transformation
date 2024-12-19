#include "surefaceview.h"
#include "ui_surefaceview.h"

SureFace::SureFace(const QList<XmlScanData> &scandata, QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::SureFace)
{
    ui->setupUi(this);

    setWindowTitle("3d Ansicht");

    m_proxy = new QSurfaceDataProxy();
    m_Series = new QSurface3DSeries(m_proxy);
    m_graph = new Q3DSurface();

    m_graph->axisX()->setLabelFormat("%.0f");
    m_graph->axisY()->setLabelFormat("%.0f");
    m_graph->axisZ()->setLabelFormat("%.0f");

    // Anpassung der Achsen
    m_graph->axisX()->setTitle("X Axis");
    m_graph->axisY()->setTitle("Y Axis");
    m_graph->axisZ()->setTitle("Z Axis");
    // Anpassung der Achsen
    m_graph->axisX()->setRange(-100, 100);
    //graph->axisY()->setRange(1000, 1200);
    m_graph->axisZ()->setRange(-100, 100);

    m_graph->addSeries(m_Series);
    m_graph->scene()->activeCamera()->setCameraPosition(45, 45);

    m_scandata = scandata;



    QWidget *container = QWidget::createWindowContainer(m_graph);
    container->setMinimumSize(QSize(800, 600));
    //  container->setMaximumSize(QSize(800, 600));
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->setFocusPolicy(Qt::StrongFocus);

    if (ui->widget_Sureface->layout() == nullptr) {
        QVBoxLayout *layout = new QVBoxLayout(ui->widget_Sureface);
        ui->widget_Sureface->setLayout(layout);
        qDebug() << "Sureface: ";
    }

    ui->widget_Sureface->layout()->addWidget(container);

    ui->horizontalSlider_XAxis->blockSignals(true);
    ui->horizontalSlider_XAxis->setSingleStep(1);
    ui->horizontalSlider_XAxis->setRange(0, 200);
    ui->horizontalSlider_XAxis->blockSignals(false);

    connect(ui->horizontalSlider_XAxis, SIGNAL(valueChanged(int)), this, SLOT(setAxisRange()));

    QThread *thread = QThread::create([=]() {
        SortScandData();
        setupGraph();
    });
    thread->start();
}

SureFace::~SureFace()
{
    delete ui;
}

void SureFace::setupGraph()
{

    QList<XmlScanData> m_scandatatmp = m_sortdatalist;

    QSurfaceDataArray *dataArray = new QSurfaceDataArray;
    dataArray->reserve(m_scandatatmp.size());  // Anzahl der Zeilen

    for (int i = 0; i < m_scandatatmp.size(); ++i) {
        QSurfaceDataRow *newRow = new QSurfaceDataRow(91);  // Anzahl der Spalten
        for (int j = 0; j <= m_scandatatmp.at(i).cardcordinateX.size(); ++j) {

            if(j <  m_scandatatmp.at(i).cardcordinateX.size()){

               float x = m_scandatatmp.at(i).cardcordinateX.at(j);
               float z = m_scandatatmp.at(i).cardcordinateZ.at(j);
               float y = m_scandatatmp.at(i).cardcordinateY.at(j);
               (*newRow)[j].setPosition(QVector3D(x, z, y));
            }
            else{//damit das ding wieder geschlossen ist
               float x = m_scandatatmp.at(i).cardcordinateX.at(0);
               float z = m_scandatatmp.at(i).cardcordinateZ.at(0);
               float y = m_scandata.at(i).cardcordinateY.at(0);
               (*newRow)[j].setPosition(QVector3D(x, z, y));
            }
      }
      dataArray->append(newRow);
  }

m_proxy->resetArray(dataArray);

   // DebugXmlData();
}


void SureFace::setAxisRange()
{
    int pos = ui->horizontalSlider_XAxis->value();
    qDebug() << "x Axis: " << ui->horizontalSlider_XAxis->value() ;

    m_graph->axisX()->setRange(pos-100, 100);
}


void SureFace::SortScandData(){

    QList<XmlScanData> datalist_tmp = m_scandata;

    float depthold = 0 ;
    float depthaverage = 0;

    float zMin = datalist_tmp.at(0).z;
    float zMax = datalist_tmp.at(0).z;

    for(int i=0; i < datalist_tmp.size(); i++){
        if (zMin > datalist_tmp.at(i).z)zMin=datalist_tmp.at(i).z;
        if (zMax < datalist_tmp.at(i).z)zMax=datalist_tmp.at(i).z;
    }
    int i = 0, a = 0, s = 0;

    i = 0;
    do{

        depthaverage = datalist_tmp.at(i).averageZ;
        //   qDebug() << "i: " << i << " while old: " << depthold << " tmp " << depthtemp;
        if(depthold > depthaverage){

            /***Einlaufrohr Höhe und entferne***/
            if(zMax > datalist_tmp.at(i).z && datalist_tmp.at(i).tilt > 80){
                //   qDebug() << "i: " << i << " depth: " << scan_datalist.at(i).depth << " tilt: " << scan_datalist.at(i).tilt;
                 datalist_tmp.remove(i, 1);
            }
            else{
                a = i;
                depthold = depthaverage;
            }
        }

        i++;

         if(i >= datalist_tmp.size()){

            m_sortdatalist.append(datalist_tmp.at(a));
            datalist_tmp.remove(a, 1);
            //    qDebug() << "a: " << a << " depth: " << sortdatalist.at(s).depth <<  "datalist_tmp size :" << datalist_tmp.size()  ;
            s++;
            i = 0;
            a = 0;
            depthold = 0;
        }
    } while (!datalist_tmp.isEmpty());
}


void SureFace::DebugXmlData(){

    qDebug() << "Scan Data List:";
    for (const XmlScanData &xmlscandata : m_scandata) {

        qDebug() << "\n\nPos: " << xmlscandata.scanpos
                 << ", Z: " << xmlscandata.z
                 << ", Tilt: " << xmlscandata.tilt
                 << ", Date: " << xmlscandata.date
                 << ", Time: " << xmlscandata.time
                 << ", Average Z: " << xmlscandata.averageZ;

        qDebug() << "\npolarcordinate:" << m_scandata.data()->polarcordinate
                 << "\npolarcordinate with out tilt:" << m_scandata.data()->polarcordinatewithouttilt
                 << "\n\nX:" << m_scandata.data()->cardcordinateX
                 << "\n\nY:" << m_scandata.data()->cardcordinateY
                 << "\n\nZ:" << m_scandata.data()->cardcordinateZ;

    }
}

