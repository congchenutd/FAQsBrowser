#ifndef HTMLCREATOR_H
#define HTMLCREATOR_H

class QString;
class QJsonObject;

class HTMLCreator
{
public:
    QString createFAQ        (const QJsonObject& apiJson) const;
    QString createProfilePage(const QJsonObject& profileJson) const;

private:
    QString createQuestions(const QJsonObject& apiJson) const;
    QString createProfile  (const QJsonObject& profileJson) const;
    QString createAPIs     (const QJsonObject& profileJson) const;
};

#endif // HTMLCREATOR_H
