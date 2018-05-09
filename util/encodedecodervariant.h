#ifndef ENCODEDECODERVARIANT_H
#define ENCODEDECODERVARIANT_H

#include <QVariant>
#include <rpc.pb.h>
#include <QLocale>
#include <QSqlField>

namespace work{
class EncodeDecoderVariant
{
public:
    EncodeDecoderVariant();
    ~EncodeDecoderVariant();
    static QVariant decode(const serviceapp::SingleCellBase &cell,const QLocale & locale);
    static  serviceapp::SingleCellBase encode(const QVariant &val,const QLocale & locale);
    //TODO ADD CONVERTER FROM ENUM TYPE TO QSQLFIELD TYPE
    static QSqlField fromfieldTypetoField(const serviceapp::TypeColumnBase & ty);
};
}

#endif // ENCODEDECODERVARIANT_H