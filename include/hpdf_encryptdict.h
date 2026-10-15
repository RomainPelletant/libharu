/* SPDX-License-Identifier: Zlib */
/*
 * << Haru Free PDF Library >> -- hpdf_encryptdict.h
 *
 * URL: http://libharu.org
 *
 * Copyright (c) 1999-2006 Takeshi Kanno <takeshi_kanno@est.hi-ho.ne.jp>
 * Copyright (c) 2007-2009 Antony Dovgal <tony@daylessday.org>
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.
 * It is provided "as is" without express or implied warranty.
 *
 */

#ifndef _HPDF_ENCRYPTDICT_H
#define _HPDF_ENCRYPTDICT_H

#include "hpdf_objects.h"

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*/
/*------ HPDF_EncryptDict ---------------------------------------------------*/

#if !defined(__ZEPHYR__) || defined(CONFIG_LIBHARU_ENCRYPTION)

HPDF_EncryptDict
HPDF_EncryptDict_New  (HPDF_MMgr  mmgr,
                       HPDF_Xref  xref);


void
HPDF_EncryptDict_CreateID  (HPDF_EncryptDict  dict,
                            HPDF_Dict         info,
                            HPDF_Xref         xref);


void
HPDF_EncryptDict_OnFree  (HPDF_Dict  obj);


HPDF_STATUS
HPDF_EncryptDict_SetPassword  (HPDF_EncryptDict  dict,
                               const char   *owner_passwd,
                               const char   *user_passwd);


HPDF_BOOL
HPDF_EncryptDict_Validate  (HPDF_EncryptDict  dict);


HPDF_STATUS
HPDF_EncryptDict_Prepare  (HPDF_EncryptDict  dict,
                           HPDF_Dict         info,
                           HPDF_Xref         xref);


HPDF_Encrypt
HPDF_EncryptDict_GetAttr (HPDF_EncryptDict  dict);

#else /* Zephyr: hpdf_encryptdict.c is not built, password APIs fail */

static inline HPDF_EncryptDict
HPDF_EncryptDict_New  (HPDF_MMgr  mmgr,
                       HPDF_Xref  xref)
{
    (void)xref;
    HPDF_SetError (mmgr->error, HPDF_UNSUPPORTED_FUNC, 0);
    return NULL;
}


static inline HPDF_STATUS
HPDF_EncryptDict_SetPassword  (HPDF_EncryptDict  dict,
                               const char   *owner_passwd,
                               const char   *user_passwd)
{
    (void)owner_passwd; (void)user_passwd;
    return HPDF_SetError (dict->error, HPDF_UNSUPPORTED_FUNC, 0);
}


static inline HPDF_STATUS
HPDF_EncryptDict_Prepare  (HPDF_EncryptDict  dict,
                           HPDF_Dict         info,
                           HPDF_Xref         xref)
{
    (void)info; (void)xref;
    return HPDF_SetError (dict->error, HPDF_UNSUPPORTED_FUNC, 0);
}


static inline HPDF_Encrypt
HPDF_EncryptDict_GetAttr (HPDF_EncryptDict  dict)
{
    (void)dict;
    return NULL;
}

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _HPDF_ENCRYPTDICT_H */

