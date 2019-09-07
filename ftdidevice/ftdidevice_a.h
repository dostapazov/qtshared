#ifndef FTDIDEVICE_A_H
#define FTDIDEVICE_A_H

#include <QtCore>
#include <QAndroidJniEnvironment>
#include <QtAndroidExtras>

QStringList scan_a()
{
    QAndroidJniObject dvl =  QAndroidJniObject::callStaticObjectMethod("org/qtproject/deviceinterface/usbftdi/ftdiActivity","busScan","()Ljava/lang/String;");
    QString devList = dvl.toString();

    return devList.split(",",QString::SkipEmptyParts);

}

bool open_a(QString serialNum, quint64 baudRate, quint8 bits, quint8 stopBits, quint8 parity, int* handle)
{
    QAndroidJniObject serial = QAndroidJniObject::fromString(serialNum);
    jint r = QAndroidJniObject::callStaticMethod<jint>("org/qtproject/deviceinterface/usbftdi/ftdiActivity","open",
                                                            "(Ljava/lang/String;IBBB)I",
                                                            serial.object<jstring>(),
                                                            (jint)baudRate,
                                                            (jbyte)bits,
                                                            (jbyte)stopBits,
                                                            (jbyte)parity
                                                      );
    qDebug() << r;
    bool ok = (r > 0);
    if (ok) *handle = r;
    else *handle = 0;
    return ok;
}

void close_a(int handle)
{
    QAndroidJniObject::callStaticMethod<void>("org/qtproject/deviceinterface/usbftdi/ftdiActivity","close","(I)",(jint)handle);
}

qint64 bytesAvailable_a(int handle)
{
    //qDebug(Q_FUNC_INFO);
    return QAndroidJniObject::callStaticMethod<jint>("org/qtproject/deviceinterface/usbftdi/ftdiActivity","bytesAvailable","(I)I",(jint)handle);
}

qint64 readData_a(char* data, qint64 maxlen, int handle, QString *error)
{
    QAndroidJniEnvironment env;
    QAndroidJniObject readed = QAndroidJniObject::callStaticObjectMethod("org/qtproject/deviceinterface/usbftdi/ftdiActivity",
                                                                             "read",
                                                                             "(II)[B",
                                                                             (jint)handle,
                                                                             (jint)maxlen);

    jbyteArray a = readed.object<jbyteArray>();
    if (a)
    {
        jsize len = env->GetArrayLength(a);
        jbyte* jd = env->GetByteArrayElements(a,0);
        memcpy((void*)data,(void*)jd,qMin((qint64)len,maxlen));
        env->ReleaseByteArrayElements(a, jd, JNI_COMMIT);
        return len;
    }
    if (error) *error = "Read error";
    return -1;
}

qint64 writeData_a(const char* data, qint64 maxlen, int handle, QString *error)
{
    QByteArray d = QByteArray::fromRawData(data, maxlen);
    jint bsz = QAndroidJniObject::callStaticMethod<jint>("org/qtproject/deviceinterface/usbftdi/ftdiActivity",
                                                        "prepareSendBuffer",
                                                        "(II)I",
                                                         (jint)handle,
                                                        (jint)d.count());

    for (int i = 0; i < bsz; i++)
    {
        QAndroidJniObject::callStaticMethod<jint>("org/qtproject/deviceinterface/usbftdi/ftdiActivity",
                                                  "setByte",
                                                  "(IBI)I",
                                                  (jint)handle,
                                                  (jbyte)d[i],
                                                  (jint)i);
    }

    jint r = QAndroidJniObject::callStaticMethod<jint>("org/qtproject/deviceinterface/usbftdi/ftdiActivity",
                                                       "writeSendBuffer",
                                                       "(I)I",
                                                       (jint)handle);
    if (r < maxlen)
    {
        if (error)
        {
            if (r < 0) *error = "Device not found (null)";
            else *error = "Data lost";
        }
    }
    return r;
}

#endif // FTDIDEVICE_A_H
