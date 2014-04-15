/*
 * Copyright (c) 2014 Jerry Lundström <lundstrom.jerry@gmail.com>
 * Copyright (c) 2014 .SE (The Internet Infrastructure Foundation).
 * Copyright (c) 2014 OpenDNSSEC AB (svb)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "key_data.h"
#include "db_error.h"

#include "mm.h"

#include <stdlib.h>
#include <string.h>

static const db_enum_t __enum_set_keyrole[] = {
    { "KSK", (key_data_keyrole_t)KEY_DATA_KEYROLE_KSK },
    { "ZSK", (key_data_keyrole_t)KEY_DATA_KEYROLE_ZSK },
    { "CSK", (key_data_keyrole_t)KEY_DATA_KEYROLE_CSK },
    { NULL, 0 }
};

static const db_enum_t __enum_set_dsatparent[] = {
    { "unsubmitted", (key_data_dsatparent_t)KEY_DATA_DSATPARENT_UNSUBMITTED },
    { "submit", (key_data_dsatparent_t)KEY_DATA_DSATPARENT_SUBMIT },
    { "submitted", (key_data_dsatparent_t)KEY_DATA_DSATPARENT_SUBMITTED },
    { "seen", (key_data_dsatparent_t)KEY_DATA_DSATPARENT_SEEN },
    { "retract", (key_data_dsatparent_t)KEY_DATA_DSATPARENT_RETRACT },
    { "retracted", (key_data_dsatparent_t)KEY_DATA_DSATPARENT_RETRACTED },
    { NULL, 0 }
};

/**
 * Create a new key data object.
 * \param[in] connection a db_connection_t pointer.
 * \return an key_data_t pointer or NULL on error.
 */
static db_object_t* __key_data_new_object(const db_connection_t* connection) {
    db_object_field_list_t* object_field_list;
    db_object_field_t* object_field;
    db_object_t* object;

    if (!(object = db_object_new())
        || db_object_set_connection(object, connection)
        || db_object_set_table(object, "KeyData")
        || db_object_set_primary_key_name(object, "id")
        || !(object_field_list = db_object_field_list_new()))
    {
        db_object_free(object);
        return NULL;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "id")
        || db_object_field_set_type(object_field, DB_TYPE_PRIMARY_KEY)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        db_object_free(object);
        return NULL;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "locator")
        || db_object_field_set_type(object_field, DB_TYPE_TEXT)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        db_object_free(object);
        return NULL;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "algorithm")
        || db_object_field_set_type(object_field, DB_TYPE_INT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        db_object_free(object);
        return NULL;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "inception")
        || db_object_field_set_type(object_field, DB_TYPE_INT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        db_object_free(object);
        return NULL;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "role")
        || db_object_field_set_type(object_field, DB_TYPE_ENUM)
        || db_object_field_set_enum_set(object_field, __enum_set_keyrole)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        db_object_free(object);
        return NULL;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "introducing")
        || db_object_field_set_type(object_field, DB_TYPE_INT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        db_object_free(object);
        return NULL;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "shouldrevoke")
        || db_object_field_set_type(object_field, DB_TYPE_INT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        db_object_free(object);
        return NULL;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "standby")
        || db_object_field_set_type(object_field, DB_TYPE_INT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        db_object_free(object);
        return NULL;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "active_zsk")
        || db_object_field_set_type(object_field, DB_TYPE_INT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        db_object_free(object);
        return NULL;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "publish")
        || db_object_field_set_type(object_field, DB_TYPE_INT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        db_object_free(object);
        return NULL;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "active_ksk")
        || db_object_field_set_type(object_field, DB_TYPE_INT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        db_object_free(object);
        return NULL;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "ds_at_parent")
        || db_object_field_set_type(object_field, DB_TYPE_ENUM)
        || db_object_field_set_enum_set(object_field, __enum_set_dsatparent)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        db_object_free(object);
        return NULL;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "keytag")
        || db_object_field_set_type(object_field, DB_TYPE_INT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        db_object_free(object);
        return NULL;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "ds")
        || db_object_field_set_type(object_field, DB_TYPE_INT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        db_object_free(object);
        return NULL;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "rrsig")
        || db_object_field_set_type(object_field, DB_TYPE_INT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        db_object_free(object);
        return NULL;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "dnskey")
        || db_object_field_set_type(object_field, DB_TYPE_INT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        db_object_free(object);
        return NULL;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "rrsigdnskey")
        || db_object_field_set_type(object_field, DB_TYPE_INT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        db_object_free(object);
        return NULL;
    }

    if (db_object_set_object_field_list(object, object_field_list)) {
        db_object_field_list_free(object_field_list);
        db_object_free(object);
        return NULL;
    }

    return object;
}

/* ENFORCER ZONE */

static mm_alloc_t __key_data_alloc = MM_ALLOC_T_STATIC_NEW(sizeof(key_data_t));

key_data_t* key_data_new(const db_connection_t* connection) {
    key_data_t* key_data =
        (key_data_t*)mm_alloc_new0(&__key_data_alloc);

    if (key_data) {
        if (!(key_data->dbo = __key_data_new_object(connection))) {
            mm_alloc_delete(&__key_data_alloc, key_data);
            return NULL;
        }
        key_data->role = KEY_DATA_KEYROLE_INVALID;
        key_data->ds_at_parent = KEY_DATA_DSATPARENT_UNSUBMITTED;
    }

    return key_data;
}

void key_data_free(key_data_t* key_data) {
    if (key_data) {
        if (key_data->dbo) {
            db_object_free(key_data->dbo);
        }
        if (key_data->locator) {
            free(key_data->locator);
        }
        key_data->introducing = 1;
        if (key_data->key_state_ds) {
            key_state_free(key_data->key_state_ds);
        }
        if (key_data->key_state_rrsig) {
            key_state_free(key_data->key_state_rrsig);
        }
        if (key_data->key_state_dnskey) {
            key_state_free(key_data->key_state_dnskey);
        }
        if (key_data->key_state_rrsigdnskey) {
            key_state_free(key_data->key_state_rrsigdnskey);
        }
        mm_alloc_delete(&__key_data_alloc, key_data);
    }
}

void key_data_reset(key_data_t* key_data) {
    if (key_data) {
        key_data->id = 0;
        if (key_data->locator) {
            free(key_data->locator);
        }
        key_data->locator = NULL;
        key_data->algorithm = 0;
        key_data->inception = 0;
        key_data->role = KEY_DATA_KEYROLE_INVALID;
        key_data->introducing = 1;
        key_data->shouldrevoke = 0;
        key_data->standby = 0;
        key_data->active_zsk = 0;
        key_data->publish = 0;
        key_data->active_ksk = 0;
        key_data->ds_at_parent = KEY_DATA_DSATPARENT_UNSUBMITTED;
        key_data->keytag = 0;
        key_data->ds = 0;
        key_data->rrsig = 0;
        key_data->dnskey = 0;
        key_data->rrsigdnskey = 0;
        if (key_data->key_state_ds) {
            key_state_free(key_data->key_state_ds);
        }
        key_data->key_state_ds = NULL;
        if (key_data->key_state_rrsig) {
            key_state_free(key_data->key_state_rrsig);
        }
        key_data->key_state_rrsig = NULL;
        if (key_data->key_state_dnskey) {
            key_state_free(key_data->key_state_dnskey);
        }
        key_data->key_state_dnskey = NULL;
        if (key_data->key_state_rrsigdnskey) {
            key_state_free(key_data->key_state_rrsigdnskey);
        }
        key_data->key_state_rrsigdnskey = NULL;
    }
}

int key_data_from_result(key_data_t* key_data, const db_result_t* result) {
    const db_value_set_t* value_set;
    int role;
    int ds_at_parent;

    if (!key_data) {
        return DB_ERROR_UNKNOWN;
    }
    if (!result) {
        return DB_ERROR_UNKNOWN;
    }

    key_data_reset(key_data);
    if (!(value_set = db_result_value_set(result))
        || db_value_set_size(value_set) != 17
        || db_value_to_int32(db_value_set_at(value_set, 0), &(key_data->id))
        || db_value_to_text(db_value_set_at(value_set, 1), &(key_data->locator))
        || db_value_to_int32(db_value_set_at(value_set, 2), &(key_data->algorithm))
        || db_value_to_int32(db_value_set_at(value_set, 3), &(key_data->inception))
        || db_value_to_enum_value(db_value_set_at(value_set, 4), &role, __enum_set_keyrole)
        || db_value_to_int32(db_value_set_at(value_set, 5), &(key_data->introducing))
        || db_value_to_int32(db_value_set_at(value_set, 6), &(key_data->shouldrevoke))
        || db_value_to_int32(db_value_set_at(value_set, 7), &(key_data->standby))
        || db_value_to_int32(db_value_set_at(value_set, 8), &(key_data->active_zsk))
        || db_value_to_int32(db_value_set_at(value_set, 9), &(key_data->publish))
        || db_value_to_int32(db_value_set_at(value_set, 10), &(key_data->active_ksk))
        || db_value_to_enum_value(db_value_set_at(value_set, 11), &ds_at_parent, __enum_set_dsatparent)
        || db_value_to_int32(db_value_set_at(value_set, 12), &(key_data->keytag))
        || db_value_to_int32(db_value_set_at(value_set, 13), &(key_data->ds))
        || db_value_to_int32(db_value_set_at(value_set, 14), &(key_data->rrsig))
        || db_value_to_int32(db_value_set_at(value_set, 15), &(key_data->dnskey))
        || db_value_to_int32(db_value_set_at(value_set, 16), &(key_data->rrsigdnskey)))
    {
        return DB_ERROR_UNKNOWN;
    }

    if (role == (key_data_keyrole_t)KEY_DATA_KEYROLE_KSK) {
        key_data->role = KEY_DATA_KEYROLE_KSK;
    }
    else if (role == (key_data_keyrole_t)KEY_DATA_KEYROLE_ZSK) {
        key_data->role = KEY_DATA_KEYROLE_ZSK;
    }
    else if (role == (key_data_keyrole_t)KEY_DATA_KEYROLE_CSK) {
        key_data->role = KEY_DATA_KEYROLE_CSK;
    }
    else {
        return DB_ERROR_UNKNOWN;
    }

    if (ds_at_parent == (key_data_dsatparent_t)KEY_DATA_DSATPARENT_UNSUBMITTED) {
        key_data->ds_at_parent = KEY_DATA_DSATPARENT_UNSUBMITTED;
    }
    else if (ds_at_parent == (key_data_dsatparent_t)KEY_DATA_DSATPARENT_SUBMIT) {
        key_data->ds_at_parent = KEY_DATA_DSATPARENT_SUBMIT;
    }
    else if (ds_at_parent == (key_data_dsatparent_t)KEY_DATA_DSATPARENT_SUBMITTED) {
        key_data->ds_at_parent = KEY_DATA_DSATPARENT_SUBMITTED;
    }
    else if (ds_at_parent == (key_data_dsatparent_t)KEY_DATA_DSATPARENT_SEEN) {
        key_data->ds_at_parent = KEY_DATA_DSATPARENT_SEEN;
    }
    else if (ds_at_parent == (key_data_dsatparent_t)KEY_DATA_DSATPARENT_RETRACT) {
        key_data->ds_at_parent = KEY_DATA_DSATPARENT_RETRACT;
    }
    else if (ds_at_parent == (key_data_dsatparent_t)KEY_DATA_DSATPARENT_RETRACTED) {
        key_data->ds_at_parent = KEY_DATA_DSATPARENT_RETRACTED;
    }
    else {
        return DB_ERROR_UNKNOWN;
    }

    return DB_OK;
}

int key_data_id(const key_data_t* key_data) {
    if (!key_data) {
        return DB_ERROR_UNKNOWN;
    }

    return key_data->id;
}

const char* key_data_locator(const key_data_t* key_data) {
    if (!key_data) {
        return NULL;
    }

    return key_data->locator;
}

int key_data_algorithm(const key_data_t* key_data) {
    if (!key_data) {
        return DB_ERROR_UNKNOWN;
    }

    return key_data->algorithm;
}

int key_data_inception(const key_data_t* key_data) {
    if (!key_data) {
        return DB_ERROR_UNKNOWN;
    }

    return key_data->inception;
}

key_data_keyrole_t key_data_role(const key_data_t* key_data) {
    if (!key_data) {
        return KEY_DATA_KEYROLE_INVALID;
    }

    return key_data->role;
}

const char* key_data_role_text(const key_data_t* key_data) {
    const db_enum_t* enum_set = __enum_set_keyrole;

    if (!key_data) {
        return NULL;
    }

    while (enum_set->text) {
        if (enum_set->value == key_data->role) {
            return enum_set->text;
        }
        enum_set++;
    }
    return NULL;
}

int key_data_introducing(const key_data_t* key_data) {
    if (!key_data) {
        return DB_ERROR_UNKNOWN;
    }

    return key_data->introducing;
}

int key_data_shouldrevoke(const key_data_t* key_data) {
    if (!key_data) {
        return DB_ERROR_UNKNOWN;
    }

    return key_data->shouldrevoke;
}

int key_data_standby(const key_data_t* key_data) {
    if (!key_data) {
        return DB_ERROR_UNKNOWN;
    }

    return key_data->standby;
}

int key_data_active_zsk(const key_data_t* key_data) {
    if (!key_data) {
        return DB_ERROR_UNKNOWN;
    }

    return key_data->active_zsk;
}

int key_data_publish(const key_data_t* key_data) {
    if (!key_data) {
        return DB_ERROR_UNKNOWN;
    }

    return key_data->publish;
}

int key_data_active_ksk(const key_data_t* key_data) {
    if (!key_data) {
        return DB_ERROR_UNKNOWN;
    }

    return key_data->active_ksk;
}

key_data_dsatparent_t key_data_ds_at_parent(const key_data_t* key_data) {
    if (!key_data) {
        return KEY_DATA_DSATPARENT_INVALID;
    }

    return key_data->ds_at_parent;
}

const char* key_data_ds_at_parent_text(const key_data_t* key_data) {
    const db_enum_t* enum_set = __enum_set_dsatparent;

    if (!key_data) {
        return NULL;
    }

    while (enum_set->text) {
        if (enum_set->value == key_data->ds_at_parent) {
            return enum_set->text;
        }
        enum_set++;
    }
    return NULL;
}

int key_data_set_locator(key_data_t* key_data, const char* locator) {
    char* new_locator;

    if (!key_data) {
        return DB_ERROR_UNKNOWN;
    }
    if (!locator) {
        return DB_ERROR_UNKNOWN;
    }

    if (!(new_locator = strdup(locator))) {
        return DB_ERROR_UNKNOWN;
    }

    if (key_data->locator) {
        free(key_data->locator);
    }
    key_data->locator = new_locator;

    return DB_OK;
}

int key_data_set_algorithm(key_data_t* key_data, int algorithm) {
    if (!key_data) {
        return DB_ERROR_UNKNOWN;
    }

    key_data->algorithm = algorithm;

    return DB_OK;
}

int key_data_set_inception(key_data_t* key_data, int inception) {
    if (!key_data) {
        return DB_ERROR_UNKNOWN;
    }

    key_data->inception = inception;

    return DB_OK;
}

int key_data_set_role(key_data_t* key_data, key_data_keyrole_t role) {
    if (!key_data) {
        return DB_ERROR_UNKNOWN;
    }

    key_data->role = role;

    return DB_OK;
}

int key_data_set_role_text(key_data_t* key_data, const char* role) {
    const db_enum_t* enum_set = __enum_set_keyrole;

    if (!key_data) {
        return DB_ERROR_UNKNOWN;
    }

    while (enum_set->text) {
        if (!strcmp(enum_set->text, role)) {
            key_data->role = enum_set->value;
            return DB_OK;
        }
        enum_set++;
    }
    return DB_ERROR_UNKNOWN;
}

int key_data_set_introducing(key_data_t* key_data, int introducing) {
    if (!key_data) {
        return DB_ERROR_UNKNOWN;
    }

    key_data->introducing = introducing;

    return DB_OK;
}

int key_data_set_shouldrevoke(key_data_t* key_data, int shouldrevoke) {
    if (!key_data) {
        return DB_ERROR_UNKNOWN;
    }

    key_data->shouldrevoke = shouldrevoke;

    return DB_OK;
}

int key_data_set_standby(key_data_t* key_data, int standby) {
    if (!key_data) {
        return DB_ERROR_UNKNOWN;
    }

    key_data->standby = standby;

    return DB_OK;
}

int key_data_set_active_zsk(key_data_t* key_data, int active_zsk) {
    if (!key_data) {
        return DB_ERROR_UNKNOWN;
    }

    key_data->active_zsk = active_zsk;

    return DB_OK;
}

int key_data_set_publish(key_data_t* key_data, int publish) {
    if (!key_data) {
        return DB_ERROR_UNKNOWN;
    }

    key_data->publish = publish;

    return DB_OK;
}

int key_data_set_active_ksk(key_data_t* key_data, int active_ksk) {
    if (!key_data) {
        return DB_ERROR_UNKNOWN;
    }

    key_data->active_ksk = active_ksk;

    return DB_OK;
}

int key_data_set_ds_at_parent(key_data_t* key_data, key_data_dsatparent_t ds_at_parent) {
    if (!key_data) {
        return DB_ERROR_UNKNOWN;
    }

    key_data->ds_at_parent = ds_at_parent;

    return DB_OK;
}

int key_data_set_ds_at_parent_text(key_data_t* key_data, const char* ds_at_parent) {
    const db_enum_t* enum_set = __enum_set_dsatparent;

    if (!key_data) {
        return DB_ERROR_UNKNOWN;
    }

    while (enum_set->text) {
        if (!strcmp(enum_set->text, ds_at_parent)) {
            key_data->ds_at_parent = enum_set->value;
            return DB_OK;
        }
        enum_set++;
    }
    return DB_ERROR_UNKNOWN;
}

int key_data_get_key_state_list(key_data_t* key_data) {
    key_state_list_t* key_state_list;
    const key_state_t* key_state;
    key_state_t* key_state_ds = NULL;
    key_state_t* key_state_rrsig = NULL;
    key_state_t* key_state_dnskey = NULL;
    key_state_t* key_state_rrsigdnskey = NULL;

    if (!key_data) {
        return DB_ERROR_UNKNOWN;
    }
    if (!key_data->dbo) {
        return DB_ERROR_UNKNOWN;
    }
    if (!key_data->ds) {
        return DB_ERROR_UNKNOWN;
    }
    if (!key_data->rrsig) {
        return DB_ERROR_UNKNOWN;
    }
    if (!key_data->dnskey) {
        return DB_ERROR_UNKNOWN;
    }
    if (!key_data->rrsigdnskey) {
        return DB_ERROR_UNKNOWN;
    }

    key_state_list = key_state_list_new(db_object_connection(key_data->dbo));
    if (!key_state_list) {
        return DB_ERROR_UNKNOWN;
    }
    if (key_state_list_get_4_by_id(key_state_list, key_data->ds, key_data->rrsig, key_data->dnskey, key_data->rrsigdnskey)) {
        key_state_list_free(key_state_list);
        return DB_ERROR_UNKNOWN;
    }

    key_state = key_state_list_begin(key_state_list);
    while (key_state) {
        if (key_state_id(key_state) == key_data->ds) {
            if (!(key_state_ds = key_state_new(db_object_connection(key_data->dbo)))
                || key_state_copy(key_state_ds, key_state))
            {
                key_state_free(key_state_ds);
                key_state_free(key_state_rrsig);
                key_state_free(key_state_dnskey);
                key_state_free(key_state_rrsigdnskey);
                key_state_list_free(key_state_list);
                return DB_ERROR_UNKNOWN;
            }
        }
        else if (key_state_id(key_state) == key_data->rrsig) {
            if (!(key_state_rrsig = key_state_new(db_object_connection(key_data->dbo)))
                || key_state_copy(key_state_rrsig, key_state))
            {
                key_state_free(key_state_ds);
                key_state_free(key_state_rrsig);
                key_state_free(key_state_dnskey);
                key_state_free(key_state_rrsigdnskey);
                key_state_list_free(key_state_list);
                return DB_ERROR_UNKNOWN;
            }
        }
        else if (key_state_id(key_state) == key_data->dnskey) {
            if (!(key_state_dnskey = key_state_new(db_object_connection(key_data->dbo)))
                || key_state_copy(key_state_dnskey, key_state))
            {
                key_state_free(key_state_ds);
                key_state_free(key_state_rrsig);
                key_state_free(key_state_dnskey);
                key_state_free(key_state_rrsigdnskey);
                key_state_list_free(key_state_list);
                return DB_ERROR_UNKNOWN;
            }
        }
        else if (key_state_id(key_state) == key_data->rrsigdnskey) {
            if (!(key_state_rrsigdnskey = key_state_new(db_object_connection(key_data->dbo)))
                || key_state_copy(key_state_rrsigdnskey, key_state))
            {
                key_state_free(key_state_ds);
                key_state_free(key_state_rrsig);
                key_state_free(key_state_dnskey);
                key_state_free(key_state_rrsigdnskey);
                key_state_list_free(key_state_list);
                return DB_ERROR_UNKNOWN;
            }
        }
        key_state = key_state_list_next(key_state_list);
    }
    key_state_list_free(key_state_list);

    if (!key_state_ds || !key_state_rrsig || !key_state_dnskey || !key_state_rrsigdnskey) {
        key_state_free(key_state_ds);
        key_state_free(key_state_rrsig);
        key_state_free(key_state_dnskey);
        key_state_free(key_state_rrsigdnskey);
        return DB_ERROR_UNKNOWN;
    }

    if (key_data->key_state_ds) {
        key_state_free(key_data->key_state_ds);
    }
    key_data->key_state_ds = key_state_ds;
    if (key_data->key_state_rrsig) {
        key_state_free(key_data->key_state_rrsig);
    }
    key_data->key_state_rrsig = key_state_rrsig;
    if (key_data->key_state_dnskey) {
        key_state_free(key_data->key_state_dnskey);
    }
    key_data->key_state_dnskey = key_state_dnskey;
    if (key_data->key_state_rrsigdnskey) {
        key_state_free(key_data->key_state_rrsigdnskey);
    }
    key_data->key_state_rrsigdnskey = key_state_rrsigdnskey;
    return DB_OK;
}

const key_state_t* key_data_get_ds(key_data_t* key_data) {
    if (!key_data) {
        return NULL;
    }
    if (!key_data->dbo) {
        return NULL;
    }
    if (!key_data->ds) {
        return NULL;
    }

    if (!key_data->key_state_ds) {
        key_data->key_state_ds = key_state_new(db_object_connection(key_data->dbo));
        if (key_data->key_state_ds) {
            if (key_state_get_by_id(key_data->key_state_ds, key_data->ds)) {
                key_state_free(key_data->key_state_ds);
                key_data->key_state_ds = NULL;
                return NULL;
            }
        }
    }
    return key_data->key_state_ds;
}

const key_state_t* key_data_get_rrsig(key_data_t* key_data) {
    if (!key_data) {
        return NULL;
    }
    if (!key_data->dbo) {
        return NULL;
    }
    if (!key_data->rrsig) {
        return NULL;
    }

    if (!key_data->key_state_rrsig) {
        key_data->key_state_rrsig = key_state_new(db_object_connection(key_data->dbo));
        if (key_data->key_state_rrsig) {
            if (key_state_get_by_id(key_data->key_state_rrsig, key_data->rrsig)) {
                key_state_free(key_data->key_state_rrsig);
                key_data->key_state_rrsig = NULL;
                return NULL;
            }
        }
    }
    return key_data->key_state_rrsig;
}

const key_state_t* key_data_get_dnskey(key_data_t* key_data) {
    if (!key_data) {
        return NULL;
    }
    if (!key_data->dbo) {
        return NULL;
    }
    if (!key_data->dnskey) {
        return NULL;
    }

    if (!key_data->key_state_dnskey) {
        key_data->key_state_dnskey = key_state_new(db_object_connection(key_data->dbo));
        if (key_data->key_state_dnskey) {
            if (key_state_get_by_id(key_data->key_state_dnskey, key_data->dnskey)) {
                key_state_free(key_data->key_state_dnskey);
                key_data->key_state_dnskey = NULL;
                return NULL;
            }
        }
    }
    return key_data->key_state_dnskey;
}

const key_state_t* key_data_get_rrsigdnskey(key_data_t* key_data) {
    if (!key_data) {
        return NULL;
    }
    if (!key_data->dbo) {
        return NULL;
    }
    if (!key_data->rrsigdnskey) {
        return NULL;
    }

    if (!key_data->key_state_rrsigdnskey) {
        key_data->key_state_rrsigdnskey = key_state_new(db_object_connection(key_data->dbo));
        if (key_data->key_state_rrsigdnskey) {
            if (key_state_get_by_id(key_data->key_state_rrsigdnskey, key_data->rrsigdnskey)) {
                key_state_free(key_data->key_state_rrsigdnskey);
                key_data->key_state_rrsigdnskey = NULL;
                return NULL;
            }
        }
    }
    return key_data->key_state_rrsigdnskey;
}

int key_data_create(key_data_t* key_data) {
    db_object_field_list_t* object_field_list;
    db_object_field_t* object_field;
    db_value_set_t* value_set;
    int ret;

    if (!key_data) {
        return DB_ERROR_UNKNOWN;
    }
    if (!key_data->dbo) {
        return DB_ERROR_UNKNOWN;
    }
    if (key_data->id) {
        return DB_ERROR_UNKNOWN;
    }
    if (!key_data->locator) {
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field_list = db_object_field_list_new())) {
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "locator")
        || db_object_field_set_type(object_field, DB_TYPE_TEXT)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "algorithm")
        || db_object_field_set_type(object_field, DB_TYPE_INT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "inception")
        || db_object_field_set_type(object_field, DB_TYPE_INT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "role")
        || db_object_field_set_type(object_field, DB_TYPE_ENUM)
        || db_object_field_set_enum_set(object_field, __enum_set_keyrole)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "introducing")
        || db_object_field_set_type(object_field, DB_TYPE_INT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "shouldrevoke")
        || db_object_field_set_type(object_field, DB_TYPE_INT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "standby")
        || db_object_field_set_type(object_field, DB_TYPE_INT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "active_zsk")
        || db_object_field_set_type(object_field, DB_TYPE_INT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "publish")
        || db_object_field_set_type(object_field, DB_TYPE_INT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "active_ksk")
        || db_object_field_set_type(object_field, DB_TYPE_INT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "ds_at_parent")
        || db_object_field_set_type(object_field, DB_TYPE_ENUM)
        || db_object_field_set_enum_set(object_field, __enum_set_dsatparent)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "keytag")
        || db_object_field_set_type(object_field, DB_TYPE_INT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "ds")
        || db_object_field_set_type(object_field, DB_TYPE_INT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "rrsig")
        || db_object_field_set_type(object_field, DB_TYPE_INT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "dnskey")
        || db_object_field_set_type(object_field, DB_TYPE_INT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "rrsigdnskey")
        || db_object_field_set_type(object_field, DB_TYPE_INT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(value_set = db_value_set_new(16))) {
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (db_value_from_text(db_value_set_get(value_set, 0), key_data->locator)
        || db_value_from_int32(db_value_set_get(value_set, 1), key_data->algorithm)
        || db_value_from_int32(db_value_set_get(value_set, 2), key_data->inception)
        || db_value_from_enum_value(db_value_set_get(value_set, 3), key_data->role, __enum_set_keyrole)
        || db_value_from_int32(db_value_set_get(value_set, 4), key_data->introducing)
        || db_value_from_int32(db_value_set_get(value_set, 5), key_data->shouldrevoke)
        || db_value_from_int32(db_value_set_get(value_set, 6), key_data->standby)
        || db_value_from_int32(db_value_set_get(value_set, 7), key_data->active_zsk)
        || db_value_from_int32(db_value_set_get(value_set, 8), key_data->publish)
        || db_value_from_int32(db_value_set_get(value_set, 9), key_data->active_ksk)
        || db_value_from_enum_value(db_value_set_get(value_set, 10), key_data->ds_at_parent, __enum_set_dsatparent)
        || db_value_from_int32(db_value_set_get(value_set, 11), key_data->keytag)
        || db_value_from_int32(db_value_set_get(value_set, 12), key_data->ds)
        || db_value_from_int32(db_value_set_get(value_set, 13), key_data->rrsig)
        || db_value_from_int32(db_value_set_get(value_set, 14), key_data->dnskey)
        || db_value_from_int32(db_value_set_get(value_set, 15), key_data->rrsigdnskey))
    {
        db_value_set_free(value_set);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    ret = db_object_create(key_data->dbo, object_field_list, value_set);
    db_value_set_free(value_set);
    db_object_field_list_free(object_field_list);
    return ret;
}

int key_data_get_by_id(key_data_t* key_data, int id) {
    db_clause_list_t* clause_list;
    db_clause_t* clause;
    db_result_list_t* result_list;
    const db_result_t* result;

    if (!key_data) {
        return DB_ERROR_UNKNOWN;
    }
    if (!key_data->dbo) {
        return DB_ERROR_UNKNOWN;
    }

    if (!(clause_list = db_clause_list_new())) {
        return DB_ERROR_UNKNOWN;
    }

    if (!(clause = db_clause_new())
        || db_clause_set_field(clause, "id")
        || db_clause_set_type(clause, DB_CLAUSE_EQUAL)
        || db_value_from_int32(db_clause_get_value(clause), id)
        || db_clause_list_add(clause_list, clause))
    {
        db_clause_free(clause);
        db_clause_list_free(clause_list);
        return DB_ERROR_UNKNOWN;
    }

    result_list = db_object_read(key_data->dbo, NULL, clause_list);
    db_clause_list_free(clause_list);

    if (result_list) {
        result = db_result_list_begin(result_list);
        if (result) {
            if (db_result_list_next(result_list)) {
                db_result_list_free(result_list);
                return DB_ERROR_UNKNOWN;
            }

            key_data_from_result(key_data, result);
            db_result_list_free(result_list);
            return DB_OK;
        }
    }

    db_result_list_free(result_list);
    return DB_ERROR_UNKNOWN;
}

int key_data_update(key_data_t* key_data) {
    db_object_field_list_t* object_field_list;
    db_object_field_t* object_field;
    db_value_set_t* value_set;
    db_clause_list_t* clause_list;
    db_clause_t* clause;
    int ret;

    if (!key_data) {
        return DB_ERROR_UNKNOWN;
    }
    if (!key_data->dbo) {
        return DB_ERROR_UNKNOWN;
    }
    if (!key_data->id) {
        return DB_ERROR_UNKNOWN;
    }
    if (!key_data->locator) {
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field_list = db_object_field_list_new())) {
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "locator")
        || db_object_field_set_type(object_field, DB_TYPE_TEXT)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "algorithm")
        || db_object_field_set_type(object_field, DB_TYPE_INT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "inception")
        || db_object_field_set_type(object_field, DB_TYPE_INT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "role")
        || db_object_field_set_type(object_field, DB_TYPE_ENUM)
        || db_object_field_set_enum_set(object_field, __enum_set_keyrole)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "introducing")
        || db_object_field_set_type(object_field, DB_TYPE_INT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "shouldrevoke")
        || db_object_field_set_type(object_field, DB_TYPE_INT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "standby")
        || db_object_field_set_type(object_field, DB_TYPE_INT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "active_zsk")
        || db_object_field_set_type(object_field, DB_TYPE_INT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "publish")
        || db_object_field_set_type(object_field, DB_TYPE_INT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "active_ksk")
        || db_object_field_set_type(object_field, DB_TYPE_INT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "ds_at_parent")
        || db_object_field_set_type(object_field, DB_TYPE_ENUM)
        || db_object_field_set_enum_set(object_field, __enum_set_dsatparent)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "keytag")
        || db_object_field_set_type(object_field, DB_TYPE_INT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "ds")
        || db_object_field_set_type(object_field, DB_TYPE_INT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "rrsig")
        || db_object_field_set_type(object_field, DB_TYPE_INT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "dnskey")
        || db_object_field_set_type(object_field, DB_TYPE_INT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "rrsigdnskey")
        || db_object_field_set_type(object_field, DB_TYPE_INT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(value_set = db_value_set_new(16))) {
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (db_value_from_text(db_value_set_get(value_set, 0), key_data->locator)
        || db_value_from_int32(db_value_set_get(value_set, 1), key_data->algorithm)
        || db_value_from_int32(db_value_set_get(value_set, 2), key_data->inception)
        || db_value_from_enum_value(db_value_set_get(value_set, 3), key_data->role, __enum_set_keyrole)
        || db_value_from_int32(db_value_set_get(value_set, 4), key_data->introducing)
        || db_value_from_int32(db_value_set_get(value_set, 5), key_data->shouldrevoke)
        || db_value_from_int32(db_value_set_get(value_set, 6), key_data->standby)
        || db_value_from_int32(db_value_set_get(value_set, 7), key_data->active_zsk)
        || db_value_from_int32(db_value_set_get(value_set, 8), key_data->publish)
        || db_value_from_int32(db_value_set_get(value_set, 9), key_data->active_ksk)
        || db_value_from_enum_value(db_value_set_get(value_set, 10), key_data->ds_at_parent, __enum_set_dsatparent)
        || db_value_from_int32(db_value_set_get(value_set, 11), key_data->keytag)
        || db_value_from_int32(db_value_set_get(value_set, 12), key_data->ds)
        || db_value_from_int32(db_value_set_get(value_set, 13), key_data->rrsig)
        || db_value_from_int32(db_value_set_get(value_set, 14), key_data->dnskey)
        || db_value_from_int32(db_value_set_get(value_set, 15), key_data->rrsigdnskey))
    {
        db_value_set_free(value_set);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(clause_list = db_clause_list_new())) {
        db_value_set_free(value_set);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(clause = db_clause_new())
        || db_clause_set_field(clause, "id")
        || db_clause_set_type(clause, DB_CLAUSE_EQUAL)
        || db_value_from_int32(db_clause_get_value(clause), key_data->id)
        || db_clause_list_add(clause_list, clause))
    {
        db_clause_free(clause);
        db_clause_list_free(clause_list);
        db_value_set_free(value_set);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    ret = db_object_update(key_data->dbo, object_field_list, value_set, clause_list);
    db_value_set_free(value_set);
    db_object_field_list_free(object_field_list);
    db_clause_list_free(clause_list);
    return ret;
}

int key_data_delete(key_data_t* key_data) {
    db_clause_list_t* clause_list;
    db_clause_t* clause;
    int ret;

    if (!key_data) {
        return DB_ERROR_UNKNOWN;
    }
    if (!key_data->dbo) {
        return DB_ERROR_UNKNOWN;
    }
    if (!key_data->id) {
        return DB_ERROR_UNKNOWN;
    }

    if (!(clause_list = db_clause_list_new())) {
        return DB_ERROR_UNKNOWN;
    }

    if (!(clause = db_clause_new())
        || db_clause_set_field(clause, "id")
        || db_clause_set_type(clause, DB_CLAUSE_EQUAL)
        || db_value_from_int32(db_clause_get_value(clause), key_data->id)
        || db_clause_list_add(clause_list, clause))
    {
        db_clause_free(clause);
        db_clause_list_free(clause_list);
        return DB_ERROR_UNKNOWN;
    }

    ret = db_object_delete(key_data->dbo, clause_list);
    db_clause_list_free(clause_list);
    return ret;
}

/* ENFORCER ZONE LIST */

static mm_alloc_t __key_data_list_alloc = MM_ALLOC_T_STATIC_NEW(sizeof(key_data_list_t));

key_data_list_t* key_data_list_new(const db_connection_t* connection) {
    key_data_list_t* key_data_list =
        (key_data_list_t*)mm_alloc_new0(&__key_data_list_alloc);

    if (key_data_list) {
        if (!(key_data_list->dbo = __key_data_new_object(connection))) {
            mm_alloc_delete(&__key_data_list_alloc, key_data_list);
            return NULL;
        }
    }

    return key_data_list;
}

void key_data_list_free(key_data_list_t* key_data_list) {
    if (key_data_list) {
        if (key_data_list->dbo) {
            db_object_free(key_data_list->dbo);
        }
        if (key_data_list->result_list) {
            db_result_list_free(key_data_list->result_list);
        }
        if (key_data_list->key_data) {
            key_data_free(key_data_list->key_data);
        }
        mm_alloc_delete(&__key_data_list_alloc, key_data_list);
    }
}

int key_data_list_get_by_enforcer_zone_id(key_data_list_t* key_data_list, int enforcer_zone_id) {
    db_join_list_t* join_list;
    db_join_t* join;
    db_clause_list_t* clause_list;
    db_clause_t* clause;

    if (!key_data_list) {
        return DB_ERROR_UNKNOWN;
    }

    if (!(join_list = db_join_list_new())) {
        return DB_ERROR_UNKNOWN;
    }
    if (!(join = db_join_new())
        || db_join_set_from_table(join, "KeyData")
        || db_join_set_from_field(join, "id")
        || db_join_set_to_table(join, "EnforcerZone_keys")
        || db_join_set_to_field(join, "child_id")
        || db_join_list_add(join_list, join))
    {
        db_join_free(join);
        db_join_list_free(join_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(clause_list = db_clause_list_new())) {
        db_join_list_free(join_list);
        return DB_ERROR_UNKNOWN;
    }
    if (!(clause = db_clause_new())
        || db_clause_set_table(clause, "EnforcerZone_keys")
        || db_clause_set_field(clause, "parent_id")
        || db_clause_set_type(clause, DB_CLAUSE_EQUAL)
        || db_value_from_int32(db_clause_get_value(clause), enforcer_zone_id)
        || db_clause_list_add(clause_list, clause))
    {
        db_join_list_free(join_list);
        db_clause_free(clause);
        db_clause_list_free(clause_list);
        return DB_ERROR_UNKNOWN;
    }

    if (key_data_list->result_list) {
        db_result_list_free(key_data_list->result_list);
    }

    key_data_list->result_list = db_object_read(key_data_list->dbo, join_list, clause_list);

    db_join_list_free(join_list);
    db_clause_list_free(clause_list);

    if (!key_data_list->result_list) {
        return DB_ERROR_UNKNOWN;
    }
    return DB_OK;
}

const key_data_t* key_data_list_begin(key_data_list_t* key_data_list) {
    const db_result_t* result;

    if (!key_data_list) {
        return NULL;
    }
    if (!key_data_list->result_list) {
        return NULL;
    }

    if (!(result = db_result_list_begin(key_data_list->result_list))) {
        return NULL;
    }
    if (!key_data_list->key_data) {
        if (!(key_data_list->key_data = key_data_new(db_object_connection(key_data_list->dbo)))) {
            return NULL;
        }
    }
    if (key_data_from_result(key_data_list->key_data, result)) {
        return NULL;
    }
    return key_data_list->key_data;
}

const key_data_t* key_data_list_next(key_data_list_t* key_data_list) {
    const db_result_t* result;

    if (!key_data_list) {
        return NULL;
    }

    if (!(result = db_result_list_next(key_data_list->result_list))) {
        return NULL;
    }
    if (!key_data_list->key_data) {
        if (!(key_data_list->key_data = key_data_new(db_object_connection(key_data_list->dbo)))) {
            return NULL;
        }
    }
    if (key_data_from_result(key_data_list->key_data, result)) {
        return NULL;
    }
    return key_data_list->key_data;
}
