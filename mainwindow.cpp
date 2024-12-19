#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
     ui(new Ui::MainWindow),
     xmlfilecontent("Initial Text"),
     calculation(xmlfilecontent, this),
     surfaceView(nullptr),
     polarchartview(new PolarChartView(this)),
     areachartview(new AreaChartView(this))// Initialisiere das AreaChartView
{
    ui->setupUi(this);
    setWindowTitle("Koordinate Transformation 2024");

    connect(ui->actionDatei_einlesen, SIGNAL (triggered()), this, SLOT (fileopen()));
    connect(ui->actionReport_speichern, SIGNAL (triggered()), this, SLOT (savereport())); 
    connect(ui->action3d_view, SIGNAL (triggered()), this, SLOT (showSurfaceView()));
    connect(ui->actionBeenden, SIGNAL(triggered()), this, SLOT(quit()));
    connect(ui->actionInfo, SIGNAL(triggered()), this, SLOT(showMessageInfo()));
    connect(ui->actionAreaView, SIGNAL(triggered()), this, SLOT(showDockView()));

    connect(&calculation, &Calculation::calculationFinished, this, &MainWindow::onCalculationFinished);
    connect(polarchartview, &PolarChartView::tableUpdated, this, &MainWindow::updateTable);

    setupCharts();

    lastfileavailable();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::onCalculationFinished(const QList<XmlScanData> &data, const XmlMainData &maindata){

    m_xmlmaindata = maindata;
    m_datalist = data;

    visualizeData();
}

void MainWindow::visualizeData(){

    QString mainData = "<b><i>Date: " + m_xmlmaindata.date + "</i></b>"
                       + "<br><br>Messnummer:  " + m_xmlmaindata.measurenumber
                       + "<br>Operator:  " + m_xmlmaindata.operatorr
                       + "<br><br>comment:  " + m_xmlmaindata.comment
                       + "<br>steps:  " +  QString::number(m_xmlmaindata.sonarsteps);

    ui->label_maindata->clear();
    ui->label_maindata->setText(mainData);

    visualizePolarChart();
    visualizeAreaChart();
}

void MainWindow::visualizePolarChart(){

    polarchartview->setScanData(m_datalist);
}


void MainWindow::visualizeAreaChart(){

    areachartview->setScanData(m_datalist);
}


void MainWindow::setupCharts(){

    if (ui->widget_Polarview->layout() == nullptr) {
        QVBoxLayout *layout = new QVBoxLayout(ui->widget_Polarview);
        ui->widget_Polarview->setLayout(layout);
    }

    ui->widget_Polarview->layout()->addWidget(polarchartview);

    if (ui->widget_Areaview->layout() == nullptr) {
        QVBoxLayout *layout = new QVBoxLayout(ui->widget_Areaview);
        ui->widget_Areaview->setLayout(layout);
    }

    ui->widget_Areaview->layout()->addWidget(areachartview);
}

void MainWindow::updateTable(const int &comboBoxCurrentIndex){

    comboIndex = comboBoxCurrentIndex;

    QThread *thread = QThread::create([=]() {

        ui->mainTableWidget->setRowCount(m_datalist.at(comboIndex).polarcordinate.size());
        ui->mainTableWidget->setColumnCount(5);
        ui->mainTableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem("XML Polar Koordinaten"));
        ui->mainTableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem("Polar Koordinaten ohne Tilt"));
        ui->mainTableWidget->setHorizontalHeaderItem(2, new QTableWidgetItem("X Koordinaten"));
        ui->mainTableWidget->setHorizontalHeaderItem(3, new QTableWidgetItem("Y Koordinaten"));
        ui->mainTableWidget->setHorizontalHeaderItem(4, new QTableWidgetItem("Z Koordinaten"));

        for(int i=0; i < m_datalist.at(comboIndex).polarcordinate.size(); i++){

            QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(m_datalist.at(comboIndex).polarcordinate.at(i)));
            item1->setTextAlignment(Qt::AlignCenter);  // Zentriere den Text

            QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(m_datalist.at(comboIndex).polarcordinatewithouttilt.at(i)));
            item2->setTextAlignment(Qt::AlignCenter);  // Zentriere den Text

            QTableWidgetItem *item3 = new QTableWidgetItem(QString::number(m_datalist.at(comboIndex).cardcordinateX.at(i)));
            item3->setTextAlignment(Qt::AlignCenter);  // Zentriere den Text

            QTableWidgetItem *item4 = new QTableWidgetItem(QString::number(m_datalist.at(comboIndex).cardcordinateY.at(i)));
            item4->setTextAlignment(Qt::AlignCenter);  // Zentriere den Text

            QTableWidgetItem *item5 = new QTableWidgetItem(QString::number(m_datalist.at(comboIndex).cardcordinateZ.at(i)));
            item5->setTextAlignment(Qt::AlignCenter);  // Zentriere den Text

            ui->mainTableWidget->setItem(i, 0, item1);
            ui->mainTableWidget->setItem(i, 1, item2);
            ui->mainTableWidget->setItem(i, 2, item3);
            ui->mainTableWidget->setItem(i, 3, item4);
            ui->mainTableWidget->setItem(i, 4, item5);
        }

        ui->mainTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

     });
     thread->start();
}



void MainWindow::fileopen(){

    QSettings settings("MichaSwDev", "CavernMeasurments");
    lastDirFile = settings.value("lastFilePath", QDir::homePath()).toString();
  //  qDebug() << "xml size: " <<  xmlfilecontent.size();
    xmlfilecontent.clear();

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), lastDirFile,
                                                    tr("XML Files (*.xml);;Text files (*.txt);;All files (*.*)"));

    if (fileName != "") {

        QFile file(fileName);
        settings.setValue("lastFilePath", fileName);

        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(this, tr("Error"),
                                  tr("Could not open file"));
            return;
        }

        xmlfilecontent = file.readAll().constData();
     //  ui->plainTextEdit->setPlainText(xmlfilecontent);
        file.close();

        calculation.settTextXML(xmlfilecontent);
    }
}


void MainWindow::lastfileavailable(){

    QSettings settings("MichaSwDev", "CavernMeasurments");
    QString lastFilePath = settings.value("lastFilePath", "").toString();

    if (!lastFilePath.isEmpty()) {
        // Versuche die letzte Datei zu öffnen
        QFile file(lastFilePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            xmlfilecontent = in.readAll();
            file.close();

            calculation.settTextXML(xmlfilecontent);
        }
    }
}


void MainWindow::savereport(){
    QSettings settings("MichaSwDev", "CavernMeasurments");
    QString lastFilePath = settings.value("lastFilePath", "").toString();

    qDebug() << "lastFilePath: " << lastFilePath;
    QFile file;

    file.setFileName(lastFilePath+"_Calc" + QDate::currentDate().toString() + ".txt");

    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Konnte die Datei nicht öffnen!";
        return;
    }

    // Verwende QDataStream zum Schreiben der Daten
    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_15);  // Setze die Qt-Version für den Stream

    // Schreibe Main zuerst
    out << "Date:  \t\t" + m_xmlmaindata.date
        + "\nMessnummer:  \t" + m_xmlmaindata.measurenumber
        + "\nOperator:  \t" + m_xmlmaindata.operatorr
        + "\ncomment:  \t" + m_xmlmaindata.comment
        + "\nsonar steps:  \t" +  QString::number(m_xmlmaindata.sonarsteps) + "\n";


    for(int i=0; i < m_datalist.size(); i++){

        out << "\n\nPos: " +  QString::number(m_datalist.at(i).scanpos)
                   + ", Z: " << QString::number(m_datalist.at(i).z)
                   + ", Tilt: " << QString::number(m_datalist.at(i).tilt)
                   + ", Date: " << m_datalist.at(i).date
                   + ", Time: " << m_datalist.at(i).time
                   + ", Average Z: " << QString::number(m_datalist.at(i).averageZ)
                   + "\n\npolarcordinate, polarcordinate with out tilt, X, Y, Z \n";


        for(int a=0; a < m_datalist.at(i).polarcordinate.size(); a++){

            out << "\n" + QString::number(m_datalist.at(i).polarcordinate.at(a)) + ","
                 + QString::number(m_datalist.at(i).polarcordinatewithouttilt.at(a)) + ","
                 + QString::number(m_datalist.at(i).cardcordinateX.at(a)) + ","
                 + QString::number(m_datalist.at(i).cardcordinateY.at(a)) + ","
                 + QString::number(m_datalist.at(i).cardcordinateZ.at(a))+ ",";
        }
    }

    file.close();
    qDebug() << "Daten erfolgreich in die Datei geschrieben!";
}


void MainWindow::quit(){
    this->quit();
}



void MainWindow::DebugXmlData(){

        // Ausgabe der Header-Parameter
    qDebug() << "Header :";
    qDebug() << "MeasureNumber:" << m_xmlmaindata.measurenumber ;
    qDebug() << "Date:" << m_xmlmaindata.date ;
    qDebug() << "Operator:" << m_xmlmaindata.operatorr ;
    qDebug() << "Comment:" << m_xmlmaindata.comment ;
    qDebug() << "Sonar:" << m_xmlmaindata.sonarsteps << "\n\n";

    // Ausgabe der Scan Daten

    qDebug() << "Scan Data List:";
    for (const XmlScanData &xmlscandata : m_datalist) {

        qDebug() << "\n\nPos: " << xmlscandata.scanpos
                 << ", Z: " << xmlscandata.z
                 << ", Tilt: " << xmlscandata.tilt
                 << ", Date: " << xmlscandata.date
                 << ", Time: " << xmlscandata.time
                 << ", Average Z: " << xmlscandata.averageZ;

        qDebug() << "\npolarcordinate:" << m_datalist.data()->polarcordinate
                 << "\npolarcordinate with out tilt:" << m_datalist.data()->polarcordinatewithouttilt
                 << "\n\nX:" << m_datalist.data()->cardcordinateX
                 << "\n\nY:" << m_datalist.data()->cardcordinateY
                 << "\n\nZ:" << m_datalist.data()->cardcordinateZ;
    }

    qDebug() << "Vorgang komplett abgeschlossen " ;
}



void MainWindow::showDockView(){
    if(!ui->dockWidget->isVisible()) ui->dockWidget->setVisible(true);
}

void MainWindow::showSurfaceView()
{
    if (!surfaceView)surfaceView = new SureFace(m_datalist);  // Erzeuge die Surface-Ansicht
    surfaceView->show();  // Zeige die Surface-Ansicht an
}

void MainWindow::showMessageInfo(){

    QMessageBox msgBox;
    msgBox.setWindowTitle("Information");
    msgBox.setText("Cavern Vermessung\nFür Ekki von Micha mit QT6\nViel Spass");
   // msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);

    int ret = msgBox.exec();

    switch (ret) {
    case QMessageBox::Ok:
        break;
    case QMessageBox::Cancel:
        break;
    }
}
