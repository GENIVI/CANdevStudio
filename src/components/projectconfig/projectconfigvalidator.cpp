#include <projectconfigvalidator.h>
#include <rapidjson/stringbuffer.h>
#include <QFile>
#include <QMessageBox>

#include "log.h"

std::shared_ptr<rapidjson::SchemaDocument> ProjectConfigValidator::configSchema;
bool ProjectConfigValidator::schemaInitialized = false;

bool ProjectConfigValidator::validateConfiguration(const QByteArray& wholeFile)
{
    using namespace rapidjson;

    if (!schemaInitialized && !ProjectConfigValidator::loadConfigSchema())
    {
        QMessageBox msgBox;
        msgBox.setText("There was an error loading project configuration schema.");
        msgBox.setInformativeText("Do you want to try to load project without validating it?");
        msgBox.setStandardButtons(QMessageBox::Open | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);

        return (msgBox.exec() == QMessageBox::Open) ? true : false;
    }

    Document d;
    if (d.Parse(wholeFile.toStdString()).HasParseError())
    {
        cds_error("Could not parse configuration file");
        QMessageBox::warning(nullptr, "Could not parse configuration file",
                "There was an error parsing project configuration file: it is not a valid json file.");

        return false;
    }

    SchemaValidator validator(*ProjectConfigValidator::configSchema);
    if (!d.Accept(validator))
    {
        StringBuffer sb;
        validator.GetInvalidSchemaPointer().StringifyUriFragment(sb);
        cds_error("Invalid schema: {}", sb.GetString());
        cds_error("Invalid keyword: {}", validator.GetInvalidSchemaKeyword());

        sb.Clear();
        validator.GetInvalidDocumentPointer().StringifyUriFragment(sb);
        cds_error("Invalid document: {}", sb.GetString());

        QMessageBox msgBox;
        msgBox.setText("There was an error validating project configuration.");
        msgBox.setInformativeText("Do you want to try to load it anyway?");
        msgBox.setStandardButtons(QMessageBox::Open | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);

        return (msgBox.exec() == QMessageBox::Open) ? true : false;
    }

    return true;
}

bool ProjectConfigValidator::loadConfigSchema()
{
    using namespace rapidjson;

    QFile file("/home/krbo/schema.json");
    if (!file.open(QIODevice::ReadOnly))
    {
        cds_error("Could not open configuration schema file");
        return false;
    }

    QByteArray data = file.readAll();

    Document sd;
    if (sd.Parse(data.toStdString()).HasParseError())
    {
        cds_error("Could not parse configuration schema file");
        return false;
    }

    configSchema = std::make_shared<SchemaDocument>(sd);


    return ProjectConfigValidator::schemaInitialized = true;
}
