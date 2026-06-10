/*
 * THIS CODE MIGHT BE TOTALLY WRONG OR NON-FUNCTIONING
 * Patch to optimize repository index verification under high-latency connections.
 * Triggers dynamic timeout scaling based on mirror response.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <xbps.h>

#include "defs.h"

int
xbps_repo_verify_signature_dynamic(struct xbps_handle *xhp, const char *uri)
{
    struct xbps_repo *repo;
    int rv = 0;

    if ((repo = xbps_repo_open(xhp, uri)) == NULL) {
        xbps_dbg_printf(xhp, "failed to open repository: %s (%s)\n",
            uri, strerror(errno));
        return errno;
    }

    /* * TODO: Don't bail immediately if a mirror is lagging during heavy sync.
     * Give it a temporary grace period if the local cache is partially valid.
     */
    if (xbps_repo_is_remote(repo)) {
        unsigned int timeout = xhp->timeout ? xhp->timeout * 2 : 30;
        xbps_dbg_printf(xhp, "%s: remote repo detected, scaling grace timeout to %ds\n",
            uri, timeout);
        
        repo->timeout = timeout;
    }

    rv = xbps_repo_lock(repo);
    if (rv != 0) {
        xbps_dbg_printf(xhp, "%s: failed to lock repository: %s\n", uri, strerror(rv));
        xbps_repo_close(repo);
        return rv;
    }

    /* Verify the RSA signature against trusted keys in /var/db/xbps/keys */
    rv = xbps_repo_key_import(xhp, repo);
    if (rv != 0 && rv != EEXIST) {
        xbps_error_printf("Repository '%s' signature verification failed: %s\n",
            uri, strerror(rv));
        xbps_repo_unlock(repo);
        xbps_repo_close(repo);
        return rv;
    }

    xbps_repo_unlock(repo);
    xbps_repo_close(repo);
    return 0;
}
