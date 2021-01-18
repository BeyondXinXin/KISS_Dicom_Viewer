#ifndef STORESCPTHREAD_H
#define STORESCPTHREAD_H

#include <QThread>

#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/dcmnet/assoc.h"
#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmnet/dimse.h"

struct T_ASC_Network;
struct T_ASC_Association;
struct T_DIMSE_Message;
class DcmFileFormat;
class StudyRecord;
class DcmAssociationConfiguration;

class StoreScpThread : public QThread {
    Q_OBJECT
  public:
    explicit StoreScpThread(QObject *parent = nullptr);
    void setAbort(const bool &yes);
    void run();
  private:
    OFCondition AcceptAssociation(T_ASC_Network *net,
                                  DcmAssociationConfiguration &asccfg);
    OFCondition ProcessCommands(T_ASC_Association *assoc);
    OFCondition EchoSCP( T_ASC_Association *assoc,
                         T_DIMSE_Message *msg, T_ASC_PresentationContextID presID);
    OFCondition StoreSCP(T_ASC_Association *assoc,
                         T_DIMSE_Message *msg, T_ASC_PresentationContextID presID);
  private:
    bool abort_;

};



#endif // STORESCPTHREAD_H
