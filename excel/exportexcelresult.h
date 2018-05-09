#ifndef EXPORTEXCELRESULT_H
#define EXPORTEXCELRESULT_H
enum class ExportExcelResult {
  exportsuccess = 0x10,
  exportfailnotwritable,
  exportfailunknown,
    exportfailnullpointertableviewdatasetup,
    exportfailexportexcelnameempty,
    exportsuccessemptycol
};
#endif  // EXPORTEXCELRESULT_H
