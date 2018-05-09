#include "utilrandkode.h"
#include <random>
#include <QDateTime>
UtilRandKode::UtilRandKode()
{
}

QString UtilRandKode::genrandkode(const QString &prefix, int suffixlong)
{
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0,9);
  QString nums;
  if(suffixlong==0)
    {
      suffixlong=5;
    }
  for (int n=0; n<suffixlong; ++n){
      int r=dis(gen);
        nums.append(QString::number(r));
      }

  QString fmt("%1-%2-%3");
  QDateTime dt = QDateTime::currentDateTime();
  return fmt.arg(prefix).arg(dt.toString("ddMMyyyyhhmmzzz")).arg(nums);
}
QString UtilRandKode::genrandkodeDatar( int suffixlong)
{
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0,9);
  QString nums;
  if(suffixlong==0)
    {
      suffixlong=7;
    }
  for (int n=0; n<suffixlong; ++n){
      int r=dis(gen);
        nums.append(QString::number(r));
      }

  QString fmt("%1%2");
  QDateTime dt = QDateTime::currentDateTime();
  return fmt.arg(dt.toString("ddMMyyyyhh")).arg(nums);
}
