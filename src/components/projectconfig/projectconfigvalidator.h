#ifndef SRC_COMPONENTS_PROJECTCONFIG_PROJECTCONFIGVALIDATOR_H_
#define SRC_COMPONENTS_PROJECTCONFIG_PROJECTCONFIGVALIDATOR_H_

#include <QByteArray>

#include <rapidjson/schema.h>

#include <memory>

class ProjectConfigValidator
{
public:
    static bool loadConfigSchema(const char* filename=":/files/json/projectConfigSchema.json");
    static bool validateConfiguration(const QByteArray& wholeFile);

private:
    static std::shared_ptr<rapidjson::SchemaDocument> configSchema;
    static bool schemaInitialized;
};

#endif /* SRC_COMPONENTS_PROJECTCONFIG_PROJECTCONFIGVALIDATOR_H_ */
