#ifndef __BCASTMSGS_H
#define __BCASTMSGS_H

#include <QString>

namespace BcastMsg {

static const QString NodeCreated = "node_created";
static const QString RequestCanDb = "request_can_db";
static const QString ConfigChanged = "config_changed";
static const QString NodeDeleted = "node_deleted";
static const QString InitDone = "init_done";
static const QString CanDbUpdated = "can_db_updated";

}

#endif /* !__BCASTMSGS_H */
