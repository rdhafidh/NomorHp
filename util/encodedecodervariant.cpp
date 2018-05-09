#include "encodedecodervariant.h"
#include <QDate>
#include <QDateTime>
#include <QTime>
#include <QUuid>
#include <iostream>
#include <limits.h>

work::EncodeDecoderVariant::EncodeDecoderVariant() {}

work::EncodeDecoderVariant::~EncodeDecoderVariant() {}

QVariant
work::EncodeDecoderVariant::decode(const serviceapp::SingleCellBase &cell,
                                   const QLocale &locale) {
  QVariant val;
  bool okconvertnilai = false;
  QVariant tmpval = QVariant();
  switch (cell.actualtype()) {
  case serviceapp::TypeColumnBase::QVariantBool:
    if (cell.celldata() == "1" || cell.celldata() == "true") {
      val = true;
    } else {
      val = false;
    }
    break;
  case serviceapp::TypeColumnBase::QVariantByteArray:
    val = QByteArray::fromStdString(cell.celldata());
    break;
  case serviceapp::TypeColumnBase::QVariantChar:
    if (cell.celldata().size() < 1) {
      val = QChar();
    } else {
      val = QChar(cell.celldata().at(0));
    }
    break;
  case serviceapp::TypeColumnBase::QVariantDate:
#ifdef DEBUGB
    std::cout << "\nexecuted: " << __FILE__ << " line " << __LINE__ << "\n";
#endif
    if (!cell.datefmt().empty()) {
#ifdef DEBUGB
      std::cout << "\nexecuted: " << __FILE__ << " line " << __LINE__ << "\n";
#endif
      val = locale.toDate(QString::fromStdString(cell.celldata()),
                          QString::fromStdString(cell.datefmt()));
    } else {
#ifdef DEBUGB
      std::cout << "\nexecuted: " << __FILE__ << " line " << __LINE__ << "\n";
#endif
      val = locale.toDate(QString::fromStdString(cell.celldata()),
                          "dddd dd MMM yyyy");
    }
    break;
  case serviceapp::TypeColumnBase::QVariantDateTime:
    if (!cell.datetimefmt().empty()) {
      val = locale.toDateTime(QString::fromStdString(cell.celldata()),
                              QString::fromStdString(cell.datetimefmt()));
    } else {
      val = locale.toDateTime(QString::fromStdString(cell.celldata()),
                              "dddd dd MMM yyyy hh:mm:ss");
    }
    break;
  case serviceapp::TypeColumnBase::QVariantDouble:
    tmpval = QString::fromStdString(cell.celldata());
    tmpval = tmpval.toDouble(&okconvertnilai);
    if (!okconvertnilai) {
      val = 0.0;
    } else {
      val = tmpval;
    }
    break;
  case serviceapp::TypeColumnBase::QVariantInt:
    tmpval = QString::fromStdString(cell.celldata());
    tmpval = tmpval.toInt(&okconvertnilai);
    if (!okconvertnilai) {
      val = 0;
    } else {
      val = tmpval;
    }
    break;
  case serviceapp::TypeColumnBase::QVariantInvalid:
    val = tmpval;
    break;
  case serviceapp::TypeColumnBase::QVariantLongLong:
    tmpval = QString::fromStdString(cell.celldata());
    tmpval = tmpval.toLongLong(&okconvertnilai);
    if (!okconvertnilai) {
      val = 0;
    } else {
      val = tmpval;
    }
    break;
  case serviceapp::TypeColumnBase::QVariantString:
    val = QString::fromStdString(cell.celldata());
    break;
  case serviceapp::TypeColumnBase::QVariantTime:
    if (!cell.timefmt().empty()) {
      val = locale.toTime(QString::fromStdString(cell.celldata()),
                          QString::fromStdString(cell.timefmt()));
    } else {
      val = locale.toTime(QString::fromStdString(cell.celldata()),
                          "hh:mm:ss.zzz");
    }
    break;
  case serviceapp::TypeColumnBase::QVariantUInt:
    tmpval = QString::fromStdString(cell.celldata());
    tmpval = tmpval.toUInt(&okconvertnilai);
    if (!okconvertnilai) {
      val = 0;
    } else {
      val = tmpval;
    }
    break;
  case serviceapp::TypeColumnBase::QVariantULongLong:
    tmpval = QString::fromStdString(cell.celldata());
    tmpval = tmpval.toULongLong(&okconvertnilai);
    if (!okconvertnilai) {
      val = 0;
    } else {
      val = tmpval;
    }
    break;
  case serviceapp::TypeColumnBase::QVariantUuid:
    tmpval = QString::fromStdString(cell.celldata());
    val = tmpval.toUuid();
    break;
  default:
    break;
  }
  return val;
}

serviceapp::SingleCellBase
work::EncodeDecoderVariant::encode(const QVariant &val, const QLocale &locale) {
  serviceapp::SingleCellBase cell;
  QVariant localval;
  switch (val.type()) {
  case QVariant::Invalid:
    cell.set_actualtype(serviceapp::TypeColumnBase::QVariantInvalid);
    cell.set_celldata("");
    break;
  case QVariant::Bool:
    cell.set_actualtype(serviceapp::TypeColumnBase::QVariantBool);
    localval = val.toBool();
    if (localval.toBool()) {
      cell.set_celldata("true");
    } else {
      cell.set_celldata("false");
    }
    break;
  case QVariant::ByteArray:
    cell.set_actualtype(serviceapp::TypeColumnBase::QVariantByteArray);
    cell.set_celldata(val.toString().toStdString());
    break;
  case QVariant::Char:
    cell.set_actualtype(serviceapp::TypeColumnBase::QVariantChar);
    cell.set_celldata(val.toString().toStdString());
    break;
  case QVariant::Date:
    cell.set_actualtype(serviceapp::TypeColumnBase::QVariantDate);
    cell.set_datefmt("dddd dd MMM yyyy");
    cell.set_celldata(
        locale.toString(val.toDate(), "dddd dd MMM yyyy").toStdString());
    break;
  case QVariant::DateTime:
    cell.set_actualtype(serviceapp::TypeColumnBase::QVariantDateTime);
    cell.set_datetimefmt("dddd dd MMM yyyy hh:mm:ss");
    cell.set_celldata(
        locale.toString(val.toDateTime(), "dddd dd MMM yyyy hh:mm:ss")
            .toStdString());
    break;
  case QVariant::Double:
    cell.set_actualtype(serviceapp::TypeColumnBase::QVariantDouble);
    cell.set_celldata(val.toString().toStdString());
    break;
  case QVariant::Uuid:
    cell.set_actualtype(serviceapp::TypeColumnBase::QVariantUuid);
    cell.set_celldata(val.toString().toStdString());
    break;
  case QVariant::Int:
    cell.set_actualtype(serviceapp::TypeColumnBase::QVariantInt);
    cell.set_celldata(val.toString().toStdString());
    break;
  case QVariant::LongLong:
    cell.set_actualtype(serviceapp::TypeColumnBase::QVariantLongLong);
    cell.set_celldata(val.toString().toStdString());
    break;
  case QVariant::String:
    cell.set_actualtype(serviceapp::TypeColumnBase::QVariantString);
    cell.set_celldata(val.toString().toStdString());
    break;
  case QVariant::Time:
    cell.set_actualtype(serviceapp::TypeColumnBase::QVariantTime);
    // respect time format
    cell.set_timefmt("hh:mm:ss.zzz");
    cell.set_celldata(
        locale.toString(val.toTime(), "hh:mm:ss.zzz").toStdString());
    break;
  case QVariant::UInt:
    cell.set_actualtype(serviceapp::TypeColumnBase::QVariantUInt);
    cell.set_celldata(val.toString().toStdString());
    break;
  case QVariant::ULongLong:
    cell.set_actualtype(serviceapp::TypeColumnBase::QVariantULongLong);
    cell.set_celldata(val.toString().toStdString());
    break;
  default:
    break;
  }
  return cell;
}

QSqlField work::EncodeDecoderVariant::fromfieldTypetoField(
    const serviceapp::TypeColumnBase &ty) {
  QSqlField field;
  switch (ty) {
  case serviceapp::TypeColumnBase::QVariantBool:
    field.setName("x");
    field.setType(QVariant::Bool);
    field.setValue(true);
    break;
  case serviceapp::TypeColumnBase::QVariantByteArray:
    field.setName("x");
    field.setType(QVariant::ByteArray);
    field.setValue(QVariant(QByteArray("")));
    break;
  case serviceapp::TypeColumnBase::QVariantChar:
    field.setName("x");
    field.setType(QVariant::ByteArray);
    field.setValue(QVariant(QChar()));
    break;
  case serviceapp::TypeColumnBase::QVariantDate:
    field.setName("x");
    field.setType(QVariant::Date);
    field.setValue(QVariant(QDate()));
    break;
  case serviceapp::TypeColumnBase::QVariantDateTime:
    field.setName("x");
    field.setType(QVariant::DateTime);
    field.setValue(QVariant(QDateTime()));
    break;
  case serviceapp::TypeColumnBase::QVariantDouble:
    field.setName("x");
    field.setType(QVariant::Double);
    field.setValue(QVariant(0.0));
    break;
  case serviceapp::TypeColumnBase::QVariantInt:
    field.setName("x");
    field.setType(QVariant::Int);
    field.setValue(QVariant(0));
    break;
  case serviceapp::TypeColumnBase::QVariantInvalid:
    field.setName("x");
    field.setType(QVariant::Invalid);
    field.setValue(QVariant());
    break;
  case serviceapp::TypeColumnBase::QVariantLongLong:
    field.setName("x");
    field.setType(QVariant::LongLong);
    field.setValue(QVariant(std::numeric_limits<qint64>::max() - 10));
    break;
  case serviceapp::TypeColumnBase::QVariantString:
    field.setName("x");
    field.setType(QVariant::String);
    field.setValue(QVariant(QString("dad")));
    break;
  case serviceapp::TypeColumnBase::QVariantTime:
    field.setName("x");
    field.setType(QVariant::Time);
    field.setValue(QVariant(QTime()));
    break;
  case serviceapp::TypeColumnBase::QVariantUInt:
    field.setName("x");
    field.setType(QVariant::Invalid);
    field.setValue(QVariant(0));
    break;
  case serviceapp::TypeColumnBase::QVariantULongLong:
    field.setName("x");
    field.setType(QVariant::ULongLong);
    field.setValue(QVariant(std::numeric_limits<qint64>::max() - 10000));
    break;
  case serviceapp::TypeColumnBase::QVariantUuid:
    field.setName("x");
    field.setType(QVariant::Uuid);
    field.setValue(QVariant(QUuid()));
    break;
  default:
    field.setName("x");
    field.setType(QVariant::Invalid);
    field.setValue(QVariant());
    break;
  }
  return field;
}
