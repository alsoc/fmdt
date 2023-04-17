/*!
 * \file
 * \brief FMDT version.
 */

#pragma once

/** Git full SHA1 hash */
extern char g_fmdt_sha1[256];

/** Git full version, in the following form: vM.m.p-b-g[hash7] (example : v1.0.0-2-g1dd9353) */
extern char g_fmdt_version[256];

/** Git major version M */
extern unsigned g_fmdt_version_major;

/** Git minor version m */
extern unsigned g_fmdt_version_minor;

/** Git patch p */
extern unsigned g_fmdt_version_patch;

/** Git build b */
extern unsigned g_fmdt_build;

/**
 * Print the FMDT version in the standard output.
 * @param bin_name Name of the current executable.
 */
void print_version(const char* bin_name);
