#@<OUT> util help
NAME
      util - Global object that groups miscellaneous tools like upgrade checker
             and JSON import.

DESCRIPTION
      Global object that groups miscellaneous tools like upgrade checker and
      JSON import.

PROPERTIES
      debug
            Debugging and diagnostic utilities.

FUNCTIONS
      check_for_server_upgrade([connectionData][, options])
            Performs series of tests on specified MySQL server to check if the
            upgrade process will succeed.

      dump_instance(outputUrl[, options])
            Dumps the whole database to files in the output directory.

      dump_schemas(schemas, outputUrl[, options])
            Dumps the specified schemas to the files in the output directory.

      dump_tables(schema, tables, outputUrl[, options])
            Dumps the specified tables or views from the given schema to the
            files in the target directory.

      export_table(table, outputUrl[, options])
            Exports the specified table to the data dump file.

      help([member])
            Provides help about this object and it's members

      import_json(file[, options])
            Import JSON documents from file to collection or table in MySQL
            Server using X Protocol session.

      import_table(files[, options])
            Import table dump stored in files to target table using LOAD DATA
            LOCAL INFILE calls in parallel connections.

      load_dump(url[, options])
            Loads database dumps created by MySQL Shell.

#@<OUT> util check_for_server_upgrade help
NAME
      check_for_server_upgrade - Performs series of tests on specified MySQL
                                 server to check if the upgrade process will
                                 succeed.

SYNTAX
      util.check_for_server_upgrade([connectionData][, options])

WHERE
      connectionData: The connection data to server to be checked
      options: Dictionary of options to modify tool behaviour.

DESCRIPTION
      If no connectionData is specified tool will try to establish connection
      using data from current session.

      Tool behaviour can be modified with following options:

      - configPath - full path to MySQL server configuration file.
      - outputFormat - value can be either TEXT (default) or JSON.
      - targetVersion - version to which upgrade will be checked
        (default=<<<__mysh_version>>>)
      - password - password for connection.

      The connection data may be specified in the following formats:

      - A URI string
      - A dictionary with the connection options

      A basic URI string has the following format:

      [scheme://][user[:password]@]<host[:port]|socket>[/schema][?option=value&option=value...]

      Connection Options

      The following options are valid for use either in a URI or in a
      dictionary:

      - ssl-mode: The SSL mode to be used in the connection.
      - ssl-ca: The path to the X509 certificate authority file in PEM format.
      - ssl-capath: The path to the directory that contains the X509
        certificate authority files in PEM format.
      - ssl-cert: The path to the SSL public key certificate file in PEM
        format.
      - ssl-key: The path to the SSL private key file in PEM format.
      - ssl-crl: The path to file that contains certificate revocation lists.
      - ssl-crlpath: The path of directory that contains certificate revocation
        list files.
      - ssl-cipher: The list of permissible encryption ciphers for connections
        that use TLS protocols up through TLSv1.2.
      - tls-version: List of protocols permitted for secure connections.
      - tls-ciphers: List of TLS v1.3 ciphers to use.
      - auth-method: Authentication method.
      - get-server-public-key: Request public key from the server required for
        RSA key pair-based password exchange. Use when connecting to MySQL 8.0
        servers with classic MySQL sessions with SSL mode DISABLED.
      - server-public-key-path: The path name to a file containing a
        client-side copy of the public key required by the server for RSA key
        pair-based password exchange. Use when connecting to MySQL 8.0 servers
        with classic MySQL sessions with SSL mode DISABLED.
      - connect-timeout: The connection timeout in milliseconds. If not
        provided a default timeout of 10 seconds will be used. Specifying a
        value of 0 disables the connection timeout.
      - compression: Enable compression in client/server protocol.
      - compression-algorithms: Use compression algorithm in server/client
        protocol.
      - compression-level: Use this compression level in the client/server
        protocol.
      - connection-attributes: List of connection attributes to be registered
        at the PERFORMANCE_SCHEMA connection attributes tables.
      - local-infile: Enable/disable LOAD DATA LOCAL INFILE.
      - net-buffer-length: The buffer size for TCP/IP and socket communication.

      When these options are defined in a URI, their values must be URL
      encoded.

      The following options are also valid when a dictionary is used:

      Base Connection Options

      - uri: a URI string.
      - scheme: the protocol to be used on the connection.
      - user: the MySQL user name to be used on the connection.
      - dbUser: alias for user.
      - password: the password to be used on the connection.
      - dbPassword: same as password.
      - host: the hostname or IP address to be used on the connection.
      - port: the port to be used in a TCP connection.
      - socket: the socket file name to be used on a connection through unix
        sockets.
      - schema: the schema to be selected once the connection is done.

      SSH Tunnel Connection Options

      - ssh: a SSHURI string used when SSH tunnel is required.
      - ssh-password: the password the be used on the SSH connection.
      - ssh-identity-file: the key file to be used on the SSH connection.
      - ssh-identity-file-password: the SSH key file password.
      - ssh-config-file: the SSH configuration file, default is the value of
        shell.options['ssh.configFile']

      ATTENTION: The dbUser and dbPassword options are will be removed in a
                 future release.

      ATTENTION: The connection options have precedence over options specified
                 in the connection options uri

      The connection options are case insensitive and can only be defined once.

      If an option is defined more than once, an error will be generated.

      For additional information on connection data use \? connection.

#@<OUT> util dump_instance help
NAME
      dump_instance - Dumps the whole database to files in the output
                      directory.

SYNTAX
      util.dump_instance(outputUrl[, options])

WHERE
      outputUrl: Target directory to store the dump files.
      options: Dictionary with the dump options.

DESCRIPTION
      The outputUrl specifies where the dump is going to be stored.

      By default, a local directory is used, and in this case outputUrl can be
      prefixed with file:// scheme. If a relative path is given, the absolute
      path is computed as relative to the current working directory. If the
      output directory does not exist but its parent does, it is created. If
      the output directory exists, it must be empty. All directories are
      created with the following access rights (on operating systems which
      support them): rwxr-x---. All files are created with the following access
      rights (on operating systems which support them): rw-r-----.

      The following options are supported:

      - excludeSchemas: list of strings (default: empty) - List of schemas to
        be excluded from the dump.
      - includeSchemas: list of strings (default: empty) - List of schemas to
        be included in the dump.
      - excludeTables: list of strings (default: empty) - List of tables or
        views to be excluded from the dump in the format of schema.table.
      - includeTables: list of strings (default: empty) - List of tables or
        views to be included in the dump in the format of schema.table.
      - ocimds: bool (default: false) - Enable checks for compatibility with
        MySQL HeatWave Service.
      - compatibility: list of strings (default: empty) - Apply MySQL HeatWave
        Service compatibility modifications when writing dump files. Supported
        values: "create_invisible_pks", "force_innodb", "ignore_missing_pks",
        "ignore_wildcard_grants", "skip_invalid_accounts", "strip_definers",
        "strip_invalid_grants", "strip_restricted_grants", "strip_tablespaces".
      - events: bool (default: true) - Include events from each dumped schema.
      - excludeEvents: list of strings (default: empty) - List of events to be
        excluded from the dump in the format of schema.event.
      - includeEvents: list of strings (default: empty) - List of events to be
        included in the dump in the format of schema.event.
      - routines: bool (default: true) - Include functions and stored
        procedures for each dumped schema.
      - excludeRoutines: list of strings (default: empty) - List of routines to
        be excluded from the dump in the format of schema.routine.
      - includeRoutines: list of strings (default: empty) - List of routines to
        be included in the dump in the format of schema.routine.
      - users: bool (default: true) - Include users, roles and grants in the
        dump file.
      - excludeUsers: array of strings (default not set) - Skip dumping the
        specified users. Each user is in the format of 'user_name'[@'host']. If
        the host is not specified, all the accounts with the given user name
        are excluded.
      - includeUsers: array of strings (default not set) - Dump only the
        specified users. Each user is in the format of 'user_name'[@'host']. If
        the host is not specified, all the accounts with the given user name
        are included. By default, all users are included.
      - triggers: bool (default: true) - Include triggers for each dumped
        table.
      - excludeTriggers: list of strings (default: empty) - List of triggers to
        be excluded from the dump in the format of schema.table (all triggers
        from the specified table) or schema.table.trigger (the individual
        trigger).
      - includeTriggers: list of strings (default: empty) - List of triggers to
        be included in the dump in the format of schema.table (all triggers
        from the specified table) or schema.table.trigger (the individual
        trigger).
      - where: dictionary (default: not set) - A key-value pair of a table name
        in the format of schema.table and a valid SQL condition expression used
        to filter the data being exported.
      - partitions: dictionary (default: not set) - A key-value pair of a table
        name in the format of schema.table and a list of valid partition names
        used to limit the data export to just the specified partitions.
      - tzUtc: bool (default: true) - Convert TIMESTAMP data to UTC.
      - consistent: bool (default: true) - Enable or disable consistent data
        dumps.
      - skipConsistencyChecks: bool (default: false) - Skips additional
        consistency checks which are executed when running consistent dumps and
        i.e. backup lock cannot not be acquired.
      - ddlOnly: bool (default: false) - Only dump Data Definition Language
        (DDL) from the database.
      - dataOnly: bool (default: false) - Only dump data from the database.
      - dryRun: bool (default: false) - Print information about what would be
        dumped, but do not dump anything.
      - chunking: bool (default: true) - Enable chunking of the tables.
      - bytesPerChunk: string (default: "64M") - Sets average estimated number
        of bytes to be written to each chunk file, enables chunking.
      - threads: int (default: 4) - Use N threads to dump data chunks from the
        server.
      - fieldsTerminatedBy: string (default: "\t") - This option has the same
        meaning as the corresponding clause for SELECT ... INTO OUTFILE.
      - fieldsEnclosedBy: char (default: '') - This option has the same meaning
        as the corresponding clause for SELECT ... INTO OUTFILE.
      - fieldsEscapedBy: char (default: '\') - This option has the same meaning
        as the corresponding clause for SELECT ... INTO OUTFILE.
      - fieldsOptionallyEnclosed: bool (default: false) - Set to true if the
        input values are not necessarily enclosed within quotation marks
        specified by fieldsEnclosedBy option. Set to false if all fields are
        quoted by character specified by fieldsEnclosedBy option.
      - linesTerminatedBy: string (default: "\n") - This option has the same
        meaning as the corresponding clause for SELECT ... INTO OUTFILE. See
        Section 13.2.10.1, "SELECT ... INTO Statement".
      - dialect: enum (default: "default") - Setup fields and lines options
        that matches specific data file format. Can be used as base dialect and
        customized with fieldsTerminatedBy, fieldsEnclosedBy, fieldsEscapedBy,
        fieldsOptionallyEnclosed and linesTerminatedBy options. Must be one of
        the following values: default, csv, tsv or csv-unix.
      - maxRate: string (default: "0") - Limit data read throughput to maximum
        rate, measured in bytes per second per thread. Use maxRate="0" to set
        no limit.
      - showProgress: bool (default: true if stdout is a TTY device, false
        otherwise) - Enable or disable dump progress information.
      - defaultCharacterSet: string (default: "utf8mb4") - Character set used
        for the dump.
      - compression: string (default: "zstd") - Compression used when writing
        the data dump files, one of: "none", "gzip", "zstd".
      - osBucketName: string (default: not set) - Use specified OCI bucket for
        the location of the dump.
      - osNamespace: string (default: not set) - Specifies the namespace where
        the bucket is located, if not given it will be obtained using the
        tenancy id on the OCI configuration.
      - ociConfigFile: string (default: not set) - Use the specified OCI
        configuration file instead of the one at the default location.
      - ociProfile: string (default: not set) - Use the specified OCI profile
        instead of the default one.
      - ociParManifest: bool (default: not set) - Enables the generation of the
        PAR manifest while the dump operation is being executed. Deprecated.
      - ociParExpireTime: string (default: not set) - Allows defining the
        expiration time for the PARs generated when ociParManifest is enabled.
        Deprecated.
      - s3BucketName: string (default: not set) - Name of the AWS S3 bucket to
        use. The bucket must already exist.
      - s3CredentialsFile: string (default: not set) - Use the specified AWS
        credentials file.
      - s3ConfigFile: string (default: not set) - Use the specified AWS config
        file.
      - s3Profile: string (default: not set) - Use the specified AWS profile.
      - s3Region: string (default: not set) - Use the specified AWS region.
      - s3EndpointOverride: string (default: not set) - Use the specified AWS
        S3 API endpoint instead of the default one.
      - azureContainerName: string (default: not set) - Name of the Azure
        container to use. The container must already exist.
      - azureConfigFile: string (default: not set) - Use the specified Azure
        configuration file instead of the one at the default location.
      - azureStorageAccount: string (default: not set) - The account to be used
        for the operation.
      - azureStorageSasToken: string (default: not set) - Azure Shared Access
        Signature (SAS) token, to be used for the authentication of the
        operation, instead of a key.

      Requirements

      - MySQL Server 5.7 or newer is required.
      - Size limit for individual files uploaded to the OCI or AWS S3 bucket is
        1.2 TiB.
      - Columns with data types which are not safe to be stored in text form
        (i.e. BLOB) are converted to Base64, hence the size of such columns
        cannot exceed approximately 0.74 * max_allowed_packet bytes, as
        configured through that system variable at the target server.
      - Schema object names must use latin1 or utf8 character set.
      - Only tables which use the InnoDB storage engine are guaranteed to be
        dumped with consistent data.

      Details

      This operation writes SQL files per each schema, table and view dumped,
      along with some global SQL files.

      Table data dumps are written to text files using the specified file
      format, optionally splitting them into multiple chunk files.

      Requires an open, global Shell session, and uses its connection options,
      such as compression, ssl-mode, etc., to establish additional connections.

      Data dumps cannot be created for the following tables:

      - mysql.apply_status
      - mysql.general_log
      - mysql.schema
      - mysql.slow_log

      Dumps cannot be created for the following schemas:

      - information_schema,
      - mysql,
      - ndbinfo,
      - performance_schema,
      - sys.

      Options

      If the excludeSchemas or includeSchemas options contain a schema which is
      not included in the dump or does not exist, it is ignored.

      The names given in the exclude{object}, include{object}, where or
      partitions options should be valid MySQL identifiers, quoted using
      backtick characters when required.

      If the exclude{object}, include{object}, where or partitions options
      contain an object which does not exist, or an object which belongs to a
      schema which does not exist, it is ignored.

      The tzUtc option allows dumping TIMESTAMP data when a server has data in
      different time zones or data is being moved between servers with
      different time zones.

      If the consistent option is set to true, a global read lock is set using
      the FLUSH TABLES WITH READ LOCK statement, all threads establish
      connections with the server and start transactions using:

      - SET SESSION TRANSACTION ISOLATION LEVEL REPEATABLE READ
      - START TRANSACTION WITH CONSISTENT SNAPSHOT

      Once all the threads start transactions, the instance is locked for
      backup and the global read lock is released.

      If the account used for the dump does not have enough privileges to
      execute FLUSH TABLES, LOCK TABLES will be used as a fallback instead. All
      tables being dumped, in addition to DDL and GRANT related tables in the
      mysql schema will be temporarily locked.

      The ddlOnly and dataOnly options cannot both be set to true at the same
      time.

      The chunking option causes the the data from each table to be split and
      written to multiple chunk files. If this option is set to false, table
      data is written to a single file.

      If the chunking option is set to true, but a table to be dumped cannot be
      chunked (for example if it does not contain a primary key or a unique
      index), data is dumped to multiple files using a single thread.

      The value of the threads option must be a positive number.

      The dialect option predefines the set of options fieldsTerminatedBy (FT),
      fieldsEnclosedBy (FE), fieldsOptionallyEnclosed (FOE), fieldsEscapedBy
      (FESC) and linesTerminatedBy (LT) in the following manner:

      - default: no quoting, tab-separated, LF line endings. (LT=<LF>,
        FESC='\', FT=<TAB>, FE=<empty>, FOE=false)
      - csv: optionally quoted, comma-separated, CRLF line endings.
        (LT=<CR><LF>, FESC='\', FT=",", FE='"', FOE=true)
      - tsv: optionally quoted, tab-separated, CRLF line endings. (LT=<CR><LF>,
        FESC='\', FT=<TAB>, FE='"', FOE=true)
      - csv-unix: fully quoted, comma-separated, LF line endings. (LT=<LF>,
        FESC='\', FT=",", FE='"', FOE=false)

      Both the bytesPerChunk and maxRate options support unit suffixes:

      - k - for kilobytes,
      - M - for Megabytes,
      - G - for Gigabytes,

      i.e. maxRate="2k" - limit throughput to 2000 bytes per second.

      The value of the bytesPerChunk option cannot be smaller than "128k".

      MySQL HeatWave Service Compatibility

      The MySQL HeatWave Service has a few security related restrictions that
      are not present in a regular, on-premise instance of MySQL. In order to
      make it easier to load existing databases into the Service, the dump
      commands in the MySQL Shell has options to detect potential issues and in
      some cases, to automatically adjust your schema definition to be
      compliant. For best results, always use the latest available version of
      MySQL Shell.

      The ocimds option, when set to true, will perform schema checks for most
      of these issues and abort the dump if any are found. The load_dump()
      command will also only allow loading dumps that have been created with
      the "ocimds" option enabled.

      Some issues found by the ocimds option may require you to manually make
      changes to your database schema before it can be loaded into the MySQL
      HeatWave Service. However, the compatibility option can be used to
      automatically modify the dumped schema SQL scripts, resolving some of
      these compatibility issues. You may pass one or more of the following
      values to the "compatibility" option.

      create_invisible_pks - Each table which does not have a Primary Key will
      have one created when the dump is loaded. The following Primary Key is
      added to the table:
      `my_row_id` BIGINT UNSIGNED AUTO_INCREMENT INVISIBLE PRIMARY KEY

      Dumps created with this value can be used with Inbound Replication into
      an MySQL HeatWave Service DB System instance with High Availability, as
      long as target instance has version 8.0.32 or newer. Mutually exclusive
      with the ignore_missing_pks value.

      force_innodb - The MySQL HeatWave Service requires use of the InnoDB
      storage engine. This option will modify the ENGINE= clause of CREATE
      TABLE statements that use incompatible storage engines and replace them
      with InnoDB. It will also remove the ROW_FORMAT=FIXED option, as it is
      not supported by the InnoDB storage engine.

      ignore_missing_pks - Ignore errors caused by tables which do not have
      Primary Keys. Dumps created with this value cannot be used in MySQL
      HeatWave Service DB System instance with High Availability. Mutually
      exclusive with the create_invisible_pks value.

      ignore_wildcard_grants - Ignore errors from grants on schemas with
      wildcards, which are interpreted differently in systems where
      partial_revokes system variable is enabled.

      skip_invalid_accounts - Skips accounts which do not have a password or
      use authentication methods (plugins) not supported by the MySQL HeatWave
      Service.

      strip_definers - Strips the "DEFINER=account" clause from views,
      routines, events and triggers. The MySQL HeatWave Service requires
      special privileges to create these objects with a definer other than the
      user loading the schema. By stripping the DEFINER clause, these objects
      will be created with that default definer. Views and routines will
      additionally have their SQL SECURITY clause changed from DEFINER to
      INVOKER. If this characteristic is missing, SQL SECURITY INVOKER clause
      will be added. This ensures that the access permissions of the account
      querying or calling these are applied, instead of the user that created
      them. This should be sufficient for most users, but if your database
      security model requires that views and routines have more privileges than
      their invoker, you will need to manually modify the schema before loading
      it.

      Please refer to the MySQL manual for details about DEFINER and SQL
      SECURITY.

      strip_invalid_grants - Strips grant statements which would fail when
      users are loaded, i.e. grants referring to a specific routine which does
      not exist.

      strip_restricted_grants - Certain privileges are restricted in the MySQL
      HeatWave Service. Attempting to create users granting these privileges
      would fail, so this option allows dumped GRANT statements to be stripped
      of these privileges.

      strip_tablespaces - Tablespaces have some restrictions in the MySQL
      HeatWave Service. If you'd like to have tables created in their default
      tablespaces, this option will strip the TABLESPACE= option from CREATE
      TABLE statements.

      Additionally, the following changes will always be made to DDL scripts
      when the ocimds option is enabled:

      - DATA DIRECTORY, INDEX DIRECTORY and ENCRYPTION options in CREATE TABLE
        statements will be commented out.

      In order to use Inbound Replication into an MySQL HeatWave Service DB
      System instance with High Availability where instance has version older
      than 8.0.32, all tables at the source server need to have Primary Keys.
      This needs to be fixed manually before running the dump. Starting with
      MySQL 8.0.23 invisible columns may be used to add Primary Keys without
      changing the schema compatibility, for more information see:
      https://dev.mysql.com/doc/refman/en/invisible-columns.html.

      In order to use Inbound Replication into an MySQL HeatWave Service DB
      System instance with High Availability, please see
      https://docs.oracle.com/en-us/iaas/mysql-database/doc/creating-replication-channel.html.

      In order to use MySQL HeatWave Service DB Service instance with High
      Availability, all tables must have a Primary Key. This can be fixed
      automatically using the create_invisible_pks compatibility value.

      Please refer to the MySQL HeatWave Service documentation for more
      information about restrictions and compatibility.

      Dumping to a Bucket in the OCI Object Storage

      If the osBucketName option is used, the dump is stored in the specified
      OCI bucket, connection is established using the local OCI profile. The
      directory structure is simulated within the object name.

      The osNamespace, ociConfigFile and ociProfile options cannot be used if
      the osBucketName option is set to an empty string.

      The osNamespace option overrides the OCI namespace obtained based on the
      tenancy ID from the local OCI profile.

      Enabling dump loading using pre-authenticated requests

      The load_dump utility supports loading a dump using a pre-authenticated
      request (PAR). The simplest way to do this is by providing a PAR to the
      location of the dump in a bucket, the PAR must be created with the
      following permissions:

      - Permits object reads
      - Enables object listing

      The generated URL can be used to load the dump, see \? load_dump for more
      details.

      ATTENTION: The ociParManifest and ociParExpireTime options described
                 below are deprecated and will be removed in a future release.

      Another way to enable loading a dump without requiring an OCI Profile, is
      to execute the dump operations enabling the ociParManifest option which
      will cause the dump operation automatically generates a PAR for every
      file in the dump, and will store them as part of the dump in a file named
      "@.manifest.json". The manifest is updated as the dump operation
      progresses.

      Using a PAR with permissions to read the manifest is another option to
      load the dump using PAR.

      The ociParManifest option cannot be used if osBucketName is not set.

      When creating PARs, an expiration time is required, it can be defined
      through the ociParExpireTime option. If the option is not used, a
      predefined expiration time will be used equivalent to a week after the
      dump operation started. The values assigned to this option should be
      conformant to RFC3339.

      The ociParExpireTime option cannot be used if the ociParManifest option
      is not enabled.

      Dumping to a Bucket in the AWS S3 Object Storage

      If the s3BucketName option is used, the dump is stored in the specified
      AWS S3 bucket. Connection is established using default local AWS
      configuration paths and profiles, unless overridden. The directory
      structure is simulated within the object name.

      The s3CredentialsFile, s3ConfigFile, s3Profile, s3Region and
      s3EndpointOverride options cannot be used if the s3BucketName option is
      not set or set to an empty string.

      Handling of the AWS settings

      The AWS options are evaluated in the order of precedence, the first
      available value is used.

      1. Name of the AWS profile:

      - the s3Profile option
      - the AWS_PROFILE environment variable
      - the AWS_DEFAULT_PROFILE environment variable
      - the default value of default

      2. Location of the credentials file:

      - the s3CredentialsFile option
      - the AWS_SHARED_CREDENTIALS_FILE environment variable
      - the default value of ~/.aws/credentials

      3. Location of the config file:

      - the s3ConfigFile option
      - the AWS_CONFIG_FILE environment variable
      - the default value of ~/.aws/config

      4. Name of the AWS region:

      - the s3Region option
      - the AWS_REGION environment variable
      - the AWS_DEFAULT_REGION environment variable
      - the region setting from the config file for the specified profile
      - the default value of us-east-1

      5. URI of AWS S3 API endpoint

      - the s3EndpointOverride option
      - the default value of https://<s3BucketName>.s3.<region>.amazonaws.com

      The AWS credentials are fetched from the following providers, in the
      order of precedence:

      1. Environment variables:

      - AWS_ACCESS_KEY_ID
      - AWS_SECRET_ACCESS_KEY
      - AWS_SESSION_TOKEN

      2. Settings from the credentials file for the specified profile:

      - aws_access_key_id
      - aws_secret_access_key
      - aws_session_token

      3. Process specified by the credential_process setting from the config
         file for the specified profile:

      - AccessKeyId
      - SecretAccessKey
      - SessionToken

      4. Settings from the config file for the specified profile:

      - aws_access_key_id
      - aws_secret_access_key
      - aws_session_token

      The items specified above correspond to the following credentials:

      - the AWS access key
      - the secret key associated with the AWS access key
      - the AWS session token for the temporary security credentials

      The process/command line specified by the credential_process setting must
      write a JSON object to the standard output in the following form:
      {
        "Version": 1,
        "AccessKeyId": "AWS access key",
        "SecretAccessKey": "secret key associated with the AWS access key",
        "SessionToken": "temporary AWS session token, optional",
        "Expiration": "RFC3339 timestamp, optional"
      }

      The Expiration key, if given, specifies when the credentials are going to
      expire, they will be automatically refreshed before this happens.

      The following credential handling rules apply:

      - If the s3Profile option is set to a non-empty string, the environment
        variables are not used as a potential credential provider.
      - If either an access key or a secret key is available in a potential
        credential provider, it is selected as the credential provider.
      - If either the access key or the secret key is missing in the selected
        credential provider, an exception is thrown.
      - If the session token is missing in the selected credential provider, or
        if it is set to an empty string, it is not used to authenticate the
        user.

      Dumping to a Container in the Azure Blob Storage

      If the azureContainerName option is used, the dump is stored in the
      specified Azure container. Connection is established using the
      configuration at the local Azure configuration file.The directory
      structure is simulated within the blob name.

      The azureConfigFile option cannot be used if the azureContainerName
      option is not set or set to an empty string.

      Handling of the Azure settings

      1. The following settings are read from the storage section in the config
         file:

      - connection_string
      - account
      - key
      - sas_token

      Additionally, the connection options may be defined using the standard
      Azure environment variables:

      - AZURE_STORAGE_CONNECTION_STRING
      - AZURE_STORAGE_ACCOUNT
      - AZURE_STORAGE_KEY
      - AZURE_STORAGE_SAS_TOKEN

      The Azure configuration values are evaluated in the following precedence:

      - Options parameter - Environment Variables - Configuration File

      If a connection string is defined either case in the environment variable
      or the configuration option, the individual configuration values for
      account and key will be ignored.

      If a SAS Token is defined, it will be used for the authorization
      (ignoring any defined account key).

      The default Azure Blob Endpoint to be used in the operations is defined
      by:

      https://<account>.blob.core.windows.net

      Unless a different EndPoint is defined in the connection string.

EXCEPTIONS
      ArgumentError in the following scenarios:

      - If any of the input arguments contains an invalid value.

      RuntimeError in the following scenarios:

      - If there is no open global session.
      - If creating the output directory fails.
      - If creating or writing to the output file fails.

#@<OUT> util dump_schemas help
NAME
      dump_schemas - Dumps the specified schemas to the files in the output
                     directory.

SYNTAX
      util.dump_schemas(schemas, outputUrl[, options])

WHERE
      schemas: List of schemas to be dumped.
      outputUrl: Target directory to store the dump files.
      options: Dictionary with the dump options.

DESCRIPTION
      The schemas parameter cannot be an empty list.

      The outputUrl specifies where the dump is going to be stored.

      By default, a local directory is used, and in this case outputUrl can be
      prefixed with file:// scheme. If a relative path is given, the absolute
      path is computed as relative to the current working directory. If the
      output directory does not exist but its parent does, it is created. If
      the output directory exists, it must be empty. All directories are
      created with the following access rights (on operating systems which
      support them): rwxr-x---. All files are created with the following access
      rights (on operating systems which support them): rw-r-----.

      The following options are supported:

      - excludeTables: list of strings (default: empty) - List of tables or
        views to be excluded from the dump in the format of schema.table.
      - includeTables: list of strings (default: empty) - List of tables or
        views to be included in the dump in the format of schema.table.
      - ocimds: bool (default: false) - Enable checks for compatibility with
        MySQL HeatWave Service.
      - compatibility: list of strings (default: empty) - Apply MySQL HeatWave
        Service compatibility modifications when writing dump files. Supported
        values: "create_invisible_pks", "force_innodb", "ignore_missing_pks",
        "ignore_wildcard_grants", "skip_invalid_accounts", "strip_definers",
        "strip_invalid_grants", "strip_restricted_grants", "strip_tablespaces".
      - events: bool (default: true) - Include events from each dumped schema.
      - excludeEvents: list of strings (default: empty) - List of events to be
        excluded from the dump in the format of schema.event.
      - includeEvents: list of strings (default: empty) - List of events to be
        included in the dump in the format of schema.event.
      - routines: bool (default: true) - Include functions and stored
        procedures for each dumped schema.
      - excludeRoutines: list of strings (default: empty) - List of routines to
        be excluded from the dump in the format of schema.routine.
      - includeRoutines: list of strings (default: empty) - List of routines to
        be included in the dump in the format of schema.routine.
      - triggers: bool (default: true) - Include triggers for each dumped
        table.
      - excludeTriggers: list of strings (default: empty) - List of triggers to
        be excluded from the dump in the format of schema.table (all triggers
        from the specified table) or schema.table.trigger (the individual
        trigger).
      - includeTriggers: list of strings (default: empty) - List of triggers to
        be included in the dump in the format of schema.table (all triggers
        from the specified table) or schema.table.trigger (the individual
        trigger).
      - where: dictionary (default: not set) - A key-value pair of a table name
        in the format of schema.table and a valid SQL condition expression used
        to filter the data being exported.
      - partitions: dictionary (default: not set) - A key-value pair of a table
        name in the format of schema.table and a list of valid partition names
        used to limit the data export to just the specified partitions.
      - tzUtc: bool (default: true) - Convert TIMESTAMP data to UTC.
      - consistent: bool (default: true) - Enable or disable consistent data
        dumps.
      - skipConsistencyChecks: bool (default: false) - Skips additional
        consistency checks which are executed when running consistent dumps and
        i.e. backup lock cannot not be acquired.
      - ddlOnly: bool (default: false) - Only dump Data Definition Language
        (DDL) from the database.
      - dataOnly: bool (default: false) - Only dump data from the database.
      - dryRun: bool (default: false) - Print information about what would be
        dumped, but do not dump anything.
      - chunking: bool (default: true) - Enable chunking of the tables.
      - bytesPerChunk: string (default: "64M") - Sets average estimated number
        of bytes to be written to each chunk file, enables chunking.
      - threads: int (default: 4) - Use N threads to dump data chunks from the
        server.
      - fieldsTerminatedBy: string (default: "\t") - This option has the same
        meaning as the corresponding clause for SELECT ... INTO OUTFILE.
      - fieldsEnclosedBy: char (default: '') - This option has the same meaning
        as the corresponding clause for SELECT ... INTO OUTFILE.
      - fieldsEscapedBy: char (default: '\') - This option has the same meaning
        as the corresponding clause for SELECT ... INTO OUTFILE.
      - fieldsOptionallyEnclosed: bool (default: false) - Set to true if the
        input values are not necessarily enclosed within quotation marks
        specified by fieldsEnclosedBy option. Set to false if all fields are
        quoted by character specified by fieldsEnclosedBy option.
      - linesTerminatedBy: string (default: "\n") - This option has the same
        meaning as the corresponding clause for SELECT ... INTO OUTFILE. See
        Section 13.2.10.1, "SELECT ... INTO Statement".
      - dialect: enum (default: "default") - Setup fields and lines options
        that matches specific data file format. Can be used as base dialect and
        customized with fieldsTerminatedBy, fieldsEnclosedBy, fieldsEscapedBy,
        fieldsOptionallyEnclosed and linesTerminatedBy options. Must be one of
        the following values: default, csv, tsv or csv-unix.
      - maxRate: string (default: "0") - Limit data read throughput to maximum
        rate, measured in bytes per second per thread. Use maxRate="0" to set
        no limit.
      - showProgress: bool (default: true if stdout is a TTY device, false
        otherwise) - Enable or disable dump progress information.
      - defaultCharacterSet: string (default: "utf8mb4") - Character set used
        for the dump.
      - compression: string (default: "zstd") - Compression used when writing
        the data dump files, one of: "none", "gzip", "zstd".
      - osBucketName: string (default: not set) - Use specified OCI bucket for
        the location of the dump.
      - osNamespace: string (default: not set) - Specifies the namespace where
        the bucket is located, if not given it will be obtained using the
        tenancy id on the OCI configuration.
      - ociConfigFile: string (default: not set) - Use the specified OCI
        configuration file instead of the one at the default location.
      - ociProfile: string (default: not set) - Use the specified OCI profile
        instead of the default one.
      - ociParManifest: bool (default: not set) - Enables the generation of the
        PAR manifest while the dump operation is being executed. Deprecated.
      - ociParExpireTime: string (default: not set) - Allows defining the
        expiration time for the PARs generated when ociParManifest is enabled.
        Deprecated.
      - s3BucketName: string (default: not set) - Name of the AWS S3 bucket to
        use. The bucket must already exist.
      - s3CredentialsFile: string (default: not set) - Use the specified AWS
        credentials file.
      - s3ConfigFile: string (default: not set) - Use the specified AWS config
        file.
      - s3Profile: string (default: not set) - Use the specified AWS profile.
      - s3Region: string (default: not set) - Use the specified AWS region.
      - s3EndpointOverride: string (default: not set) - Use the specified AWS
        S3 API endpoint instead of the default one.
      - azureContainerName: string (default: not set) - Name of the Azure
        container to use. The container must already exist.
      - azureConfigFile: string (default: not set) - Use the specified Azure
        configuration file instead of the one at the default location.
      - azureStorageAccount: string (default: not set) - The account to be used
        for the operation.
      - azureStorageSasToken: string (default: not set) - Azure Shared Access
        Signature (SAS) token, to be used for the authentication of the
        operation, instead of a key.

      Requirements

      - MySQL Server 5.7 or newer is required.
      - Size limit for individual files uploaded to the OCI or AWS S3 bucket is
        1.2 TiB.
      - Columns with data types which are not safe to be stored in text form
        (i.e. BLOB) are converted to Base64, hence the size of such columns
        cannot exceed approximately 0.74 * max_allowed_packet bytes, as
        configured through that system variable at the target server.
      - Schema object names must use latin1 or utf8 character set.
      - Only tables which use the InnoDB storage engine are guaranteed to be
        dumped with consistent data.

      Details

      This operation writes SQL files per each schema, table and view dumped,
      along with some global SQL files.

      Table data dumps are written to text files using the specified file
      format, optionally splitting them into multiple chunk files.

      Requires an open, global Shell session, and uses its connection options,
      such as compression, ssl-mode, etc., to establish additional connections.

      Data dumps cannot be created for the following tables:

      - mysql.apply_status
      - mysql.general_log
      - mysql.schema
      - mysql.slow_log

      Options

      The names given in the exclude{object}, include{object}, where or
      partitions options should be valid MySQL identifiers, quoted using
      backtick characters when required.

      If the exclude{object}, include{object}, where or partitions options
      contain an object which does not exist, or an object which belongs to a
      schema which does not exist, it is ignored.

      The tzUtc option allows dumping TIMESTAMP data when a server has data in
      different time zones or data is being moved between servers with
      different time zones.

      If the consistent option is set to true, a global read lock is set using
      the FLUSH TABLES WITH READ LOCK statement, all threads establish
      connections with the server and start transactions using:

      - SET SESSION TRANSACTION ISOLATION LEVEL REPEATABLE READ
      - START TRANSACTION WITH CONSISTENT SNAPSHOT

      Once all the threads start transactions, the instance is locked for
      backup and the global read lock is released.

      If the account used for the dump does not have enough privileges to
      execute FLUSH TABLES, LOCK TABLES will be used as a fallback instead. All
      tables being dumped, in addition to DDL and GRANT related tables in the
      mysql schema will be temporarily locked.

      The ddlOnly and dataOnly options cannot both be set to true at the same
      time.

      The chunking option causes the the data from each table to be split and
      written to multiple chunk files. If this option is set to false, table
      data is written to a single file.

      If the chunking option is set to true, but a table to be dumped cannot be
      chunked (for example if it does not contain a primary key or a unique
      index), data is dumped to multiple files using a single thread.

      The value of the threads option must be a positive number.

      The dialect option predefines the set of options fieldsTerminatedBy (FT),
      fieldsEnclosedBy (FE), fieldsOptionallyEnclosed (FOE), fieldsEscapedBy
      (FESC) and linesTerminatedBy (LT) in the following manner:

      - default: no quoting, tab-separated, LF line endings. (LT=<LF>,
        FESC='\', FT=<TAB>, FE=<empty>, FOE=false)
      - csv: optionally quoted, comma-separated, CRLF line endings.
        (LT=<CR><LF>, FESC='\', FT=",", FE='"', FOE=true)
      - tsv: optionally quoted, tab-separated, CRLF line endings. (LT=<CR><LF>,
        FESC='\', FT=<TAB>, FE='"', FOE=true)
      - csv-unix: fully quoted, comma-separated, LF line endings. (LT=<LF>,
        FESC='\', FT=",", FE='"', FOE=false)

      Both the bytesPerChunk and maxRate options support unit suffixes:

      - k - for kilobytes,
      - M - for Megabytes,
      - G - for Gigabytes,

      i.e. maxRate="2k" - limit throughput to 2000 bytes per second.

      The value of the bytesPerChunk option cannot be smaller than "128k".

      MySQL HeatWave Service Compatibility

      The MySQL HeatWave Service has a few security related restrictions that
      are not present in a regular, on-premise instance of MySQL. In order to
      make it easier to load existing databases into the Service, the dump
      commands in the MySQL Shell has options to detect potential issues and in
      some cases, to automatically adjust your schema definition to be
      compliant. For best results, always use the latest available version of
      MySQL Shell.

      The ocimds option, when set to true, will perform schema checks for most
      of these issues and abort the dump if any are found. The load_dump()
      command will also only allow loading dumps that have been created with
      the "ocimds" option enabled.

      Some issues found by the ocimds option may require you to manually make
      changes to your database schema before it can be loaded into the MySQL
      HeatWave Service. However, the compatibility option can be used to
      automatically modify the dumped schema SQL scripts, resolving some of
      these compatibility issues. You may pass one or more of the following
      values to the "compatibility" option.

      create_invisible_pks - Each table which does not have a Primary Key will
      have one created when the dump is loaded. The following Primary Key is
      added to the table:
      `my_row_id` BIGINT UNSIGNED AUTO_INCREMENT INVISIBLE PRIMARY KEY

      Dumps created with this value can be used with Inbound Replication into
      an MySQL HeatWave Service DB System instance with High Availability, as
      long as target instance has version 8.0.32 or newer. Mutually exclusive
      with the ignore_missing_pks value.

      force_innodb - The MySQL HeatWave Service requires use of the InnoDB
      storage engine. This option will modify the ENGINE= clause of CREATE
      TABLE statements that use incompatible storage engines and replace them
      with InnoDB. It will also remove the ROW_FORMAT=FIXED option, as it is
      not supported by the InnoDB storage engine.

      ignore_missing_pks - Ignore errors caused by tables which do not have
      Primary Keys. Dumps created with this value cannot be used in MySQL
      HeatWave Service DB System instance with High Availability. Mutually
      exclusive with the create_invisible_pks value.

      ignore_wildcard_grants - Ignore errors from grants on schemas with
      wildcards, which are interpreted differently in systems where
      partial_revokes system variable is enabled.

      skip_invalid_accounts - Skips accounts which do not have a password or
      use authentication methods (plugins) not supported by the MySQL HeatWave
      Service.

      strip_definers - Strips the "DEFINER=account" clause from views,
      routines, events and triggers. The MySQL HeatWave Service requires
      special privileges to create these objects with a definer other than the
      user loading the schema. By stripping the DEFINER clause, these objects
      will be created with that default definer. Views and routines will
      additionally have their SQL SECURITY clause changed from DEFINER to
      INVOKER. If this characteristic is missing, SQL SECURITY INVOKER clause
      will be added. This ensures that the access permissions of the account
      querying or calling these are applied, instead of the user that created
      them. This should be sufficient for most users, but if your database
      security model requires that views and routines have more privileges than
      their invoker, you will need to manually modify the schema before loading
      it.

      Please refer to the MySQL manual for details about DEFINER and SQL
      SECURITY.

      strip_invalid_grants - Strips grant statements which would fail when
      users are loaded, i.e. grants referring to a specific routine which does
      not exist.

      strip_restricted_grants - Certain privileges are restricted in the MySQL
      HeatWave Service. Attempting to create users granting these privileges
      would fail, so this option allows dumped GRANT statements to be stripped
      of these privileges.

      strip_tablespaces - Tablespaces have some restrictions in the MySQL
      HeatWave Service. If you'd like to have tables created in their default
      tablespaces, this option will strip the TABLESPACE= option from CREATE
      TABLE statements.

      Additionally, the following changes will always be made to DDL scripts
      when the ocimds option is enabled:

      - DATA DIRECTORY, INDEX DIRECTORY and ENCRYPTION options in CREATE TABLE
        statements will be commented out.

      In order to use Inbound Replication into an MySQL HeatWave Service DB
      System instance with High Availability where instance has version older
      than 8.0.32, all tables at the source server need to have Primary Keys.
      This needs to be fixed manually before running the dump. Starting with
      MySQL 8.0.23 invisible columns may be used to add Primary Keys without
      changing the schema compatibility, for more information see:
      https://dev.mysql.com/doc/refman/en/invisible-columns.html.

      In order to use Inbound Replication into an MySQL HeatWave Service DB
      System instance with High Availability, please see
      https://docs.oracle.com/en-us/iaas/mysql-database/doc/creating-replication-channel.html.

      In order to use MySQL HeatWave Service DB Service instance with High
      Availability, all tables must have a Primary Key. This can be fixed
      automatically using the create_invisible_pks compatibility value.

      Please refer to the MySQL HeatWave Service documentation for more
      information about restrictions and compatibility.

      Dumping to a Bucket in the OCI Object Storage

      If the osBucketName option is used, the dump is stored in the specified
      OCI bucket, connection is established using the local OCI profile. The
      directory structure is simulated within the object name.

      The osNamespace, ociConfigFile and ociProfile options cannot be used if
      the osBucketName option is set to an empty string.

      The osNamespace option overrides the OCI namespace obtained based on the
      tenancy ID from the local OCI profile.

      Enabling dump loading using pre-authenticated requests

      The load_dump utility supports loading a dump using a pre-authenticated
      request (PAR). The simplest way to do this is by providing a PAR to the
      location of the dump in a bucket, the PAR must be created with the
      following permissions:

      - Permits object reads
      - Enables object listing

      The generated URL can be used to load the dump, see \? load_dump for more
      details.

      ATTENTION: The ociParManifest and ociParExpireTime options described
                 below are deprecated and will be removed in a future release.

      Another way to enable loading a dump without requiring an OCI Profile, is
      to execute the dump operations enabling the ociParManifest option which
      will cause the dump operation automatically generates a PAR for every
      file in the dump, and will store them as part of the dump in a file named
      "@.manifest.json". The manifest is updated as the dump operation
      progresses.

      Using a PAR with permissions to read the manifest is another option to
      load the dump using PAR.

      The ociParManifest option cannot be used if osBucketName is not set.

      When creating PARs, an expiration time is required, it can be defined
      through the ociParExpireTime option. If the option is not used, a
      predefined expiration time will be used equivalent to a week after the
      dump operation started. The values assigned to this option should be
      conformant to RFC3339.

      The ociParExpireTime option cannot be used if the ociParManifest option
      is not enabled.

      Dumping to a Bucket in the AWS S3 Object Storage

      If the s3BucketName option is used, the dump is stored in the specified
      AWS S3 bucket. Connection is established using default local AWS
      configuration paths and profiles, unless overridden. The directory
      structure is simulated within the object name.

      The s3CredentialsFile, s3ConfigFile, s3Profile, s3Region and
      s3EndpointOverride options cannot be used if the s3BucketName option is
      not set or set to an empty string.

      Handling of the AWS settings

      The AWS options are evaluated in the order of precedence, the first
      available value is used.

      1. Name of the AWS profile:

      - the s3Profile option
      - the AWS_PROFILE environment variable
      - the AWS_DEFAULT_PROFILE environment variable
      - the default value of default

      2. Location of the credentials file:

      - the s3CredentialsFile option
      - the AWS_SHARED_CREDENTIALS_FILE environment variable
      - the default value of ~/.aws/credentials

      3. Location of the config file:

      - the s3ConfigFile option
      - the AWS_CONFIG_FILE environment variable
      - the default value of ~/.aws/config

      4. Name of the AWS region:

      - the s3Region option
      - the AWS_REGION environment variable
      - the AWS_DEFAULT_REGION environment variable
      - the region setting from the config file for the specified profile
      - the default value of us-east-1

      5. URI of AWS S3 API endpoint

      - the s3EndpointOverride option
      - the default value of https://<s3BucketName>.s3.<region>.amazonaws.com

      The AWS credentials are fetched from the following providers, in the
      order of precedence:

      1. Environment variables:

      - AWS_ACCESS_KEY_ID
      - AWS_SECRET_ACCESS_KEY
      - AWS_SESSION_TOKEN

      2. Settings from the credentials file for the specified profile:

      - aws_access_key_id
      - aws_secret_access_key
      - aws_session_token

      3. Process specified by the credential_process setting from the config
         file for the specified profile:

      - AccessKeyId
      - SecretAccessKey
      - SessionToken

      4. Settings from the config file for the specified profile:

      - aws_access_key_id
      - aws_secret_access_key
      - aws_session_token

      The items specified above correspond to the following credentials:

      - the AWS access key
      - the secret key associated with the AWS access key
      - the AWS session token for the temporary security credentials

      The process/command line specified by the credential_process setting must
      write a JSON object to the standard output in the following form:
      {
        "Version": 1,
        "AccessKeyId": "AWS access key",
        "SecretAccessKey": "secret key associated with the AWS access key",
        "SessionToken": "temporary AWS session token, optional",
        "Expiration": "RFC3339 timestamp, optional"
      }

      The Expiration key, if given, specifies when the credentials are going to
      expire, they will be automatically refreshed before this happens.

      The following credential handling rules apply:

      - If the s3Profile option is set to a non-empty string, the environment
        variables are not used as a potential credential provider.
      - If either an access key or a secret key is available in a potential
        credential provider, it is selected as the credential provider.
      - If either the access key or the secret key is missing in the selected
        credential provider, an exception is thrown.
      - If the session token is missing in the selected credential provider, or
        if it is set to an empty string, it is not used to authenticate the
        user.

      Dumping to a Container in the Azure Blob Storage

      If the azureContainerName option is used, the dump is stored in the
      specified Azure container. Connection is established using the
      configuration at the local Azure configuration file.The directory
      structure is simulated within the blob name.

      The azureConfigFile option cannot be used if the azureContainerName
      option is not set or set to an empty string.

      Handling of the Azure settings

      1. The following settings are read from the storage section in the config
         file:

      - connection_string
      - account
      - key
      - sas_token

      Additionally, the connection options may be defined using the standard
      Azure environment variables:

      - AZURE_STORAGE_CONNECTION_STRING
      - AZURE_STORAGE_ACCOUNT
      - AZURE_STORAGE_KEY
      - AZURE_STORAGE_SAS_TOKEN

      The Azure configuration values are evaluated in the following precedence:

      - Options parameter - Environment Variables - Configuration File

      If a connection string is defined either case in the environment variable
      or the configuration option, the individual configuration values for
      account and key will be ignored.

      If a SAS Token is defined, it will be used for the authorization
      (ignoring any defined account key).

      The default Azure Blob Endpoint to be used in the operations is defined
      by:

      https://<account>.blob.core.windows.net

      Unless a different EndPoint is defined in the connection string.

EXCEPTIONS
      ArgumentError in the following scenarios:

      - If any of the input arguments contains an invalid value.

      RuntimeError in the following scenarios:

      - If there is no open global session.
      - If creating the output directory fails.
      - If creating or writing to the output file fails.

#@<OUT> util dump_tables help
NAME
      dump_tables - Dumps the specified tables or views from the given schema
                    to the files in the target directory.

SYNTAX
      util.dump_tables(schema, tables, outputUrl[, options])

WHERE
      schema: Name of the schema that contains tables/views to be dumped.
      tables: List of tables/views to be dumped.
      outputUrl: Target directory to store the dump files.
      options: Dictionary with the dump options.

DESCRIPTION
      The tables parameter cannot be an empty list.

      The outputUrl specifies where the dump is going to be stored.

      By default, a local directory is used, and in this case outputUrl can be
      prefixed with file:// scheme. If a relative path is given, the absolute
      path is computed as relative to the current working directory. If the
      output directory does not exist but its parent does, it is created. If
      the output directory exists, it must be empty. All directories are
      created with the following access rights (on operating systems which
      support them): rwxr-x---. All files are created with the following access
      rights (on operating systems which support them): rw-r-----.

      The following options are supported:

      - all: bool (default: false) - Dump all views and tables from the
        specified schema.
      - ocimds: bool (default: false) - Enable checks for compatibility with
        MySQL HeatWave Service.
      - compatibility: list of strings (default: empty) - Apply MySQL HeatWave
        Service compatibility modifications when writing dump files. Supported
        values: "create_invisible_pks", "force_innodb", "ignore_missing_pks",
        "ignore_wildcard_grants", "skip_invalid_accounts", "strip_definers",
        "strip_invalid_grants", "strip_restricted_grants", "strip_tablespaces".
      - triggers: bool (default: true) - Include triggers for each dumped
        table.
      - excludeTriggers: list of strings (default: empty) - List of triggers to
        be excluded from the dump in the format of schema.table (all triggers
        from the specified table) or schema.table.trigger (the individual
        trigger).
      - includeTriggers: list of strings (default: empty) - List of triggers to
        be included in the dump in the format of schema.table (all triggers
        from the specified table) or schema.table.trigger (the individual
        trigger).
      - where: dictionary (default: not set) - A key-value pair of a table name
        in the format of schema.table and a valid SQL condition expression used
        to filter the data being exported.
      - partitions: dictionary (default: not set) - A key-value pair of a table
        name in the format of schema.table and a list of valid partition names
        used to limit the data export to just the specified partitions.
      - tzUtc: bool (default: true) - Convert TIMESTAMP data to UTC.
      - consistent: bool (default: true) - Enable or disable consistent data
        dumps.
      - skipConsistencyChecks: bool (default: false) - Skips additional
        consistency checks which are executed when running consistent dumps and
        i.e. backup lock cannot not be acquired.
      - ddlOnly: bool (default: false) - Only dump Data Definition Language
        (DDL) from the database.
      - dataOnly: bool (default: false) - Only dump data from the database.
      - dryRun: bool (default: false) - Print information about what would be
        dumped, but do not dump anything.
      - chunking: bool (default: true) - Enable chunking of the tables.
      - bytesPerChunk: string (default: "64M") - Sets average estimated number
        of bytes to be written to each chunk file, enables chunking.
      - threads: int (default: 4) - Use N threads to dump data chunks from the
        server.
      - fieldsTerminatedBy: string (default: "\t") - This option has the same
        meaning as the corresponding clause for SELECT ... INTO OUTFILE.
      - fieldsEnclosedBy: char (default: '') - This option has the same meaning
        as the corresponding clause for SELECT ... INTO OUTFILE.
      - fieldsEscapedBy: char (default: '\') - This option has the same meaning
        as the corresponding clause for SELECT ... INTO OUTFILE.
      - fieldsOptionallyEnclosed: bool (default: false) - Set to true if the
        input values are not necessarily enclosed within quotation marks
        specified by fieldsEnclosedBy option. Set to false if all fields are
        quoted by character specified by fieldsEnclosedBy option.
      - linesTerminatedBy: string (default: "\n") - This option has the same
        meaning as the corresponding clause for SELECT ... INTO OUTFILE. See
        Section 13.2.10.1, "SELECT ... INTO Statement".
      - dialect: enum (default: "default") - Setup fields and lines options
        that matches specific data file format. Can be used as base dialect and
        customized with fieldsTerminatedBy, fieldsEnclosedBy, fieldsEscapedBy,
        fieldsOptionallyEnclosed and linesTerminatedBy options. Must be one of
        the following values: default, csv, tsv or csv-unix.
      - maxRate: string (default: "0") - Limit data read throughput to maximum
        rate, measured in bytes per second per thread. Use maxRate="0" to set
        no limit.
      - showProgress: bool (default: true if stdout is a TTY device, false
        otherwise) - Enable or disable dump progress information.
      - defaultCharacterSet: string (default: "utf8mb4") - Character set used
        for the dump.
      - compression: string (default: "zstd") - Compression used when writing
        the data dump files, one of: "none", "gzip", "zstd".
      - osBucketName: string (default: not set) - Use specified OCI bucket for
        the location of the dump.
      - osNamespace: string (default: not set) - Specifies the namespace where
        the bucket is located, if not given it will be obtained using the
        tenancy id on the OCI configuration.
      - ociConfigFile: string (default: not set) - Use the specified OCI
        configuration file instead of the one at the default location.
      - ociProfile: string (default: not set) - Use the specified OCI profile
        instead of the default one.
      - ociParManifest: bool (default: not set) - Enables the generation of the
        PAR manifest while the dump operation is being executed. Deprecated.
      - ociParExpireTime: string (default: not set) - Allows defining the
        expiration time for the PARs generated when ociParManifest is enabled.
        Deprecated.
      - s3BucketName: string (default: not set) - Name of the AWS S3 bucket to
        use. The bucket must already exist.
      - s3CredentialsFile: string (default: not set) - Use the specified AWS
        credentials file.
      - s3ConfigFile: string (default: not set) - Use the specified AWS config
        file.
      - s3Profile: string (default: not set) - Use the specified AWS profile.
      - s3Region: string (default: not set) - Use the specified AWS region.
      - s3EndpointOverride: string (default: not set) - Use the specified AWS
        S3 API endpoint instead of the default one.
      - azureContainerName: string (default: not set) - Name of the Azure
        container to use. The container must already exist.
      - azureConfigFile: string (default: not set) - Use the specified Azure
        configuration file instead of the one at the default location.
      - azureStorageAccount: string (default: not set) - The account to be used
        for the operation.
      - azureStorageSasToken: string (default: not set) - Azure Shared Access
        Signature (SAS) token, to be used for the authentication of the
        operation, instead of a key.

      Requirements

      - MySQL Server 5.7 or newer is required.
      - Size limit for individual files uploaded to the OCI or AWS S3 bucket is
        1.2 TiB.
      - Columns with data types which are not safe to be stored in text form
        (i.e. BLOB) are converted to Base64, hence the size of such columns
        cannot exceed approximately 0.74 * max_allowed_packet bytes, as
        configured through that system variable at the target server.
      - Schema object names must use latin1 or utf8 character set.
      - Only tables which use the InnoDB storage engine are guaranteed to be
        dumped with consistent data.
      - Views and triggers to be dumped must not use qualified names to
        reference other views or tables.
      - Since util.dump_tables() function does not dump routines, any routines
        referenced by the dumped objects are expected to already exist when the
        dump is loaded.

      Details

      This operation writes SQL files per each table and view dumped, along
      with some global SQL files. The information about the source schema is
      also saved, meaning that when using the util.load_dump() function to load
      the dump, it is automatically recreated. Alternatively, dump can be
      loaded into another existing schema using the schema option.

      Table data dumps are written to text files using the specified file
      format, optionally splitting them into multiple chunk files.

      Requires an open, global Shell session, and uses its connection options,
      such as compression, ssl-mode, etc., to establish additional connections.

      Options

      If the all option is set to true and the tables parameter is set to an
      empty array, all views and tables from the specified schema are going to
      be dumped. If the tables parameter is not set to an empty array, an
      exception is thrown.

      The names given in the exclude{object}, include{object}, where or
      partitions options should be valid MySQL identifiers, quoted using
      backtick characters when required.

      If the exclude{object}, include{object}, where or partitions options
      contain an object which does not exist, or an object which belongs to a
      schema which does not exist, it is ignored.

      The tzUtc option allows dumping TIMESTAMP data when a server has data in
      different time zones or data is being moved between servers with
      different time zones.

      If the consistent option is set to true, a global read lock is set using
      the FLUSH TABLES WITH READ LOCK statement, all threads establish
      connections with the server and start transactions using:

      - SET SESSION TRANSACTION ISOLATION LEVEL REPEATABLE READ
      - START TRANSACTION WITH CONSISTENT SNAPSHOT

      Once all the threads start transactions, the instance is locked for
      backup and the global read lock is released.

      If the account used for the dump does not have enough privileges to
      execute FLUSH TABLES, LOCK TABLES will be used as a fallback instead. All
      tables being dumped, in addition to DDL and GRANT related tables in the
      mysql schema will be temporarily locked.

      The ddlOnly and dataOnly options cannot both be set to true at the same
      time.

      The chunking option causes the the data from each table to be split and
      written to multiple chunk files. If this option is set to false, table
      data is written to a single file.

      If the chunking option is set to true, but a table to be dumped cannot be
      chunked (for example if it does not contain a primary key or a unique
      index), data is dumped to multiple files using a single thread.

      The value of the threads option must be a positive number.

      The dialect option predefines the set of options fieldsTerminatedBy (FT),
      fieldsEnclosedBy (FE), fieldsOptionallyEnclosed (FOE), fieldsEscapedBy
      (FESC) and linesTerminatedBy (LT) in the following manner:

      - default: no quoting, tab-separated, LF line endings. (LT=<LF>,
        FESC='\', FT=<TAB>, FE=<empty>, FOE=false)
      - csv: optionally quoted, comma-separated, CRLF line endings.
        (LT=<CR><LF>, FESC='\', FT=",", FE='"', FOE=true)
      - tsv: optionally quoted, tab-separated, CRLF line endings. (LT=<CR><LF>,
        FESC='\', FT=<TAB>, FE='"', FOE=true)
      - csv-unix: fully quoted, comma-separated, LF line endings. (LT=<LF>,
        FESC='\', FT=",", FE='"', FOE=false)

      Both the bytesPerChunk and maxRate options support unit suffixes:

      - k - for kilobytes,
      - M - for Megabytes,
      - G - for Gigabytes,

      i.e. maxRate="2k" - limit throughput to 2000 bytes per second.

      The value of the bytesPerChunk option cannot be smaller than "128k".

      MySQL HeatWave Service Compatibility

      The MySQL HeatWave Service has a few security related restrictions that
      are not present in a regular, on-premise instance of MySQL. In order to
      make it easier to load existing databases into the Service, the dump
      commands in the MySQL Shell has options to detect potential issues and in
      some cases, to automatically adjust your schema definition to be
      compliant. For best results, always use the latest available version of
      MySQL Shell.

      The ocimds option, when set to true, will perform schema checks for most
      of these issues and abort the dump if any are found. The load_dump()
      command will also only allow loading dumps that have been created with
      the "ocimds" option enabled.

      Some issues found by the ocimds option may require you to manually make
      changes to your database schema before it can be loaded into the MySQL
      HeatWave Service. However, the compatibility option can be used to
      automatically modify the dumped schema SQL scripts, resolving some of
      these compatibility issues. You may pass one or more of the following
      values to the "compatibility" option.

      create_invisible_pks - Each table which does not have a Primary Key will
      have one created when the dump is loaded. The following Primary Key is
      added to the table:
      `my_row_id` BIGINT UNSIGNED AUTO_INCREMENT INVISIBLE PRIMARY KEY

      Dumps created with this value can be used with Inbound Replication into
      an MySQL HeatWave Service DB System instance with High Availability, as
      long as target instance has version 8.0.32 or newer. Mutually exclusive
      with the ignore_missing_pks value.

      force_innodb - The MySQL HeatWave Service requires use of the InnoDB
      storage engine. This option will modify the ENGINE= clause of CREATE
      TABLE statements that use incompatible storage engines and replace them
      with InnoDB. It will also remove the ROW_FORMAT=FIXED option, as it is
      not supported by the InnoDB storage engine.

      ignore_missing_pks - Ignore errors caused by tables which do not have
      Primary Keys. Dumps created with this value cannot be used in MySQL
      HeatWave Service DB System instance with High Availability. Mutually
      exclusive with the create_invisible_pks value.

      ignore_wildcard_grants - Ignore errors from grants on schemas with
      wildcards, which are interpreted differently in systems where
      partial_revokes system variable is enabled.

      skip_invalid_accounts - Skips accounts which do not have a password or
      use authentication methods (plugins) not supported by the MySQL HeatWave
      Service.

      strip_definers - Strips the "DEFINER=account" clause from views,
      routines, events and triggers. The MySQL HeatWave Service requires
      special privileges to create these objects with a definer other than the
      user loading the schema. By stripping the DEFINER clause, these objects
      will be created with that default definer. Views and routines will
      additionally have their SQL SECURITY clause changed from DEFINER to
      INVOKER. If this characteristic is missing, SQL SECURITY INVOKER clause
      will be added. This ensures that the access permissions of the account
      querying or calling these are applied, instead of the user that created
      them. This should be sufficient for most users, but if your database
      security model requires that views and routines have more privileges than
      their invoker, you will need to manually modify the schema before loading
      it.

      Please refer to the MySQL manual for details about DEFINER and SQL
      SECURITY.

      strip_invalid_grants - Strips grant statements which would fail when
      users are loaded, i.e. grants referring to a specific routine which does
      not exist.

      strip_restricted_grants - Certain privileges are restricted in the MySQL
      HeatWave Service. Attempting to create users granting these privileges
      would fail, so this option allows dumped GRANT statements to be stripped
      of these privileges.

      strip_tablespaces - Tablespaces have some restrictions in the MySQL
      HeatWave Service. If you'd like to have tables created in their default
      tablespaces, this option will strip the TABLESPACE= option from CREATE
      TABLE statements.

      Additionally, the following changes will always be made to DDL scripts
      when the ocimds option is enabled:

      - DATA DIRECTORY, INDEX DIRECTORY and ENCRYPTION options in CREATE TABLE
        statements will be commented out.

      In order to use Inbound Replication into an MySQL HeatWave Service DB
      System instance with High Availability where instance has version older
      than 8.0.32, all tables at the source server need to have Primary Keys.
      This needs to be fixed manually before running the dump. Starting with
      MySQL 8.0.23 invisible columns may be used to add Primary Keys without
      changing the schema compatibility, for more information see:
      https://dev.mysql.com/doc/refman/en/invisible-columns.html.

      In order to use Inbound Replication into an MySQL HeatWave Service DB
      System instance with High Availability, please see
      https://docs.oracle.com/en-us/iaas/mysql-database/doc/creating-replication-channel.html.

      In order to use MySQL HeatWave Service DB Service instance with High
      Availability, all tables must have a Primary Key. This can be fixed
      automatically using the create_invisible_pks compatibility value.

      Please refer to the MySQL HeatWave Service documentation for more
      information about restrictions and compatibility.

      Dumping to a Bucket in the OCI Object Storage

      If the osBucketName option is used, the dump is stored in the specified
      OCI bucket, connection is established using the local OCI profile. The
      directory structure is simulated within the object name.

      The osNamespace, ociConfigFile and ociProfile options cannot be used if
      the osBucketName option is set to an empty string.

      The osNamespace option overrides the OCI namespace obtained based on the
      tenancy ID from the local OCI profile.

      Enabling dump loading using pre-authenticated requests

      The load_dump utility supports loading a dump using a pre-authenticated
      request (PAR). The simplest way to do this is by providing a PAR to the
      location of the dump in a bucket, the PAR must be created with the
      following permissions:

      - Permits object reads
      - Enables object listing

      The generated URL can be used to load the dump, see \? load_dump for more
      details.

      ATTENTION: The ociParManifest and ociParExpireTime options described
                 below are deprecated and will be removed in a future release.

      Another way to enable loading a dump without requiring an OCI Profile, is
      to execute the dump operations enabling the ociParManifest option which
      will cause the dump operation automatically generates a PAR for every
      file in the dump, and will store them as part of the dump in a file named
      "@.manifest.json". The manifest is updated as the dump operation
      progresses.

      Using a PAR with permissions to read the manifest is another option to
      load the dump using PAR.

      The ociParManifest option cannot be used if osBucketName is not set.

      When creating PARs, an expiration time is required, it can be defined
      through the ociParExpireTime option. If the option is not used, a
      predefined expiration time will be used equivalent to a week after the
      dump operation started. The values assigned to this option should be
      conformant to RFC3339.

      The ociParExpireTime option cannot be used if the ociParManifest option
      is not enabled.

      Dumping to a Bucket in the AWS S3 Object Storage

      If the s3BucketName option is used, the dump is stored in the specified
      AWS S3 bucket. Connection is established using default local AWS
      configuration paths and profiles, unless overridden. The directory
      structure is simulated within the object name.

      The s3CredentialsFile, s3ConfigFile, s3Profile, s3Region and
      s3EndpointOverride options cannot be used if the s3BucketName option is
      not set or set to an empty string.

      Handling of the AWS settings

      The AWS options are evaluated in the order of precedence, the first
      available value is used.

      1. Name of the AWS profile:

      - the s3Profile option
      - the AWS_PROFILE environment variable
      - the AWS_DEFAULT_PROFILE environment variable
      - the default value of default

      2. Location of the credentials file:

      - the s3CredentialsFile option
      - the AWS_SHARED_CREDENTIALS_FILE environment variable
      - the default value of ~/.aws/credentials

      3. Location of the config file:

      - the s3ConfigFile option
      - the AWS_CONFIG_FILE environment variable
      - the default value of ~/.aws/config

      4. Name of the AWS region:

      - the s3Region option
      - the AWS_REGION environment variable
      - the AWS_DEFAULT_REGION environment variable
      - the region setting from the config file for the specified profile
      - the default value of us-east-1

      5. URI of AWS S3 API endpoint

      - the s3EndpointOverride option
      - the default value of https://<s3BucketName>.s3.<region>.amazonaws.com

      The AWS credentials are fetched from the following providers, in the
      order of precedence:

      1. Environment variables:

      - AWS_ACCESS_KEY_ID
      - AWS_SECRET_ACCESS_KEY
      - AWS_SESSION_TOKEN

      2. Settings from the credentials file for the specified profile:

      - aws_access_key_id
      - aws_secret_access_key
      - aws_session_token

      3. Process specified by the credential_process setting from the config
         file for the specified profile:

      - AccessKeyId
      - SecretAccessKey
      - SessionToken

      4. Settings from the config file for the specified profile:

      - aws_access_key_id
      - aws_secret_access_key
      - aws_session_token

      The items specified above correspond to the following credentials:

      - the AWS access key
      - the secret key associated with the AWS access key
      - the AWS session token for the temporary security credentials

      The process/command line specified by the credential_process setting must
      write a JSON object to the standard output in the following form:
      {
        "Version": 1,
        "AccessKeyId": "AWS access key",
        "SecretAccessKey": "secret key associated with the AWS access key",
        "SessionToken": "temporary AWS session token, optional",
        "Expiration": "RFC3339 timestamp, optional"
      }

      The Expiration key, if given, specifies when the credentials are going to
      expire, they will be automatically refreshed before this happens.

      The following credential handling rules apply:

      - If the s3Profile option is set to a non-empty string, the environment
        variables are not used as a potential credential provider.
      - If either an access key or a secret key is available in a potential
        credential provider, it is selected as the credential provider.
      - If either the access key or the secret key is missing in the selected
        credential provider, an exception is thrown.
      - If the session token is missing in the selected credential provider, or
        if it is set to an empty string, it is not used to authenticate the
        user.

      Dumping to a Container in the Azure Blob Storage

      If the azureContainerName option is used, the dump is stored in the
      specified Azure container. Connection is established using the
      configuration at the local Azure configuration file.The directory
      structure is simulated within the blob name.

      The azureConfigFile option cannot be used if the azureContainerName
      option is not set or set to an empty string.

      Handling of the Azure settings

      1. The following settings are read from the storage section in the config
         file:

      - connection_string
      - account
      - key
      - sas_token

      Additionally, the connection options may be defined using the standard
      Azure environment variables:

      - AZURE_STORAGE_CONNECTION_STRING
      - AZURE_STORAGE_ACCOUNT
      - AZURE_STORAGE_KEY
      - AZURE_STORAGE_SAS_TOKEN

      The Azure configuration values are evaluated in the following precedence:

      - Options parameter - Environment Variables - Configuration File

      If a connection string is defined either case in the environment variable
      or the configuration option, the individual configuration values for
      account and key will be ignored.

      If a SAS Token is defined, it will be used for the authorization
      (ignoring any defined account key).

      The default Azure Blob Endpoint to be used in the operations is defined
      by:

      https://<account>.blob.core.windows.net

      Unless a different EndPoint is defined in the connection string.

EXCEPTIONS
      ArgumentError in the following scenarios:

      - If any of the input arguments contains an invalid value.

      RuntimeError in the following scenarios:

      - If there is no open global session.
      - If creating the output directory fails.
      - If creating or writing to the output file fails.

#@<OUT> util export_table help
NAME
      export_table - Exports the specified table to the data dump file.

SYNTAX
      util.export_table(table, outputUrl[, options])

WHERE
      table: Name of the table to be exported.
      outputUrl: Target file to store the data.
      options: Dictionary with the export options.

DESCRIPTION
      The value of table parameter should be in form of table or schema.table,
      quoted using backtick characters when required. If schema is omitted, an
      active schema on the global Shell session is used. If there is none, an
      exception is raised.

      The outputUrl specifies where the dump is going to be stored.

      By default, a local file is used, and in this case outputUrl can be
      prefixed with file:// scheme. If a relative path is given, the absolute
      path is computed as relative to the current working directory. The parent
      directory of the output file must exist. If the output file exists, it is
      going to be overwritten. The output file is created with the following
      access rights (on operating systems which support them): rw-r-----.

      The following options are supported:

      - where: string (default: not set) - A valid SQL condition expression
        used to filter the data being exported.
      - partitions: list of strings (default: not set) - A list of valid
        partition names used to limit the data export to just the specified
        partitions.
      - fieldsTerminatedBy: string (default: "\t") - This option has the same
        meaning as the corresponding clause for SELECT ... INTO OUTFILE.
      - fieldsEnclosedBy: char (default: '') - This option has the same meaning
        as the corresponding clause for SELECT ... INTO OUTFILE.
      - fieldsEscapedBy: char (default: '\') - This option has the same meaning
        as the corresponding clause for SELECT ... INTO OUTFILE.
      - fieldsOptionallyEnclosed: bool (default: false) - Set to true if the
        input values are not necessarily enclosed within quotation marks
        specified by fieldsEnclosedBy option. Set to false if all fields are
        quoted by character specified by fieldsEnclosedBy option.
      - linesTerminatedBy: string (default: "\n") - This option has the same
        meaning as the corresponding clause for SELECT ... INTO OUTFILE. See
        Section 13.2.10.1, "SELECT ... INTO Statement".
      - dialect: enum (default: "default") - Setup fields and lines options
        that matches specific data file format. Can be used as base dialect and
        customized with fieldsTerminatedBy, fieldsEnclosedBy, fieldsEscapedBy,
        fieldsOptionallyEnclosed and linesTerminatedBy options. Must be one of
        the following values: default, csv, tsv or csv-unix.
      - maxRate: string (default: "0") - Limit data read throughput to maximum
        rate, measured in bytes per second per thread. Use maxRate="0" to set
        no limit.
      - showProgress: bool (default: true if stdout is a TTY device, false
        otherwise) - Enable or disable dump progress information.
      - defaultCharacterSet: string (default: "utf8mb4") - Character set used
        for the dump.
      - compression: string (default: "none") - Compression used when writing
        the data dump files, one of: "none", "gzip", "zstd".
      - osBucketName: string (default: not set) - Use specified OCI bucket for
        the location of the dump.
      - osNamespace: string (default: not set) - Specifies the namespace where
        the bucket is located, if not given it will be obtained using the
        tenancy id on the OCI configuration.
      - ociConfigFile: string (default: not set) - Use the specified OCI
        configuration file instead of the one at the default location.
      - ociProfile: string (default: not set) - Use the specified OCI profile
        instead of the default one.
      - s3BucketName: string (default: not set) - Name of the AWS S3 bucket to
        use. The bucket must already exist.
      - s3CredentialsFile: string (default: not set) - Use the specified AWS
        credentials file.
      - s3ConfigFile: string (default: not set) - Use the specified AWS config
        file.
      - s3Profile: string (default: not set) - Use the specified AWS profile.
      - s3Region: string (default: not set) - Use the specified AWS region.
      - s3EndpointOverride: string (default: not set) - Use the specified AWS
        S3 API endpoint instead of the default one.
      - azureContainerName: string (default: not set) - Name of the Azure
        container to use. The container must already exist.
      - azureConfigFile: string (default: not set) - Use the specified Azure
        configuration file instead of the one at the default location.
      - azureStorageAccount: string (default: not set) - The account to be used
        for the operation.
      - azureStorageSasToken: string (default: not set) - Azure Shared Access
        Signature (SAS) token, to be used for the authentication of the
        operation, instead of a key.

      Requirements

      - MySQL Server 5.7 or newer is required.
      - Size limit for individual files uploaded to the OCI or AWS S3 bucket is
        1.2 TiB.
      - Columns with data types which are not safe to be stored in text form
        (i.e. BLOB) are converted to Base64, hence the size of such columns
        cannot exceed approximately 0.74 * max_allowed_packet bytes, as
        configured through that system variable at the target server.

      Details

      This operation writes table data dump to the specified by the user files.

      Requires an open, global Shell session, and uses its connection options,
      such as compression, ssl-mode, etc., to establish additional connections.

      Options

      The dialect option predefines the set of options fieldsTerminatedBy (FT),
      fieldsEnclosedBy (FE), fieldsOptionallyEnclosed (FOE), fieldsEscapedBy
      (FESC) and linesTerminatedBy (LT) in the following manner:

      - default: no quoting, tab-separated, LF line endings. (LT=<LF>,
        FESC='\', FT=<TAB>, FE=<empty>, FOE=false)
      - csv: optionally quoted, comma-separated, CRLF line endings.
        (LT=<CR><LF>, FESC='\', FT=",", FE='"', FOE=true)
      - tsv: optionally quoted, tab-separated, CRLF line endings. (LT=<CR><LF>,
        FESC='\', FT=<TAB>, FE='"', FOE=true)
      - csv-unix: fully quoted, comma-separated, LF line endings. (LT=<LF>,
        FESC='\', FT=",", FE='"', FOE=false)

      The maxRate option supports unit suffixes:

      - k - for kilobytes,
      - M - for Megabytes,
      - G - for Gigabytes,

      i.e. maxRate="2k" - limit throughput to 2000 bytes per second.

      Dumping to a Bucket in the OCI Object Storage

      If the osBucketName option is used, the dump is stored in the specified
      OCI bucket, connection is established using the local OCI profile. The
      directory structure is simulated within the object name.

      The osNamespace, ociConfigFile and ociProfile options cannot be used if
      the osBucketName option is set to an empty string.

      The osNamespace option overrides the OCI namespace obtained based on the
      tenancy ID from the local OCI profile.

      Dumping to a Bucket in the AWS S3 Object Storage

      If the s3BucketName option is used, the dump is stored in the specified
      AWS S3 bucket. Connection is established using default local AWS
      configuration paths and profiles, unless overridden. The directory
      structure is simulated within the object name.

      The s3CredentialsFile, s3ConfigFile, s3Profile, s3Region and
      s3EndpointOverride options cannot be used if the s3BucketName option is
      not set or set to an empty string.

      Handling of the AWS settings

      The AWS options are evaluated in the order of precedence, the first
      available value is used.

      1. Name of the AWS profile:

      - the s3Profile option
      - the AWS_PROFILE environment variable
      - the AWS_DEFAULT_PROFILE environment variable
      - the default value of default

      2. Location of the credentials file:

      - the s3CredentialsFile option
      - the AWS_SHARED_CREDENTIALS_FILE environment variable
      - the default value of ~/.aws/credentials

      3. Location of the config file:

      - the s3ConfigFile option
      - the AWS_CONFIG_FILE environment variable
      - the default value of ~/.aws/config

      4. Name of the AWS region:

      - the s3Region option
      - the AWS_REGION environment variable
      - the AWS_DEFAULT_REGION environment variable
      - the region setting from the config file for the specified profile
      - the default value of us-east-1

      5. URI of AWS S3 API endpoint

      - the s3EndpointOverride option
      - the default value of https://<s3BucketName>.s3.<region>.amazonaws.com

      The AWS credentials are fetched from the following providers, in the
      order of precedence:

      1. Environment variables:

      - AWS_ACCESS_KEY_ID
      - AWS_SECRET_ACCESS_KEY
      - AWS_SESSION_TOKEN

      2. Settings from the credentials file for the specified profile:

      - aws_access_key_id
      - aws_secret_access_key
      - aws_session_token

      3. Process specified by the credential_process setting from the config
         file for the specified profile:

      - AccessKeyId
      - SecretAccessKey
      - SessionToken

      4. Settings from the config file for the specified profile:

      - aws_access_key_id
      - aws_secret_access_key
      - aws_session_token

      The items specified above correspond to the following credentials:

      - the AWS access key
      - the secret key associated with the AWS access key
      - the AWS session token for the temporary security credentials

      The process/command line specified by the credential_process setting must
      write a JSON object to the standard output in the following form:
      {
        "Version": 1,
        "AccessKeyId": "AWS access key",
        "SecretAccessKey": "secret key associated with the AWS access key",
        "SessionToken": "temporary AWS session token, optional",
        "Expiration": "RFC3339 timestamp, optional"
      }

      The Expiration key, if given, specifies when the credentials are going to
      expire, they will be automatically refreshed before this happens.

      The following credential handling rules apply:

      - If the s3Profile option is set to a non-empty string, the environment
        variables are not used as a potential credential provider.
      - If either an access key or a secret key is available in a potential
        credential provider, it is selected as the credential provider.
      - If either the access key or the secret key is missing in the selected
        credential provider, an exception is thrown.
      - If the session token is missing in the selected credential provider, or
        if it is set to an empty string, it is not used to authenticate the
        user.

      Dumping to a Container in the Azure Blob Storage

      If the azureContainerName option is used, the dump is stored in the
      specified Azure container. Connection is established using the
      configuration at the local Azure configuration file.The directory
      structure is simulated within the blob name.

      The azureConfigFile option cannot be used if the azureContainerName
      option is not set or set to an empty string.

      Handling of the Azure settings

      1. The following settings are read from the storage section in the config
         file:

      - connection_string
      - account
      - key
      - sas_token

      Additionally, the connection options may be defined using the standard
      Azure environment variables:

      - AZURE_STORAGE_CONNECTION_STRING
      - AZURE_STORAGE_ACCOUNT
      - AZURE_STORAGE_KEY
      - AZURE_STORAGE_SAS_TOKEN

      The Azure configuration values are evaluated in the following precedence:

      - Options parameter - Environment Variables - Configuration File

      If a connection string is defined either case in the environment variable
      or the configuration option, the individual configuration values for
      account and key will be ignored.

      If a SAS Token is defined, it will be used for the authorization
      (ignoring any defined account key).

      The default Azure Blob Endpoint to be used in the operations is defined
      by:

      https://<account>.blob.core.windows.net

      Unless a different EndPoint is defined in the connection string.

EXCEPTIONS
      ArgumentError in the following scenarios:

      - If any of the input arguments contains an invalid value.

      RuntimeError in the following scenarios:

      - If there is no open global session.
      - If creating or writing to the output file fails.

#@<OUT> util import_json help
NAME
      import_json - Import JSON documents from file to collection or table in
                    MySQL Server using X Protocol session.

SYNTAX
      util.import_json(file[, options])

WHERE
      file: Path to JSON documents file
      options: Dictionary with import options

DESCRIPTION
      This function reads standard JSON documents from a file, however, it also
      supports converting BSON Data Types represented using the MongoDB
      Extended Json (strict mode) into MySQL values.

      The options dictionary supports the following options:

      - schema: string - name of target schema.
      - collection: string - name of collection where the data will be
        imported.
      - table: string - name of table where the data will be imported.
      - tableColumn: string (default: "doc") - name of column in target table
        where the imported JSON documents will be stored.
      - convertBsonTypes: bool (default: false) - enables the BSON data type
        conversion.
      - convertBsonOid: bool (default: the value of convertBsonTypes) - enables
        conversion of the BSON ObjectId values.
      - extractOidTime: string (default: empty) - creates a new field based on
        the ObjectID timestamp. Only valid if convertBsonOid is enabled.

      The following options are valid only when convertBsonTypes is enabled.
      They are all boolean flags. ignoreRegexOptions is enabled by default,
      rest are disabled by default.

      - ignoreDate: disables conversion of BSON Date values
      - ignoreTimestamp: disables conversion of BSON Timestamp values
      - ignoreRegex: disables conversion of BSON Regex values.
      - ignoreBinary: disables conversion of BSON BinData values.
      - decimalAsDouble: causes BSON Decimal values to be imported as double
        values.
      - ignoreRegexOptions: causes regex options to be ignored when processing
        a Regex BSON value. This option is only valid if ignoreRegex is
        disabled.

      If the schema is not provided, an active schema on the global session, if
      set, will be used.

      The collection and the table options cannot be combined. If they are not
      provided, the basename of the file without extension will be used as
      target collection name.

      If the target collection or table does not exist, they are created,
      otherwise the data is inserted into the existing collection or table.

      The tableColumn implies the use of the table option and cannot be
      combined with the collection option.

      BSON Data Type Processing.

      If only convertBsonOid is enabled, no conversion will be done on the rest
      of the BSON Data Types.

      To use extractOidTime, it should be set to a name which will be used to
      insert an additional field into the main document. The value of the new
      field will be the timestamp obtained from the ObjectID value. Note that
      this will be done only for an ObjectID value associated to the '_id'
      field of the main document.

      NumberLong and NumberInt values will be converted to integer values.

      NumberDecimal values are imported as strings, unless decimalAsDouble is
      enabled.

      Regex values will be converted to strings containing the regular
      expression. The regular expression options are ignored unless
      ignoreRegexOptions is disabled. When ignoreRegexOptions is disabled the
      regular expression will be converted to the form: /<regex>/<options>.

EXCEPTIONS
      Throws ArgumentError when:

      - Option name is invalid
      - Required options are not set and cannot be deduced
      - Shell is not connected to MySQL Server using X Protocol
      - Schema is not provided and there is no active schema on the global
        session
      - Both collection and table are specified

      Throws LogicError when:

      - Path to JSON document does not exists or is not a file

      Throws RuntimeError when:

      - The schema does not exists
      - MySQL Server returns an error

      Throws InvalidJson when:

      - JSON document is ill-formed

#@<OUT> util import_table help
NAME
      import_table - Import table dump stored in files to target table using
                     LOAD DATA LOCAL INFILE calls in parallel connections.

SYNTAX
      util.import_table(files[, options])

WHERE
      files: Path or list of paths to files with user data. Path name can
             contain a glob pattern with wildcard '*' and/or '?'. All selected
             files must be chunks of the same target table.
      options: Dictionary with import options

DESCRIPTION
      The scheme part of a filename contains infomation about the transport
      backend. Supported transport backends are: file://, http://, https://. If
      the scheme part of a filename is omitted, then file:// transport backend
      will be chosen.

      Supported filename formats:

      - /path/to/file - Path to a locally or remotely (e.g. in OCI Object
        Storage) accessible file or directory
      - file:///path/to/file - Path to a locally accessible file or directory
      - http[s]://host.domain[:port]/path/to/file - Location of a remote file
        accessible through HTTP(s) (import_table() only)

      If the osBucketName option is given, the path argument must specify a
      plain path in that OCI (Oracle Cloud Infrastructure) Object Storage
      bucket.

      The OCI configuration profile is located through the oci.profile and
      oci.configFile global shell options and can be overridden with ociProfile
      and ociConfigFile, respectively.

      If the s3BucketName option is given, the path argument must specify a
      plain path in that AWS S3 bucket.

      If the azureContainerName option is given, the path argument must specify
      a plain path in that Azure container.

      Options dictionary:

      - schema: string (default: current shell active schema) - Name of target
        schema
      - table: string (default: filename without extension) - Name of target
        table
      - columns: array of strings and/or integers (default: empty array) - This
        option takes an array of column names as its value. The order of the
        column names indicates how to match data file columns with table
        columns. Use non-negative integer `i` to capture column value into user
        variable @i. With user variables, the decodeColumns option enables you
        to perform preprocessing transformations on their values before
        assigning the result to columns.
      - fieldsTerminatedBy: string (default: "\t") - This option has the same
        meaning as the corresponding clause for LOAD DATA INFILE.
      - fieldsEnclosedBy: char (default: '') - This option has the same meaning
        as the corresponding clause for LOAD DATA INFILE.
      - fieldsEscapedBy: char (default: '\') - This option has the same meaning
        as the corresponding clause for LOAD DATA INFILE.
      - fieldsOptionallyEnclosed: bool (default: false) - Set to true if the
        input values are not necessarily enclosed within quotation marks
        specified by fieldsEnclosedBy option. Set to false if all fields are
        quoted by character specified by fieldsEnclosedBy option.
      - linesTerminatedBy: string (default: "\n") - This option has the same
        meaning as the corresponding clause for LOAD DATA INFILE. For example,
        to import Windows files that have lines terminated with carriage
        return/linefeed pairs, use --lines-terminated-by="\r\n". (You might
        have to double the backslashes, depending on the escaping conventions
        of your command interpreter.) See Section 13.2.7, "LOAD DATA INFILE
        Syntax".
      - replaceDuplicates: bool (default: false) - If true, input rows that
        have the same value for a primary key or unique index as an existing
        row will be replaced, otherwise input rows will be skipped.
      - threads: int (default: 8) - Use N threads to sent file chunks to the
        server.
      - bytesPerChunk: string (minimum: "131072", default: "50M") - Send
        bytesPerChunk (+ bytes to end of the row) in single LOAD DATA call.
        Unit suffixes, k - for Kilobytes (n * 1'000 bytes), M - for Megabytes
        (n * 1'000'000 bytes), G - for Gigabytes (n * 1'000'000'000 bytes),
        bytesPerChunk="2k" - ~2 kilobyte data chunk will send to the MySQL
        Server. Not available for multiple files import.
      - maxBytesPerTransaction: string (default: empty) - Specifies the maximum
        number of bytes that can be loaded from a dump data file per single
        LOAD DATA statement. If a content size of data file is bigger than this
        option value, then multiple LOAD DATA statements will be executed per
        single file. If this option is not specified explicitly, dump data file
        sub-chunking will be disabled. Use this option with value less or equal
        to global variable 'max_binlog_cache_size' to mitigate "MySQL Error
        1197 (HY000): Multi-statement transaction required more than
        'max_binlog_cache_size' bytes of storage". Unit suffixes: k
        (Kilobytes), M (Megabytes), G (Gigabytes). Minimum value: 4096.
      - maxRate: string (default: "0") - Limit data send throughput to maxRate
        in bytes per second per thread. maxRate="0" - no limit. Unit suffixes,
        k - for Kilobytes (n * 1'000 bytes), M - for Megabytes (n * 1'000'000
        bytes), G - for Gigabytes (n * 1'000'000'000 bytes), maxRate="2k" -
        limit to 2 kilobytes per second.
      - showProgress: bool (default: true if stdout is a tty, false otherwise)
        - Enable or disable import progress information.
      - skipRows: int (default: 0) - Skip first N physical lines from each of
        the imported files. You can use this option to skip an initial header
        line containing column names.
      - dialect: enum (default: "default") - Setup fields and lines options
        that matches specific data file format. Can be used as base dialect and
        customized with fieldsTerminatedBy, fieldsEnclosedBy,
        fieldsOptionallyEnclosed, fieldsEscapedBy and linesTerminatedBy
        options. Must be one of the following values: default, csv, tsv, json
        or csv-unix.
      - decodeColumns: map (default: not set) - a map between columns names and
        SQL expressions to be applied on the loaded data. Column value captured
        in 'columns' by integer is available as user variable '@i', where `i`
        is that integer. Requires 'columns' to be set.
      - characterSet: string (default: not set) - Interpret the information in
        the input file using this character set encoding. characterSet set to
        "binary" specifies "no conversion". If not set, the server will use the
        character set indicated by the character_set_database system variable
        to interpret the information in the file.
      - sessionInitSql: list of strings (default: []) - execute the given list
        of SQL statements in each session about to load data.

      OCI Object Storage Options

      - osBucketName: string (default: not set) - Name of the OCI Object
        Storage bucket to use. The bucket must already exist.
      - osNamespace: string (default: not set) - Specifies the namespace where
        the bucket is located, if not given it will be obtained using the
        tenancy id on the OCI configuration.
      - ociConfigFile: string (default: not set) - Override oci.configFile
        shell option, to specify the path to the OCI configuration file.
      - ociProfile: string (default: not set) - Override oci.profile shell
        option, to specify the name of the OCI profile to use.

      AWS S3 Object Storage Options

      - s3BucketName: string (default: not set) - Name of the AWS S3 bucket to
        use. The bucket must already exist.
      - s3CredentialsFile: string (default: not set) - Use the specified AWS
        credentials file.
      - s3ConfigFile: string (default: not set) - Use the specified AWS config
        file.
      - s3Profile: string (default: not set) - Use the specified AWS profile.
      - s3Region: string (default: not set) - Use the specified AWS region.
      - s3EndpointOverride: string (default: not set) - Use the specified AWS
        S3 API endpoint instead of the default one.

      If the s3BucketName option is used, the dump is stored in the specified
      AWS S3 bucket. Connection is established using default local AWS
      configuration paths and profiles, unless overridden. The directory
      structure is simulated within the object name.

      The s3CredentialsFile, s3ConfigFile, s3Profile, s3Region and
      s3EndpointOverride options cannot be used if the s3BucketName option is
      not set or set to an empty string.

      Handling of the AWS settings

      The AWS options are evaluated in the order of precedence, the first
      available value is used.

      1. Name of the AWS profile:

      - the s3Profile option
      - the AWS_PROFILE environment variable
      - the AWS_DEFAULT_PROFILE environment variable
      - the default value of default

      2. Location of the credentials file:

      - the s3CredentialsFile option
      - the AWS_SHARED_CREDENTIALS_FILE environment variable
      - the default value of ~/.aws/credentials

      3. Location of the config file:

      - the s3ConfigFile option
      - the AWS_CONFIG_FILE environment variable
      - the default value of ~/.aws/config

      4. Name of the AWS region:

      - the s3Region option
      - the AWS_REGION environment variable
      - the AWS_DEFAULT_REGION environment variable
      - the region setting from the config file for the specified profile
      - the default value of us-east-1

      5. URI of AWS S3 API endpoint

      - the s3EndpointOverride option
      - the default value of https://<s3BucketName>.s3.<region>.amazonaws.com

      The AWS credentials are fetched from the following providers, in the
      order of precedence:

      1. Environment variables:

      - AWS_ACCESS_KEY_ID
      - AWS_SECRET_ACCESS_KEY
      - AWS_SESSION_TOKEN

      2. Settings from the credentials file for the specified profile:

      - aws_access_key_id
      - aws_secret_access_key
      - aws_session_token

      3. Process specified by the credential_process setting from the config
         file for the specified profile:

      - AccessKeyId
      - SecretAccessKey
      - SessionToken

      4. Settings from the config file for the specified profile:

      - aws_access_key_id
      - aws_secret_access_key
      - aws_session_token

      The items specified above correspond to the following credentials:

      - the AWS access key
      - the secret key associated with the AWS access key
      - the AWS session token for the temporary security credentials

      The process/command line specified by the credential_process setting must
      write a JSON object to the standard output in the following form:
      {
        "Version": 1,
        "AccessKeyId": "AWS access key",
        "SecretAccessKey": "secret key associated with the AWS access key",
        "SessionToken": "temporary AWS session token, optional",
        "Expiration": "RFC3339 timestamp, optional"
      }

      The Expiration key, if given, specifies when the credentials are going to
      expire, they will be automatically refreshed before this happens.

      The following credential handling rules apply:

      - If the s3Profile option is set to a non-empty string, the environment
        variables are not used as a potential credential provider.
      - If either an access key or a secret key is available in a potential
        credential provider, it is selected as the credential provider.
      - If either the access key or the secret key is missing in the selected
        credential provider, an exception is thrown.
      - If the session token is missing in the selected credential provider, or
        if it is set to an empty string, it is not used to authenticate the
        user.

      Azure Blob Storage Options

      - azureContainerName: string (default: not set) - Name of the Azure
        container to use. The container must already exist.
      - azureConfigFile: string (default: not set) - Use the specified Azure
        configuration file instead of the one at the default location.
      - azureStorageAccount: string (default: not set) - The account to be used
        for the operation.
      - azureStorageSasToken: string (default: not set) - Azure Shared Access
        Signature (SAS) token, to be used for the authentication of the
        operation, instead of a key.

      If the azureContainerName option is used, the dump is stored in the
      specified Azure container. Connection is established using the
      configuration at the local Azure configuration file.The directory
      structure is simulated within the blob name.

      The azureConfigFile option cannot be used if the azureContainerName
      option is not set or set to an empty string.

      Handling of the Azure settings

      1. The following settings are read from the storage section in the config
         file:

      - connection_string
      - account
      - key
      - sas_token

      Additionally, the connection options may be defined using the standard
      Azure environment variables:

      - AZURE_STORAGE_CONNECTION_STRING
      - AZURE_STORAGE_ACCOUNT
      - AZURE_STORAGE_KEY
      - AZURE_STORAGE_SAS_TOKEN

      The Azure configuration values are evaluated in the following precedence:

      - Options parameter - Environment Variables - Configuration File

      If a connection string is defined either case in the environment variable
      or the configuration option, the individual configuration values for
      account and key will be ignored.

      If a SAS Token is defined, it will be used for the authorization
      (ignoring any defined account key).

      The default Azure Blob Endpoint to be used in the operations is defined
      by:

      https://<account>.blob.core.windows.net

      Unless a different EndPoint is defined in the connection string.

      dialect predefines following set of options fieldsTerminatedBy (FT),
      fieldsEnclosedBy (FE), fieldsOptionallyEnclosed (FOE), fieldsEscapedBy
      (FESC) and linesTerminatedBy (LT) in following manner:

      - default: no quoting, tab-separated, lf line endings. (LT=<LF>,
        FESC='\', FT=<TAB>, FE=<empty>, FOE=false)
      - csv: optionally quoted, comma-separated, crlf line endings.
        (LT=<CR><LF>, FESC='\', FT=",", FE='"', FOE=true)
      - tsv: optionally quoted, tab-separated, crlf line endings. (LT=<CR><LF>,
        FESC='\', FT=<TAB>, FE='"', FOE=true)
      - json: one JSON document per line. (LT=<LF>, FESC=<empty>, FT=<LF>,
        FE=<empty>, FOE=false)
      - csv-unix: fully quoted, comma-separated, lf line endings. (LT=<LF>,
        FESC='\', FT=",", FE='"', FOE=false)

      If the schema is not provided, an active schema on the global session, if
      set, will be used.

      If the input values are not necessarily enclosed within fieldsEnclosedBy,
      set fieldsOptionallyEnclosed to true.

      If you specify one separator that is the same as or a prefix of another,
      LOAD DATA INFILE cannot interpret the input properly.

      Connection options set in the global session, such as compression,
      ssl-mode, etc. are used in parallel connections.

      Each parallel connection sets the following session variables:

      - SET SQL_MODE = ''; -- Clear SQL Mode
      - SET NAMES ?; -- Set to characterSet option if provided by user.
      - SET unique_checks = 0
      - SET foreign_key_checks = 0
      - SET SESSION TRANSACTION ISOLATION LEVEL READ UNCOMMITTED

      Note: because of storage engine limitations, table locks held by MyISAM
      will cause imports of such tables to be sequential, regardless of the
      number of threads used.

#@<OUT> util load_dump help
NAME
      load_dump - Loads database dumps created by MySQL Shell.

SYNTAX
      util.load_dump(url[, options])

WHERE
      url: defines the location of the dump to be loaded
      options: Dictionary with load options

DESCRIPTION
      Depending on how the dump was created, the url identifies the location
      and in some cases the access method to the dump, i.e. for dumps to be
      loaded using pre-authenticated requests (PAR). Allowed values:

      - /path/to/folder - to load a dump from local storage
      - /oci/bucket/path - to load a dump from OCI Object Storage using an OCI
        profile
      - /aws/bucket/path - to load a dump from AWS S3 Object Storage using the
        AWS settings stored in the credentials and config files
      - PAR to the dump manifest - to load a dump from OCI Object Storage
        created with the ociParManifest option
      - PAR to the dump location - to load a dump from OCI Object Storage using
        a single PAR

      load_dump() will load a dump from the specified path. It transparently
      handles compressed files and directly streams data when loading from
      remote storage (currently HTTP, OCI Object Storage, AWS S3 Object Storage
      and Azure Containers). If the 'waitDumpTimeout' option is set, it will
      load a dump on-the-fly, loading table data chunks as the dumper produces
      them.

      Table data will be loaded in parallel using the configured number of
      threads (4 by default). Multiple threads per table can be used if the
      dump was created with table chunking enabled. Data loads are scheduled
      across threads in a way that tries to maximize parallelism, while also
      minimizing lock contention from concurrent loads to the same table. If
      there are more tables than threads, different tables will be loaded per
      thread, larger tables first. If there are more threads than tables, then
      chunks from larger tables will be proportionally assigned more threads.

      LOAD DATA LOCAL INFILE is used to load table data and thus, the
      'local_infile' MySQL global setting must be enabled.

      Resuming

      The load command will store progress information into a file for each
      step of the loading process, including successfully completed and
      interrupted/failed ones. If that file already exists, its contents will
      be used to skip steps that have already been completed and retry those
      that failed or didn't start yet.

      When resuming, table chunks that have started being loaded but didn't
      finish are loaded again. Duplicate rows are discarded by the server.
      Tables that do not have unique keys are truncated before the load is
      resumed.

      IMPORTANT: Resuming assumes that no changes have been made to the
      partially loaded data between the failure and the retry. Resuming after
      external changes has undefined behavior and may lead to data loss.

      The progress state file has a default name of
      load-progress.<server_uuid>.json and is written to the same location as
      the dump. If 'progressFile' is specified, progress will be written to
      either a local file at the given path, or, if the HTTP(S) scheme is used,
      to a remote file using HTTP PUT requests. Setting it to '' will disable
      progress tracking and resuming.

      If the 'resetProgress' option is enabled, progress information from
      previous load attempts of the dump to the destination server is discarded
      and the load is restarted. You may use this option to retry loading the
      whole dump from the beginning. However, changes made to the database are
      not reverted, so previously loaded objects should be manually dropped
      first.

      Options dictionary:

      - analyzeTables: "off", "on", "histogram" (default: off) - If 'on',
        executes ANALYZE TABLE for all tables, once loaded. If set to
        'histogram', only tables that have histogram information stored in the
        dump will be analyzed. This option can be used even if all 'load'
        options are disabled.
      - backgroundThreads: int (default not set) - Number of additional threads
        to use to fetch contents of metadata and DDL files. If not set, loader
        will use the value of the threads option in case of a local dump, or
        four times that value in case on a non-local dump.
      - characterSet: string (default taken from dump) - Overrides the
        character set to be used for loading dump data. By default, the same
        character set used for dumping will be used (utf8mb4 if not set on
        dump).
      - createInvisiblePKs: bool (default taken from dump) - Automatically
        create an invisible Primary Key for each table which does not have one.
        By default, set to true if dump was created with create_invisible_pks
        compatibility option, false otherwise. Requires server 8.0.24 or newer.
      - deferTableIndexes: "off", "fulltext", "all" (default: fulltext) - If
        "all", creation of "all" indexes except PRIMARY is deferred until after
        table data is loaded, which in many cases can reduce load times. If
        "fulltext", only full-text indexes will be deferred.
      - dryRun: bool (default: false) - Scans the dump and prints everything
        that would be performed, without actually doing so.
      - excludeEvents: array of strings (default not set) - Skip loading
        specified events from the dump. Strings are in format schema.event,
        quoted using backtick characters when required.
      - excludeRoutines: array of strings (default not set) - Skip loading
        specified routines from the dump. Strings are in format schema.routine,
        quoted using backtick characters when required.
      - excludeSchemas: array of strings (default not set) - Skip loading
        specified schemas from the dump.
      - excludeTables: array of strings (default not set) - Skip loading
        specified tables from the dump. Strings are in format schema.table,
        quoted using backtick characters when required.
      - excludeTriggers: array of strings (default not set) - Skip loading
        specified triggers from the dump. Strings are in format schema.table
        (all triggers from the specified table) or schema.table.trigger (the
        individual trigger), quoted using backtick characters when required.
      - excludeUsers: array of strings (default not set) - Skip loading
        specified users from the dump. Each user is in the format of
        'user_name'[@'host']. If the host is not specified, all the accounts
        with the given user name are excluded.
      - handleGrantErrors: "abort", "drop_account", "ignore" (default: abort) -
        Specifies action to be performed in case of errors related to the
        GRANT/REVOKE statements, "abort": throws an error and aborts the load,
        "drop_account": deletes the problematic account and continues,
        "ignore": ignores the error and continues loading the account.
      - ignoreExistingObjects: bool (default false) - Load the dump even if it
        contains user accounts or DDL objects that already exist in the target
        database. If this option is set to false, any existing object results
        in an error. Setting it to true ignores existing objects, but the
        CREATE statements are still going to be executed.
      - ignoreVersion: bool (default false) - Load the dump even if the major
        version number of the server where it was created is different from
        where it will be loaded.
      - includeEvents: array of strings (default not set) - Loads only the
        specified events from the dump. Strings are in format schema.event,
        quoted using backtick characters when required. By default, all events
        are included.
      - includeRoutines: array of strings (default not set) - Loads only the
        specified routines from the dump. Strings are in format schema.routine,
        quoted using backtick characters when required. By default, all
        routines are included.
      - includeSchemas: array of strings (default not set) - Loads only the
        specified schemas from the dump. By default, all schemas are included.
      - includeTables: array of strings (default not set) - Loads only the
        specified tables from the dump. Strings are in format schema.table,
        quoted using backtick characters when required. By default, all tables
        from all schemas are included.
      - includeTriggers: array of strings (default not set) - Loads only the
        specified triggers from the dump. Strings are in format schema.table
        (all triggers from the specified table) or schema.table.trigger (the
        individual trigger), quoted using backtick characters when required. By
        default, all triggers are included.
      - includeUsers: array of strings (default not set) - Load only the
        specified users from the dump. Each user is in the format of
        'user_name'[@'host']. If the host is not specified, all the accounts
        with the given user name are included. By default, all users are
        included.
      - loadData: bool (default: true) - Loads table data from the dump.
      - loadDdl: bool (default: true) - Executes DDL/SQL scripts in the dump.
      - loadIndexes: bool (default: true) - use together with deferTableIndexes
        to control whether secondary indexes should be recreated at the end of
        the load. Useful when loading DDL and data separately.
      - loadUsers: bool (default: false) - Executes SQL scripts for user
        accounts, roles and grants contained in the dump. Note: statements for
        the current user will be skipped.
      - maxBytesPerTransaction: string (default taken from dump) - Specifies
        the maximum number of bytes that can be loaded from a dump data file
        per single LOAD DATA statement. Supports unit suffixes: k (kilobytes),
        M (Megabytes), G (Gigabytes). Minimum value: 4096. If this option is
        not specified explicitly, the value of the bytesPerChunk dump option is
        used, but only in case of the files with data size greater than 1.5 *
        bytesPerChunk.
      - progressFile: path (default: load-progress.<server_uuid>.progress) -
        Stores load progress information in the given local file path.
      - resetProgress: bool (default: false) - Discards progress information of
        previous load attempts to the destination server and loads the whole
        dump again.
      - schema: string (default not set) - Load the dump into the given schema.
        This option can only be used when loading just one schema, (either only
        one schema was dumped, or schema filters result in only one schema).
      - sessionInitSql: list of strings (default: []) - execute the given list
        of SQL statements in each session about to load data.
      - showMetadata: bool (default: false) - Displays the metadata information
        stored in the dump files, i.e. binary log file name and position.
      - showProgress: bool (default: true if stdout is a tty, false otherwise)
        - Enable or disable import progress information.
      - skipBinlog: bool (default: false) - Disables the binary log for the
        MySQL sessions used by the loader (set sql_log_bin=0).
      - threads: int (default: 4) - Number of threads to use to import table
        data.
      - updateGtidSet: "off", "replace", "append" (default: off) - if set to a
        value other than 'off' updates GTID_PURGED by either replacing its
        contents or appending to it the gtid set present in the dump.
      - waitDumpTimeout: float (default: 0) - Loads a dump while it's still
        being created. Once all uploaded tables are processed the command will
        either wait for more data, the dump is marked as completed or the given
        timeout (in seconds) passes. <= 0 disables waiting.
      - sessionInitSql: list of strings (default: []) - execute the given list
        of SQL statements in each session about to load data.
      - osBucketName: string (default: not set) - Use specified OCI bucket for
        the location of the dump.
      - osNamespace: string (default: not set) - Specifies the namespace where
        the bucket is located, if not given it will be obtained using the
        tenancy id on the OCI configuration.
      - ociConfigFile: string (default: not set) - Use the specified OCI
        configuration file instead of the one at the default location.
      - ociProfile: string (default: not set) - Use the specified OCI profile
        instead of the default one.
      - s3BucketName: string (default: not set) - Name of the AWS S3 bucket to
        use. The bucket must already exist.
      - s3CredentialsFile: string (default: not set) - Use the specified AWS
        credentials file.
      - s3ConfigFile: string (default: not set) - Use the specified AWS config
        file.
      - s3Profile: string (default: not set) - Use the specified AWS profile.
      - s3Region: string (default: not set) - Use the specified AWS region.
      - s3EndpointOverride: string (default: not set) - Use the specified AWS
        S3 API endpoint instead of the default one.
      - azureContainerName: string (default: not set) - Name of the Azure
        container to use. The container must already exist.
      - azureConfigFile: string (default: not set) - Use the specified Azure
        configuration file instead of the one at the default location.
      - azureStorageAccount: string (default: not set) - The account to be used
        for the operation.
      - azureStorageSasToken: string (default: not set) - Azure Shared Access
        Signature (SAS) token, to be used for the authentication of the
        operation, instead of a key.

      Connection options set in the global session, such as compression,
      ssl-mode, etc. are inherited by load sessions.

      Examples:

      util.load_dump('sakila_dump')

      util.load_dump('mysql/sales', {
          'osBucketName': 'mybucket',    // OCI Object Storage bucket
          'waitDumpTimeout': 1800        // wait for new data for up to 30mins
      })

      Loading a dump using Pre-authenticated Requests (PAR)

      When a dump is created in OCI Object Storage, it is possible to load it
      using a single pre-authenticated request which gives access to the
      location of the dump. The requirements for this PAR include:

      - Permits object reads
      - Enables object listing

      Given a dump located at a bucket root and a PAR created for the bucket,
      the dump can be loaded by providing the PAR as the url parameter:

      Example:

      Dump Location: root of 'test' bucket

      util.load_dump(
        'https://objectstorage.*.oraclecloud.com/p/*/n/main/b/test/o/', {
          'progressFile': 'load_progress.txt'
        }
      )

      Given a dump located at some folder within a bucket and a PAR created for
      the given folder, the dump can be loaded by providing the PAR and the
      prefix as the url parameter:

      Example:

      Dump Location: folder 'dump' at the 'test' bucket
      PAR created using the 'dump/' prefix.

      util.load_dump(
        'https://objectstorage.*.oraclecloud.com/p/*/n/main/b/test/o/dump/', {
          'progressFile': 'load_progress.txt'
        }
      )

      In both of the above cases the load is done using pure HTTP GET requests
      and the progressFile option is mandatory.

      A legacy method to create a dump loadable through PAR is still supported,
      this is done by using the ociParManifest option when creating the dump.
      When this is enabled, a manifest file "@.manifest.json" will be
      generated, to be used as the entry point to load the dump using a PAR to
      this file.

      When using a Manifest PAR to load a dump, the progressFile option is
      mandatory.

      To store the progress on dump location, create an ObjectReadWrite PAR to
      the desired progress file (it does not need to exist), it should be
      located on the same location of the "@.manifest.json" file. Finally
      specify the PAR URL on the progressFile option.

      Example:

      Dump Location: root of 'test' bucket:

      util.load_dump(

      'https://objectstorage.*.oraclecloud.com/p/*/n/main/b/test/o/@.manifest.json',
        { 'progressFile': 'load_progress.txt' }
      )

#@<OUT> util debug collect_diagnostics (full path)
NAME
      collect_diagnostics - Collects MySQL diagnostics information for
                            standalone and managed topologies

SYNTAX
      util.debug.collect_diagnostics(path[, options])

WHERE
      path: String - path to write the zip file with diagnostics information.
      options: Dictionary - Optional arguments.

DESCRIPTION
      A zip file containing diagnostics information collected from the server
      connected to and other members of the same topology.

      The following information is collected

      General - Error logs from performance_schema (if available) - Shell logs,
      configuration options and hostname where it is executed - InnoDB locks
      and metrics - InnoDB storage engine status and metrics - Names of tables
      without a Primary Key - Slow queries (if enabled) - Performance Schema
      configuration

      Replication/InnoDB Cluster - mysql_innodb_cluster_metadata schema and
      contents - Replication related tables in performance_schema - Replication
      status information - InnoDB Cluster accounts and their grants - InnoDB
      Cluster metadata schema - Output of ping for 5s - NDB Thread Blocks

      Schema Statistics - Number of schema objects (sys.schema_table_overview)
      - Top 20 biggest tables with index, blobs and partitioning information

      All members of a managed topology (e.g. InnoDB Cluster) will be scanned
      by default. If the `allMembers` option is set to False, then only the
      member the Shell is connected to is scanned.

      The options parameter accepts the following options:

      - allMembers: Bool - If true, collects information of all members of the
        topology, plus ping stats. Default false.
      - innodbMutex: Bool - If true, also collects output of SHOW ENGINE INNODB
        MUTEX. Disabled by default, as this command can have some impact on
        production performance.
      - schemaStats: Bool - If true, collects schema size statistics. Default
        false.
      - slowQueries: Bool - If true, collects slow query information. Requires
        slow_log to be enabled and configured for TABLE output. Default false.
      - ignoreErrors: Bool - If true, ignores query errors during collection.
        Default false.
      - customSql: Array - Custom list of SQL statements to execute.
      - customShell: Array - Custom list of shell commands to execute.

#@<OUT> util debug collect_diagnostics with util.debug.help (partial path)
NAME
      collect_diagnostics - Collects MySQL diagnostics information for
                            standalone and managed topologies

SYNTAX
      util.debug.collect_diagnostics(path[, options])

WHERE
      path: String - path to write the zip file with diagnostics information.
      options: Dictionary - Optional arguments.

DESCRIPTION
      A zip file containing diagnostics information collected from the server
      connected to and other members of the same topology.

      The following information is collected

      General - Error logs from performance_schema (if available) - Shell logs,
      configuration options and hostname where it is executed - InnoDB locks
      and metrics - InnoDB storage engine status and metrics - Names of tables
      without a Primary Key - Slow queries (if enabled) - Performance Schema
      configuration

      Replication/InnoDB Cluster - mysql_innodb_cluster_metadata schema and
      contents - Replication related tables in performance_schema - Replication
      status information - InnoDB Cluster accounts and their grants - InnoDB
      Cluster metadata schema - Output of ping for 5s - NDB Thread Blocks

      Schema Statistics - Number of schema objects (sys.schema_table_overview)
      - Top 20 biggest tables with index, blobs and partitioning information

      All members of a managed topology (e.g. InnoDB Cluster) will be scanned
      by default. If the `allMembers` option is set to False, then only the
      member the Shell is connected to is scanned.

      The options parameter accepts the following options:

      - allMembers: Bool - If true, collects information of all members of the
        topology, plus ping stats. Default false.
      - innodbMutex: Bool - If true, also collects output of SHOW ENGINE INNODB
        MUTEX. Disabled by default, as this command can have some impact on
        production performance.
      - schemaStats: Bool - If true, collects schema size statistics. Default
        false.
      - slowQueries: Bool - If true, collects slow query information. Requires
        slow_log to be enabled and configured for TABLE output. Default false.
      - ignoreErrors: Bool - If true, ignores query errors during collection.
        Default false.
      - customSql: Array - Custom list of SQL statements to execute.
      - customShell: Array - Custom list of shell commands to execute.

#@<OUT> util debug collect_high_load_diagnostics
NAME
      collect_high_load_diagnostics - Collects MySQL high load diagnostics
                                      information

SYNTAX
      util.debug.collect_high_load_diagnostics(path[, options])

WHERE
      path: String - path to write the zip file with diagnostics information.
      options: Dictionary - Optional arguments.

DESCRIPTION
      A zip file containing diagnostics information collected from the server
      connected to.

      The following information is collected

      General - Error logs from performance_schema or error log file (if
      available) - Shell configuration options and hostname where it is
      executed - Names of tables without a Primary Key - Benchmark info (SELECT
      BENCHMARK()) - Process list, open tables, host cache - System info
      collected from the OS (if connected to localhost)

      Schema Statistics - Number of schema objects (sys.schema_table_overview)
      - Full list of tables with basic stats - Top 20 biggest tables with
      index, blobs and partitioning information - Stored procedure and function
      stats

      The following performance metrics are collected multiple times, as
      specified by the `iterations` option.

      - InnoDB locks and metrics - InnoDB storage engine status and metrics -
        InnoDB buffer pool stats - InnoDB transaction info

      If the `pfsInstrumentation` option is set to a value other than
      'current', additional PERFORMANCE_SCHEMA instruments and consumers are
      enabled for the duration of the collection. If set to 'full', all
      instruments and consumers are enabled. If set to 'medium', non-history
      consumers and instruments other than wait/synch/% are enabled. Enabling
      additional instrumentation may provide additional insights, at the cost
      of an larger impact on server performance.

      The options parameter accepts the following options:

      - iterations: Integer - Number of iterations to collect perf stats
        (default 2).
      - delay: Integer - Number of seconds to wait between iterations (default
        5min).
      - innodbMutex: Bool - If true, also collects output of SHOW ENGINE INNODB
        MUTEX. Disabled by default, as this command can have some impact on
        production performance.
      - pfsInstrumentation: String - One of current, medium, full. Controls
        whether additional PERFORMANCE_SCHEMA instruments and consumers are
        temporarily enabled (default 'current').
      - customSql: Array - Custom list of SQL statements to execute. If the
        statement is prefixed with `before:` or nothing, it will be executed
        once, before the metrics collection loop. If prefixed with `after:`, it
        will be executed after the loop. If prefixed with `during:`, it will be
        executed once for each iteration of the collection loop.
      - customShell: Array - Custom list of shell commands to execute. If the
        command is prefixed with `before:` or nothing, it will be executed
        once, before the metrics collection loop. If prefixed with `after:`, it
        will be executed after the loop. If prefixed with `during:`, it will be
        executed once for each iteration of the collection loop.

#@<OUT> util debug collect_slow_query_diagnostics
NAME
      collect_slow_query_diagnostics - Collects MySQL diagnostics and profiling
                                       information for a slow query

SYNTAX
      util.debug.collect_slow_query_diagnostics(path, query[, options])

WHERE
      path: String - path to write the zip file with diagnostics information.
      query: String - query to be analyzed.
      options: Dictionary - Optional arguments.

DESCRIPTION
      A zip file containing diagnostics information collected from the server
      connected to.

      The given query is executed once. Performance metrics are collected
      during execution of the query.

      The following information is collected, in addition to what's collected
      by `util.debug.collectHighLoadDiagnostics()`

      - EXPLAIN output of the query - Optimizer trace of the query - DDL of
        tables used in the query - Warnings generated by the query

      If the `pfsInstrumentation` option is set to a value other than
      'current', additional PERFORMANCE_SCHEMA instruments and consumers are
      enabled for the duration of the collection. If set to 'full', all
      instruments and consumers are enabled. If set to 'medium', non-history
      consumers and instruments other than wait/synch/% are enabled. Enabling
      additional instrumentation may provide additional insights, at the cost
      of an larger impact on server performance.

      The options parameter accepts the following options:

      - delay: Integer - Number of seconds to wait between collection
        iterations (default 5s).
      - innodbMutex: Bool - If true, also collects output of SHOW ENGINE INNODB
        MUTEX. Disabled by default, as this command can have some impact on
        production performance.
      - pfsInstrumentation: String - One of current, medium, full. Controls
        whether additional PERFORMANCE_SCHEMA instruments and consumers are
        temporarily enabled (default 'current').
      - customSql: Array - Custom list of SQL statements to execute. If the
        statement is prefixed with `before:` or nothing, it will be executed
        once, before the metrics collection loop. If prefixed with `after:`, it
        will be executed after the loop. If prefixed with `during:`, it will be
        executed once for each iteration of the collection loop.
      - customShell: Array - Custom list of shell commands to execute. If the
        command is prefixed with `before:` or nothing, it will be executed
        once, before the metrics collection loop. If prefixed with `after:`, it
        will be executed after the loop. If prefixed with `during:`, it will be
        executed once for each iteration of the collection loop.
