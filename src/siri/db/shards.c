/*
 * shards.c - Collection of SiriDB shards.
 *
 * Info shards->mutex:
 *
 *  Main thread:
 *      siridb->shards :    read (lock)         write (lock)

 *  Other threads:
 *      siridb->shards :    read (lock)         write (lock)
 *
 *  Note: since series->idx hold a reference to a shard, a lock to the
 *        series_mutex is required in some cases.
 */
#include <ctype.h>
#include <dirent.h>
#include <logger/logger.h>
#include <siri/db/shard.h>
#include <siri/db/shards.h>
#include <siri/db/series.inline.h>
#include <siri/db/misc.h>
#include <siri/siri.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <siri/db/db.h>
#include <xpath/xpath.h>
#include <omap/omap.h>

#define SIRIDB_SHARD_LEN 37


static bool SHARDS_read_id_and_duration(
        char * fn,
        const char * ext,
        uint64_t * shard_id,
        uint64_t * duration)
{
    size_t n = strlen(fn);
    char * tmp = NULL;

    if (n != SIRIDB_SHARD_LEN)
    {
        return false;
    }

    *shard_id = strtoull(fn, &tmp, 16);
    if (tmp == NULL)
    {
        return false;
    }
    fn = tmp;

    if (*fn != '_')
    {
        return false;
    }

    *duration = strtoull(fn, &tmp, 16);
    if (tmp == NULL)
    {
        return false;
    }
    fn = tmp;

    return strcmp(fn, ext) == 0;
}

/*
 * Returns true if fn is a temp shard or index filename, false if not.
 */
static bool SHARDS_is_temp_fn(char * fn)
{
    int i;
    uint64_t shard_id, duration;
    for (i = 0; i < 2; i++, fn++)
    {
        if (*fn != '_')
        {
            return false;
        }
    }

    return (
        SHARDS_read_id_and_duration(fn, ".sdb", &shard_id, &duration) ||
        SHARDS_read_id_and_duration(fn, ".idx", &shard_id, &duration)
    );
}


/*
 * Returns 0 if successful or -1 in case of an error.
 * (a SIGNAL might be raised in case of an error)
 */
int siridb_shards_load(siridb_t * siridb)
{
    struct stat st;
    struct dirent ** shard_list;
    char buffer[XPATH_MAX];
    int n, total, rc = 0;
    uint64_t shard_id, duration;

    memset(&st, 0, sizeof(struct stat));

    log_info("Loading shards");

    siridb_misc_get_fn(path, siridb->dbpath, SIRIDB_SHARDS_PATH);

    if (strlen(path) >= XPATH_MAX - SIRIDB_SHARD_LEN - 1)
    {
        log_error("Shard path too long: '%s'", path);
        return -1;
    }

    if (stat(path, &st) == -1)
    {
        log_warning(
                "Shards directory not found, creating directory '%s'.",
                path);
        if (mkdir(path, 0700) == -1)
        {
            log_error("Cannot create directory '%s'.", path);
            return -1;
        }
    }

    total = scandir(path, &shard_list, NULL, alphasort);

    if (total < 0)
    {
        /* no need to free shard_list when total < 0 */
        log_error("Cannot read shards directory '%s'.", path);
        return -1;
    }

    for (n = 0; n < total; n++)
    {
        if (SHARDS_is_temp_fn(shard_list[n]->d_name))
        {
            snprintf(buffer, XPATH_MAX, "%s%s",
                   path, shard_list[n]->d_name);

            log_warning("Removing temporary file: '%s'", buffer);

            if (unlink(buffer))
            {
                log_error("Error while removing temporary file: '%s'", buffer);
                rc = -1;
                break;
            }
        }

        if (!SHARDS_read_id_and_duration(
                shard_list[n]->d_name,
                ".sdb",
                &shard_id,
                &duration))
        {
            /* TODO: migration code, for backwards compatibility */
            continue;
        }

        /* we are sure this fits since the filename is checked */
        if (siridb_shard_load(siridb, shard_id, duration))
        {
           log_error("Error while loading shard: '%s'", shard_list[n]->d_name);
           rc = -1;
           break;
        }
    }

    while (total--)
    {
        free(shard_list[total]);
    }
    free(shard_list);

    return rc;
}

void siridb_shards_destroy_cb(omap_t * shards)
{
    omap_destroy(shards, (omap_destroy_cb) &siridb__shard_decref);
}

/*
 * Returns siri_err which is 0 if successful or a negative integer in case
 * of an error. (a SIGNAL is also raised in case of an error)
 */
int siridb_shards_add_points(
        siridb_t * siridb,
        siridb_series_t * series,
        siridb_points_t * points)
{
    _Bool is_num = siridb_series_isnum(series);
    siridb_shard_t * shard;
    omap_t * shards;
    uint64_t duration = siridb_series_duration(series);

    uv_mutex_lock(&siridb->values_mutex);

    uint64_t expire_at = is_num ? siridb->exp_at_num : siridb->exp_at_log;

    if (duration == 0)
    {
        uint64_t interval = siridb_points_get_interval(points);

        duration = interval
            ? siridb_shard_duration_from_interval(siridb, interval)
            : is_num
            ? siridb->duration_num
            : siridb->duration_log;
    }

    uv_mutex_unlock(&siridb->values_mutex);

    uint64_t shard_start, shard_end, shard_id;
    uint_fast32_t start, end, num_chunks, pstart, pend;
    uint16_t chunk_sz;
    uint16_t cinfo = 0;
    size_t size, pos;

    for (end = 0; end < points->len;)
    {
        shard_start = points->data[end].ts / duration * duration;
        shard_end = shard_start + duration;
        shard_id = shard_start + series->mask;

        for (   start = end;
                end < points->len && points->data[end].ts < shard_end;
                end++);

        if (shard_end < expire_at)
        {
            series->length -= end - start;
            series_update_start_end(series);
            continue;
        }

        shard = NULL;
        shards = imap_get(siridb->shards, shard_id);
        if (shards != NULL)
        {
            shard = omap_get(shards, duration);
            /* shard may be NULL if no shard according the duration is found */
        }
        else
        {
            shards = omap_create();
            if (shards == NULL || imap_add(siridb->shards, shard_id, shards))
            {
                ERR_ALLOC
                return -1;  /* might leak a few bytes */
            }
        }

        if (shard == NULL)
        {
            shard = siridb_shard_create(
                    siridb,
                    shards,
                    shard_id,
                    duration,
                    is_num ? SIRIDB_SHARD_TP_NUMBER : SIRIDB_SHARD_TP_LOG,
                    NULL);
            if (shard == NULL)
            {
                return -1;  /* signal is raised */
            }
        }

        if (start != end)
        {
            size = end - start;

            num_chunks = (size - 1) / shard->max_chunk_sz + 1;
            chunk_sz = size / num_chunks + (size % num_chunks != 0);

            for (pstart = start; pstart < end; pstart += chunk_sz)
            {
                pend = pstart + chunk_sz;
                if (pend > end)
                {
                    pend = end;
                }

                if ((pos = siridb_shard_write_points(
                        siridb,
                        series,
                        shard,
                        points,
                        pstart,
                        pend,
                        NULL,
                        &cinfo)) == 0)
                {
                    log_critical(
                            "Could not write points to shard '%s'",
                            shard->fn);
                }
                else
                {
                    siridb_series_add_idx(
                            series,
                            shard,
                            points->data[pstart].ts,
                            points->data[pend - 1].ts,
                            pos,
                            pend - pstart,
                            cinfo);
                    if (shard->replacing != NULL)
                    {
                        siridb_shard_write_points(
                               siridb,
                               series,
                               shard->replacing,
                               points,
                               pstart,
                               pend,
                               NULL,
                               &cinfo);
                    }
                }
            }
        }
    }
    return siri_err;
}

double siridb_shards_count_percent(
        siridb_t * siridb,
        uint64_t end_ts,
        uint8_t tp)
{
    size_t i;
    double percent = 1.0;
    vec_t * shards_list = NULL;
    size_t count = 0;
    size_t total = 0;
    uint64_t duration = tp == SIRIDB_SHARD_TP_NUMBER
            ? siridb->duration_num
            : siridb->duration_log;

    uv_mutex_lock(&siridb->shards_mutex);

    if (siridb->shards->len == 0)
    {
        percent = 0.0;
    }
    else
    {
        shards_list = imap_2vec_ref(siridb->shards);
    }

    uv_mutex_unlock(&siridb->shards_mutex);

    if (shards_list == NULL)
        return percent;

    for (i = 0; i < shards_list->len; i++)
    {
        siridb_shard_t * shard = (siridb_shard_t *) shards_list->data[i];
        if (shard->tp == tp)
        {
            ++total;
            count += ((shard->id - shard->id % duration) + duration) < end_ts;
        }
        siridb_shard_decref(shard);
    }

    percent = total ? (double) count / (double) total : 0.0;
    vec_free(shards_list);
    return percent;
}
