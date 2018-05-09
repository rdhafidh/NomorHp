#include "numberparsertype.h"
#include <QtCore>

nomorutil::NumberParserType::NumberParserType() {}

nomorutil::NumberParserType::~NumberParserType() {}

nomorutil::ReturnDeterminedNumber nomorutil::NumberParserType::determineNumber(
    const std::string &number) {
  nomorutil::ReturnDeterminedNumber rdn;
  bool isokbignum = true;
  for (decltype(number.size()) nn = 0; nn < number.size(); nn++) {
    bool isnum =
        (number.at(nn) == '0' || number.at(nn) == '1' || number.at(nn) == '2' ||
         number.at(nn) == '3' || number.at(nn) == '4' || number.at(nn) == '5' ||
         number.at(nn) == '6' || number.at(nn) == '7' || number.at(nn) == '8' ||
         number.at(nn) == '9');
    if (!isnum) {
      isokbignum = false;
      break;
    }
  }
  if (!isokbignum) {
    //qDebug() << Q_FUNC_INFO << __FILE__ << __LINE__ << "called";
    rdn.isnumbervalid = false;
    rdn.nomor_string_diprocess = number;
    return rdn;
  }
  std::string localnum;
  QVector<QString> telkomsel = {"0811", "0812", "0813", "0821", "0822",
                                "0823", "0852", "0853", "0851"};
  QVector<QString> indosat = {"0855", "0856", "0857", "0858",
                              "0814", "0815", "0816"};
  QVector<QString> xl = {"0817", "0818", "0819", "0859", "0877", "0878"};
  QVector<QString> three3 = {"0896", "0897", "0898", "0899"};
  QVector<QString> axis = {"0831", "0832", "0833", "0838"};
  if (number.substr(0, 2) == "62") {
    localnum = number.substr(2, number.size());
    localnum = "0" + localnum;
  }else{
      localnum=number;
  }

  rdn.nomor_string_diprocess = localnum;
  if (localnum.size() == 10) {
    rdn.km = KategoriNomor::NOMOR_10_DIGIT;
    QString prefix_provider = QString::fromStdString(localnum.substr(0, 4));
    bool known_provider = false;
    for (int x = 0; x < telkomsel.size(); x++) {
      if (telkomsel.at(x) == prefix_provider) {
        known_provider = true;
        rdn.isnumbervalid = true;
        rdn.nomor_string_diprocess = localnum;
        rdn.pn = ProviderNomor::PROVIDER_TELKOMSEL;
      }
    }
    if (!known_provider) {
      for (int l = 0; l < indosat.size(); l++) {
        if (indosat.at(l) == prefix_provider) {
          known_provider = true;
          rdn.isnumbervalid = true;
          rdn.nomor_string_diprocess = localnum;
          rdn.pn = ProviderNomor::PROVIDER_INDOSAT;
        }
      }
    }
    if (!known_provider) {
      for (int m = 0; m < xl.size(); m++) {
        if (xl.at(m) == prefix_provider) {
          known_provider = true;
          rdn.isnumbervalid = true;
          rdn.nomor_string_diprocess = localnum;
          rdn.pn = ProviderNomor::PROVIDER_XL;
        }
      }
    }
    if (!known_provider) {
      for (int n = 0; n < three3.size(); n++) {
        if (three3.at(n) == prefix_provider) {
          known_provider = true;
          rdn.isnumbervalid = true;
          rdn.nomor_string_diprocess = localnum;
          rdn.pn = ProviderNomor::PROVIDER_3;
        }
      }
    }
    if (!known_provider) {
      for (int o = 0; o < axis.size(); o++) {
        if (axis.at(o) == prefix_provider) {
          known_provider = true;
          rdn.isnumbervalid = true;
          rdn.nomor_string_diprocess = localnum;
          rdn.pn = ProviderNomor::PROVIDER_AXIS;
        }
      }
    }
    if (!known_provider) {
      rdn.isnumbervalid = false;
      rdn.nomor_string_diprocess = localnum;
      rdn.pn = ProviderNomor::PROVIDER_UNKNOWN;
      return rdn;
    }
    return rdn;
  }

  bool oknum = false;
  if (localnum.size() == 11) {
    oknum = true;
  }
  if (localnum.size() == 12) {
    oknum = true;
  }

  if (!oknum) {
    rdn.isnumbervalid = false;
    rdn.nomor_string_diprocess = number;
    //qDebug() << Q_FUNC_INFO << __FILE__ << __LINE__ << "called";
    return rdn;
  }

  // bsa jadi nomor cantik
  // atau nomor acak
  // cek dulunomor cantik atau bukan
  if (nomorutil::NumberParserType::isNomorCantikKah(localnum)) {
    rdn.km = KategoriNomor::NOMOR_CANTIK;
  } else {
    rdn.km = KategoriNomor::NOMOR_ACAK;
  }

  // langsung cek apakah dia termasuk punyavaliid provder?
  QString prefix_provider = QString::fromStdString(localnum.substr(0, 4));
  bool known_provider = false;
  for (int x = 0; x < telkomsel.size(); x++) {
    if (telkomsel.at(x) == prefix_provider) {
      known_provider = true;
      rdn.isnumbervalid = true;
      rdn.nomor_string_diprocess = localnum;
      rdn.pn = ProviderNomor::PROVIDER_TELKOMSEL;
    }
  }
  if (!known_provider) {
    for (int l = 0; l < indosat.size(); l++) {
      if (indosat.at(l) == prefix_provider) {
        known_provider = true;
        rdn.isnumbervalid = true;
        rdn.nomor_string_diprocess = localnum;
        rdn.pn = ProviderNomor::PROVIDER_INDOSAT;
      }
    }
  }
  if (!known_provider) {
    for (int m = 0; m < xl.size(); m++) {
      if (xl.at(m) == prefix_provider) {
        known_provider = true;
        rdn.isnumbervalid = true;
        rdn.nomor_string_diprocess = localnum;
        rdn.pn = ProviderNomor::PROVIDER_XL;
      }
    }
  }
  if (!known_provider) {
    for (int o = 0; o < axis.size(); o++) {
      if (axis.at(o) == prefix_provider) {
        known_provider = true;
        rdn.isnumbervalid = true;
        rdn.nomor_string_diprocess = localnum;
        rdn.pn = ProviderNomor::PROVIDER_AXIS;
      }
    }
  }
  if (!known_provider) {
    for (int n = 0; n < three3.size(); n++) {
      if (three3.at(n) == prefix_provider) {
        known_provider = true;
        rdn.isnumbervalid = true;
        rdn.nomor_string_diprocess = localnum;
        rdn.pn = ProviderNomor::PROVIDER_3;
      }
    }
  }

  if (!known_provider) {
    rdn.isnumbervalid = false;
    rdn.nomor_string_diprocess = localnum;
    rdn.pn = ProviderNomor::PROVIDER_UNKNOWN;
    return rdn;
  }
  return rdn;
}

bool nomorutil::NumberParserType::isNomorCantikKah(
    const std::string &nomorBerawalan08x) {
  std::string data = nomorBerawalan08x.substr(3, nomorBerawalan08x.size());
  int dwi = 2;
  int triple = 3;
  int quartet = 4;
  int panca = 5;
  int sat = 6;

  QRegExp rx("(\\d)\\1");
  int pos = 0;
  int count = 0;
  QSet<int> founditem;
  QVector<int> dup_data_times;

  while (pos >= 0) {
    pos = rx.indexIn(data.c_str(), pos);
    if (pos >= 0) {
      // hitung jumlah ketemunya setiap akhir angka yang sama
      // via qvector.count(founditem.at(x));
      dup_data_times << rx.cap(1).toInt();
      // std::cout<<"\nfound: "<<rx.cap (1).toStdString ();
      ++pos;
      ++count;
      founditem << rx.cap(1).toInt();
    }
  }
 // std::cout << "\nfound digit bersamaan: " << founditem.size();
  bool nomorcantik = false;
  int counter_dwi = 0;
  int counter_triple = 0;
  int counter_quartet = 0;
  int counter_panca = 0;
  int counter_sat = 0;

  QSet<int>::const_iterator i = founditem.constBegin();

  while (i != founditem.constEnd ()) {
 //   std::cout << "\nvalue: " << *i
   //           << " ditemukan: " << dup_data_times.count(*i) + 1 << "x";
    int totalcount = dup_data_times.count(*i) + 1;
    if (totalcount == dwi) {
      ++counter_dwi;
    }
    if (totalcount == triple) {
      ++counter_triple;
    }
    if (totalcount == quartet) {
      ++counter_quartet;
    }
    if (totalcount == panca) {
      ++counter_panca;
    }
    if (totalcount == sat) {
      ++counter_sat;
    }
    ++i;
  }
  if (counter_sat >= 1 || counter_panca >= 1 || counter_quartet >= 1) {
    nomorcantik = true;
  }
  if (counter_dwi >= 1) {
    // cek lagi karena nomor dwi berpasang
    if (counter_dwi == 3) {
      // nomorcantik sudah
      nomorcantik = true;
    }
    if (!nomorcantik) {
      // min ada triple 1 atau
      if (counter_triple >= 1) {
        nomorcantik = true;
      }
      if (!nomorcantik && counter_quartet == 1) {
        nomorcantik = true;
      }
      if (!nomorcantik && counter_panca == 1) {
        nomorcantik = true;
      }
      if (!nomorcantik && counter_sat == 1) {
        nomorcantik = true;
      }
    }
  }
  return nomorcantik;
}
