#ifndef WRP_PROJECT_H
#define WRP_PROJECT_H

#include <QStandardItemModel>
#include <QString>
#include <QDomElement>

const int UUID_ROLE      = Qt::UserRole + 100;
const int TYPE_ROLE      = Qt::UserRole + 101;
const int MSG_LIST_ROLE  = Qt::UserRole + 102;   //其实是icdname不是msgid
const int FULL_PATH_ROLE = Qt::UserRole + 103;

//type属性 package =0;block=2;fmu=1;icdSource=5;dataOutPuter=6

class WRP_Project
{
public:
    WRP_Project();

    QString getFilePath() const;
    bool setFilePath(const QString& filePath);

    QStandardItemModel* getSystemArchitectureModel();

    const QMap<QString,QString>& getIcdContainer();
private:
    bool loadFile();
    QList<QStandardItem*> getArchitectureItemFromModules(QDomElement modulesElement, QList<QString> dataSource, QList<QString> dataOutputer);
    void loadIcdFormicds(QDomElement icdsElement);
private:
    QString m_filePath;
    QStandardItemModel m_systemArchitectureModel;
    QMap<QString,QString> m_icdContainer;

};

#endif // WRP_PROJECT_H
