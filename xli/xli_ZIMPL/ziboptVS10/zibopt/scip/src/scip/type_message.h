/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*                  This file is part of the program and library             */
/*         SCIP --- Solving Constraint Integer Programs                      */
/*                                                                           */
/*    Copyright (C) 2002-2010 Konrad-Zuse-Zentrum                            */
/*                            fuer Informationstechnik Berlin                */
/*                                                                           */
/*  SCIP is distributed under the terms of the ZIB Academic License.         */
/*                                                                           */
/*  You should have received a copy of the ZIB Academic License              */
/*  along with SCIP; see the file COPYING. If not email to scip@zib.de.      */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma ident "@(#) $Id: type_message.h,v 1.11 2010/01/04 20:35:51 bzfheinz Exp $"

/**@file   type_message.h
 * @ingroup TYPEDEFINITIONS
 * @brief  type definitions for message output methods
 * @author Tobias Achterberg
 */

#ifndef __SCIP_TYPE_MESSAGE_H__
#define __SCIP_TYPE_MESSAGE_H__


#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/** verbosity levels of output */
enum SCIP_VerbLevel
{
   SCIP_VERBLEVEL_NONE    = 0,           /**< only error and warning messages are displayed */
   SCIP_VERBLEVEL_DIALOG  = 1,           /**< only interactive dialogs, errors, and warnings are displayed */
   SCIP_VERBLEVEL_MINIMAL = 2,           /**< only important messages are displayed */
   SCIP_VERBLEVEL_NORMAL  = 3,           /**< standard messages are displayed */
   SCIP_VERBLEVEL_HIGH    = 4,           /**< a lot of information is displayed */
   SCIP_VERBLEVEL_FULL    = 5            /**< all messages are displayed */
};
typedef enum SCIP_VerbLevel SCIP_VERBLEVEL;

typedef struct SCIP_Messagehdlr SCIP_MESSAGEHDLR;  /**< message handler */
typedef struct SCIP_MessagehdlrData SCIP_MESSAGEHDLRDATA; /**< message handler data */

/** error message print method of message handler
 *
 *  This method is invoked, if SCIP wants to display an error message to the screen or a file
 *
 *  input:
 *  - messagehdlr     : the message handler itself
 *  - file            : file stream to print into
 *  - msg             : string to output into the file
 */
#define SCIP_DECL_MESSAGEERROR(x) void x (SCIP_MESSAGEHDLR* messagehdlr, FILE* file, const char* msg)

/** warning message print method of message handler
 *
 *  This method is invoked, if SCIP wants to display a warning message to the screen or a file
 *
 *  input:
 *  - messagehdlr     : the message handler itself
 *  - file            : file stream to print into
 *  - msg             : string to output into the file
 */
#define SCIP_DECL_MESSAGEWARNING(x) void x (SCIP_MESSAGEHDLR* messagehdlr, FILE* file, const char* msg)

/** dialog message print method of message handler
 *
 *  This method is invoked, if SCIP wants to display a dialog message to the screen or a file
 *
 *  input:
 *  - messagehdlr     : the message handler itself
 *  - file            : file stream to print into
 *  - msg             : string to output into the file
 */
#define SCIP_DECL_MESSAGEDIALOG(x) void x (SCIP_MESSAGEHDLR* messagehdlr, FILE* file, const char* msg)

/** info message print method of message handler
 *
 *  This method is invoked, if SCIP wants to display an information message to the screen or a file
 *
 *  input:
 *  - messagehdlr     : the message handler itself
 *  - file            : file stream to print into
 *  - msg             : string to output into the file
 */
#define SCIP_DECL_MESSAGEINFO(x) void x (SCIP_MESSAGEHDLR* messagehdlr, FILE* file, const char* msg)

#ifdef __cplusplus
}
#endif

#endif
