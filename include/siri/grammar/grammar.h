/*
 * This grammar is generated using the Grammar.export_c() method and
 * should be used with the cleri module.
 *
 * Source class: SiriGrammar
 * Created at: 2017-01-05 15:50:35
 */

#pragma once

#include <siri/grammar/grammar.h>
#include <cleri/object.h>

cleri_grammar_t * compile_grammar(void);

enum cleri_grammar_ids {
    CLERI_NONE,   // used for objects with no name
    CLERI_GID_ACCESS_EXPR,
    CLERI_GID_ACCESS_KEYWORDS,
    CLERI_GID_AFTER_EXPR,
    CLERI_GID_AGGREGATE_FUNCTIONS,
    CLERI_GID_ALTER_DATABASE,
    CLERI_GID_ALTER_GROUP,
    CLERI_GID_ALTER_SERVER,
    CLERI_GID_ALTER_SERVERS,
    CLERI_GID_ALTER_STMT,
    CLERI_GID_ALTER_USER,
    CLERI_GID_BEFORE_EXPR,
    CLERI_GID_BETWEEN_EXPR,
    CLERI_GID_BOOL_OPERATOR,
    CLERI_GID_CALC_STMT,
    CLERI_GID_COUNT_GROUPS,
    CLERI_GID_COUNT_POOLS,
    CLERI_GID_COUNT_SERIES,
    CLERI_GID_COUNT_SERIES_LENGTH,
    CLERI_GID_COUNT_SERVERS,
    CLERI_GID_COUNT_SERVERS_RECEIVED,
    CLERI_GID_COUNT_SHARDS,
    CLERI_GID_COUNT_SHARDS_SIZE,
    CLERI_GID_COUNT_STMT,
    CLERI_GID_COUNT_USERS,
    CLERI_GID_CREATE_GROUP,
    CLERI_GID_CREATE_STMT,
    CLERI_GID_CREATE_USER,
    CLERI_GID_C_DIFFERENCE,
    CLERI_GID_DROP_GROUP,
    CLERI_GID_DROP_SERIES,
    CLERI_GID_DROP_SERVER,
    CLERI_GID_DROP_SHARDS,
    CLERI_GID_DROP_STMT,
    CLERI_GID_DROP_USER,
    CLERI_GID_F_COUNT,
    CLERI_GID_F_DERIVATIVE,
    CLERI_GID_F_DIFFERENCE,
    CLERI_GID_F_FILTER,
    CLERI_GID_F_MAX,
    CLERI_GID_F_MEAN,
    CLERI_GID_F_MEDIAN,
    CLERI_GID_F_MEDIAN_HIGH,
    CLERI_GID_F_MEDIAN_LOW,
    CLERI_GID_F_MIN,
    CLERI_GID_F_POINTS,
    CLERI_GID_F_PVARIANCE,
    CLERI_GID_F_SUM,
    CLERI_GID_F_VARIANCE,
    CLERI_GID_GRANT_STMT,
    CLERI_GID_GRANT_USER,
    CLERI_GID_GROUP_COLUMNS,
    CLERI_GID_GROUP_MATCH,
    CLERI_GID_GROUP_NAME,
    CLERI_GID_HELP,
    CLERI_GID_HELP_ACCESS,
    CLERI_GID_HELP_ALTER,
    CLERI_GID_HELP_ALTER_DATABASE,
    CLERI_GID_HELP_ALTER_GROUP,
    CLERI_GID_HELP_ALTER_SERVER,
    CLERI_GID_HELP_ALTER_SERVERS,
    CLERI_GID_HELP_ALTER_USER,
    CLERI_GID_HELP_COUNT,
    CLERI_GID_HELP_COUNT_GROUPS,
    CLERI_GID_HELP_COUNT_POOLS,
    CLERI_GID_HELP_COUNT_SERIES,
    CLERI_GID_HELP_COUNT_SERVERS,
    CLERI_GID_HELP_COUNT_SHARDS,
    CLERI_GID_HELP_COUNT_USERS,
    CLERI_GID_HELP_CREATE,
    CLERI_GID_HELP_CREATE_GROUP,
    CLERI_GID_HELP_CREATE_USER,
    CLERI_GID_HELP_DROP,
    CLERI_GID_HELP_DROP_GROUP,
    CLERI_GID_HELP_DROP_SERIES,
    CLERI_GID_HELP_DROP_SERVER,
    CLERI_GID_HELP_DROP_SHARDS,
    CLERI_GID_HELP_DROP_USER,
    CLERI_GID_HELP_FUNCTIONS,
    CLERI_GID_HELP_GRANT,
    CLERI_GID_HELP_LIST,
    CLERI_GID_HELP_LIST_GROUPS,
    CLERI_GID_HELP_LIST_POOLS,
    CLERI_GID_HELP_LIST_SERIES,
    CLERI_GID_HELP_LIST_SERVERS,
    CLERI_GID_HELP_LIST_SHARDS,
    CLERI_GID_HELP_LIST_USERS,
    CLERI_GID_HELP_NOACCESS,
    CLERI_GID_HELP_REVOKE,
    CLERI_GID_HELP_SELECT,
    CLERI_GID_HELP_SHOW,
    CLERI_GID_HELP_TIMEIT,
    CLERI_GID_HELP_TIMEZONES,
    CLERI_GID_INT_EXPR,
    CLERI_GID_INT_OPERATOR,
    CLERI_GID_K_ACCESS,
    CLERI_GID_K_ACTIVE_HANDLES,
    CLERI_GID_K_ADDRESS,
    CLERI_GID_K_AFTER,
    CLERI_GID_K_ALTER,
    CLERI_GID_K_AND,
    CLERI_GID_K_AS,
    CLERI_GID_K_BACKUP_MODE,
    CLERI_GID_K_BEFORE,
    CLERI_GID_K_BETWEEN,
    CLERI_GID_K_BUFFER_PATH,
    CLERI_GID_K_BUFFER_SIZE,
    CLERI_GID_K_COUNT,
    CLERI_GID_K_CREATE,
    CLERI_GID_K_CRITICAL,
    CLERI_GID_K_DATABASE,
    CLERI_GID_K_DBNAME,
    CLERI_GID_K_DBPATH,
    CLERI_GID_K_DEBUG,
    CLERI_GID_K_DERIVATIVE,
    CLERI_GID_K_DIFFERENCE,
    CLERI_GID_K_DROP,
    CLERI_GID_K_DROP_THRESHOLD,
    CLERI_GID_K_DURATION_LOG,
    CLERI_GID_K_DURATION_NUM,
    CLERI_GID_K_END,
    CLERI_GID_K_ERROR,
    CLERI_GID_K_EXPRESSION,
    CLERI_GID_K_FALSE,
    CLERI_GID_K_FILTER,
    CLERI_GID_K_FLOAT,
    CLERI_GID_K_FOR,
    CLERI_GID_K_FROM,
    CLERI_GID_K_FULL,
    CLERI_GID_K_GRANT,
    CLERI_GID_K_GROUP,
    CLERI_GID_K_GROUPS,
    CLERI_GID_K_HELP,
    CLERI_GID_K_IGNORE_THRESHOLD,
    CLERI_GID_K_INFO,
    CLERI_GID_K_INSERT,
    CLERI_GID_K_INTEGER,
    CLERI_GID_K_INTERSECTION,
    CLERI_GID_K_IP_SUPPORT,
    CLERI_GID_K_LENGTH,
    CLERI_GID_K_LIBUV,
    CLERI_GID_K_LIMIT,
    CLERI_GID_K_LIST,
    CLERI_GID_K_LOG,
    CLERI_GID_K_LOG_LEVEL,
    CLERI_GID_K_MAX,
    CLERI_GID_K_MAX_OPEN_FILES,
    CLERI_GID_K_MEAN,
    CLERI_GID_K_MEDIAN,
    CLERI_GID_K_MEDIAN_HIGH,
    CLERI_GID_K_MEDIAN_LOW,
    CLERI_GID_K_MEM_USAGE,
    CLERI_GID_K_MERGE,
    CLERI_GID_K_MIN,
    CLERI_GID_K_MODIFY,
    CLERI_GID_K_NAME,
    CLERI_GID_K_NOW,
    CLERI_GID_K_NUMBER,
    CLERI_GID_K_ONLINE,
    CLERI_GID_K_OPEN_FILES,
    CLERI_GID_K_OR,
    CLERI_GID_K_PASSWORD,
    CLERI_GID_K_POINTS,
    CLERI_GID_K_POOL,
    CLERI_GID_K_POOLS,
    CLERI_GID_K_PORT,
    CLERI_GID_K_PREFIX,
    CLERI_GID_K_PVARIANCE,
    CLERI_GID_K_READ,
    CLERI_GID_K_RECEIVED_POINTS,
    CLERI_GID_K_REINDEX_PROGRESS,
    CLERI_GID_K_REVOKE,
    CLERI_GID_K_SELECT,
    CLERI_GID_K_SERIES,
    CLERI_GID_K_SERVER,
    CLERI_GID_K_SERVERS,
    CLERI_GID_K_SET,
    CLERI_GID_K_SHARDS,
    CLERI_GID_K_SHOW,
    CLERI_GID_K_SID,
    CLERI_GID_K_SIZE,
    CLERI_GID_K_START,
    CLERI_GID_K_STARTUP_TIME,
    CLERI_GID_K_STATUS,
    CLERI_GID_K_STRING,
    CLERI_GID_K_SUFFIX,
    CLERI_GID_K_SUM,
    CLERI_GID_K_SYMMETRIC_DIFFERENCE,
    CLERI_GID_K_SYNC_PROGRESS,
    CLERI_GID_K_TIMEIT,
    CLERI_GID_K_TIMEZONE,
    CLERI_GID_K_TIME_PRECISION,
    CLERI_GID_K_TO,
    CLERI_GID_K_TRUE,
    CLERI_GID_K_TYPE,
    CLERI_GID_K_UNION,
    CLERI_GID_K_UPTIME,
    CLERI_GID_K_USER,
    CLERI_GID_K_USERS,
    CLERI_GID_K_USING,
    CLERI_GID_K_UUID,
    CLERI_GID_K_VARIANCE,
    CLERI_GID_K_VERSION,
    CLERI_GID_K_WARNING,
    CLERI_GID_K_WHERE,
    CLERI_GID_K_WHO_AM_I,
    CLERI_GID_K_WRITE,
    CLERI_GID_LIMIT_EXPR,
    CLERI_GID_LIST_GROUPS,
    CLERI_GID_LIST_POOLS,
    CLERI_GID_LIST_SERIES,
    CLERI_GID_LIST_SERVERS,
    CLERI_GID_LIST_SHARDS,
    CLERI_GID_LIST_STMT,
    CLERI_GID_LIST_USERS,
    CLERI_GID_LOG_KEYWORDS,
    CLERI_GID_MERGE_AS,
    CLERI_GID_POOL_COLUMNS,
    CLERI_GID_POOL_PROPS,
    CLERI_GID_PREFIX_EXPR,
    CLERI_GID_REVOKE_STMT,
    CLERI_GID_REVOKE_USER,
    CLERI_GID_R_COMMENT,
    CLERI_GID_R_DOUBLEQ_STR,
    CLERI_GID_R_FLOAT,
    CLERI_GID_R_GRAVE_STR,
    CLERI_GID_R_INTEGER,
    CLERI_GID_R_REGEX,
    CLERI_GID_R_SINGLEQ_STR,
    CLERI_GID_R_TIME_STR,
    CLERI_GID_R_UINTEGER,
    CLERI_GID_R_UUID_STR,
    CLERI_GID_SELECT_AGGREGATE,
    CLERI_GID_SELECT_STMT,
    CLERI_GID_SERIES_COLUMNS,
    CLERI_GID_SERIES_MATCH,
    CLERI_GID_SERIES_NAME,
    CLERI_GID_SERIES_RE,
    CLERI_GID_SERIES_SEP,
    CLERI_GID_SERVER_COLUMNS,
    CLERI_GID_SET_ADDRESS,
    CLERI_GID_SET_BACKUP_MODE,
    CLERI_GID_SET_DROP_THRESHOLD,
    CLERI_GID_SET_EXPRESSION,
    CLERI_GID_SET_IGNORE_THRESHOLD,
    CLERI_GID_SET_LOG_LEVEL,
    CLERI_GID_SET_NAME,
    CLERI_GID_SET_PASSWORD,
    CLERI_GID_SET_PORT,
    CLERI_GID_SET_TIMEZONE,
    CLERI_GID_SHARD_COLUMNS,
    CLERI_GID_SHOW_STMT,
    CLERI_GID_START,
    CLERI_GID_STRING,
    CLERI_GID_STR_OPERATOR,
    CLERI_GID_SUFFIX_EXPR,
    CLERI_GID_TIMEIT_STMT,
    CLERI_GID_TIME_EXPR,
    CLERI_GID_USER_COLUMNS,
    CLERI_GID_UUID,
    CLERI_GID_WHERE_GROUP,
    CLERI_GID_WHERE_POOL,
    CLERI_GID_WHERE_SERIES,
    CLERI_GID_WHERE_SERVER,
    CLERI_GID_WHERE_SHARD,
    CLERI_GID_WHERE_USER,
    CLERI_GID__BOOLEAN,
    CLERI_END // can be used to get the enum length
};

