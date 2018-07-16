#include <projectconfigvalidator.h>
#include <rapidjson/stringbuffer.h>
#include <QDirIterator>
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
        cds_error("Failed to load project configuration schema.");

        return false;
    }

    Document d;
    if (d.Parse(wholeFile.toStdString()).HasParseError())
    {
        cds_error("Could not parse configuration file");

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

        return false;
    }

    return true;
}

bool ProjectConfigValidator::loadConfigSchema(const char* filename)
{
    using namespace rapidjson;

    QFile file(filename);
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
