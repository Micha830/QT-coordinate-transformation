#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>

#include "calculation.h"
#include "surefaceview.h"
#include "areachartview.h"
#include "polarchartview.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private:

    Ui::MainWindow *ui;

    int comboIndex;

    QString xmlfilecontent;
    QString lastDirFile;
    Calculation calculation;

    SureFace *surfaceView;
    PolarChartView *polarchartview;
    AreaChartView *areachartview;

    QList<XmlScanData> m_datalist;

    XmlMainData m_xmlmaindata;
    XmlScanData m_xmlscandata;

    void DebugXmlData();
    void visualizeData();

    void visualizeTable();
    void setupCharts();

public:
    void setDataList(const QList<XmlScanData> &data);
    void setMainData(const XmlMainData &data);

private slots:
    void fileopen();
    void lastfileavailable();
    void savereport();
    void quit();
    void onCalculationFinished(const QList<XmlScanData> &data, const XmlMainData &maindata);
    void visualizePolarChart();
    void visualizeAreaChart();

    void showSurfaceView();
    void showMessageInfo();
    void updateTable(const int &comboindex);
    void showDockView();
};
#endif // MAINWINDOW_H
