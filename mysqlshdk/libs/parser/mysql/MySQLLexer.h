// clang-format off
/*
 * Copyright (c) 2020, 2024, Oracle and/or its affiliates.
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
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 * the GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */




// Generated from /Users/kojima/dev/ngshell/mysqlshdk/libs/parser/grammars/MySQLLexer.g4 by ANTLR 4.10.1

#pragma once


#include "antlr4-runtime.h"

#include "mysqlshdk/libs/parser/MySQLBaseLexer.h"

namespace parsers {


class PARSERS_PUBLIC_TYPE MySQLLexer : public MySQLBaseLexer {
public:
  enum {
    ACCESSIBLE_SYMBOL = 1, ACCOUNT_SYMBOL = 2, ACTION_SYMBOL = 3, ADD_SYMBOL = 4, 
    ADDDATE_SYMBOL = 5, AFTER_SYMBOL = 6, AGAINST_SYMBOL = 7, AGGREGATE_SYMBOL = 8, 
    ALGORITHM_SYMBOL = 9, ALL_SYMBOL = 10, ALTER_SYMBOL = 11, ALWAYS_SYMBOL = 12, 
    ANALYSE_SYMBOL = 13, ANALYZE_SYMBOL = 14, AND_SYMBOL = 15, ANY_SYMBOL = 16, 
    AS_SYMBOL = 17, ASC_SYMBOL = 18, ASCII_SYMBOL = 19, ASENSITIVE_SYMBOL = 20, 
    AT_SYMBOL = 21, AUTHORS_SYMBOL = 22, AUTOEXTEND_SIZE_SYMBOL = 23, AUTO_INCREMENT_SYMBOL = 24, 
    AVG_ROW_LENGTH_SYMBOL = 25, AVG_SYMBOL = 26, BACKUP_SYMBOL = 27, BEFORE_SYMBOL = 28, 
    BEGIN_SYMBOL = 29, BETWEEN_SYMBOL = 30, BIGINT_SYMBOL = 31, BINARY_SYMBOL = 32, 
    BINLOG_SYMBOL = 33, BIN_NUM_SYMBOL = 34, BIT_AND_SYMBOL = 35, BIT_OR_SYMBOL = 36, 
    BIT_SYMBOL = 37, BIT_XOR_SYMBOL = 38, BLOB_SYMBOL = 39, BLOCK_SYMBOL = 40, 
    BOOLEAN_SYMBOL = 41, BOOL_SYMBOL = 42, BOTH_SYMBOL = 43, BTREE_SYMBOL = 44, 
    BY_SYMBOL = 45, BYTE_SYMBOL = 46, CACHE_SYMBOL = 47, CALL_SYMBOL = 48, 
    CASCADE_SYMBOL = 49, CASCADED_SYMBOL = 50, CASE_SYMBOL = 51, CAST_SYMBOL = 52, 
    CATALOG_NAME_SYMBOL = 53, CHAIN_SYMBOL = 54, CHANGE_SYMBOL = 55, CHANGED_SYMBOL = 56, 
    CHANNEL_SYMBOL = 57, CHARSET_SYMBOL = 58, CHARACTER_SYMBOL = 59, CHAR_SYMBOL = 60, 
    CHECKSUM_SYMBOL = 61, CHECK_SYMBOL = 62, CIPHER_SYMBOL = 63, CLASS_ORIGIN_SYMBOL = 64, 
    CLIENT_SYMBOL = 65, CLOSE_SYMBOL = 66, COALESCE_SYMBOL = 67, CODE_SYMBOL = 68, 
    COLLATE_SYMBOL = 69, COLLATION_SYMBOL = 70, COLUMNS_SYMBOL = 71, COLUMN_SYMBOL = 72, 
    COLUMN_NAME_SYMBOL = 73, COLUMN_FORMAT_SYMBOL = 74, COMMENT_SYMBOL = 75, 
    COMMITTED_SYMBOL = 76, COMMIT_SYMBOL = 77, COMPACT_SYMBOL = 78, COMPLETION_SYMBOL = 79, 
    COMPRESSED_SYMBOL = 80, COMPRESSION_SYMBOL = 81, CONCURRENT_SYMBOL = 82, 
    CONDITION_SYMBOL = 83, CONNECTION_SYMBOL = 84, CONSISTENT_SYMBOL = 85, 
    CONSTRAINT_SYMBOL = 86, CONSTRAINT_CATALOG_SYMBOL = 87, CONSTRAINT_NAME_SYMBOL = 88, 
    CONSTRAINT_SCHEMA_SYMBOL = 89, CONTAINS_SYMBOL = 90, CONTEXT_SYMBOL = 91, 
    CONTINUE_SYMBOL = 92, CONTRIBUTORS_SYMBOL = 93, CONVERT_SYMBOL = 94, 
    COUNT_SYMBOL = 95, CPU_SYMBOL = 96, CREATE_SYMBOL = 97, CROSS_SYMBOL = 98, 
    CUBE_SYMBOL = 99, CURDATE_SYMBOL = 100, CURRENT_SYMBOL = 101, CURRENT_DATE_SYMBOL = 102, 
    CURRENT_TIME_SYMBOL = 103, CURRENT_TIMESTAMP_SYMBOL = 104, CURRENT_USER_SYMBOL = 105, 
    CURSOR_SYMBOL = 106, CURSOR_NAME_SYMBOL = 107, CURTIME_SYMBOL = 108, 
    DATABASE_SYMBOL = 109, DATABASES_SYMBOL = 110, DATAFILE_SYMBOL = 111, 
    DATA_SYMBOL = 112, DATETIME_SYMBOL = 113, DATE_ADD_SYMBOL = 114, DATE_SUB_SYMBOL = 115, 
    DATE_SYMBOL = 116, DAYOFMONTH_SYMBOL = 117, DAY_HOUR_SYMBOL = 118, DAY_MICROSECOND_SYMBOL = 119, 
    DAY_MINUTE_SYMBOL = 120, DAY_SECOND_SYMBOL = 121, DAY_SYMBOL = 122, 
    DEALLOCATE_SYMBOL = 123, DEC_SYMBOL = 124, DECIMAL_NUM_SYMBOL = 125, 
    DECIMAL_SYMBOL = 126, DECLARE_SYMBOL = 127, DEFAULT_SYMBOL = 128, DEFAULT_AUTH_SYMBOL = 129, 
    DEFINER_SYMBOL = 130, DELAYED_SYMBOL = 131, DELAY_KEY_WRITE_SYMBOL = 132, 
    DELETE_SYMBOL = 133, DESC_SYMBOL = 134, DESCRIBE_SYMBOL = 135, DES_KEY_FILE_SYMBOL = 136, 
    DETERMINISTIC_SYMBOL = 137, DIAGNOSTICS_SYMBOL = 138, DIRECTORY_SYMBOL = 139, 
    DISABLE_SYMBOL = 140, DISCARD_SYMBOL = 141, DISK_SYMBOL = 142, DISTINCT_SYMBOL = 143, 
    DISTINCTROW_SYMBOL = 144, DIV_SYMBOL = 145, DOUBLE_SYMBOL = 146, DO_SYMBOL = 147, 
    DROP_SYMBOL = 148, DUAL_SYMBOL = 149, DUMPFILE_SYMBOL = 150, DUPLICATE_SYMBOL = 151, 
    DYNAMIC_SYMBOL = 152, EACH_SYMBOL = 153, ELSE_SYMBOL = 154, ELSEIF_SYMBOL = 155, 
    ENABLE_SYMBOL = 156, ENCLOSED_SYMBOL = 157, ENCRYPTION_SYMBOL = 158, 
    END_SYMBOL = 159, ENDS_SYMBOL = 160, END_OF_INPUT_SYMBOL = 161, ENGINES_SYMBOL = 162, 
    ENGINE_SYMBOL = 163, ENUM_SYMBOL = 164, ERROR_SYMBOL = 165, ERRORS_SYMBOL = 166, 
    ESCAPED_SYMBOL = 167, ESCAPE_SYMBOL = 168, EVENTS_SYMBOL = 169, EVENT_SYMBOL = 170, 
    EVERY_SYMBOL = 171, EXCHANGE_SYMBOL = 172, EXECUTE_SYMBOL = 173, EXISTS_SYMBOL = 174, 
    EXIT_SYMBOL = 175, EXPANSION_SYMBOL = 176, EXPIRE_SYMBOL = 177, EXPLAIN_SYMBOL = 178, 
    EXPORT_SYMBOL = 179, EXTENDED_SYMBOL = 180, EXTENT_SIZE_SYMBOL = 181, 
    EXTRACT_SYMBOL = 182, FALSE_SYMBOL = 183, FAST_SYMBOL = 184, FAULTS_SYMBOL = 185, 
    FETCH_SYMBOL = 186, FIELDS_SYMBOL = 187, FILE_SYMBOL = 188, FILE_BLOCK_SIZE_SYMBOL = 189, 
    FILTER_SYMBOL = 190, FIRST_SYMBOL = 191, FIXED_SYMBOL = 192, FLOAT4_SYMBOL = 193, 
    FLOAT8_SYMBOL = 194, FLOAT_SYMBOL = 195, FLUSH_SYMBOL = 196, FOLLOWS_SYMBOL = 197, 
    FORCE_SYMBOL = 198, FOREIGN_SYMBOL = 199, FOR_SYMBOL = 200, FORMAT_SYMBOL = 201, 
    FOUND_SYMBOL = 202, FROM_SYMBOL = 203, FULL_SYMBOL = 204, FULLTEXT_SYMBOL = 205, 
    FUNCTION_SYMBOL = 206, GET_SYMBOL = 207, GENERAL_SYMBOL = 208, GENERATED_SYMBOL = 209, 
    GROUP_REPLICATION_SYMBOL = 210, GEOMETRYCOLLECTION_SYMBOL = 211, GEOMETRY_SYMBOL = 212, 
    GET_FORMAT_SYMBOL = 213, GLOBAL_SYMBOL = 214, GRANT_SYMBOL = 215, GRANTS_SYMBOL = 216, 
    GROUP_SYMBOL = 217, GROUP_CONCAT_SYMBOL = 218, HANDLER_SYMBOL = 219, 
    HASH_SYMBOL = 220, HAVING_SYMBOL = 221, HELP_SYMBOL = 222, HIGH_PRIORITY_SYMBOL = 223, 
    HOST_SYMBOL = 224, HOSTS_SYMBOL = 225, HOUR_MICROSECOND_SYMBOL = 226, 
    HOUR_MINUTE_SYMBOL = 227, HOUR_SECOND_SYMBOL = 228, HOUR_SYMBOL = 229, 
    IDENTIFIED_SYMBOL = 230, IF_SYMBOL = 231, IGNORE_SYMBOL = 232, IGNORE_SERVER_IDS_SYMBOL = 233, 
    IMPORT_SYMBOL = 234, INDEXES_SYMBOL = 235, INDEX_SYMBOL = 236, INFILE_SYMBOL = 237, 
    INITIAL_SIZE_SYMBOL = 238, INNER_SYMBOL = 239, INOUT_SYMBOL = 240, INSENSITIVE_SYMBOL = 241, 
    INSERT_SYMBOL = 242, INSERT_METHOD_SYMBOL = 243, INSTANCE_SYMBOL = 244, 
    INSTALL_SYMBOL = 245, INTEGER_SYMBOL = 246, INTERVAL_SYMBOL = 247, INTO_SYMBOL = 248, 
    INT_SYMBOL = 249, INVOKER_SYMBOL = 250, IN_SYMBOL = 251, IO_AFTER_GTIDS_SYMBOL = 252, 
    IO_BEFORE_GTIDS_SYMBOL = 253, IO_THREAD_SYMBOL = 254, IO_SYMBOL = 255, 
    IPC_SYMBOL = 256, IS_SYMBOL = 257, ISOLATION_SYMBOL = 258, ISSUER_SYMBOL = 259, 
    ITERATE_SYMBOL = 260, JOIN_SYMBOL = 261, JSON_SYMBOL = 262, KEYS_SYMBOL = 263, 
    KEY_BLOCK_SIZE_SYMBOL = 264, KEY_SYMBOL = 265, KILL_SYMBOL = 266, LANGUAGE_SYMBOL = 267, 
    LAST_SYMBOL = 268, LEADING_SYMBOL = 269, LEAVES_SYMBOL = 270, LEAVE_SYMBOL = 271, 
    LEFT_SYMBOL = 272, LESS_SYMBOL = 273, LEVEL_SYMBOL = 274, LIKE_SYMBOL = 275, 
    LIMIT_SYMBOL = 276, LINEAR_SYMBOL = 277, LINES_SYMBOL = 278, LINESTRING_SYMBOL = 279, 
    LIST_SYMBOL = 280, LOAD_SYMBOL = 281, LOCALTIME_SYMBOL = 282, LOCALTIMESTAMP_SYMBOL = 283, 
    LOCAL_SYMBOL = 284, LOCATOR_SYMBOL = 285, LOCKS_SYMBOL = 286, LOCK_SYMBOL = 287, 
    LOGFILE_SYMBOL = 288, LOGS_SYMBOL = 289, LONGBLOB_SYMBOL = 290, LONGTEXT_SYMBOL = 291, 
    LONG_NUM_SYMBOL = 292, LONG_SYMBOL = 293, LOOP_SYMBOL = 294, LOW_PRIORITY_SYMBOL = 295, 
    MASTER_AUTO_POSITION_SYMBOL = 296, MASTER_BIND_SYMBOL = 297, MASTER_CONNECT_RETRY_SYMBOL = 298, 
    MASTER_DELAY_SYMBOL = 299, MASTER_HOST_SYMBOL = 300, MASTER_LOG_FILE_SYMBOL = 301, 
    MASTER_LOG_POS_SYMBOL = 302, MASTER_PASSWORD_SYMBOL = 303, MASTER_PORT_SYMBOL = 304, 
    MASTER_RETRY_COUNT_SYMBOL = 305, MASTER_SERVER_ID_SYMBOL = 306, MASTER_SSL_CAPATH_SYMBOL = 307, 
    MASTER_SSL_CA_SYMBOL = 308, MASTER_SSL_CERT_SYMBOL = 309, MASTER_SSL_CIPHER_SYMBOL = 310, 
    MASTER_SSL_CRL_SYMBOL = 311, MASTER_SSL_CRLPATH_SYMBOL = 312, MASTER_SSL_KEY_SYMBOL = 313, 
    MASTER_SSL_SYMBOL = 314, MASTER_SSL_VERIFY_SERVER_CERT_SYMBOL = 315, 
    MASTER_SYMBOL = 316, MASTER_TLS_VERSION_SYMBOL = 317, MASTER_USER_SYMBOL = 318, 
    MASTER_HEARTBEAT_PERIOD_SYMBOL = 319, MATCH_SYMBOL = 320, MAX_CONNECTIONS_PER_HOUR_SYMBOL = 321, 
    MAX_QUERIES_PER_HOUR_SYMBOL = 322, MAX_ROWS_SYMBOL = 323, MAX_SIZE_SYMBOL = 324, 
    MAX_STATEMENT_TIME_SYMBOL = 325, MAX_SYMBOL = 326, MAX_UPDATES_PER_HOUR_SYMBOL = 327, 
    MAX_USER_CONNECTIONS_SYMBOL = 328, MAXVALUE_SYMBOL = 329, MEDIUMBLOB_SYMBOL = 330, 
    MEDIUMINT_SYMBOL = 331, MEDIUMTEXT_SYMBOL = 332, MEDIUM_SYMBOL = 333, 
    MEMORY_SYMBOL = 334, MERGE_SYMBOL = 335, MESSAGE_TEXT_SYMBOL = 336, 
    MICROSECOND_SYMBOL = 337, MID_SYMBOL = 338, MIDDLEINT_SYMBOL = 339, 
    MIGRATE_SYMBOL = 340, MINUTE_MICROSECOND_SYMBOL = 341, MINUTE_SECOND_SYMBOL = 342, 
    MINUTE_SYMBOL = 343, MIN_ROWS_SYMBOL = 344, MIN_SYMBOL = 345, MODE_SYMBOL = 346, 
    MODIFIES_SYMBOL = 347, MODIFY_SYMBOL = 348, MOD_SYMBOL = 349, MONTH_SYMBOL = 350, 
    MULTILINESTRING_SYMBOL = 351, MULTIPOINT_SYMBOL = 352, MULTIPOLYGON_SYMBOL = 353, 
    MUTEX_SYMBOL = 354, MYSQL_ERRNO_SYMBOL = 355, NAMES_SYMBOL = 356, NAME_SYMBOL = 357, 
    NATIONAL_SYMBOL = 358, NATURAL_SYMBOL = 359, NCHAR_STRING_SYMBOL = 360, 
    NCHAR_SYMBOL = 361, NDB_SYMBOL = 362, NDBCLUSTER_SYMBOL = 363, NEG_SYMBOL = 364, 
    NEVER_SYMBOL = 365, NEW_SYMBOL = 366, NEXT_SYMBOL = 367, NODEGROUP_SYMBOL = 368, 
    NONE_SYMBOL = 369, NONBLOCKING_SYMBOL = 370, NOT_SYMBOL = 371, NOW_SYMBOL = 372, 
    NO_SYMBOL = 373, NO_WAIT_SYMBOL = 374, NO_WRITE_TO_BINLOG_SYMBOL = 375, 
    NULL_SYMBOL = 376, NUMBER_SYMBOL = 377, NUMERIC_SYMBOL = 378, NVARCHAR_SYMBOL = 379, 
    OFFLINE_SYMBOL = 380, OFFSET_SYMBOL = 381, OLD_PASSWORD_SYMBOL = 382, 
    ON_SYMBOL = 383, ONE_SYMBOL = 384, ONLINE_SYMBOL = 385, ONLY_SYMBOL = 386, 
    OPEN_SYMBOL = 387, OPTIMIZE_SYMBOL = 388, OPTIMIZER_COSTS_SYMBOL = 389, 
    OPTIONS_SYMBOL = 390, OPTION_SYMBOL = 391, OPTIONALLY_SYMBOL = 392, 
    ORDER_SYMBOL = 393, OR_SYMBOL = 394, OUTER_SYMBOL = 395, OUTFILE_SYMBOL = 396, 
    OUT_SYMBOL = 397, OWNER_SYMBOL = 398, PACK_KEYS_SYMBOL = 399, PAGE_SYMBOL = 400, 
    PARSER_SYMBOL = 401, PARTIAL_SYMBOL = 402, PARTITIONING_SYMBOL = 403, 
    PARTITIONS_SYMBOL = 404, PARTITION_SYMBOL = 405, PASSWORD_SYMBOL = 406, 
    PHASE_SYMBOL = 407, PLUGINS_SYMBOL = 408, PLUGIN_DIR_SYMBOL = 409, PLUGIN_SYMBOL = 410, 
    POINT_SYMBOL = 411, POLYGON_SYMBOL = 412, PORT_SYMBOL = 413, POSITION_SYMBOL = 414, 
    PRECEDES_SYMBOL = 415, PRECISION_SYMBOL = 416, PREPARE_SYMBOL = 417, 
    PRESERVE_SYMBOL = 418, PREV_SYMBOL = 419, PRIMARY_SYMBOL = 420, PRIVILEGES_SYMBOL = 421, 
    PROCEDURE_SYMBOL = 422, PROCESS_SYMBOL = 423, PROCESSLIST_SYMBOL = 424, 
    PROFILE_SYMBOL = 425, PROFILES_SYMBOL = 426, PROXY_SYMBOL = 427, PURGE_SYMBOL = 428, 
    QUARTER_SYMBOL = 429, QUERY_SYMBOL = 430, QUICK_SYMBOL = 431, RANGE_SYMBOL = 432, 
    READS_SYMBOL = 433, READ_ONLY_SYMBOL = 434, READ_SYMBOL = 435, READ_WRITE_SYMBOL = 436, 
    REAL_SYMBOL = 437, REBUILD_SYMBOL = 438, RECOVER_SYMBOL = 439, REDOFILE_SYMBOL = 440, 
    REDO_BUFFER_SIZE_SYMBOL = 441, REDUNDANT_SYMBOL = 442, REFERENCES_SYMBOL = 443, 
    REGEXP_SYMBOL = 444, RELAY_SYMBOL = 445, RELAYLOG_SYMBOL = 446, RELAY_LOG_FILE_SYMBOL = 447, 
    RELAY_LOG_POS_SYMBOL = 448, RELAY_THREAD_SYMBOL = 449, RELEASE_SYMBOL = 450, 
    RELOAD_SYMBOL = 451, REMOVE_SYMBOL = 452, RENAME_SYMBOL = 453, REORGANIZE_SYMBOL = 454, 
    REPAIR_SYMBOL = 455, REPEATABLE_SYMBOL = 456, REPEAT_SYMBOL = 457, REPLACE_SYMBOL = 458, 
    REPLICATION_SYMBOL = 459, REPLICATE_DO_DB_SYMBOL = 460, REPLICATE_IGNORE_DB_SYMBOL = 461, 
    REPLICATE_DO_TABLE_SYMBOL = 462, REPLICATE_IGNORE_TABLE_SYMBOL = 463, 
    REPLICATE_WILD_DO_TABLE_SYMBOL = 464, REPLICATE_WILD_IGNORE_TABLE_SYMBOL = 465, 
    REPLICATE_REWRITE_DB_SYMBOL = 466, REQUIRE_SYMBOL = 467, RESET_SYMBOL = 468, 
    RESIGNAL_SYMBOL = 469, RESTORE_SYMBOL = 470, RESTRICT_SYMBOL = 471, 
    RESUME_SYMBOL = 472, RETURNED_SQLSTATE_SYMBOL = 473, RETURNS_SYMBOL = 474, 
    RETURN_SYMBOL = 475, REVERSE_SYMBOL = 476, REVOKE_SYMBOL = 477, RIGHT_SYMBOL = 478, 
    RLIKE_SYMBOL = 479, ROLLBACK_SYMBOL = 480, ROLLUP_SYMBOL = 481, ROTATE_SYMBOL = 482, 
    ROUTINE_SYMBOL = 483, ROWS_SYMBOL = 484, ROW_COUNT_SYMBOL = 485, ROW_FORMAT_SYMBOL = 486, 
    ROW_SYMBOL = 487, RTREE_SYMBOL = 488, SAVEPOINT_SYMBOL = 489, SCHEDULE_SYMBOL = 490, 
    SCHEMA_SYMBOL = 491, SCHEMA_NAME_SYMBOL = 492, SCHEMAS_SYMBOL = 493, 
    SECOND_MICROSECOND_SYMBOL = 494, SECOND_SYMBOL = 495, SECURITY_SYMBOL = 496, 
    SELECT_SYMBOL = 497, SENSITIVE_SYMBOL = 498, SEPARATOR_SYMBOL = 499, 
    SERIALIZABLE_SYMBOL = 500, SERIAL_SYMBOL = 501, SESSION_SYMBOL = 502, 
    SERVER_SYMBOL = 503, SERVER_OPTIONS_SYMBOL = 504, SESSION_USER_SYMBOL = 505, 
    SET_SYMBOL = 506, SET_VAR_SYMBOL = 507, SHARE_SYMBOL = 508, SHOW_SYMBOL = 509, 
    SHUTDOWN_SYMBOL = 510, SIGNAL_SYMBOL = 511, SIGNED_SYMBOL = 512, SIMPLE_SYMBOL = 513, 
    SLAVE_SYMBOL = 514, SLOW_SYMBOL = 515, SMALLINT_SYMBOL = 516, SNAPSHOT_SYMBOL = 517, 
    SOME_SYMBOL = 518, SOCKET_SYMBOL = 519, SONAME_SYMBOL = 520, SOUNDS_SYMBOL = 521, 
    SOURCE_SYMBOL = 522, SPATIAL_SYMBOL = 523, SPECIFIC_SYMBOL = 524, SQLEXCEPTION_SYMBOL = 525, 
    SQLSTATE_SYMBOL = 526, SQLWARNING_SYMBOL = 527, SQL_AFTER_GTIDS_SYMBOL = 528, 
    SQL_AFTER_MTS_GAPS_SYMBOL = 529, SQL_BEFORE_GTIDS_SYMBOL = 530, SQL_BIG_RESULT_SYMBOL = 531, 
    SQL_BUFFER_RESULT_SYMBOL = 532, SQL_CACHE_SYMBOL = 533, SQL_CALC_FOUND_ROWS_SYMBOL = 534, 
    SQL_NO_CACHE_SYMBOL = 535, SQL_SMALL_RESULT_SYMBOL = 536, SQL_SYMBOL = 537, 
    SQL_THREAD_SYMBOL = 538, SSL_SYMBOL = 539, STACKED_SYMBOL = 540, STARTING_SYMBOL = 541, 
    STARTS_SYMBOL = 542, START_SYMBOL = 543, STATS_AUTO_RECALC_SYMBOL = 544, 
    STATS_PERSISTENT_SYMBOL = 545, STATS_SAMPLE_PAGES_SYMBOL = 546, STATUS_SYMBOL = 547, 
    STDDEV_SAMP_SYMBOL = 548, STDDEV_SYMBOL = 549, STDDEV_POP_SYMBOL = 550, 
    STD_SYMBOL = 551, STOP_SYMBOL = 552, STORAGE_SYMBOL = 553, STORED_SYMBOL = 554, 
    STRAIGHT_JOIN_SYMBOL = 555, STRING_SYMBOL = 556, SUBCLASS_ORIGIN_SYMBOL = 557, 
    SUBDATE_SYMBOL = 558, SUBJECT_SYMBOL = 559, SUBPARTITIONS_SYMBOL = 560, 
    SUBPARTITION_SYMBOL = 561, SUBSTR_SYMBOL = 562, SUBSTRING_SYMBOL = 563, 
    SUM_SYMBOL = 564, SUPER_SYMBOL = 565, SUSPEND_SYMBOL = 566, SWAPS_SYMBOL = 567, 
    SWITCHES_SYMBOL = 568, SYSDATE_SYMBOL = 569, SYSTEM_USER_SYMBOL = 570, 
    TABLES_SYMBOL = 571, TABLESPACE_SYMBOL = 572, TABLE_REF_PRIORITY_SYMBOL = 573, 
    TABLE_SYMBOL = 574, TABLE_CHECKSUM_SYMBOL = 575, TABLE_NAME_SYMBOL = 576, 
    TEMPORARY_SYMBOL = 577, TEMPTABLE_SYMBOL = 578, TERMINATED_SYMBOL = 579, 
    TEXT_SYMBOL = 580, THAN_SYMBOL = 581, THEN_SYMBOL = 582, TIMESTAMP_SYMBOL = 583, 
    TIMESTAMP_ADD_SYMBOL = 584, TIMESTAMP_DIFF_SYMBOL = 585, TIME_SYMBOL = 586, 
    TINYBLOB_SYMBOL = 587, TINYINT_SYMBOL = 588, TINYTEXT_SYMBOL = 589, 
    TO_SYMBOL = 590, TRAILING_SYMBOL = 591, TRANSACTION_SYMBOL = 592, TRIGGERS_SYMBOL = 593, 
    TRIGGER_SYMBOL = 594, TRIM_SYMBOL = 595, TRUE_SYMBOL = 596, TRUNCATE_SYMBOL = 597, 
    TYPES_SYMBOL = 598, TYPE_SYMBOL = 599, UDF_RETURNS_SYMBOL = 600, UNCOMMITTED_SYMBOL = 601, 
    UNDEFINED_SYMBOL = 602, UNDOFILE_SYMBOL = 603, UNDO_BUFFER_SIZE_SYMBOL = 604, 
    UNDO_SYMBOL = 605, UNICODE_SYMBOL = 606, UNINSTALL_SYMBOL = 607, UNION_SYMBOL = 608, 
    UNIQUE_SYMBOL = 609, UNKNOWN_SYMBOL = 610, UNLOCK_SYMBOL = 611, UNSIGNED_SYMBOL = 612, 
    UNTIL_SYMBOL = 613, UPDATE_SYMBOL = 614, UPGRADE_SYMBOL = 615, USAGE_SYMBOL = 616, 
    USER_RESOURCES_SYMBOL = 617, USER_SYMBOL = 618, USE_FRM_SYMBOL = 619, 
    USE_SYMBOL = 620, USING_SYMBOL = 621, UTC_DATE_SYMBOL = 622, UTC_TIMESTAMP_SYMBOL = 623, 
    UTC_TIME_SYMBOL = 624, VALIDATION_SYMBOL = 625, VALUES_SYMBOL = 626, 
    VALUE_SYMBOL = 627, VARBINARY_SYMBOL = 628, VARCHAR_SYMBOL = 629, VARCHARACTER_SYMBOL = 630, 
    VARIABLES_SYMBOL = 631, VARIANCE_SYMBOL = 632, VARYING_SYMBOL = 633, 
    VAR_POP_SYMBOL = 634, VAR_SAMP_SYMBOL = 635, VIEW_SYMBOL = 636, VIRTUAL_SYMBOL = 637, 
    WAIT_SYMBOL = 638, WARNINGS_SYMBOL = 639, WEEK_SYMBOL = 640, WEIGHT_STRING_SYMBOL = 641, 
    WHEN_SYMBOL = 642, WHERE_SYMBOL = 643, WHILE_SYMBOL = 644, WITH_SYMBOL = 645, 
    WITHOUT_SYMBOL = 646, WORK_SYMBOL = 647, WRAPPER_SYMBOL = 648, WRITE_SYMBOL = 649, 
    X509_SYMBOL = 650, XA_SYMBOL = 651, XID_SYMBOL = 652, XML_SYMBOL = 653, 
    XOR_SYMBOL = 654, YEAR_MONTH_SYMBOL = 655, YEAR_SYMBOL = 656, ZEROFILL_SYMBOL = 657, 
    PERSIST_SYMBOL = 658, ROLE_SYMBOL = 659, ADMIN_SYMBOL = 660, INVISIBLE_SYMBOL = 661, 
    VISIBLE_SYMBOL = 662, EXCEPT_SYMBOL = 663, COMPONENT_SYMBOL = 664, RECURSIVE_SYMBOL = 665, 
    JSON_OBJECTAGG_SYMBOL = 666, JSON_ARRAYAGG_SYMBOL = 667, OF_SYMBOL = 668, 
    SKIP_SYMBOL = 669, LOCKED_SYMBOL = 670, NOWAIT_SYMBOL = 671, GROUPING_SYMBOL = 672, 
    PERSIST_ONLY_SYMBOL = 673, HISTOGRAM_SYMBOL = 674, BUCKETS_SYMBOL = 675, 
    REMOTE_SYMBOL = 676, CLONE_SYMBOL = 677, CUME_DIST_SYMBOL = 678, DENSE_RANK_SYMBOL = 679, 
    EXCLUDE_SYMBOL = 680, FIRST_VALUE_SYMBOL = 681, FOLLOWING_SYMBOL = 682, 
    GROUPS_SYMBOL = 683, LAG_SYMBOL = 684, LAST_VALUE_SYMBOL = 685, LEAD_SYMBOL = 686, 
    NTH_VALUE_SYMBOL = 687, NTILE_SYMBOL = 688, NULLS_SYMBOL = 689, OTHERS_SYMBOL = 690, 
    OVER_SYMBOL = 691, PERCENT_RANK_SYMBOL = 692, PRECEDING_SYMBOL = 693, 
    RANK_SYMBOL = 694, RESPECT_SYMBOL = 695, ROW_NUMBER_SYMBOL = 696, TIES_SYMBOL = 697, 
    UNBOUNDED_SYMBOL = 698, WINDOW_SYMBOL = 699, EMPTY_SYMBOL = 700, JSON_TABLE_SYMBOL = 701, 
    NESTED_SYMBOL = 702, ORDINALITY_SYMBOL = 703, PATH_SYMBOL = 704, HISTORY_SYMBOL = 705, 
    REUSE_SYMBOL = 706, SRID_SYMBOL = 707, THREAD_PRIORITY_SYMBOL = 708, 
    RESOURCE_SYMBOL = 709, SYSTEM_SYMBOL = 710, VCPU_SYMBOL = 711, MASTER_PUBLIC_KEY_PATH_SYMBOL = 712, 
    GET_MASTER_PUBLIC_KEY_SYMBOL = 713, RESTART_SYMBOL = 714, DEFINITION_SYMBOL = 715, 
    DESCRIPTION_SYMBOL = 716, ORGANIZATION_SYMBOL = 717, REFERENCE_SYMBOL = 718, 
    OPTIONAL_SYMBOL = 719, SECONDARY_SYMBOL = 720, SECONDARY_ENGINE_SYMBOL = 721, 
    SECONDARY_LOAD_SYMBOL = 722, SECONDARY_UNLOAD_SYMBOL = 723, ACTIVE_SYMBOL = 724, 
    INACTIVE_SYMBOL = 725, LATERAL_SYMBOL = 726, RETAIN_SYMBOL = 727, OLD_SYMBOL = 728, 
    NETWORK_NAMESPACE_SYMBOL = 729, ENFORCED_SYMBOL = 730, ARRAY_SYMBOL = 731, 
    OJ_SYMBOL = 732, MEMBER_SYMBOL = 733, RANDOM_SYMBOL = 734, MASTER_COMPRESSION_ALGORITHM_SYMBOL = 735, 
    MASTER_ZSTD_COMPRESSION_LEVEL_SYMBOL = 736, PRIVILEGE_CHECKS_USER_SYMBOL = 737, 
    MASTER_TLS_CIPHERSUITES_SYMBOL = 738, REQUIRE_ROW_FORMAT_SYMBOL = 739, 
    PASSWORD_LOCK_TIME_SYMBOL = 740, FAILED_LOGIN_ATTEMPTS_SYMBOL = 741, 
    REQUIRE_TABLE_PRIMARY_KEY_CHECK_SYMBOL = 742, STREAM_SYMBOL = 743, OFF_SYMBOL = 744, 
    NOT2_SYMBOL = 745, CONCAT_PIPES_SYMBOL = 746, INT_NUMBER = 747, LONG_NUMBER = 748, 
    ULONGLONG_NUMBER = 749, EQUAL_OPERATOR = 750, ASSIGN_OPERATOR = 751, 
    NULL_SAFE_EQUAL_OPERATOR = 752, GREATER_OR_EQUAL_OPERATOR = 753, GREATER_THAN_OPERATOR = 754, 
    LESS_OR_EQUAL_OPERATOR = 755, LESS_THAN_OPERATOR = 756, NOT_EQUAL_OPERATOR = 757, 
    PLUS_OPERATOR = 758, MINUS_OPERATOR = 759, MULT_OPERATOR = 760, DIV_OPERATOR = 761, 
    MOD_OPERATOR = 762, LOGICAL_NOT_OPERATOR = 763, BITWISE_NOT_OPERATOR = 764, 
    SHIFT_LEFT_OPERATOR = 765, SHIFT_RIGHT_OPERATOR = 766, LOGICAL_AND_OPERATOR = 767, 
    BITWISE_AND_OPERATOR = 768, BITWISE_XOR_OPERATOR = 769, LOGICAL_OR_OPERATOR = 770, 
    BITWISE_OR_OPERATOR = 771, DOT_SYMBOL = 772, COMMA_SYMBOL = 773, SEMICOLON_SYMBOL = 774, 
    COLON_SYMBOL = 775, OPEN_PAR_SYMBOL = 776, CLOSE_PAR_SYMBOL = 777, OPEN_CURLY_SYMBOL = 778, 
    CLOSE_CURLY_SYMBOL = 779, UNDERLINE_SYMBOL = 780, JSON_SEPARATOR_SYMBOL = 781, 
    JSON_UNQUOTED_SEPARATOR_SYMBOL = 782, AT_SIGN_SYMBOL = 783, AT_AT_SIGN_SYMBOL = 784, 
    NULL2_SYMBOL = 785, PARAM_MARKER = 786, HEX_NUMBER = 787, BIN_NUMBER = 788, 
    DECIMAL_NUMBER = 789, FLOAT_NUMBER = 790, TIMESTAMPADD_SYMBOL = 791, 
    TIMESTAMPDIFF_SYMBOL = 792, RETURNING_SYMBOL = 793, JSON_VALUE_SYMBOL = 794, 
    TLS_SYMBOL = 795, ATTRIBUTE_SYMBOL = 796, ENGINE_ATTRIBUTE_SYMBOL = 797, 
    SECONDARY_ENGINE_ATTRIBUTE_SYMBOL = 798, SOURCE_CONNECTION_AUTO_FAILOVER_SYMBOL = 799, 
    ZONE_SYMBOL = 800, GRAMMAR_SELECTOR_DERIVED_EXPR = 801, REPLICA_SYMBOL = 802, 
    REPLICAS_SYMBOL = 803, ASSIGN_GTIDS_TO_ANONYMOUS_TRANSACTIONS_SYMBOL = 804, 
    GET_SOURCE_PUBLIC_KEY_SYMBOL = 805, SOURCE_AUTO_POSITION_SYMBOL = 806, 
    SOURCE_BIND_SYMBOL = 807, SOURCE_COMPRESSION_ALGORITHM_SYMBOL = 808, 
    SOURCE_CONNECT_RETRY_SYMBOL = 809, SOURCE_DELAY_SYMBOL = 810, SOURCE_HEARTBEAT_PERIOD_SYMBOL = 811, 
    SOURCE_HOST_SYMBOL = 812, SOURCE_LOG_FILE_SYMBOL = 813, SOURCE_LOG_POS_SYMBOL = 814, 
    SOURCE_PASSWORD_SYMBOL = 815, SOURCE_PORT_SYMBOL = 816, SOURCE_PUBLIC_KEY_PATH_SYMBOL = 817, 
    SOURCE_RETRY_COUNT_SYMBOL = 818, SOURCE_SSL_SYMBOL = 819, SOURCE_SSL_CA_SYMBOL = 820, 
    SOURCE_SSL_CAPATH_SYMBOL = 821, SOURCE_SSL_CERT_SYMBOL = 822, SOURCE_SSL_CIPHER_SYMBOL = 823, 
    SOURCE_SSL_CRL_SYMBOL = 824, SOURCE_SSL_CRLPATH_SYMBOL = 825, SOURCE_SSL_KEY_SYMBOL = 826, 
    SOURCE_SSL_VERIFY_SERVER_CERT_SYMBOL = 827, SOURCE_TLS_CIPHERSUITES_SYMBOL = 828, 
    SOURCE_TLS_VERSION_SYMBOL = 829, SOURCE_USER_SYMBOL = 830, SOURCE_ZSTD_COMPRESSION_LEVEL_SYMBOL = 831, 
    ST_COLLECT_SYMBOL = 832, KEYRING_SYMBOL = 833, AUTHENTICATION_SYMBOL = 834, 
    FACTOR_SYMBOL = 835, FINISH_SYMBOL = 836, INITIATE_SYMBOL = 837, REGISTRATION_SYMBOL = 838, 
    UNREGISTER_SYMBOL = 839, INITIAL_SYMBOL = 840, CHALLENGE_RESPONSE_SYMBOL = 841, 
    GTID_ONLY_SYMBOL = 842, WHITESPACE = 843, INVALID_INPUT = 844, UNDERSCORE_CHARSET = 845, 
    IDENTIFIER = 846, NCHAR_TEXT = 847, BACK_TICK_QUOTED_ID = 848, DOUBLE_QUOTED_TEXT = 849, 
    SINGLE_QUOTED_TEXT = 850, VERSION_COMMENT_START = 851, MYSQL_COMMENT_START = 852, 
    VERSION_COMMENT_END = 853, BLOCK_COMMENT = 854, INVALID_BLOCK_COMMENT = 855, 
    POUND_COMMENT = 856, DASHDASH_COMMENT = 857, SIMPLE_IDENTIFIER = 858, 
    NOT_EQUAL2_OPERATOR = 859
  };

  enum {
    HIDDEN_TOKEN = 1
  };

  explicit MySQLLexer(antlr4::CharStream *input);

  ~MySQLLexer() override;


  std::string getGrammarFileName() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const std::vector<std::string>& getChannelNames() const override;

  const std::vector<std::string>& getModeNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;

  const antlr4::atn::ATN& getATN() const override;

  void action(antlr4::RuleContext *context, size_t ruleIndex, size_t actionIndex) override;

  bool sempred(antlr4::RuleContext *_localctx, size_t ruleIndex, size_t predicateIndex) override;

  // By default the static state used to implement the lexer is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:

  // Individual action functions triggered by action() above.
  void LOGICAL_OR_OPERATORAction(antlr4::RuleContext *context, size_t actionIndex);
  void AT_TEXT_SUFFIXAction(antlr4::RuleContext *context, size_t actionIndex);
  void AT_AT_SIGN_SYMBOLAction(antlr4::RuleContext *context, size_t actionIndex);
  void INT_NUMBERAction(antlr4::RuleContext *context, size_t actionIndex);
  void DOT_IDENTIFIERAction(antlr4::RuleContext *context, size_t actionIndex);
  void ADDDATE_SYMBOLAction(antlr4::RuleContext *context, size_t actionIndex);
  void BIT_AND_SYMBOLAction(antlr4::RuleContext *context, size_t actionIndex);
  void BIT_OR_SYMBOLAction(antlr4::RuleContext *context, size_t actionIndex);
  void BIT_XOR_SYMBOLAction(antlr4::RuleContext *context, size_t actionIndex);
  void CAST_SYMBOLAction(antlr4::RuleContext *context, size_t actionIndex);
  void COUNT_SYMBOLAction(antlr4::RuleContext *context, size_t actionIndex);
  void CURDATE_SYMBOLAction(antlr4::RuleContext *context, size_t actionIndex);
  void CURRENT_DATE_SYMBOLAction(antlr4::RuleContext *context, size_t actionIndex);
  void CURRENT_TIME_SYMBOLAction(antlr4::RuleContext *context, size_t actionIndex);
  void CURTIME_SYMBOLAction(antlr4::RuleContext *context, size_t actionIndex);
  void DATE_ADD_SYMBOLAction(antlr4::RuleContext *context, size_t actionIndex);
  void DATE_SUB_SYMBOLAction(antlr4::RuleContext *context, size_t actionIndex);
  void EXTRACT_SYMBOLAction(antlr4::RuleContext *context, size_t actionIndex);
  void GROUP_CONCAT_SYMBOLAction(antlr4::RuleContext *context, size_t actionIndex);
  void MAX_SYMBOLAction(antlr4::RuleContext *context, size_t actionIndex);
  void MID_SYMBOLAction(antlr4::RuleContext *context, size_t actionIndex);
  void MIN_SYMBOLAction(antlr4::RuleContext *context, size_t actionIndex);
  void NOT_SYMBOLAction(antlr4::RuleContext *context, size_t actionIndex);
  void NOW_SYMBOLAction(antlr4::RuleContext *context, size_t actionIndex);
  void POSITION_SYMBOLAction(antlr4::RuleContext *context, size_t actionIndex);
  void SESSION_USER_SYMBOLAction(antlr4::RuleContext *context, size_t actionIndex);
  void STDDEV_SAMP_SYMBOLAction(antlr4::RuleContext *context, size_t actionIndex);
  void STDDEV_SYMBOLAction(antlr4::RuleContext *context, size_t actionIndex);
  void STDDEV_POP_SYMBOLAction(antlr4::RuleContext *context, size_t actionIndex);
  void STD_SYMBOLAction(antlr4::RuleContext *context, size_t actionIndex);
  void SUBDATE_SYMBOLAction(antlr4::RuleContext *context, size_t actionIndex);
  void SUBSTR_SYMBOLAction(antlr4::RuleContext *context, size_t actionIndex);
  void SUBSTRING_SYMBOLAction(antlr4::RuleContext *context, size_t actionIndex);
  void SUM_SYMBOLAction(antlr4::RuleContext *context, size_t actionIndex);
  void SYSDATE_SYMBOLAction(antlr4::RuleContext *context, size_t actionIndex);
  void SYSTEM_USER_SYMBOLAction(antlr4::RuleContext *context, size_t actionIndex);
  void TRIM_SYMBOLAction(antlr4::RuleContext *context, size_t actionIndex);
  void VARIANCE_SYMBOLAction(antlr4::RuleContext *context, size_t actionIndex);
  void VAR_POP_SYMBOLAction(antlr4::RuleContext *context, size_t actionIndex);
  void VAR_SAMP_SYMBOLAction(antlr4::RuleContext *context, size_t actionIndex);
  void UNDERSCORE_CHARSETAction(antlr4::RuleContext *context, size_t actionIndex);
  void MYSQL_COMMENT_STARTAction(antlr4::RuleContext *context, size_t actionIndex);
  void VERSION_COMMENT_ENDAction(antlr4::RuleContext *context, size_t actionIndex);

  // Individual semantic predicate functions triggered by sempred() above.
  bool JSON_SEPARATOR_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool JSON_UNQUOTED_SEPARATOR_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool ACCOUNT_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool ALWAYS_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool ANALYSE_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool BIN_NUM_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool CHANNEL_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool COMPRESSION_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool DECIMAL_NUM_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool DES_KEY_FILE_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool ENCRYPTION_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool END_OF_INPUT_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool FILE_BLOCK_SIZE_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool GENERATED_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool GROUP_REPLICATION_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool INSTANCE_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool JSON_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool LOCATOR_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool LONG_NUM_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool MASTER_TLS_VERSION_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool MAX_STATEMENT_TIME_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool NCHAR_STRING_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool NEG_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool NEVER_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool NONBLOCKING_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool OLD_PASSWORD_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool OPTIMIZER_COSTS_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool REDOFILE_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool ROTATE_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool SERVER_OPTIONS_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool SET_VAR_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool SQL_CACHE_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool STORED_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool TABLE_REF_PRIORITY_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool VALIDATION_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool VIRTUAL_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool XID_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool PERSIST_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool ROLE_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool ADMIN_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool INVISIBLE_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool VISIBLE_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool EXCEPT_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool COMPONENT_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool RECURSIVE_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool JSON_OBJECTAGG_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool JSON_ARRAYAGG_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool OF_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool SKIP_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool LOCKED_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool NOWAIT_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool GROUPING_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool PERSIST_ONLY_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool HISTOGRAM_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool BUCKETS_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool REMOTE_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool CLONE_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool CUME_DIST_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool DENSE_RANK_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool EXCLUDE_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool FIRST_VALUE_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool FOLLOWING_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool GROUPS_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool LAG_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool LAST_VALUE_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool LEAD_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool NTH_VALUE_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool NTILE_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool NULLS_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool OTHERS_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool OVER_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool PERCENT_RANK_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool PRECEDING_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool RANK_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool RESPECT_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool ROW_NUMBER_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool TIES_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool UNBOUNDED_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool WINDOW_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool EMPTY_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool JSON_TABLE_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool NESTED_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool ORDINALITY_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool PATH_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool HISTORY_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool REUSE_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool SRID_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool THREAD_PRIORITY_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool RESOURCE_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool SYSTEM_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool VCPU_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool MASTER_PUBLIC_KEY_PATH_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool GET_MASTER_PUBLIC_KEY_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool RESTART_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool DEFINITION_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool DESCRIPTION_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool ORGANIZATION_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool REFERENCE_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool OPTIONAL_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool SECONDARY_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool SECONDARY_ENGINE_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool SECONDARY_LOAD_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool SECONDARY_UNLOAD_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool ACTIVE_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool INACTIVE_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool LATERAL_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool RETAIN_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool OLD_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool NETWORK_NAMESPACE_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool ENFORCED_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool ARRAY_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool OJ_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool MEMBER_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool RANDOM_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool MASTER_COMPRESSION_ALGORITHM_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool MASTER_ZSTD_COMPRESSION_LEVEL_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool PRIVILEGE_CHECKS_USER_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool MASTER_TLS_CIPHERSUITES_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool REQUIRE_ROW_FORMAT_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool PASSWORD_LOCK_TIME_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool FAILED_LOGIN_ATTEMPTS_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool REQUIRE_TABLE_PRIMARY_KEY_CHECK_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool STREAM_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool OFF_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool RETURNING_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool JSON_VALUE_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool TLS_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool ATTRIBUTE_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool ENGINE_ATTRIBUTE_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool SECONDARY_ENGINE_ATTRIBUTE_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool SOURCE_CONNECTION_AUTO_FAILOVER_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool ZONE_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool GRAMMAR_SELECTOR_DERIVED_EXPRSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool REPLICA_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool REPLICAS_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool ASSIGN_GTIDS_TO_ANONYMOUS_TRANSACTIONS_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool GET_SOURCE_PUBLIC_KEY_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool SOURCE_AUTO_POSITION_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool SOURCE_BIND_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool SOURCE_COMPRESSION_ALGORITHM_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool SOURCE_CONNECT_RETRY_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool SOURCE_DELAY_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool SOURCE_HEARTBEAT_PERIOD_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool SOURCE_HOST_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool SOURCE_LOG_FILE_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool SOURCE_LOG_POS_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool SOURCE_PASSWORD_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool SOURCE_PORT_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool SOURCE_PUBLIC_KEY_PATH_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool SOURCE_RETRY_COUNT_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool SOURCE_SSL_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool SOURCE_SSL_CA_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool SOURCE_SSL_CAPATH_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool SOURCE_SSL_CERT_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool SOURCE_SSL_CIPHER_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool SOURCE_SSL_CRL_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool SOURCE_SSL_CRLPATH_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool SOURCE_SSL_KEY_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool SOURCE_SSL_VERIFY_SERVER_CERT_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool SOURCE_TLS_CIPHERSUITES_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool SOURCE_TLS_VERSION_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool SOURCE_USER_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool SOURCE_ZSTD_COMPRESSION_LEVEL_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool ST_COLLECT_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool KEYRING_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool AUTHENTICATION_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool FACTOR_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool FINISH_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool INITIATE_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool REGISTRATION_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool UNREGISTER_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool INITIAL_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool CHALLENGE_RESPONSE_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool GTID_ONLY_SYMBOLSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool BACK_TICK_QUOTED_IDSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool DOUBLE_QUOTED_TEXTSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool SINGLE_QUOTED_TEXTSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool VERSION_COMMENT_STARTSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool VERSION_COMMENT_ENDSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);

};

}  // namespace parsers
