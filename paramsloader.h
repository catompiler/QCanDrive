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

    enum Operation {
        OP_NONE = 0,
        OP_UPLOAD_FROM_DRIVE = 1,
        OP_DOWNLOAD_TO_DRIVE = 2
    };


    explicit ParamsLoader(QObject *parent = nullptr);
    ~ParamsLoader();

    SLCanOpenNode* getSLCanOpenNode();
    void setSLCanOpenNode(SLCanOpenNode* slcon);

    CO::NodeId nodeId() const;
    void setNodeId(CO::NodeId newNodeId);

    RegVarList varList();
    const RegVarList& varList() const;
    void setVarList(RegVarList newVarList);

    RegValuesList valuesList();
    const RegValuesList& valuesList() const;
    void setValuesList(RegValuesList newValuesList);

    Operation lastOperation() const;

    bool busy() const;
    bool cancelled() const;

    void clear();

    bool uploadFromDrive();
    bool downloadToDrive();
    bool downloadToDrive(RegValuesList newValuesList);

    int valuesToProcess() const;
    int valuesProcessed() const;

signals:
    void errorOccured(const QString& errStr);
    void progressChanged(int progress); //!< От 0 до размера RegVarList.
    void done();
    void finished();

public slots:
    void cancel();

private slots:
    void sdoFinished();

private:
    SLCanOpenNode* m_slcon;
    CO::NodeId m_nodeId;

    SDOComm* m_comm;

    State m_state;
    Operation m_lastOp;

    RegVarList* m_regVarList;
    int m_curVarIndex;
    int m_valuesProcessed;
    int32_t m_value;

    RegValuesList* m_valuesList;

    bool processUpload();
    bool processDownload();
};

#endif // PARAMSLOADER_H
