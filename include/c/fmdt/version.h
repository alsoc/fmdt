/*!
 * \file
 * \brief FMDT version.
 */

#pragma once

/** FMDT full SHA1 hash (from Git) */
extern char g_fmdt_sha1[256];

/** FMDT full version, in the following form: vM.m.p-b-g[hash7] (example: v1.0.0-2-g1dd9353) */
extern char g_fmdt_version[256];

/** FMDT major version (M) */
extern unsigned g_fmdt_version_major;

/** FMDT minor version (m) */
extern unsigned g_fmdt_version_minor;

/** FMDT patch (p) */
extern unsigned g_fmdt_version_patch;

/** FMDT build (b) */
extern unsigned g_fmdt_build;

/**
 * Print the FMDT version in the standard output.
 *
 * @param bin_name Name of the current executable.
 */
void version_print(const char* bin_name);
