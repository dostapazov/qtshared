#ifndef STYLES_CPP
#define STYLES_CPP
#include "styles.h"


static QStringList legasyList =
{
  "Plastique"
 ,"Cleanlooks"
#ifdef Q_OS_WIN
 ,"dotNET"
#endif
};

QStringList legasyStyles()
{
    return legasyList;
}

QStyle* getLegasyStyle(QString styleName)
{
  #ifdef Q_OS_WIN
    if (styleName == "dotNET") return new QtDotNetStyle;
  #endif
    if (styleName == "Plastique") return new QPlastiqueStyle;
    if (styleName == "Cleanlooks") return new QCleanlooksStyle;

    return Q_NULLPTR;
}

#endif // STYLES_CPP
