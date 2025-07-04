/*
 * Copyright (c) 2017, 2024, Oracle and/or its affiliates.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2.0,
 * as published by the Free Software Foundation.
 *
 * This program is designed to work with certain software (including
 * but not limited to OpenSSL) that is licensed under separate terms,
 * as designated in a particular file or component or in included license
 * documentation.  The authors of MySQL hereby grant you an additional
 * permission to link the program and your derivative works with the
 * separately licensed software that they have either included with
 * the program or referenced in the documentation.
 *
 * This program is distributed in the hope that it will be useful,  but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#pragma once

/*
  This file is generated, do not edit.
  See file sql/gen_keyword_list.cc.
*/

// typedef struct { const char *word; int reserved; } keyword_t;

static const keyword_t keyword_list80[] = {
    {"ACCESSIBLE", 1},
    {"ACCOUNT", 0},
    {"ACTION", 0},
    {"ACTIVE", 0},
    {"ADD", 1},
    {"ADMIN", 0},
    {"AFTER", 0},
    {"AGAINST", 0},
    {"AGGREGATE", 0},
    {"ALGORITHM", 0},
    {"ALL", 1},
    {"ALTER", 1},
    {"ALWAYS", 0},
    {"ANALYZE", 1},
    {"AND", 1},
    {"ANY", 0},
    {"ARRAY", 0},
    {"AS", 1},
    {"ASC", 1},
    {"ASCII", 0},
    {"ASENSITIVE", 1},
    {"AT", 0},
    {"AUTOEXTEND_SIZE", 0},
    {"AUTO_INCREMENT", 0},
    {"AVG", 0},
    {"AVG_ROW_LENGTH", 0},
    {"BACKUP", 0},
    {"BEFORE", 1},
    {"BEGIN", 0},
    {"BETWEEN", 1},
    {"BIGINT", 1},
    {"BINARY", 1},
    {"BINLOG", 0},
    {"BIT", 0},
    {"BLOB", 1},
    {"BLOCK", 0},
    {"BOOL", 0},
    {"BOOLEAN", 0},
    {"BOTH", 1},
    {"BTREE", 0},
    {"BUCKETS", 0},
    {"BY", 1},
    {"BYTE", 0},
    {"CACHE", 0},
    {"CALL", 1},
    {"CASCADE", 1},
    {"CASCADED", 0},
    {"CASE", 1},
    {"CATALOG_NAME", 0},
    {"CHAIN", 0},
    {"CHANGE", 1},
    {"CHANGED", 0},
    {"CHANNEL", 0},
    {"CHAR", 1},
    {"CHARACTER", 1},
    {"CHARSET", 0},
    {"CHECK", 1},
    {"CHECKSUM", 0},
    {"CIPHER", 0},
    {"CLASS_ORIGIN", 0},
    {"CLIENT", 0},
    {"CLONE", 0},
    {"CLOSE", 0},
    {"COALESCE", 0},
    {"CODE", 0},
    {"COLLATE", 1},
    {"COLLATION", 0},
    {"COLUMN", 1},
    {"COLUMNS", 0},
    {"COLUMN_FORMAT", 0},
    {"COLUMN_NAME", 0},
    {"COMMENT", 0},
    {"COMMIT", 0},
    {"COMMITTED", 0},
    {"COMPACT", 0},
    {"COMPLETION", 0},
    {"COMPONENT", 0},
    {"COMPRESSED", 0},
    {"COMPRESSION", 0},
    {"CONCURRENT", 0},
    {"CONDITION", 1},
    {"CONNECTION", 0},
    {"CONSISTENT", 0},
    {"CONSTRAINT", 1},
    {"CONSTRAINT_CATALOG", 0},
    {"CONSTRAINT_NAME", 0},
    {"CONSTRAINT_SCHEMA", 0},
    {"CONTAINS", 0},
    {"CONTEXT", 0},
    {"CONTINUE", 1},
    {"CONVERT", 1},
    {"CPU", 0},
    {"CREATE", 1},
    {"CROSS", 1},
    {"CUBE", 1},
    {"CUME_DIST", 1},
    {"CURRENT", 0},
    {"CURRENT_DATE", 1},
    {"CURRENT_TIME", 1},
    {"CURRENT_TIMESTAMP", 1},
    {"CURRENT_USER", 1},
    {"CURSOR", 1},
    {"CURSOR_NAME", 0},
    {"DATA", 0},
    {"DATABASE", 1},
    {"DATABASES", 1},
    {"DATAFILE", 0},
    {"DATE", 0},
    {"DATETIME", 0},
    {"DAY", 0},
    {"DAY_HOUR", 1},
    {"DAY_MICROSECOND", 1},
    {"DAY_MINUTE", 1},
    {"DAY_SECOND", 1},
    {"DEALLOCATE", 0},
    {"DEC", 1},
    {"DECIMAL", 1},
    {"DECLARE", 1},
    {"DEFAULT", 1},
    {"DEFAULT_AUTH", 0},
    {"DEFINER", 0},
    {"DEFINITION", 0},
    {"DELAYED", 1},
    {"DELAY_KEY_WRITE", 0},
    {"DELETE", 1},
    {"DENSE_RANK", 1},
    {"DESC", 1},
    {"DESCRIBE", 1},
    {"DESCRIPTION", 0},
    {"DETERMINISTIC", 1},
    {"DIAGNOSTICS", 0},
    {"DIRECTORY", 0},
    {"DISABLE", 0},
    {"DISCARD", 0},
    {"DISK", 0},
    {"DISTINCT", 1},
    {"DISTINCTROW", 1},
    {"DIV", 1},
    {"DO", 0},
    {"DOUBLE", 1},
    {"DROP", 1},
    {"DUAL", 1},
    {"DUMPFILE", 0},
    {"DUPLICATE", 0},
    {"DYNAMIC", 0},
    {"EACH", 1},
    {"ELSE", 1},
    {"ELSEIF", 1},
    {"EMPTY", 1},
    {"ENABLE", 0},
    {"ENCLOSED", 1},
    {"ENCRYPTION", 0},
    {"END", 0},
    {"ENDS", 0},
    {"ENFORCED", 0},
    {"ENGINE", 0},
    {"ENGINES", 0},
    {"ENUM", 0},
    {"ERROR", 0},
    {"ERRORS", 0},
    {"ESCAPE", 0},
    {"ESCAPED", 1},
    {"EVENT", 0},
    {"EVENTS", 0},
    {"EVERY", 0},
    {"EXCEPT", 1},
    {"EXCHANGE", 0},
    {"EXCLUDE", 0},
    {"EXECUTE", 0},
    {"EXISTS", 1},
    {"EXIT", 1},
    {"EXPANSION", 0},
    {"EXPIRE", 0},
    {"EXPLAIN", 1},
    {"EXPORT", 0},
    {"EXTENDED", 0},
    {"EXTENT_SIZE", 0},
    {"FALSE", 1},
    {"FAST", 0},
    {"FAULTS", 0},
    {"FETCH", 1},
    {"FIELDS", 0},
    {"FILE", 0},
    {"FILE_BLOCK_SIZE", 0},
    {"FILTER", 0},
    {"FIRST", 0},
    {"FIRST_VALUE", 1},
    {"FIXED", 0},
    {"FLOAT", 1},
    {"FLOAT4", 1},
    {"FLOAT8", 1},
    {"FLUSH", 0},
    {"FOLLOWING", 0},
    {"FOLLOWS", 0},
    {"FOR", 1},
    {"FORCE", 1},
    {"FOREIGN", 1},
    {"FORMAT", 0},
    {"FOUND", 0},
    {"FROM", 1},
    {"FULL", 0},
    {"FULLTEXT", 1},
    {"FUNCTION", 1},
    {"GENERAL", 0},
    {"GENERATED", 1},
    {"GEOMCOLLECTION", 0},
    {"GEOMETRY", 0},
    {"GEOMETRYCOLLECTION", 0},
    {"GET", 1},
    {"GET_FORMAT", 0},
    {"GET_MASTER_PUBLIC_KEY", 0},
    {"GLOBAL", 0},
    {"GRANT", 1},
    {"GRANTS", 0},
    {"GROUP", 1},
    {"GROUPING", 1},
    {"GROUPS", 1},
    {"GROUP_REPLICATION", 0},
    {"HANDLER", 0},
    {"HASH", 0},
    {"HAVING", 1},
    {"HELP", 0},
    {"HIGH_PRIORITY", 1},
    {"HISTOGRAM", 0},
    {"HISTORY", 0},
    {"HOST", 0},
    {"HOSTS", 0},
    {"HOUR", 0},
    {"HOUR_MICROSECOND", 1},
    {"HOUR_MINUTE", 1},
    {"HOUR_SECOND", 1},
    {"IDENTIFIED", 0},
    {"IF", 1},
    {"IGNORE", 1},
    {"IGNORE_SERVER_IDS", 0},
    {"IMPORT", 0},
    {"IN", 1},
    {"INACTIVE", 0},
    {"INDEX", 1},
    {"INDEXES", 0},
    {"INFILE", 1},
    {"INITIAL_SIZE", 0},
    {"INNER", 1},
    {"INOUT", 1},
    {"INSENSITIVE", 1},
    {"INSERT", 1},
    {"INSERT_METHOD", 0},
    {"INSTALL", 0},
    {"INSTANCE", 0},
    {"INT", 1},
    {"INT1", 1},
    {"INT2", 1},
    {"INT3", 1},
    {"INT4", 1},
    {"INT8", 1},
    {"INTEGER", 1},
    {"INTERVAL", 1},
    {"INTO", 1},
    {"INVISIBLE", 0},
    {"INVOKER", 0},
    {"IO", 0},
    {"IO_AFTER_GTIDS", 1},
    {"IO_BEFORE_GTIDS", 1},
    {"IO_THREAD", 0},
    {"IPC", 0},
    {"IS", 1},
    {"ISOLATION", 0},
    {"ISSUER", 0},
    {"ITERATE", 1},
    {"JOIN", 1},
    {"JSON", 0},
    {"JSON_TABLE", 1},
    {"KEY", 1},
    {"KEYS", 1},
    {"KEY_BLOCK_SIZE", 0},
    {"KILL", 1},
    {"LAG", 1},
    {"LANGUAGE", 0},
    {"LAST", 0},
    {"LAST_VALUE", 1},
    {"LATERAL", 1},
    {"LEAD", 1},
    {"LEADING", 1},
    {"LEAVE", 1},
    {"LEAVES", 0},
    {"LEFT", 1},
    {"LESS", 0},
    {"LEVEL", 0},
    {"LIKE", 1},
    {"LIMIT", 1},
    {"LINEAR", 1},
    {"LINES", 1},
    {"LINESTRING", 0},
    {"LIST", 0},
    {"LOAD", 1},
    {"LOCAL", 0},
    {"LOCALTIME", 1},
    {"LOCALTIMESTAMP", 1},
    {"LOCK", 1},
    {"LOCKED", 0},
    {"LOCKS", 0},
    {"LOGFILE", 0},
    {"LOGS", 0},
    {"LONG", 1},
    {"LONGBLOB", 1},
    {"LONGTEXT", 1},
    {"LOOP", 1},
    {"LOW_PRIORITY", 1},
    {"MASTER", 0},
    {"MASTER_AUTO_POSITION", 0},
    {"MASTER_BIND", 1},
    {"MASTER_COMPRESSION_ALGORITHMS", 0},
    {"MASTER_CONNECT_RETRY", 0},
    {"MASTER_DELAY", 0},
    {"MASTER_HEARTBEAT_PERIOD", 0},
    {"MASTER_HOST", 0},
    {"MASTER_LOG_FILE", 0},
    {"MASTER_LOG_POS", 0},
    {"MASTER_PASSWORD", 0},
    {"MASTER_PORT", 0},
    {"MASTER_PUBLIC_KEY_PATH", 0},
    {"MASTER_RETRY_COUNT", 0},
    {"MASTER_SERVER_ID", 0},
    {"MASTER_SSL", 0},
    {"MASTER_SSL_CA", 0},
    {"MASTER_SSL_CAPATH", 0},
    {"MASTER_SSL_CERT", 0},
    {"MASTER_SSL_CIPHER", 0},
    {"MASTER_SSL_CRL", 0},
    {"MASTER_SSL_CRLPATH", 0},
    {"MASTER_SSL_KEY", 0},
    {"MASTER_SSL_VERIFY_SERVER_CERT", 1},
    {"MASTER_TLS_VERSION", 0},
    {"MASTER_USER", 0},
    {"MASTER_ZSTD_COMPRESSION_LEVEL", 0},
    {"MATCH", 1},
    {"MAXVALUE", 1},
    {"MAX_CONNECTIONS_PER_HOUR", 0},
    {"MAX_QUERIES_PER_HOUR", 0},
    {"MAX_ROWS", 0},
    {"MAX_SIZE", 0},
    {"MAX_UPDATES_PER_HOUR", 0},
    {"MAX_USER_CONNECTIONS", 0},
    {"MEDIUM", 0},
    {"MEDIUMBLOB", 1},
    {"MEDIUMINT", 1},
    {"MEDIUMTEXT", 1},
    {"MEMBER", 1},
    {"MEMORY", 0},
    {"MERGE", 0},
    {"MESSAGE_TEXT", 0},
    {"MICROSECOND", 0},
    {"MIDDLEINT", 1},
    {"MIGRATE", 0},
    {"MINUTE", 0},
    {"MINUTE_MICROSECOND", 1},
    {"MINUTE_SECOND", 1},
    {"MIN_ROWS", 0},
    {"MOD", 1},
    {"MODE", 0},
    {"MODIFIES", 1},
    {"MODIFY", 0},
    {"MONTH", 0},
    {"MULTILINESTRING", 0},
    {"MULTIPOINT", 0},
    {"MULTIPOLYGON", 0},
    {"MUTEX", 0},
    {"MYSQL_ERRNO", 0},
    {"NAME", 0},
    {"NAMES", 0},
    {"NATIONAL", 0},
    {"NATURAL", 1},
    {"NCHAR", 0},
    {"NDB", 0},
    {"NDBCLUSTER", 0},
    {"NESTED", 0},
    {"NETWORK_NAMESPACE", 0},
    {"NEVER", 0},
    {"NEW", 0},
    {"NEXT", 0},
    {"NO", 0},
    {"NODEGROUP", 0},
    {"NONE", 0},
    {"NOT", 1},
    {"NOWAIT", 0},
    {"NO_WAIT", 0},
    {"NO_WRITE_TO_BINLOG", 1},
    {"NTH_VALUE", 1},
    {"NTILE", 1},
    {"NULL", 1},
    {"NULLS", 0},
    {"NUMBER", 0},
    {"NUMERIC", 1},
    {"NVARCHAR", 0},
    {"OF", 1},
    {"OFFSET", 0},
    {"OJ", 0},
    {"OLD", 0},
    {"ON", 1},
    {"ONE", 0},
    {"ONLY", 0},
    {"OPEN", 0},
    {"OPTIMIZE", 1},
    {"OPTIMIZER_COSTS", 1},
    {"OPTION", 1},
    {"OPTIONAL", 0},
    {"OPTIONALLY", 1},
    {"OPTIONS", 0},
    {"OR", 1},
    {"ORDER", 1},
    {"ORDINALITY", 0},
    {"ORGANIZATION", 0},
    {"OTHERS", 0},
    {"OUT", 1},
    {"OUTER", 1},
    {"OUTFILE", 1},
    {"OVER", 1},
    {"OWNER", 0},
    {"PACK_KEYS", 0},
    {"PAGE", 0},
    {"PARSER", 0},
    {"PARTIAL", 0},
    {"PARTITION", 1},
    {"PARTITIONING", 0},
    {"PARTITIONS", 0},
    {"PASSWORD", 0},
    {"PATH", 0},
    {"PERCENT_RANK", 1},
    {"PERSIST", 0},
    {"PERSIST_ONLY", 0},
    {"PHASE", 0},
    {"PLUGIN", 0},
    {"PLUGINS", 0},
    {"PLUGIN_DIR", 0},
    {"POINT", 0},
    {"POLYGON", 0},
    {"PORT", 0},
    {"PRECEDES", 0},
    {"PRECEDING", 0},
    {"PRECISION", 1},
    {"PREPARE", 0},
    {"PRESERVE", 0},
    {"PREV", 0},
    {"PRIMARY", 1},
    {"PRIVILEGE_CHECKS_USER", 0},
    {"PRIVILEGES", 0},
    {"PROCEDURE", 1},
    {"PROCESS", 0},
    {"PROCESSLIST", 0},
    {"PROFILE", 0},
    {"PROFILES", 0},
    {"PROXY", 0},
    {"PURGE", 1},
    {"QUARTER", 0},
    {"QUERY", 0},
    {"QUICK", 0},
    {"RANDOM", 0},
    {"RANGE", 1},
    {"RANK", 1},
    {"READ", 1},
    {"READS", 1},
    {"READ_ONLY", 0},
    {"READ_WRITE", 1},
    {"REAL", 1},
    {"REBUILD", 0},
    {"RECOVER", 0},
    {"RECURSIVE", 1},
    {"REDO_BUFFER_SIZE", 0},
    {"REDUNDANT", 0},
    {"REFERENCE", 0},
    {"REFERENCES", 1},
    {"REGEXP", 1},
    {"RELAY", 0},
    {"RELAYLOG", 0},
    {"RELAY_LOG_FILE", 0},
    {"RELAY_LOG_POS", 0},
    {"RELAY_THREAD", 0},
    {"RELEASE", 1},
    {"RELOAD", 0},
    {"REMOVE", 0},
    {"RENAME", 1},
    {"REORGANIZE", 0},
    {"REPAIR", 0},
    {"REPEAT", 1},
    {"REPEATABLE", 0},
    {"REPLACE", 1},
    {"REPLICATE_DO_DB", 0},
    {"REPLICATE_DO_TABLE", 0},
    {"REPLICATE_IGNORE_DB", 0},
    {"REPLICATE_IGNORE_TABLE", 0},
    {"REPLICATE_REWRITE_DB", 0},
    {"REPLICATE_WILD_DO_TABLE", 0},
    {"REPLICATE_WILD_IGNORE_TABLE", 0},
    {"REPLICATION", 0},
    {"REQUIRE", 1},
    {"RESET", 0},
    {"RESIGNAL", 1},
    {"RESOURCE", 0},
    {"RESPECT", 0},
    {"RESTART", 0},
    {"RESTORE", 0},
    {"RESTRICT", 1},
    {"RESUME", 0},
    {"RETAIN", 0},
    {"RETURN", 1},
    {"RETURNED_SQLSTATE", 0},
    {"RETURNS", 0},
    {"REUSE", 0},
    {"REVERSE", 0},
    {"REVOKE", 1},
    {"RIGHT", 1},
    {"RLIKE", 1},
    {"ROLE", 0},
    {"ROLLBACK", 0},
    {"ROLLUP", 0},
    {"ROTATE", 0},
    {"ROUTINE", 0},
    {"ROW", 1},
    {"ROWS", 1},
    {"ROW_COUNT", 0},
    {"ROW_FORMAT", 0},
    {"ROW_NUMBER", 1},
    {"RTREE", 0},
    {"SAVEPOINT", 0},
    {"SCHEDULE", 0},
    {"SCHEMA", 1},
    {"SCHEMAS", 1},
    {"SCHEMA_NAME", 0},
    {"SECOND", 0},
    {"SECONDARY", 0},
    {"SECONDARY_ENGINE", 0},
    {"SECONDARY_LOAD", 0},
    {"SECONDARY_UNLOAD", 0},
    {"SECOND_MICROSECOND", 1},
    {"SECURITY", 0},
    {"SELECT", 1},
    {"SENSITIVE", 1},
    {"SEPARATOR", 1},
    {"SERIAL", 0},
    {"SERIALIZABLE", 0},
    {"SERVER", 0},
    {"SESSION", 0},
    {"SET", 1},
    {"SHARE", 0},
    {"SHOW", 1},
    {"SHUTDOWN", 0},
    {"SIGNAL", 1},
    {"SIGNED", 0},
    {"SIMPLE", 0},
    {"SKIP", 0},
    {"SLAVE", 0},
    {"SLOW", 0},
    {"SMALLINT", 1},
    {"SNAPSHOT", 0},
    {"SOCKET", 0},
    {"SOME", 0},
    {"SONAME", 0},
    {"SOUNDS", 0},
    {"SOURCE", 0},
    {"SPATIAL", 1},
    {"SPECIFIC", 1},
    {"SQL", 1},
    {"SQLEXCEPTION", 1},
    {"SQLSTATE", 1},
    {"SQLWARNING", 1},
    {"SQL_AFTER_GTIDS", 0},
    {"SQL_AFTER_MTS_GAPS", 0},
    {"SQL_BEFORE_GTIDS", 0},
    {"SQL_BIG_RESULT", 1},
    {"SQL_BUFFER_RESULT", 0},
    {"SQL_CALC_FOUND_ROWS", 1},
    {"SQL_NO_CACHE", 0},
    {"SQL_SMALL_RESULT", 1},
    {"SQL_THREAD", 0},
    {"SQL_TSI_DAY", 0},
    {"SQL_TSI_HOUR", 0},
    {"SQL_TSI_MINUTE", 0},
    {"SQL_TSI_MONTH", 0},
    {"SQL_TSI_QUARTER", 0},
    {"SQL_TSI_SECOND", 0},
    {"SQL_TSI_WEEK", 0},
    {"SQL_TSI_YEAR", 0},
    {"SRID", 0},
    {"SSL", 1},
    {"STACKED", 0},
    {"START", 0},
    {"STARTING", 1},
    {"STARTS", 0},
    {"STATS_AUTO_RECALC", 0},
    {"STATS_PERSISTENT", 0},
    {"STATS_SAMPLE_PAGES", 0},
    {"STATUS", 0},
    {"STOP", 0},
    {"STORAGE", 0},
    {"STORED", 1},
    {"STRAIGHT_JOIN", 1},
    {"STRING", 0},
    {"SUBCLASS_ORIGIN", 0},
    {"SUBJECT", 0},
    {"SUBPARTITION", 0},
    {"SUBPARTITIONS", 0},
    {"SUPER", 0},
    {"SUSPEND", 0},
    {"SWAPS", 0},
    {"SWITCHES", 0},
    {"SYSTEM", 1},
    {"TABLE", 1},
    {"TABLES", 0},
    {"TABLESPACE", 0},
    {"TABLE_CHECKSUM", 0},
    {"TABLE_NAME", 0},
    {"TEMPORARY", 0},
    {"TEMPTABLE", 0},
    {"TERMINATED", 1},
    {"TEXT", 0},
    {"THAN", 0},
    {"THEN", 1},
    {"THREAD_PRIORITY", 0},
    {"TIES", 0},
    {"TIME", 0},
    {"TIMESTAMP", 0},
    {"TIMESTAMPADD", 0},
    {"TIMESTAMPDIFF", 0},
    {"TINYBLOB", 1},
    {"TINYINT", 1},
    {"TINYTEXT", 1},
    {"TO", 1},
    {"TRAILING", 1},
    {"TRANSACTION", 0},
    {"TRIGGER", 1},
    {"TRIGGERS", 0},
    {"TRUE", 1},
    {"TRUNCATE", 0},
    {"TYPE", 0},
    {"TYPES", 0},
    {"UNBOUNDED", 0},
    {"UNCOMMITTED", 0},
    {"UNDEFINED", 0},
    {"UNDO", 1},
    {"UNDOFILE", 0},
    {"UNDO_BUFFER_SIZE", 0},
    {"UNICODE", 0},
    {"UNINSTALL", 0},
    {"UNION", 1},
    {"UNIQUE", 1},
    {"UNKNOWN", 0},
    {"UNLOCK", 1},
    {"UNSIGNED", 1},
    {"UNTIL", 0},
    {"UPDATE", 1},
    {"UPGRADE", 0},
    {"USAGE", 1},
    {"USE", 1},
    {"USER", 0},
    {"USER_RESOURCES", 0},
    {"USE_FRM", 0},
    {"USING", 1},
    {"UTC_DATE", 1},
    {"UTC_TIME", 1},
    {"UTC_TIMESTAMP", 1},
    {"VALIDATION", 0},
    {"VALUE", 0},
    {"VALUES", 1},
    {"VARBINARY", 1},
    {"VARCHAR", 1},
    {"VARCHARACTER", 1},
    {"VARIABLES", 0},
    {"VARYING", 1},
    {"VCPU", 0},
    {"VIEW", 0},
    {"VIRTUAL", 1},
    {"VISIBLE", 0},
    {"WAIT", 0},
    {"WARNINGS", 0},
    {"WEEK", 0},
    {"WEIGHT_STRING", 0},
    {"WHEN", 1},
    {"WHERE", 1},
    {"WHILE", 1},
    {"WINDOW", 1},
    {"WITH", 1},
    {"WITHOUT", 0},
    {"WORK", 0},
    {"WRAPPER", 0},
    {"WRITE", 1},
    {"X509", 0},
    {"XA", 0},
    {"XID", 0},
    {"XML", 0},
    {"XOR", 1},
    {"YEAR", 0},
    {"YEAR_MONTH", 1},
    {"ZEROFILL", 1},

    {"MASTER_COMPRESSION_ALGORITHM", 0},
    {"MASTER_ZSTD_COMPRESSION_LEVEL", 0},
    {"PRIVILEGE_CHECKS_USER", 0},
    {"MASTER_TLS_CIPHERSUITES", 0},
    {"REQUIRE_ROW_FORMAT", 0},
    {"PASSWORD_LOCK_TIME", 0},
    {"FAILED_LOGIN_ATTEMPTS", 0},
    {"REQUIRE_TABLE_PRIMARY_KEY_CHECK", 0},
    {"STREAM", 0},
    {"OFF", 0},
}; /*keyword_list*/
