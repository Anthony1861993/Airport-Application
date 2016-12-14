#include <QtCore/QCoreApplication>
#include <QtCore>
#include <QtXml>
#include <QDebug>
#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

struct node
{
    string ID;
    double value;
};
struct edge
{
    string sourceID;
    string destID;
};

QDomElement getElement(QDomElement root, QString TagName, QString attribute, QString value);
void putIntoInuse(string ID, double distance, vector<node> &inUse);
string putIntoUsed(vector<node> &inUse, vector<node> &used);
void putIntoArrows(string sourceID, string destID, vector<edge> &arrows);
bool alreadyInVector(string ID, vector<node> theVector);

void getInput(QDomElement root, string &sourceCode, string &destCode);
void DijkstraAlgorithm(QDomElement root,string sourceCode, string destCode);
void printSolution(QDomElement root, vector<edge> solution);
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    
    QDomDocument document;

    // Loads the xml file to gain assess to all the information
    QFile file("C:/CS8/MyXML2.xml");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open file";
        return -1;
    }
    else
    {
        if(!document.setContent(&file))
        {
            qDebug() << "Failed to load document";
            return -1;
        }
        file.close();
    }
    QDomElement root = document.firstChildElement();

    string sourceCode, destCode;
    char ans = ' ';
    while (toupper(ans) != 'X')
    {
        getInput(root, sourceCode, destCode);
        cout<<"Processing (this might take a few seconds)...";
        DijkstraAlgorithm(root, sourceCode, destCode);
        cout<<"E[x]it? ";
        cin>>ans;
    }

    return 0;
    //return a.exec();
}

void getInput(QDomElement root, string &sourceCode, string &destCode)
{
    cout<<"Source airport code: ";
    cin>>sourceCode;
    cout<<"Destination airport code: ";
    cin>>destCode;

    bool validSourceCode = false, validDestCode;
    QDomNodeList items = root.elementsByTagName("Vertex");
    for(int i = 0; i < items.count(); ++i)
    {
        QDomNode itemnode = items.at(i);

        //convert to element
        if(itemnode.isElement())
        {
            QDomElement itemele = itemnode.toElement();
            if ((itemele.attribute("IATA") == QString::fromStdString(sourceCode)) || (itemele.attribute("ICAO") == QString::fromStdString(sourceCode)))
            {
                validSourceCode = true;
                sourceCode = itemele.attribute("ID").toStdString();
                continue;
            }
            if ((itemele.attribute("IATA") == QString::fromStdString(destCode)) || (itemele.attribute("ICAO") == QString::fromStdString(destCode)))
            {
                validDestCode = true;
                destCode = itemele.attribute("ID").toStdString();
                continue;
            }
        }
    }
    if ((!validSourceCode) || (!validDestCode))
    {
        cout<<"Invalid value!"<<endl;
        exit(1);
    }
}

void DijkstraAlgorithm(QDomElement root,string sourceCode, string destCode)
{
    vector<node> used, inUse;
    vector<edge> arrows;
    string ID;
    putIntoInuse(sourceCode, 0, inUse);
    while ( ((ID = putIntoUsed(inUse, used)) != "") && (!alreadyInVector(destCode, used)) )
    {
        QDomElement source = getElement(root, "Vertex", "ID", QString::fromStdString(ID));
        QDomNodeList items = source.elementsByTagName("edge");
        for (int i = 0; i < items.count(); ++i)
        {
            QDomNode itemnode = items.at(i);
            if (itemnode.isElement())
            {
                QDomElement itemele = itemnode.toElement();
                node theNode;
                theNode.ID = itemele.attribute("AirportID").toStdString();
                theNode.value =  used[used.size() - 1].value + itemele.attribute("Distance").toDouble();

                    if (alreadyInVector(theNode.ID, used))
                        continue;
                    else if (alreadyInVector(theNode.ID, inUse))
                    {
                        for (unsigned int i = 0; i < inUse.size(); ++i)
                            if (inUse[i].ID == theNode.ID)
                                if (theNode.value < inUse[i].value)
                                {
                                    inUse[i].value = theNode.value;
                                    putIntoArrows(used[used.size() - 1].ID, theNode.ID, arrows);
                                    break;
                                }
                    }
                    else
                    {
                        putIntoInuse(theNode.ID, theNode.value, inUse);
                        putIntoArrows(used[used.size() - 1].ID, theNode.ID, arrows);
                    }

            }
        }
    }
    cout<<endl<<"Finished :)"<<endl;


    if (used[used.size()-1].ID != destCode)
    {
        cout<<endl<<"Sorry, we cannnot find any path from the source to the destination..."<<endl;
        exit (1);
    }

    vector<edge> solution;
    string theID = destCode;
    while (theID != sourceCode)
    {
        for (unsigned int i = 0; i < arrows.size(); ++i)
            if (arrows[i].destID == theID)
            {
                theID = arrows[i].sourceID;
                solution.push_back(arrows[i]);
                //cout<<arrows[i].destID<<" <--- "<<arrows[i].sourceID<<endl;
                break;
            }
    }
    printSolution(root, solution);
}

void printSolution(QDomElement root, vector<edge> solution)
{

    QDomDocument document;

    // Loads the xml file to gain assess to the airlines information
    QFile file("C:/CS8/MyXML3.xml");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open file";
        exit (1);
    }
    else
    {
        if(!document.setContent(&file))
        {
            qDebug() << "Failed to load document";
            exit (1);
        }
        file.close();
    }
    QDomElement root2 = document.firstChildElement();

    for(vector<edge>::reverse_iterator it = solution.rbegin();it != solution.rend(); ++it )
    {
        //cout<<(*it).sourceID<<" ---> "<<(*it).destID<<endl;
        QDomElement source = getElement(root, "Vertex", "ID", QString::fromStdString((*it).sourceID));
        QDomElement dest = getElement(root, "Vertex", "ID", QString::fromStdString((*it).destID));
        QDomElement theEdge = getElement(source, "edge", "AirportID", QString::fromStdString((*it).destID));
        cout<<"Leaving "<<source.attribute("Name").toStdString()
           <<" ("<<source.attribute("City").toStdString()<<", "
          <<source.attribute("Country").toStdString()<<") "
         <<"for "<<dest.attribute("Name").toStdString()
        <<", flying "<<theEdge.attribute("Distance").toStdString()<<" km."<<endl;

        cout<<"Carrier choice: "<<endl;
        QDomNodeList items = source.elementsByTagName("edge");
        for(int i = 0; i < items.count(); ++i)
        {
            QDomNode itemnode = items.at(i);
            //convert to element
            if(itemnode.isElement())
            {
                QDomElement itemele = itemnode.toElement();
                if (itemele.attribute("AirportID") == QString::fromStdString((*it).destID))
                {
                    QDomElement carrier = getElement(root2, "airline", "ID", itemele.attribute("AirlineID"));
                    cout<<carrier.attribute("Name").toStdString()<<endl;
                }
            }
        }
        cout<<endl;
    }
}

bool alreadyInVector(string ID, vector<node> theVector)
{
    for (unsigned int i = 0; i < theVector.size(); ++i)
        if (theVector[i].ID == ID)
            return true;
    return false;
}

string putIntoUsed(vector<node> &inUse, vector<node> &used)
{
    if (inUse.empty()) return "";
    node min = inUse[0];
    for (unsigned int i = 0; i < inUse.size(); ++i)
        if (inUse[i].value < min.value)
            min = inUse[i];

    for (unsigned int i = 0; i < inUse.size(); ++i)
        if (inUse[i].value == min.value)
        {
            inUse.erase(inUse.begin() + i);
            break;
        }
    used.push_back(min);
    return min.ID;
}

void putIntoArrows(string sourceID, string destID, vector<edge> &arrows)
{
    bool alreadyIn = false;
    for (unsigned int i = 0; i < arrows.size(); ++i)
    {
        if (arrows[i].destID == destID)
        {
            arrows[i].sourceID = sourceID;
            alreadyIn = true;
            break;
        }
    }
    if (!alreadyIn)
    {
        edge theEdge;
        theEdge.sourceID = sourceID;
        theEdge.destID = destID;
        arrows.push_back(theEdge);
    }
}

void putIntoInuse(string ID, double distance, vector<node> &inUse)
{
    node theNode;
    theNode.ID = ID;
    theNode.value = distance;
    inUse.push_back(theNode);
}

QDomElement getElement(QDomElement root, QString TagName, QString attribute, QString value)
{
    QDomNodeList items = root.elementsByTagName(TagName);
    for(int i = 0; i < items.count(); ++i)
    {
        QDomNode itemnode = items.at(i);

        //convert to element
        if(itemnode.isElement())
        {
            QDomElement itemele = itemnode.toElement();
            if (itemele.attribute(attribute) == value)
                return itemele;
        }
    }
    cout<<"Cannot identify this value!"<<endl;
    exit(1);
}

