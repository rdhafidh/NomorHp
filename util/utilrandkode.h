#ifndef UTILRANDKODE_H
#define UTILRANDKODE_H
#include <QString>

class UtilRandKode
{
public:
  UtilRandKode();
  ~UtilRandKode(){}
  static QString genrandkode(const QString &prefix,int suffixlong=5);
  static QString genrandkodeDatar(int suffixlong=7);
};

#endif // UTILRANDKODE_H
