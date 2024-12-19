#ifndef SUREFACEVIEW_H
#define SUREFACEVIEW_H

#include <QMainWindow>
#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QSurfaceDataProxy>
#include <QtDataVisualization/QSurface3DSeries>
#include <QtWidgets/QWidget>
#include <QVBoxLayout>
#include "calculation.h"


namespace Ui {
class SureFace;
}

class SureFace : public QMainWindow
{
    Q_OBJECT

public:
    explicit SureFace(const QList<XmlScanData> &scandata, QWidget *parent = nullptr);
    ~SureFace();

private:
    Ui::SureFace *ui;
    Q3DSurface *m_graph = nullptr;  // 3D Surface Graph
    QSurfaceDataProxy *m_proxy = nullptr;
    QSurface3DSeries *m_Series = nullptr;

    QList<XmlScanData> m_scandata, m_sortdatalist, m_sortdatalistV2;

    void setupGraph();
    void DebugXmlData();
    void SortScandData();

private slots:
    void setAxisRange();
};

#endif // SUREFACEVIEW_H
