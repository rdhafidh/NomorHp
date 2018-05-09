#ifndef SQLFETCH_H
#define SQLFETCH_H
#include <QtCore>
#include <QtSql>
#include <deque>
#include <utility>
#include <tuple>
#include <iostream>
#include <memory>
namespace dbbase{

typedef QMap<QString,QVariant> configdb;
/*
 *   [
 *      {
 *          "whatevercol"
 *      }
 *     ,
 *     {
 *      "whatevernewcol"
 *     }
 *   ] 
 * 
 * */
typedef std::pair<bool,std::deque<QSqlRecord> > ret;
typedef std::tuple<QString,QSqlError::ErrorType,QString> sqlerror;

typedef std::deque<std::pair<QString,QVariant>> placeholderprepareinsert;
class SqlFetch
{
public:
    SqlFetch();
    ~SqlFetch();
    /*
     * struktur config
     * 1. dbbackend
     * 2. hostname
     * 3. dbname
     * 4. username
     * 5. password
     * 6. connectopt
     * 7. port
     * 8. conname
     *
     **/
    void setConfig(const configdb & c);
    /*
     *cek open
     * */
    bool open() const;
    void close() { 
		if (db.get() != Q_NULLPTR) {
			return db->close();
		}
	}
    sqlerror lastError() const;
    static QString errortostring(const sqlerror &e) {
        return QObject::tr("%1\nKode error: %2\nNative Database error: \n%3").arg(std::get<0>(e)).arg(std::get<1>(e)).arg(std::get<2>(e));
    }
    bool begintransaction(){ 
		if (db.get() != Q_NULLPTR) {
			return db->transaction();
		}
		return false;
	}

    bool endtransaction(){
		if (db.get() != Q_NULLPTR) {
			return db->commit();
		}
		return false;
	}
    bool tryrollback(){
		if (db.get() != Q_NULLPTR) {
			return db->rollback();
		}
		return false;
	}

    /*
    typedef std::deque<std::pair<QString,QVariant>> placeholderprepareinsert;
    */
    ret prepareSelectFilter(const QString & s, const placeholderprepareinsert &p );
    ret rawSelect(const QString & s);
    bool checkrowidexist(const QString &sql, const placeholderprepareinsert &p);
    bool preparedStm(const QString & sql, const placeholderprepareinsert &p);
    bool preparedbatchstm2(const QString &sql, const std::pair<QVariantList, QVariantList> &perrows);
    bool crdelalter(const QString &sql);
    std::pair<bool,QVariant> insertWithParam(const QString &sql, const placeholderprepareinsert &p);
    bool crdelalterWithParam(const QString &sql, const placeholderprepareinsert &p);
    qint64 selectCountJoin(const QString &sql);
    qint64 selectCount(const QString & tblname);
    qint64 prepareSelectFilterCount(const QString & s, const placeholderprepareinsert &p );
    QSqlRecord record(const QString &tblname);
    configdb usedconfig()const {return cf;}
	bool checkreopenifnot();
private:
	bool testvalidopen();
    configdb cf;
    std::unique_ptr<QSqlDatabase> db;
    sqlerror error;

};


}
#endif // SQLFETCH_H
