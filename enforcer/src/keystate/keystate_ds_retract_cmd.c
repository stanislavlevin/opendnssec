/*
 * Copyright (c) 2011 Surfnet 
 * Copyright (c) 2011 .SE (The Internet Infrastructure Foundation).
 * Copyright (c) 2011 OpenDNSSEC AB (svb)
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

#include "config.h"

#include "daemon/cmdhandler.h"
#include "daemon/engine.h"
#include "enforcer/enforce_task.h"
#include "log.h"
#include "str.h"
#include "clientpipe.h"
#include "db/key_data.h"
#include "keystate/keystate_ds.h"

#include "keystate/keystate_ds_retract_cmd.h"

static void
usage(int sockfd)
{
	client_printf(sockfd,
		"key ds-retract\n"
		"	--zone <zone>				aka -z\n"
		"	--keytag <keytag> | --cka_id <CKA_ID>	aka -x | -k\n"
/*		"      [--force]                  (aka -f)  force even if there is no configured\n"
		"                                           DelegationSignerSubmitCommand.\n" */
	);
}

static void
help(int sockfd)
{
	client_printf(sockfd,
		"Issue a ds-retract to the enforcer for a KSK.\n"
		"(This command with no parameters lists eligible keys.)\n"
		"\nOptions:\n"
		"zone		name of the zone\n"
		"keytag|cka_id	specify the keytag or the locator of the key\n\n");
}

static int
handles(const char *cmd, ssize_t n)
{
	return ods_check_command(cmd, n, key_ds_retract_funcblock()->cmdname)?1:0;
}

static int
run(int sockfd, engine_type* engine, const char *cmd, ssize_t n,
	db_connection_t *dbconn)
{
	int error;
	/* TODO, this changes the state, but sbmt cmd is not exec. */
	error = run_ds_cmd(sockfd, cmd, n, dbconn,
		KEY_DATA_DS_AT_PARENT_RETRACT,
		KEY_DATA_DS_AT_PARENT_RETRACTED, engine);
	if (error == 0) {
		flush_enforce_task(engine, 0);
	}
	return error;

}

static struct cmd_func_block funcblock = {
	"key ds-retract", &usage, &help, &handles, &run
};

struct cmd_func_block*
key_ds_retract_funcblock(void)
{
	return &funcblock;
}
