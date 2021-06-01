#include "storescpthread.h"
#include "global/KissDicomViewConfig.h"
#include "global/global.h"
#include "global/stable.h"
#include "global/studyrecord.h"
#include <dao/KissDb>

#include <QDebug>
#include <QDir>

#include "dcmtk/config/osconfig.h"
/* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTRING
#define INCLUDE_CSTDARG
#define INCLUDE_CCTYPE
#define INCLUDE_CSIGNAL

BEGIN_EXTERN_C
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
END_EXTERN_C

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcmetinf.h"
#include "dcmtk/dcmdata/dcostrmz.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmnet/cond.h"
#include "dcmtk/dcmnet/dcasccff.h"
#include "dcmtk/dcmnet/dcasccfg.h"
#include "dcmtk/dcmnet/dicom.h"
#include "dcmtk/dcmnet/dimse.h"
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmsr/dsrdoc.h"
#include "dcmtk/ofstd/ofdatime.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofstdinc.h"

//----------------------------------------------------------------
static void insertImageToDB(
  DcmFileFormat * ff, StudyRecord * study, QString & patientName);

//----------------------------------------------------------------
struct StoreCallbackData
{
    StudyRecord * study;
    DcmFileFormat * dcmff;
    T_ASC_Association * assoc;
    QString patientInfo;
};

//----------------------------------------------------------------
static void StoreSCPCallback(
  void * callbackData,
  T_DIMSE_StoreProgress * progress,
  T_DIMSE_C_StoreRQ * req,
  char * /*imageFileName*/, DcmDataset ** imageDataSet,
  T_DIMSE_C_StoreRSP * rsp,
  DcmDataset ** statusDetail)
{
    DIC_UI sopClass;
    DIC_UI sopInstance;
    if (progress->state == DIMSE_StoreEnd) {
        *statusDetail = nullptr;
        StoreCallbackData * cbdata = OFstatic_cast(StoreCallbackData *, callbackData);
        insertImageToDB(cbdata->dcmff, cbdata->study, cbdata->patientInfo);
        if (rsp->DimseStatus == STATUS_Success) {
            if (!DU_findSOPClassAndInstanceInDataSet(*imageDataSet,
                                                     sopClass, sizeof(sopClass),
                                                     sopInstance, sizeof(sopInstance))) {
                rsp->DimseStatus = STATUS_STORE_Error_CannotUnderstand;
            } else if (strcmp(sopClass, req->AffectedSOPClassUID) != 0) {
                rsp->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
            } else if (strcmp(sopInstance, req->AffectedSOPInstanceUID) != 0) {
                rsp->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
            }
        }
    }
}

//----------------------------------------------------------------
StoreScpThread::StoreScpThread(QObject * parent)
  : QThread(parent)
  , abort_(false)
{
}

//----------------------------------------------------------------
void StoreScpThread::setAbort(const bool & yes)
{
    abort_ = yes;
}

//----------------------------------------------------------------
void StoreScpThread::run()
{
    //-----------------------------初始化端口监听----------------------------------------//
    /* 创建T_ASC_Network*的实例。 */
    T_ASC_Network * net;
    DcmAssociationConfiguration asccfg;
    OFString temp_str;
    LocalSettings settings;
    int port = settings.statInfo.store_port;
    OFCondition cond = ASC_initializeNetwork(NET_ACCEPTOR, port, 30, &net);
    if (cond.bad()) {
        DimseCondition::dump(temp_str, cond);
        qDebug() << QString("无法创建网络: %1.")
                      .arg(temp_str.c_str());
    }
    //-------------------------------绑定端口提供scp服务--------------------------------------//
    while (cond.good() && (!abort_)) {
        /* 接收关联并确认或拒绝它。
         * 如果这个联系得到承认，
         * 提供相应的服务，并根据需要调用一个或多个服务。 */
        cond = AcceptAssociation(net, asccfg);
    }
    //--------------------------------销毁端口监听-------------------------------------//
    /* 释放内存 T_ASC_Network*.
     * 此调用与上面调用的ASC_initializeNetwork（…）相对应。 */
    if (cond.good()) {
        cond = ASC_dropNetwork(&net);
    }
    if (cond.bad()) {
        DimseCondition::dump(temp_str, cond);
        qDebug() << QString(temp_str.c_str());
    }
    QThread::sleep(1);
}

//----------------------------------------------------------------
/**
 * @brief StoreScpThread::AcceptAssociation
 * @param net
 * @return
 */
OFCondition StoreScpThread::AcceptAssociation(
  T_ASC_Network * net, DcmAssociationConfiguration & /*asccfg*/)
{
    //------------------------------Initialization Work----------------------------//
    char buf[BUFSIZ];
    T_ASC_Association * assoc;
    OFCondition cond;
    OFString temp_str;
    const char * knownAbstractSyntaxes[] = {
        UID_VerificationSOPClass
    };
    const char * transferSyntaxes[] = {
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
    };
    int numTransferSyntaxes = 0;
    // 尝试接收关联。在这里，我们要么使用阻塞，要么使用非阻塞，这取决于是否设置了选项--eostudy timeout。
    cond = ASC_receiveAssociation(net, &assoc, ASC_DEFAULTMAXPDU);
    // 如果出了什么差错，一定要处理好
    if (cond.bad()) {
        DimseCondition::dump(temp_str, cond);
        qDebug() << QString("接收关联失败: %1.").arg(temp_str.c_str());
    }
    if (gLocalByteOrder == EBO_LittleEndian) { /* defined in dcxfer.h */
        transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
    } else {
        transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
    }
    transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
    numTransferSyntaxes = 3;
    /* 接受验证SOP类（如有） */
    if (cond.good()) {
        cond = ASC_acceptContextsWithPreferredTransferSyntaxes(
          assoc->params, knownAbstractSyntaxes,
          DIM_OF(knownAbstractSyntaxes), transferSyntaxes, numTransferSyntaxes);
        if (cond.bad()) {
            DimseCondition::dump(temp_str, cond);
            qDebug() << QString(temp_str.c_str());
        }
    }
    /* 存储SOP类uid的数组来自dcuid.h */
    if (cond.good()) {
        cond = ASC_acceptContextsWithPreferredTransferSyntaxes(
          assoc->params, dcmAllStorageSOPClassUIDs,
          numberOfDcmAllStorageSOPClassUIDs,
          transferSyntaxes, numTransferSyntaxes);
        if (cond.bad()) {
            DimseCondition::dump(temp_str, cond);
            qDebug() << QString(temp_str.c_str());
        }
    }
    /* 设置应用程序标题 */
    LocalSettings settings;
    QString aetitle = settings.statInfo.aetitle;
    if (aetitle.isEmpty()) {
        qDebug() << "aetitle is DEFAULT 'DRDCM' ";
        aetitle = "DRDCM";
    }
    ASC_setAPTitles(assoc->params, nullptr, nullptr, aetitle.toLocal8Bit().data());
    /* 承认或拒绝此关联 */
    if (cond.good()) {
        cond = ASC_getApplicationContextName(assoc->params, buf, sizeof(buf));
        if ((cond.bad()) || strcmp(buf, UID_StandardApplicationContext) != 0) {
            /* 拒绝：不支持应用程序上下文名称 */
            T_ASC_RejectParameters rej = {
                ASC_RESULT_REJECTEDPERMANENT,
                ASC_SOURCE_SERVICEUSER,
                ASC_REASON_SU_APPCONTEXTNAMENOTSUPPORTED
            };
            DimseCondition::dump(temp_str, cond);
            qDebug() << QString("关联被拒绝：应用程序上下文名称错误: %1.").arg(buf);
            cond = ASC_rejectAssociation(assoc, &rej);
            if (cond.bad()) {
                DimseCondition::dump(temp_str, cond);
                qDebug() << QString(temp_str.c_str());
            }
        } else {
            cond = ASC_acknowledgeAssociation(assoc);
            if (cond.bad()) {
                DimseCondition::dump(temp_str, cond);
                qDebug() << QString(temp_str.c_str());
            }
        }
    }
    //------------------------------Real Work----------------------------//
    if (cond.good()) {
        // 将调用和调用的 aetitle 存储在全局变量中，以启用使用它们的--exec选项。 aetitles 可能包含空格字符。
        DIC_AE callingTitle;
        DIC_AE calledTitle;
        ASC_getAPTitles(assoc->params, callingTitle, sizeof(callingTitle),
                        calledTitle, sizeof(calledTitle), nullptr, 0)
          .good();
        // 现在做实际工作，即通过建立的网络连接接收DIMSE命令，并相应地处理这些命令.
        // 对于storscp，只能处理 C-ECHO-RQ 和 C-STORE-RQ 命令.
        cond = ProcessCommands(assoc);
        if (cond == DUL_PEERREQUESTEDRELEASE) {
            cond = ASC_acknowledgeRelease(assoc);
        } else {
            DimseCondition::dump(temp_str, cond);
            qDebug() << QString("DIMSE失败（中止关联）: %1.").arg(temp_str.c_str());
            /* 某种错误，所以中止了关联n */
            cond = ASC_abortAssociation(assoc);
        }
    }
    //------------------------------Cleanup Work----------------------------//
    cond = ASC_dropSCPAssociation(assoc);
    if (cond.bad()) {
        DimseCondition::dump(temp_str, cond);
        qDebug() << QString(temp_str.c_str());
    }
    cond = ASC_destroyAssociation(&assoc);
    if (cond.bad()) {
        DimseCondition::dump(temp_str, cond);
        qDebug() << QString(temp_str.c_str());
    }
    return cond;
}

//----------------------------------------------------------------
/**
 * @brief StoreScpThread::ProcessCommands
 * @param assoc
 * @return
 */
OFCondition StoreScpThread::ProcessCommands(T_ASC_Association * assoc)
{
    OFCondition cond = EC_Normal;
    T_DIMSE_Message msg;
    T_ASC_PresentationContextID presID = 0;
    DcmDataset * statusDetail = nullptr;
    // 启动循环以能够接收多个DIMSE命令
    while (cond == EC_Normal || cond == DIMSE_NODATAAVAILABLE || cond == DIMSE_OUTOFRESOURCES) {
        // 通过网络接收DIMSE命令
        cond = DIMSE_receiveCommand(assoc, DIMSE_BLOCKING, 0, &presID, &msg, &statusDetail);
        // 如果收到的命令有额外的状态详细信息，则转储此信息
        if (statusDetail != nullptr) {
            delete statusDetail;
        }
        // 检查对等机是否释放或中止，或者我们是否有有效的消息
        if (cond == EC_Normal) { // 收到正常请求
            switch (msg.CommandField) {
            case DIMSE_C_ECHO_RQ:
                // 处理 C-ECHO-Request
                qDebug() << QString("收到 C-ECHO-Request 服务请求，开始处理");
                cond = EchoSCP(assoc, &msg, presID);
                break;
            case DIMSE_C_STORE_RQ:
                // 处理 C-STORE-Request
                qDebug() << QString("收到 C-STORE-Request 服务请求，开始处理");
                cond = StoreSCP(assoc, &msg, presID);
                break;
            default:
                // 其他服务不处理 （查询和下载 还没空开发）
                qDebug() << QString("无法处理命令: 0x%1.").arg(static_cast<unsigned>(msg.CommandField));
                cond = DIMSE_BADCOMMANDTYPE;
                break;
            }
        }
    }
    return cond;
}

//----------------------------------------------------------------
/**
 * @brief StoreScpThread::EchoSCP
 * 处理 C-ECHO-Request
 * @param assoc
 * @param msg
 * @param presID
 * @return
 */
OFCondition StoreScpThread::EchoSCP(
  T_ASC_Association * assoc, T_DIMSE_Message * msg, T_ASC_PresentationContextID presID)
{
    // 初始化一些变量
    OFString temp_str;
    OFCondition cond = DIMSE_sendEchoResponse(assoc, presID,
                                              &msg->msg.CEchoRQ, STATUS_Success, nullptr);
    if (cond.bad()) {
        DimseCondition::dump(temp_str, cond);
        qDebug() << QString("Echo SCP 服务失败: %1.").arg(temp_str.c_str());
    } else {
        qDebug() << QString("Echo SCP 测试成功: %1").arg(QTime::currentTime().toString(NORMAL_DATETIME_FORMAT));
    }
    return cond;
}

//----------------------------------------------------------------
/**
 * @brief StoreScpThread::StoreSCP
 * 处理 C-STORE-Request
 * @param assoc
 * @param msg
 * @param presID
 * @return
 */
OFCondition StoreScpThread::StoreSCP(
  T_ASC_Association * assoc,
  T_DIMSE_Message * msg,
  T_ASC_PresentationContextID presID)
{
    OFCondition cond = EC_Normal;
    T_DIMSE_C_StoreRQ * req;
    // 将C-STORE-RQ命令的实际信息分配给局部变量
    req = &msg->msg.CStoreRQ;
    // 初始化一些变量
    StoreCallbackData callbackData;
    DcmFileFormat dcmff;
    StudyRecord study;
    callbackData.assoc = assoc;
    callbackData.dcmff = &dcmff;
    callbackData.study = &study;
    const char * aet = nullptr;
    const char * aec = nullptr;
    // 将 SourceApplicationEntityTitle 存储在 metaheader 中
    if (assoc && assoc->params) {
        aet = assoc->params->DULparams.callingAPTitle;
        aec = assoc->params->DULparams.calledAPTitle;
        if (aet) {
            dcmff.getMetaInfo()->putAndInsertString(DCM_SourceApplicationEntityTitle, aet);
        }
    }
    LocalSettings settings;
    QString aetitle = settings.statInfo.aetitle;
    if (QString(aec) != aetitle) {
        qDebug() << "名称校验失败" << aet << QString(aec) << aetitle;
    } else {
        // 定义一个地址，用于存储通过网络接收的信息
        DcmDataset * dset = dcmff.getDataset();
        cond = DIMSE_storeProvider(assoc, presID, req, nullptr, OFTrue, &dset,
                                   StoreSCPCallback, &callbackData, DIMSE_BLOCKING, 0);
        // 如果出现错误，请转储相应的信息，必要时删除输出文件
        if (cond.bad()) {
            OFString temp_str;
            DimseCondition::dump(temp_str, cond);
            qDebug() << QString("Store SCP 失败: %1.").arg(temp_str.c_str());
        } else {
            qDebug() << QString("Store SCP 成功: %1 : %2, %3").arg(callbackData.patientInfo, QString::fromLocal8Bit(aet), QTime::currentTime().toString(NORMAL_DATETIME_FORMAT));
        }
    }
    // 返回返回值
    return cond;
}

//----------------------------------------------------------------
static void insertImageToDB(
  DcmFileFormat * ff, StudyRecord * study, QString &)
{
    DcmDataset * dset;
    if (ff && (dset = ff->getDataset()) && study) {
        const char * value = nullptr;
        QString studyUid, seriesUid, instUid, sopClassUid;
        dset->findAndGetString(DCM_StudyInstanceUID, value);
        studyUid = QString::fromLatin1(value);
        dset->findAndGetString(DCM_SeriesInstanceUID, value);
        seriesUid = QString::fromLatin1(value);
        dset->findAndGetString(DCM_SOPInstanceUID, value);
        instUid = QString::fromLatin1(value);
        dset->findAndGetString(DCM_SOPClassUID, value);
        sopClassUid = QString::fromLatin1(value);
        if (!(studyUid.isEmpty() || seriesUid.isEmpty() || instUid.isEmpty() || sopClassUid.isEmpty())) {
            if (study->study_uid_ != studyUid) {
                study->study_uid_ = studyUid;
                study = new StudyRecord(studyUid);
                dset->findAndGetString(DCM_AccessionNumber, value);
                study->acc_number_ = QString::fromLocal8Bit(value).remove(QChar(' '));
                dset->findAndGetString(DCM_PatientID, value);
                study->patient_id_ = QString::fromLocal8Bit(value);
                dset->findAndGetString(DCM_PatientName, value);
                study->patient_name_ = QString::fromLocal8Bit(value);
                dset->findAndGetString(DCM_PatientSex, value);
                study->patient_sex_ = QString::fromLocal8Bit(value).remove(QChar(' '));
                dset->findAndGetString(DCM_PatientBirthDate, value);
                study->patient_birth_ = QDate::fromString(QString::fromLatin1(value), "yyyyMMdd");
                dset->findAndGetString(DCM_PatientAge, value);
                study->patient_age_ = QString::fromLocal8Bit(value).remove(QChar(' '));
                dset->findAndGetString(DCM_StudyDate, value);
                study->study_time_.setDate(QDate::fromString(QString::fromLatin1(value), "yyyyMMdd"));
                dset->findAndGetString(DCM_StudyTime, value);
                study->study_time_.setTime(FormatDicomTime(QString::fromLatin1(value)));
                dset->findAndGetString(DCM_StudyDescription, value);
                study->study_desc_ = QString::fromLocal8Bit(value);
                dset->findAndGetString(DCM_InstitutionName, value);
                study->institution_ = QString::fromLocal8Bit(value);
                dset->findAndGetString(DCM_Modality, value);
                study->modality_ = QString::fromLatin1(value);
                if (sopClassUid == UID_XRayAngiographicImageStorage || // 造影血管
                    true) {
                    OFCondition cond =
                      ff->saveFile(
                        QString("./ScpCache/tmp.dcm").toLocal8Bit().data(),
                        dset->getOriginalXfer(),
                        EET_ExplicitLength, EGL_recalcGL,
                        EPD_withoutPadding, 0, 0, EWM_fileformat);
                    if (cond.bad()) {
                        qDebug() << QString("无法写入DICOM文件: %1.").arg(cond.text());
                    } else {
                        ImageRecord * image = new ImageRecord(instUid);
                        image->sop_class_uid_ = sopClassUid;
                        image->series_uid_ = seriesUid;
                        image->study_uid_ = studyUid;
                        image->image_file_ = QString("./ScpCache/tmp.dcm");
                        study->image_list_.append(image);
                        dset->findAndGetString(DCM_ReferencedSOPInstanceUID, value, true);
                        image->ref_image_uid_ = QString::fromLatin1(value);
                        dset->findAndGetString(DCM_InstanceNumber, value);
                        image->image_number_ = QString::fromLatin1(value);
                        dset->findAndGetString(DCM_SeriesDescription, value);
                        image->image_desc_ = QString::fromLocal8Bit(value);
                        dset->findAndGetString(DCM_ContentDate, value);
                        image->image_yime_.setDate(
                          QDate::fromString(QString::fromLatin1(value), "yyyyMMdd"));
                        dset->findAndGetString(DCM_ContentTime, value);
                        image->image_yime_.setTime(FormatDicomTime(QString::fromLatin1(value)));
                    }
                }
            }
        }
        //
        StudyDao dao;
        int images = 0;
        QString study_dir_name =
          QString("%1/%2_%3").arg(study->study_time_.date().toString("yyyyMM"), study->study_time_.toString(DICOM_DATETIME_FORMAT), study->acc_number_);
        if (!dao.VerifyStudyByStuid(study->study_uid_)) {
            dao.InsertStudyToDb(*study, true);
        }
        FileUtil::DirMake(QString("%1/%2").arg(DICOM_SAVE_PATH, study_dir_name));
        foreach (ImageRecord * image, study->image_list_) {
            bool raw = image->sop_class_uid_ == QString(UID_XRayAngiographicImageStorage);
            QString src_file = image->image_file_;
            image->image_file_ =
              QString("%1/%2%3.dcm")
                .arg(study_dir_name, raw ? "XA_" : "", Kiss::GetRandString());
            QFileInfo info(QString("%1/%2").arg(DICOM_SAVE_PATH, image->image_file_));
            if (FileUtil::FileCopy(src_file, QString("%1/%2").arg(DICOM_SAVE_PATH, image->image_file_))) {
                if (!dao.VerifyImageByIMmuid(image->image_uid_)) {
                    if (dao.InsertImageToDb(*image, true)) {
                        images++;
                    } else {
                    }
                } else {
                    if (dao.UpdateImageFile(image->image_uid_, image->image_file_)) {
                        images++;
                    } else {
                        FileUtil::DeleteFileOrFolder(
                          QString("%1/%2").arg(DICOM_SAVE_PATH, image->image_file_));
                    }
                }
            }
            image->image_file_ = src_file;
        }
    }
}
