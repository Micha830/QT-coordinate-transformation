#include "calculation.h"

Calculation::Calculation(const QString &text, QObject *parent)
    : QObject(parent), m_text(text){
}


void Calculation::startCalculation()
{
    // Starte die Berechnung in einem separaten Thread
    QThread *thread = QThread::create([=]() {

        TransformKoordinatensystem();
        SortScandData();
       // emit calculationFinished(datalist, xmlmaindata);// Sende das Signal, wenn die Berechnung abgeschlossen ist
       // emit calculationFinished(sortdatalist, xmlmaindata);// Sende das Signal, wenn die Berechnung abgeschlossen ist
        emit calculationFinished(sortdatalist_v2, xmlmaindata);// Sende das Signal, wenn die Berechnung abgeschlossen ist
    });
    thread->start();
}

void Calculation::TransformKoordinatensystem (){

    int scanpos = 0;
    datalist.clear();
    sortdatalist.clear();
    sortdatalist_v2.clear();

    QXmlStreamReader xml(m_text);

    while (!xml.atEnd() && !xml.hasError()) {
        xml.readNext();
      if (xml.isStartElement()) {

          QString elementName = xml.name().toString(); // Konvertiere den Parameter zu QString
          if (elementName == "Param") {
              QString paramName = xml.attributes().value("Name").toString();
              QString value = xml.attributes().value("Value").toString();

              if (paramName == "MeasureNumber" && xmlmaindata.measurenumber.isEmpty())xmlmaindata.measurenumber = value;
              else if (paramName == "Date" && xmlmaindata.date.isEmpty())xmlmaindata.date = value;
              else if (paramName == "Operator" && xmlmaindata.operatorr.isEmpty())xmlmaindata.operatorr = value;
              else if (paramName == "Comment" && xmlmaindata.comment.isEmpty())xmlmaindata.comment = value;
          }
          else if(elementName == "Sonar"){
              xmlmaindata.sonarsteps = xml.attributes().value("Steps").toInt();
          }
          else if (elementName == "Scan") {
              float depth = xml.attributes().value("Depth").toFloat();
              float tilt = xml.attributes().value("Tilt").toFloat();
              QString date = xml.attributes().value("Date").toString();
              QString time = xml.attributes().value("Time").toString();

              float polarCoordinate=0;
              float polarcordinatewithouttilt = 0;
              float xCoordinate=0;
              float yCoordinate=0;

              xml.readNext(); // Springe zum Inhalt der Data-Elemente

              QVector<float> polarcordinates;
              QVector<float> polarcordinatewithouttilts;
              QVector<float> cardcordinateX;
              QVector<float> cardcordinateY;
              QVector<float> cardcordinateZ;

              QStringList polarcordinatesList = xml.text().toString().split(";", Qt::SkipEmptyParts);
              int i = 0;
              float radians2d = (360/(polarcordinatesList.size()-1))*M_PI/180;
              float radians3d = tilt*M_PI/180;
              float zOffset = 0;
              float averagedepth = 0;

              for (const QString& valueStr : polarcordinatesList) {

                  if(!valueStr.isEmpty()){ // Leere Messwerte werden ignoriert

                      polarCoordinate = valueStr.toFloat();


                      if(polarCoordinate != 0)//Koorrdinate 0 wird auch ignoriert
                      {
                          zOffset = polarCoordinate*qSin(radians3d); // neue Höhe unter Berücksichtigung
                          polarcordinatewithouttilt = polarCoordinate*qCos(radians3d); //Ankathete = polarvetcor*cos(tilt)

                          xCoordinate = polarcordinatewithouttilt*qCos(float(i*radians2d)); //kartesische Koordinaten
                          yCoordinate = polarcordinatewithouttilt*qSin(float(i*radians2d));

                          polarcordinates.append(polarCoordinate);
                          polarcordinatewithouttilts.append(polarcordinatewithouttilt);
                          cardcordinateX.append(xCoordinate); // Ausgeklammert wegen falsche annahmen der berechnung

                          cardcordinateY.append(yCoordinate);
                          cardcordinateZ.append((-1*depth)+zOffset);

                          qDebug() << "i: " << i << " cordinate: " << polarCoordinate << " cordinate u tilt: " << polarcordinatewithouttilt << " X: " << xCoordinate << " Y: " << yCoordinate << " Offset: " << zOffset << " Z: " << -1*depth+zOffset;

                          averagedepth = averagedepth + ((-1*depth)+zOffset);
                      }
                  }
                  i++;
              }

              xmlscandata.scanpos = scanpos++;
              xmlscandata.z = -1*depth;
              xmlscandata.tilt = tilt;
              xmlscandata.date = date;
              xmlscandata.time = time;
              xmlscandata.polarcordinate = polarcordinates;
              xmlscandata.polarcordinatewithouttilt = polarcordinatewithouttilts;
              xmlscandata.cardcordinateX = cardcordinateX;
              xmlscandata.cardcordinateY = cardcordinateY;
              xmlscandata.cardcordinateZ = cardcordinateZ;
              xmlscandata.averageZ = averagedepth/cardcordinateZ.size();

              datalist.append(xmlscandata); //Einfügen eines Nodes an das Ende einer QList :)
          }
        }
      }

      if (xml.hasError()) {
          qCritical() << "XML Parsing Error:" << xml.errorString();
      }
}

/******************sortiere hier nach der Tiefe eventuell nach weiteren Kriterien filtern*********************/
void Calculation::SortScandData(){

    QList<XmlScanData> datalist_tmp = datalist;

    float depthtemp = 0, depthold = 0 ;
    float radians3d = M_PI/180;
    float zOffset = 0;
    int i = 0, a = 0, s = 0;

    i = 0;
    do{

        zOffset = datalist_tmp.at(i).polarcordinate.at(0) * qSin(datalist_tmp.at(i).tilt * radians3d);
        depthtemp = datalist_tmp.at(i).z - zOffset;
     //   qDebug() << "i: " << i << " while old: " << depthold << " tmp " << depthtemp;
        if(depthold > depthtemp){
            a = i;
            depthold = depthtemp;
        }

        i++;

        if(i >= datalist_tmp.size()){

            sortdatalist.append(datalist_tmp.at(a));
            datalist_tmp.remove(a, 1);
            s++;
            i = 0;
            a = 0;
            depthold = 0;
        }
    } while (!datalist_tmp.isEmpty());


    datalist_tmp = datalist;
    i = 0; depthold = 0;
    do{

        zOffset = datalist_tmp.at(i).polarcordinate.at(0) * qSin(datalist_tmp.at(i).tilt * radians3d);
        depthtemp = (-1*datalist_tmp.at(i).z) + zOffset;
        //   qDebug() << "i: " << i << " while old: " << depthold << " tmp " << depthtemp;
        if(depthold > depthtemp){

            a = i;
            depthold = depthtemp;
        }

        i++;

        if(i >= datalist_tmp.size()){

            sortdatalist_v2.append(datalist_tmp.at(a));
            datalist_tmp.remove(a, 1);
            s++;
            i = 0;
            a = 0;
            depthold = 0;
        }
    } while (!datalist_tmp.isEmpty());

}

void Calculation::DebugXmlData(){

    // Ausgabe der Header-Parameter
     qDebug() << "Header :";
     qDebug() << "MeasureNumber:" << xmlmaindata.measurenumber ;
     qDebug() << "Date:" << xmlmaindata.date ;
     qDebug() << "Operator:" << xmlmaindata.operatorr ;
     qDebug() << "Comment:" << xmlmaindata.comment ;

     qDebug() << "Sonar:" << xmlmaindata.sonarsteps << "\n\n";

    // Ausgabe der Scan Daten

    qDebug() << "Scan Data List:";
    for (const XmlScanData &xmlscandata : datalist) {

        qDebug() << "\n\nPos: " << xmlscandata.scanpos
                 << ", Z: " << xmlscandata.z
                 << ", Tilt: " << xmlscandata.tilt
                 << ", Date: " << xmlscandata.date
                 << ", Time: " << xmlscandata.time
                 << ", Average Z: " << xmlscandata.averageZ;

        qDebug() << "\npolarcordinate:" << datalist.data()->polarcordinate
                 << "\npolarcordinate with out tilt:" << datalist.data()->polarcordinatewithouttilt
                 << "\n\nX:" << datalist.data()->cardcordinateX
                 << "\n\nY:" << datalist.data()->cardcordinateY
                 << "\n\nZ:" << datalist.data()->cardcordinateZ;

    }
}

void Calculation::settTextXML(const QString &text){
    m_text = text;
    startCalculation();
}
