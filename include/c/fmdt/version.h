/*!
 * \file
 * \brief FMDT version.
 */

#pragma once

extern char g_fmdt_sha1[256];
extern char g_fmdt_version[256];
extern char g_fmdt_version_major[256];
extern char g_fmdt_version_minor[256];
extern char g_fmdt_version_patch[256];

void print_version(const char* bin_name);
