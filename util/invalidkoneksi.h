#ifndef INVALIDKONEKSI_H
#define INVALIDKONEKSI_H

#include "sqlfetch.h"

static bool invalidconfig(const dbbase::configdb & c)
{ 
//    return (c.isEmpty() || (c.contains("dbbackend")==false) || (c.contains("hostname")==false)
//            || (c.contains("dbname")==false) || (c.contains("username")==false) ||
//            (c.contains("password")==false) || (c.contains("port")==false) ||
//            (c.contains("conname")==false) 
//            );
    return false;
}
static dbbase::configdb fixconname(const dbbase::configdb &c)
{
    dbbase::configdb ctemp=c;
    QUuid uuid=QUuid::createUuid();
    QDateTime tm=QDateTime::currentDateTime();  
    ctemp["conname"]=QString("koneksi-%1-id-%2").arg(tm.toString("dddd_dd_MM_yyyy::HH_mm_ss")).arg(uuid.toString());
    return ctemp;
}

#endif // INVALIDKONEKSI_H

