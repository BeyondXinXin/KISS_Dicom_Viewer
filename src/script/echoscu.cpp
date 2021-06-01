#define ECHOSCU_CPP
#include "echoscu.h"
#include <QString>

#include "dcmtk/config/osconfig.h"
/* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CSTDARG
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmnet/dimse.h"
#include "dcmtk/dcmnet/diutil.h"

//----------------------------------------------------------------
/* DICOM 标准转移语法 */
static const char * transferSyntaxes[] = {
    UID_LittleEndianImplicitTransferSyntax, /* 默认 */
    UID_LittleEndianExplicitTransferSyntax,
    UID_BigEndianExplicitTransferSyntax,
    UID_JPEGProcess1TransferSyntax,
    UID_JPEGProcess2_4TransferSyntax,
    UID_JPEGProcess3_5TransferSyntax,
    UID_JPEGProcess6_8TransferSyntax,
    UID_JPEGProcess7_9TransferSyntax,
    UID_JPEGProcess10_12TransferSyntax,
    UID_JPEGProcess11_13TransferSyntax,
    UID_JPEGProcess14TransferSyntax,
    UID_JPEGProcess15TransferSyntax,
    UID_JPEGProcess16_18TransferSyntax,
    UID_JPEGProcess17_19TransferSyntax,
    UID_JPEGProcess20_22TransferSyntax,
    UID_JPEGProcess21_23TransferSyntax,
    UID_JPEGProcess24_26TransferSyntax,
    UID_JPEGProcess25_27TransferSyntax,
    UID_JPEGProcess28TransferSyntax,
    UID_JPEGProcess29TransferSyntax,
    UID_JPEGProcess14SV1TransferSyntax,
    UID_RLELosslessTransferSyntax,
    UID_JPEGLSLosslessTransferSyntax,
    UID_JPEGLSLossyTransferSyntax,
    UID_DeflatedExplicitVRLittleEndianTransferSyntax,
    UID_JPEG2000LosslessOnlyTransferSyntax,
    UID_JPEG2000TransferSyntax,
    UID_MPEG2MainProfileAtMainLevelTransferSyntax,
    UID_MPEG2MainProfileAtHighLevelTransferSyntax,
    UID_JPEG2000Part2MulticomponentImageCompressionLosslessOnlyTransferSyntax,
    UID_JPEG2000Part2MulticomponentImageCompressionTransferSyntax
};

//----------------------------------------------------------------
bool EchoSCU(const QString & peer_title,
             const QString & our_title,
             const QString & hostname,
             int port,
             QString & msg)
{
    //------------------------------Initialization Work----------------------------//
    T_ASC_Network * net;
    T_ASC_Parameters * params;
    T_ASC_Association * assoc;
    OFString temp_str;
    bool ret = false;
    DIC_NODENAME local_host;
    DIC_NODENAME peer_host;
    DIC_US msg_id;
    DIC_US status;
    DcmDataset * status_detail = nullptr;
    int presentation_context_id = 1;
    OFCondition cond = ASC_initializeNetwork(NET_REQUESTOR, 0, 6, &net);
    if (cond.bad()) {
        DimseCondition::dump(temp_str, cond);
        msg = QString::fromLatin1(temp_str.c_str());
        goto Cleanup;
    }
    cond = ASC_createAssociationParameters(&params, ASC_DEFAULTMAXPDU);
    if (cond.bad()) {
        DimseCondition::dump(temp_str, cond);
        msg = QString::fromLatin1(temp_str.c_str());
        goto Cleanup;
    }
    ASC_setAPTitles(params,
                    our_title.toLocal8Bit().data(),
                    peer_title.toLocal8Bit().data(),
                    nullptr);
    cond = ASC_setTransportLayerType(params, OFFalse);
    if (cond.bad()) {
        DimseCondition::dump(temp_str, cond);
        msg = QString::fromLatin1(temp_str.c_str());
        goto Cleanup;
    }
    // gethostname(local_host, sizeof(local_host) - 1);
    sprintf(peer_host, "%s:%d", hostname.toLocal8Bit().data(), port);
    ASC_setPresentationAddresses(params, local_host, peer_host);
    cond = ASC_addPresentationContext(
      params, static_cast<unsigned char>(presentation_context_id),
      UID_VerificationSOPClass, transferSyntaxes, 3);
    presentation_context_id += 2;
    if (cond.bad()) {
        DimseCondition::dump(temp_str, cond);
        msg = QString::fromLatin1(temp_str.c_str());
        goto Cleanup;
    }
    cond = ASC_requestAssociation(net, params, &assoc);
    if (cond.bad()) {
        if (cond == DUL_ASSOCIATIONREJECTED) {
            T_ASC_RejectParameters rej;
            ASC_getRejectParameters(params, &rej);
            ASC_printRejectParameters(temp_str, &rej);
            msg = QString("Association rejected: %1").arg(temp_str.c_str());
            goto Cleanup;
        } else {
            DimseCondition::dump(temp_str, cond);
            msg = QString("Association request failed: %1").arg(temp_str.c_str());
            goto Cleanup;
        }
    }
    if (ASC_countAcceptedPresentationContexts(params) == 0) {
        msg = QString("No Acceptable Presentation Contexts");
        goto Cleanup;
    }
    //------------------------------Real Work----------------------------//
    msg_id = assoc->nextMsgID++;
    cond = DIMSE_echoUser(
      /* in */ assoc, msg_id,
      /* blocking info for response */ DIMSE_BLOCKING, 0,
      /* out */ &status,
      /* Detail */ &status_detail);
    if (status_detail != nullptr) {
        delete status_detail;
    }
    if (cond == EC_Normal) {
        cond = ASC_releaseAssociation(assoc);
        ret = true;
    } else if (cond == DUL_PEERABORTEDASSOCIATION) {
    } else {
        DimseCondition::dump(temp_str, cond);
        msg = QString::fromLatin1(temp_str.c_str());
        cond = ASC_abortAssociation(assoc);
    }
    //------------------------------Cleanup Work----------------------------//
Cleanup:
    cond = ASC_destroyAssociation(&assoc);
    cond = ASC_dropNetwork(&net);
    return ret;
}
