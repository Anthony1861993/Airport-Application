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
    QDomElement root = document.createElement("Airlines");
    document.appendChild(root);

    ifstream in;
    string line, word;

    // Processing "airlines.dat"
    in.open("airlines.dat");
    if(in.fail())
    {
        cout<<"File does not exist!!"<<endl;
        return -1;
    }
    while (!in.eof())
    //for (int i = 0; i < 2540; ++i)
    {
        QDomElement airline = document.createElement("airline");
        getline(in, line);
        stringstream ss(line);
        int j = 0;
        while (getline(ss, word, ',') && j < 2)
        {
            if (j)
            {
                word.erase(0, 1);
                word.erase(word.size() - 1, 1);
                airline.setAttribute("Name", QString::fromStdString(word));
            }
            else
                airline.setAttribute("ID", QString::fromStdString(word));
            ++j;
        }
        root.appendChild(airline);
    }
    in.close();


    // Write to XML file
    QFile file("C:/CS8/MyXML3.xml");
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
