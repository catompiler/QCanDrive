#ifndef PARAMSLOADER_H
#define PARAMSLOADER_H

#include <QObject>
#include "cotypes.h"
#include "reglistmodel.h"
#include "paramsreader.h"


class SDOComm;
class SLCanOpenNode;


class ParamsLoader : public QObject
{
    Q_OBJECT
public:

    using RegValuesList = ParamsReader::RegValuesList;


    enum State {
        STATE_NONE = 0,
        STATE_UPLOAD = 1,
        STATE_DOWNLOAD = 2
    };


    explicit ParamsLoader(QObject *parent = nullptr);
    ~ParamsLoader();

    SLCanOpenNode* getSLCanOpenNode();
    void setSLCanOpenNode(SLCanOpenNode* slcon);

    CO::NodeId nodeId() const;
    void setNodeId(CO::NodeId newNodeId);

    RegVarList regVarList();
    const RegVarList regVarList() const;
    void setVarList(RegVarList varList);

    void clear();

    bool uploadFromDrive();
    bool downloadToDrive(RegValuesList valuesList);

    int valuesProcessed() const;

signals:
    void errorOccured(const QString& errStr);
    void progressChanged(int progress); //!< От 0 до размера RegVarList.
    void done();
    void finished();

private slots:
    void sdoFinished();

private:
    SLCanOpenNode* m_slcon;
    CO::NodeId m_nodeId;

    SDOComm* m_comm;

    State m_state;

    RegVarList* m_regVarList;
    int m_curVarIndex;
    int m_valuesProcessed;
    int32_t m_value;

    RegValuesList* m_valuesList;

    bool processUpload();
    bool processDownload();
};

#endif // PARAMSLOADER_H
