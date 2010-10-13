/*
 * $Id$
 *
 * Copyright (c) 2009 NLNet Labs. All rights reserved.
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

/**
 * RRset.
 *
 */

#ifndef SIGNER_RRSET_H
#define SIGNER_RRSET_H

#include "config.h"
#include "signer/hsm.h"
#include "signer/rrsigs.h"
#include "signer/signconf.h"
#include "signer/stats.h"

#include <ldns/ldns.h>

typedef struct rrset_struct rrset_type;
struct rrset_struct {
    ldns_rr_type rr_type;
    uint32_t rr_count;
    uint32_t add_count;
    uint32_t del_count;
    uint32_t rrsig_count;
    uint32_t internal_serial;
    ldns_dnssec_rrs* rrs;
    ldns_dnssec_rrs* add;
    ldns_dnssec_rrs* del;
    rrsigs_type* rrsigs;
    int drop_signatures;
};

/**
 * Create new RRset.
 * \param[in] rrtype RRtype
 * \return new RRset
 *
 */
rrset_type* rrset_create(ldns_rr_type rrtype);

/**
 * Create new RRset from RR.
 * \param[in] rr RR
 * \return new RRset
 *
 */
rrset_type* rrset_create_frm_rr(ldns_rr* rr);

/**
 * Update RRset with pending changes.
 * \param[in] rrset RRset
 * \param[in] serial version to update to
 * \return 0 on success, 1 on error
 *
 */
int rrset_update(rrset_type* rrset, uint32_t serial);


/**
 * Cancel update.
 * \param[in] rrset RRset
 *
 */
void rrset_cancel_update(rrset_type* rrset);

/**
 * Add RR to RRset.
 * \param[in] rrset RRset
 * \param[in] rr RR
 * \return 0 on success, 1 on error
 *
 */
int rrset_add_rr(rrset_type* rrset, ldns_rr* rr);

/**
 * Delete RR from RRset.
 * \param[in] rrset RRset
 * \param[in] rr RR
 * \return 0 on success, 1 on error
 *
 */
int rrset_del_rr(rrset_type* rrset, ldns_rr* rr);

/**
 * Recover RR from backup.
 * \param[in] rrset RRset
 * \param[in] rr RR
 * \return 0 on success, 1 on error
 *
 */
int rrset_recover_rr_from_backup(rrset_type* rrset, ldns_rr* rr);

/**
 * Recover RRSIG from backup.
 * \param[in] rrset RRset
 * \param[in] rrsig RRSIG
 * \param[in] locator key locator
 * \param[in] flags key flags
 * \return 0 on success, 1 on error
 *
 */
int rrset_recover_rrsig_from_backup(rrset_type* rrset, ldns_rr* rrsig,
    const char* locator, uint32_t flags);

/**
 * Sign RRset.
 * \param[in] ctx HSM context
 * \param[in] rrset RRset
 * \param[in] owner owner of the zone
 * \param[in] sc sign configuration
 * \param[in] signtime time when the zone is signd
 * \param[out] stats update statistics
 * \return 0 on success, 1 on error
 *
 */
int rrset_sign(hsm_ctx_t* ctx, rrset_type* rrset, ldns_rdf* owner,
    signconf_type* sc, time_t signtime, stats_type* stats);

/**
 * Delete all RRs from RRset.
 * \param[in] rrset RRset
 * \return 0 on success, 1 on error
 *
 */
int rrset_del_rrs(rrset_type* rrset);

/**
 * Return the number of RRs in RRset.
 * \param[in] rrset RRset
 * \return number of RRs
 *
 */
int rrset_count_rr(rrset_type* rrset);

/**
 * Return the number of pending added RRs in RRset.
 * \param[in] rrset RRset
 * \return number of pending added RRs
 *
 */
int rrset_count_add(rrset_type* rrset);

/**
 * Return the number of pending deleted RRs in RRset.
 * \param[in] rrset RRset
 * \return number of pending deleted RRs
 *
 */
int rrset_count_del(rrset_type* rrset);

/**
 * Return the number of RRs in RRset after an update.
 * \param[in] rrset RRset
 * \return number of RRs after an update
 *
 */
int rrset_count_RR(rrset_type* rrset);

/**
 * Clean up RRset.
 * \param[in] rrset RRset to be cleaned up
 *
 */
void rrset_cleanup(rrset_type* rrset);

/**
 * Print RRset.
 * \param[in] fd file descriptor
 * \param[in] rrset RRset to be printed
 * \param[in] skip_rrsigs if true, don't print RRSIG records
 *
 */
void rrset_print(FILE* fd, rrset_type* rrset, int skip_rrsigs);

/**
 * Print RRSIGs from RRset.
 * \param[in] fd file descriptor
 * \param[in] rrset RRset to be printed
 *
 */
void rrset_print_rrsig(FILE* fd, rrset_type* rrset);

#endif /* SIGNER_RRSET_H */
