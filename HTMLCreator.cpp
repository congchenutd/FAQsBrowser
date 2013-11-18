#include "HTMLCreator.h"
#include "Template.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QTextStream>
#include <QDebug>

QString HTMLCreator::createFAQ(const QJsonObject& apiJson) const
{
    Template titleTemp("./Templates/FAQ.html");
    titleTemp.setValue("Questions", createQuestions(apiJson));
    return titleTemp.toString();
}

// e.g.
//{
//    "api": "javax.swing.AbstractAction.getValue(java.lang.String)",
//    "questions": [
//        {
//            "answers": [
//                {
//                    "link": "http://www.scribd.com/",
//                    "title": "Wrox.professional Java JDK Edition"
//                }
//            ],
//            "question": "question1",
//            "users": [
//                {
//                    "email": "carl@gmail.com",
//                    "name": "Carl"
//                }
//            ]
//        }
//    ]
//}
//
//<ul>
//	<li>question1 ( <a href="profile:Carl">Carl</a> )
//		<ul>
//			<li><a href="http://www.scribd.com/">Wrox.professional Java JDK Edition"</a></li>
//		</ul>
//	</li>
//</ul>
QString HTMLCreator::createQuestions(const QJsonObject& apiJson) const
{
    QJsonArray questions = apiJson.value("questions").toArray();
    Template questionsTemp("./Templates/Questions.html");

    // for each question
    for(QJsonArray::Iterator itq = questions.begin(); itq != questions.end(); ++itq)
    {
        // the question itself
        QJsonObject question = (*itq).toObject();
        Template questionTemp("./Templates/Question.html");
        questionTemp.setValue("Title", question.value("question").toString());

        // users
        QJsonArray users = question.value("users").toArray();
        for(QJsonArray::Iterator itu = users.begin(); itu != users.end(); ++itu)
        {
            QJsonObject user = (*itu).toObject();
            Template userTemp("./Templates/User.html");
            userTemp.setValue("Name", user.value("name").toString());  // format user
            questionTemp.addValue("User", userTemp.toString());        // add to the question
        }

        // answers
        QJsonArray answers = question.value("answers").toArray();
        if(answers.isEmpty())
        {
            Template answerTemp("./Templates/Answer.html");
            answerTemp.setValue("Title", "Not answered!");
            questionTemp.addValue("Answer",  answerTemp.toString());
        }
        else {
            for(QJsonArray::Iterator ita = answers.begin(); ita != answers.end(); ++ita)
            {
                QJsonObject answer = (*ita).toObject();
                QString link  = answer.value("link") .toString();
                QString title = answer.value("title").toString();
                if(title.isEmpty())
                    title = "Link";

                Template answerTemp("./Templates/Answer.html");
                answerTemp.setValue("Title", title);                      // format answer
                answerTemp.setValue("Link",  link);
                questionTemp.addValue("Answer",  answerTemp.toString());  // add to the question
            }
        }
        questionsTemp.addValue("Question", questionTemp.toString());      // add the question
    }
    return questionsTemp.toString();
}

QString HTMLCreator::createProfilePage(const QJsonObject& profileJson) const
{
    Template profilePageTemp("./Templates/ProfilePage.html");
    QString name  = profileJson.value("name").toString();
    profilePageTemp.setValue("Name",           name);
    profilePageTemp.setValue("Profile",        createProfile     (profileJson));
    profilePageTemp.setValue("InterestedAPIs", createAPIs        (profileJson));
    profilePageTemp.setValue("RelatedUsers",   createRelatedUsers(profileJson));
    return profilePageTemp.toString();
}

QString HTMLCreator::createProfile(const QJsonObject& profileJson) const
{
    QString name  = profileJson.value("name").toString();
    QString email = profileJson.value("email").toString();
    Template profileTemp("./Templates/Profile.html");
    profileTemp.setValue("Name",  name);
    profileTemp.setValue("Email", email);
    return profileTemp.toString();
}

QString HTMLCreator::createAPIs(const QJsonObject& profileJson) const
{
    Template apisTemp("./Templates/APIs.html");
    QJsonArray APIs = profileJson.value("apis").toArray();
    for(QJsonArray::Iterator it = APIs.begin(); it != APIs.end(); ++it)
    {
        QJsonObject API = (*it).toObject();
        QString apiSig = API.value("apisig").toString();

        Template apiTemp("./Templates/API.html");
        apiTemp.setValue("Signature", apiSig);
        apiTemp.setValue("Questions", createQuestions(API));

        apisTemp.addValue("API", apiTemp.toString());
    }
    return apisTemp.toString();
}

QString HTMLCreator::createRelatedUsers(const QJsonObject &profileJson) const
{
    Template usersTemp("./Templates/RelatedUsers.html");
    QJsonArray users = profileJson.value("relatedusers").toArray();
    for(QJsonArray::Iterator it = users.begin(); it != users.end(); ++it)
    {
        QJsonObject user = (*it).toObject();
        QString name  = user.value("name").toString();
        QString email = user.value("email").toString();

        Template userTemp("./Templates/RelatedUser.html");
        userTemp.setValue("Name", name);

        usersTemp.addValue("RelatedUser", userTemp.toString());
    }
    return usersTemp.toString();
}
