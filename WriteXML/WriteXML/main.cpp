#include <QtCore>
#include <QtXml>
#include <QDebug>
#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QDomDocument document;
    QDomElement root = document.createElement("Vertices");
    document.appendChild(root);

    ifstream in;
    string line, word;

    // Processing "airports.dat"
    in.open("airports.dat");
    if(in.fail())
    {
        cout<<"File does not exist!!"<<endl;
        return -1;
    }
    while (!in.eof())
    //for (int i = 0; i < 2540; ++i)
    {
        QDomElement vertex = document.createElement("Vertex");
        getline(in, line);
        stringstream ss(line);
        int j = 0;
        while (getline(ss, word, ',') && j < 8)
        {

            if (j >= 1 && j <= 5)
            {
                word.erase(0, 1);
                word.erase(word.size() - 1, 1);
            }
            //cout<<word<<endl;
            if (j == 0)
              vertex.setAttribute("ID", QString::fromStdString(word));
            if (j == 1)
              vertex.setAttribute("Name", QString::fromStdString(word));
            if (j == 2)
              vertex.setAttribute("City", QString::fromStdString(word));
            if (j == 3)
              vertex.setAttribute("Country", QString::fromStdString(word));
            if (j == 4)
              vertex.setAttribute("IATA", QString::fromStdString(word));
            if (j == 5)
              vertex.setAttribute("ICAO", QString::fromStdString(word));
            if (j == 6)
              vertex.setAttribute("Lat", QString::fromStdString(word));
            if (j == 7)
              vertex.setAttribute("Long", QString::fromStdString(word));
            ++j;
        }
        root.appendChild(vertex);
    }
    in.close();

    // Processing "routes.dat"
    in.open("routes.dat");
    if(in.fail())
    {
        cout<<"File does not exist!!"<<endl;
        return -1;
    }
    while (!in.eof())
    //for (int i = 0; i < 1136; ++i)
    {
        getline(in, line);
        stringstream ss(line);
        int j = 0;
        bool successful = false;
        string airlineID, airportID;
        QDomElement sourceVertex, destVertex;
        while (getline(ss, word, ',') && j < 6)
        {
            if (word == "\\N")
            {
                successful = false;
                break;
            }
            //cout<<word<<endl;
            if (j == 1)
                airlineID = word;
            if (j == 3)
            {
                // this is when word has the 'source ID' info, now we have to find its location in the xml file
                QDomNodeList items = root.elementsByTagName("Vertex");
                 for(int i = 0; i < items.count(); i++)
                 {
                    QDomNode itemnode = items.at(i);
                    //convert to element
                    if(itemnode.isElement())
                    {
                        QDomElement itemele = itemnode.toElement();
                        if (itemele.attribute("ID") == QString::fromStdString(word))
                        {
                            sourceVertex = itemele;
                            successful = true;
                            break;
                        }
                    }
                 }
            }
            if (j == 5)
            {
                airportID = word;

                // Now we find its location in the xml file
                QDomNodeList items = root.elementsByTagName("Vertex");
                 for(int i = 0; i < items.count(); i++)
                 {
                    QDomNode itemnode = items.at(i);
                    //convert to element
                    if(itemnode.isElement())
                    {
                        QDomElement itemele = itemnode.toElement();
                        if (itemele.attribute("ID") == QString::fromStdString(word))
                        {
                            destVertex = itemele;
                            break;
                        }
                    }
                 }
            }
            ++j;
        }
        if (successful)
        {
            QDomElement edge = document.createElement("edge");
            edge.setAttribute("AirportID", QString::fromStdString(airportID));
            edge.setAttribute("AirlineID", QString::fromStdString(airlineID));

            //calculating the great circle distance between the 2 locations

            double lat1 = atof(sourceVertex.attribute("Lat").toStdString().c_str());
            double long1 = atof(sourceVertex.attribute("Long").toStdString().c_str());
            double lat2 = atof(destVertex.attribute("Lat").toStdString().c_str());
            double long2 = atof(destVertex.attribute("Long").toStdString().c_str());

            double longDiff = long1 - long2;
            //if (longDiff < 0)
              //  longDiff *= -1.;
            double PI = 3.141592654;

            double result = 6371 * acos(sin(lat1*PI/180)*sin(lat2*PI/180) + cos(lat1*PI/180)*cos(lat2*PI/180)*cos(longDiff*PI/180));
            edge.setAttribute("Distance", QString::number(result));

            sourceVertex.appendChild(edge);
        }
    }
    in.close();




    // Write to XML file
    QFile file("C:/CS8/MyXML2.xml");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        cout<<"Failed to open file for writting"<<endl;
        return -1;
    }
    else
    {
        QTextStream stream(&file);
        stream << document.toString();
        file.close();
        cout<<"Finished"<<endl;
    }


    return 0;
    //return a.exec();
}
