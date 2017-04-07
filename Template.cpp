//#include "Template.h"
//#include <QFile>
//#include <QRegExp>

//Template::Template(const QString& fileName)
//{
//    QFile file(fileName);
//    if(file.open(QFile::ReadOnly))
//        html = file.readAll();
//}

//void Template::addValue(const QString& attribute, const QString& value)
//{
//    int placeHolder = html.indexOf("$" + attribute + "$");
//    if(placeHolder > -1)
//        html.insert(placeHolder, value);
//}

//void Template::setValue(const QString& attribute, const QString& value) {
//    html.replace("$" + attribute + "$", value);
//}

//QString Template::toString() const {
//    return QString(html).remove(QRegExp("\\$\\w+\\$"));  // remove placeholders $XXX$
//}
