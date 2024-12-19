#ifndef CALCULATION_H
#define CALCULATION_H

#include <QObject>
#include <QString>
#include <QDebug>
#include <QXmlStreamReader>
#include <QVector>
#include <QList>
#include <QtMath>
#include <QThread>


// Gemeinsamer Structur zur Verwendung in beiden Klassen
struct XmlMainData{ //XMl Kopfbereich
    QString measurenumber, date, operatorr, comment; // XML MainHeader
    int sonarsteps;
};


struct XmlScanData{ //XML Scan Bereich
    int scanpos;
    QString date, time;
    float z, averageZ, tilt;
    QVector<float> polarcordinate;
    QVector<float> polarcordinatewithouttilt;
    QVector<float> cardcordinateX;
    QVector<float> cardcordinateY;
    QVector<float> cardcordinateZ;
};


class Calculation : public QObject
{
    Q_OBJECT  // Makro notwendig, um Signale und Slots zu verwenden

public:
    explicit Calculation(const QString &text, QObject *parent = nullptr);

    void settTextXML(const QString &text);
    void startCalculation();


private:

  //  void DebugXmlData(struct XmlMainData,  QList<XmlScanData>);
    void DebugXmlData();
    void TransformKoordinatensystem();
    void SortScandData();
    QString m_text;
    QList<XmlScanData> datalist;
    QList<XmlScanData> sortdatalist;
    QList<XmlScanData> sortdatalist_v2;
    XmlMainData xmlmaindata;
    XmlScanData xmlscandata;
    XmlScanData sortscandata;

signals:
    // Signal, das gesendet wird, wenn die Berechnung abgeschlossen ist
    void calculationFinished(const QList<XmlScanData> &scanresult, const XmlMainData &mainresult);
};

#endif // CALCULATION_H
