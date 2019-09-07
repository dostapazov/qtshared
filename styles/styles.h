#ifndef STYLES_H
#define STYLES_H
#include <QtCore>
#include <QStyle>
#include "qplastiquestyle.h"
#include "qcleanlooksstyle.h"
#ifdef Q_OS_WIN
#include "qtdotnetstyle.h"
#endif

QStringList legasyStyles();
QStyle *getLegasyStyle(QString styleName);

#endif // STYLES_H
