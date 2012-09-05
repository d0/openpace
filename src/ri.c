/*
 * Copyright (c) 2010-2012 Dominik Oepen and Frank Morgner
 *
 * This file is part of OpenPACE.
 *
 * OpenPACE is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * OpenPACE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file ri.c
 * @brief Restricted Identification implementation
 *
 * @author Frank Morgner <morgner@informatik.hu-berlin.de>
 * @author Dominik Oepen <oepen@informatik.hu-berlin.de>
 * @author Paul Wilhelm  <wilhelm@math.hu-berlin.de>
 */

#include "eac_err.h"
#include "eac_lib.h"
#include "eac_util.h"
#include <eac/eac.h>
#include <openssl/crypto.h>
#include <openssl/ecdh.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/objects.h>
#include <string.h>

BUF_MEM *
RI_STEP2_compute_identifier(EAC_CTX *ctx, BUF_MEM *sector_pubkey)
{

    BUF_MEM *sector_identifier = NULL, *shared_secret = NULL;

    check((ctx && sector_pubkey && ctx->bn_ctx && ctx->ri_ctx
             && ctx->ri_ctx->compute_key
             && ctx->ri_ctx->static_key),
           "Invalid arguments");

    /* Perform the key agreement */
    shared_secret = ctx->ri_ctx->compute_key(ctx->ri_ctx->static_key,
            sector_pubkey, ctx->bn_ctx);
    if (!shared_secret)
        goto err;

    /* Compute the hash of the shared secret (which is the sector identifier) */
    sector_identifier = hash(ctx->ri_ctx->md, ctx->md_ctx, NULL, shared_secret);

err:
    if (shared_secret)
        BUF_MEM_clear_free(shared_secret);

    return sector_identifier;
}
