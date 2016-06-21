#include <siri/cfg/cfg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <cfgparser/cfgparser.h>
#include <logger/logger.h>
#include <strextra/strextra.h>
#include <sys/resource.h>

/* do not use more than x percent for the max limit for open sharding files */
#define RLIMIT_PERC_FOR_SHARDING 0.5

#define MAX_OPEN_FILES_LIMIT 32768
#define MIN_OPEN_FILES_LIMIT 3
#define DEFAULT_OPEN_FILES_LIMIT MAX_OPEN_FILES_LIMIT

#define MIN_CHUNK_POINTS 10
#define MAX_CHUNK_POINTS 65535
#define DEFAULT_CHUNK_POINTS 800

static siri_cfg_t siri_cfg = {
        .listen_client_address="localhost",
        .listen_client_port=9000,
        .listen_backend_address="localhost",
        .listen_backend_port=9010,
        .optimize_interval=900,
        .heartbeat_interval=3,
        .default_db_path="/var/lib/siridb/",
        .max_open_files=DEFAULT_OPEN_FILES_LIMIT,
        .max_chunk_points=DEFAULT_CHUNK_POINTS
};

static void SIRI_CFG_read_interval(
        cfgparser_t * cfgparser,
        const char * option_name,
        uint16_t * interval);
static void SIRI_CFG_read_address_port(
        cfgparser_t * cfgparser,
        const char * option_name,
        char * address_pt,
        uint16_t * port_pt);
static void SIRI_CFG_read_default_db_path(cfgparser_t * cfgparser);
static void SIRI_CFG_read_max_open_files(cfgparser_t * cfgparser);
static void SIRI_CFG_read_max_chunk_points(cfgparser_t * cfgparser);

void siri_cfg_init(siri_t * siri)
{
    /* Read the application configuration file. */
    cfgparser_t * cfgparser = cfgparser_new();
    cfgparser_return_t rc;
    siri->cfg = &siri_cfg;
    rc = cfgparser_read(cfgparser, siri->args->config);
    if (rc != CFGPARSER_SUCCESS)
    {
        /* we could choose to continue with defaults but this is probably
         * not what users want so lets quit.
         */
        fprintf(stderr,
                "Could not read '%s': %s\n",
                siri->args->config,
                cfgparser_errmsg(rc));
        cfgparser_free(cfgparser);
        exit(EXIT_FAILURE);
    }

    SIRI_CFG_read_address_port(
            cfgparser,
            "listen_client",
            siri_cfg.listen_client_address,
            &siri_cfg.listen_client_port);

    SIRI_CFG_read_address_port(
            cfgparser,
            "listen_backend",
            siri_cfg.listen_backend_address,
            &siri_cfg.listen_backend_port);

    SIRI_CFG_read_interval(
            cfgparser,
            "optimize_interval",
            &siri_cfg.optimize_interval);

    SIRI_CFG_read_interval(
            cfgparser,
            "heartbeat_interval",
            &siri_cfg.heartbeat_interval);

    SIRI_CFG_read_default_db_path(cfgparser);
    SIRI_CFG_read_max_open_files(cfgparser);
    SIRI_CFG_read_max_chunk_points(cfgparser);

    cfgparser_free(cfgparser);
}

static void SIRI_CFG_read_interval(
        cfgparser_t * cfgparser,
        const char * option_name,
        uint16_t * interval)
{
    cfgparser_option_t * option;
    cfgparser_return_t rc;
    rc = cfgparser_get_option(
                &option,
                cfgparser,
                "siridb",
                option_name);

    if (rc != CFGPARSER_SUCCESS)
    {
        log_warning(
                "Error reading '[siridb]%s' in '%s': %s. "
                "Using default value: '%u'",
                option_name,
                siri.args->config,
                cfgparser_errmsg(rc),
                *interval);
    }
    else if (option->tp != CFGPARSER_TP_INTEGER)
    {
        log_warning(
                "Error reading '[siridb]%s' in '%s': %s. "
                "Using default value: '%u'",
                option_name,
                siri.args->config,
                "error: expecting an integer value",
                *interval);
    }
    else if (option->val->integer < 1 || option->val->integer > 65535)
    {
        log_warning(
                "Error reading '[siridb]%s' in '%s': "
                "error: port should be between 1 and 65535, got '%d'. "
                "Using default value: '%u'",
                option_name,
                siri.args->config,
                option->val->integer,
                *interval);
    }
    else
    {
        *interval = option->val->integer;
    }
}

static void SIRI_CFG_read_default_db_path(cfgparser_t * cfgparser)
{
    cfgparser_option_t * option;
    cfgparser_return_t rc;
    size_t len;
    rc = cfgparser_get_option(
                &option,
                cfgparser,
                "siridb",
                "default_db_path");
    if (rc != CFGPARSER_SUCCESS)
        log_warning(
                "Error reading '[siridb]%s' in '%s': %s. "
                "Using default value: '%s'",
                "default_db_path",
                siri.args->config,
                cfgparser_errmsg(rc),
                siri_cfg.default_db_path);
    else if (option->tp != CFGPARSER_TP_STRING)
        log_warning(
                "Error reading '[siridb]%s' in '%s': %s. "
                "Using default value: '%s:%d'",
                "default_db_path",
                siri.args->config,
                "error: expecting a string value",
                siri_cfg.default_db_path);
    else
    {
        strncpy(siri_cfg.default_db_path,
                option->val->string,
                PATH_MAX - 1);
        len = strlen(siri_cfg.default_db_path);

        /* add trailing slash (/) if its not already there */
        if (siri_cfg.default_db_path[len - 1] != '/')
        {
            siri_cfg.default_db_path[len] = '/';
            siri_cfg.default_db_path[len + 1] = 0;
        }
    }
}

static void SIRI_CFG_read_max_open_files(cfgparser_t * cfgparser)
{
    cfgparser_option_t * option;
    cfgparser_return_t rc;
    struct rlimit rlim;
    rc = cfgparser_get_option(
                &option,
                cfgparser,
                "sharding",
                "max_open_files");
    if (rc != CFGPARSER_SUCCESS || option->tp != CFGPARSER_TP_INTEGER)
    {
        log_info(
                "Using default value for [sharding]max_open_files: %d",
                siri_cfg.max_open_files);
    }
    else
    {
        siri_cfg.max_open_files = (uint16_t) option->val->integer;
    }

    if (siri_cfg.max_open_files < MIN_OPEN_FILES_LIMIT ||
            siri_cfg.max_open_files > MAX_OPEN_FILES_LIMIT)
    {
        log_warning(
                "Value max_open_files must be a value between %d and %d "
                "but we found %d. Using default value instead: %d",
                MIN_OPEN_FILES_LIMIT, MAX_OPEN_FILES_LIMIT,
                siri_cfg.max_open_files, DEFAULT_OPEN_FILES_LIMIT);
        siri_cfg.max_open_files = DEFAULT_OPEN_FILES_LIMIT;
    }

    getrlimit(RLIMIT_NOFILE, &rlim);

    uint16_t min_limit = (uint16_t)
            ((double) siri_cfg.max_open_files / RLIMIT_PERC_FOR_SHARDING) -1;

    if (min_limit > (uint64_t) rlim.rlim_max)
    {
        siri_cfg.max_open_files =
                (uint16_t) ((double) rlim.rlim_max * RLIMIT_PERC_FOR_SHARDING);
        log_warning(
                "We want to set a max-open-files value which "
                "exceeds %d%% of the current hard limit.\n\nWe "
                "will use %d as max_open_files for now.\n"
                "Please increase the hard-limit using:\n"
                "ulimit -Hn %d\n"
                "Note: when using supervisor to start SiriDB, "
                "update '/etc/supervisor/supervisord.conf' "
                "and set 'minfds=%d', in the [supervisord] "
                "section.",
                (uint8_t) (RLIMIT_PERC_FOR_SHARDING * 100),
                siri_cfg.max_open_files,
                min_limit, min_limit);
        min_limit = siri_cfg.max_open_files * 2;
    }

    if (min_limit > (uint64_t) rlim.rlim_cur)
    {
        log_info(
                "Increasing soft-limit from %d to %d since we want "
                "to use only %d%% from the soft-limit for shard files",
                (uint64_t) rlim.rlim_cur,
                min_limit,
                (uint8_t) (RLIMIT_PERC_FOR_SHARDING * 100));
        rlim.rlim_cur = min_limit;
        if (setrlimit(RLIMIT_NOFILE, &rlim))
        {
            log_critical("Could not set the soft-limit to %d", min_limit);
        }
    }
}

static void SIRI_CFG_read_max_chunk_points(cfgparser_t * cfgparser)
{
    cfgparser_option_t * option;
    cfgparser_return_t rc;
    rc = cfgparser_get_option(
                &option,
                cfgparser,
                "sharding",
                "max_chunk_points");
    if (rc != CFGPARSER_SUCCESS || option->tp != CFGPARSER_TP_INTEGER)
    {    log_info(
                "Using default value for [sharding]max_chunk_points: %d",
                siri_cfg.max_chunk_points);
    }
    else
    {
        siri_cfg.max_chunk_points = (uint16_t) option->val->integer;
    }

    if (siri_cfg.max_chunk_points < MIN_CHUNK_POINTS ||
            siri_cfg.max_chunk_points > MAX_CHUNK_POINTS)
    {
        log_warning(
                "Value max_chunk_points must be a value between %d and %d "
                "but we found %d. Using default value instead: %d",
                MIN_CHUNK_POINTS, MAX_CHUNK_POINTS,
                siri_cfg.max_chunk_points, DEFAULT_CHUNK_POINTS);
        siri_cfg.max_chunk_points = DEFAULT_CHUNK_POINTS;
    }

}

static void SIRI_CFG_read_address_port(
        cfgparser_t * cfgparser,
        const char * option_name,
        char * address_pt,
        uint16_t * port_pt)
{
    char * port;
    char * address;
    cfgparser_option_t * option;
    cfgparser_return_t rc;
    uint16_t test_port;

    rc = cfgparser_get_option(
                &option,
                cfgparser,
                "siridb",
                option_name);
    if (rc != CFGPARSER_SUCCESS)
    {
        log_warning(
                "Error reading '[siridb]%s' in '%s': %s. "
                "Using default value: '%s:%d'",
                option_name,
                siri.args->config,
                cfgparser_errmsg(rc),
                address_pt,
                *port_pt);
    }
    else if (option->tp != CFGPARSER_TP_STRING)
    {
        log_warning(
                "Error reading '[siridb]%s' in '%s': %s. "
                "Using default value: '%s:%d'",
                option_name,
                siri.args->config,
                "error: expecting a string value",
                address_pt,
                *port_pt);
    }
    else
    {
        for (port = address = option->val->string; *port; port++)
        {
            if (*port == ':')
            {
                *port = 0;
                port++;
                break;
            }
        }
        if (    !strlen(address) ||
                strlen(address) >= PATH_MAX ||
                !strx_is_int(port)
                )
        {
            log_warning(
                    "Error reading '[siridb]%s' in '%s': "
                    "error: got an unexpected value '%s:%s'. "
                    "Using default value: '%s:%d'",
                    option_name,
                    siri.args->config,
                    address,
                    port,
                    address_pt,
                    *port_pt);
        }
        else
        {
            if (*address == '*')
                strcpy(address_pt, "0.0.0.0");
            else
                strcpy(address_pt, address);

            test_port = atoi(port);

            if (test_port < 1 || test_port > 65535)
            {
                log_warning(
                        "Error reading '[siridb]%s' in '%s': "
                        "error: port should be between 1 and 65535, got '%d'. "
                        "Using default value: '%d'",
                        option_name,
                        siri.args->config,
                        test_port,
                        *port_pt);
            }
            else
            {
                *port_pt = test_port;
            }

            log_debug("Read '%s' from config: %s:%d",
                    option_name,
                    address_pt,
                    *port_pt);
        }

    }
}
