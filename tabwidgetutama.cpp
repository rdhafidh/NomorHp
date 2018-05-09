#include "tabwidgetutama.h"
#include <QVariant>
#include <halamanpage.h>

TabWidgetUtama::TabWidgetUtama(QWidget *parent) : QTabWidget(parent) {
  connect(this, SIGNAL(tabCloseRequested(int)), SLOT(emitclosetab(int)));
}

void TabWidgetUtama::manualremovetab(int idx) { Q_EMIT tabCloseRequested(idx); }

TabWidgetUtama::~TabWidgetUtama() {
  while (count()) {
    manualremovetab(0);
  }
}

void TabWidgetUtama::asktorefreshtabDataMaster()
{
    for (int i = 0; i < count(); i++) {
      QWidget *curWidget = widget(i);
      if (curWidget != NULL) {
            HalamanPage *tp = qobject_cast<HalamanPage *>(curWidget);
            if(tp){
                tp->askrefresh ();
            }
      }
    }
}


void TabWidgetUtama::emitclosetab(int idx) {
#ifdef DEBUGB
  qDebug() << "tab idx removed: " << idx << __FILE__ << __LINE__;
#endif
  auto curWidget = widget(idx);
  if (curWidget != Q_NULLPTR) {
    curWidget->deleteLater();
  }
  removeTab(idx);
}

void TabWidgetUtama::remembercloseandOpenTab(int forceopen_window_id)
{
    bool foundtabmaster=false;
    bool foundtabnonduplicate=false;
    for (int i = 0; i < count(); i++) {
      QWidget *curWidget = widget(i);
      if (curWidget != NULL) {
          QVariant data=curWidget->property ("window");
          if(data.isValid ()
                  ){
              if(data.toInt ()==1){
                  foundtabmaster=true;
              }
              if(data.toInt ()==2){
                  foundtabnonduplicate=true;
              }
          }
      }
    }
    clear ();
    if(foundtabmaster
            ||forceopen_window_id){
        this->emitOpentabDataMaster ();
    }
    if(foundtabnonduplicate
            ||forceopen_window_id==2){
        this->emitOpentabDataNonDuplicate ();
    }
}
