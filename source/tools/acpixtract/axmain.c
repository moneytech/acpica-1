/******************************************************************************
 *
 * Module Name: axmain - main module for acpixtract utility
 *
 *****************************************************************************/

/******************************************************************************
 *
 * 1. Copyright Notice
 *
 * Some or all of this work - Copyright (c) 1999 - 2013, Intel Corp.
 * All rights reserved.
 *
 * 2. License
 *
 * 2.1. This is your license from Intel Corp. under its intellectual property
 * rights. You may have additional license terms from the party that provided
 * you this software, covering your right to use that party's intellectual
 * property rights.
 *
 * 2.2. Intel grants, free of charge, to any person ("Licensee") obtaining a
 * copy of the source code appearing in this file ("Covered Code") an
 * irrevocable, perpetual, worldwide license under Intel's copyrights in the
 * base code distributed originally by Intel ("Original Intel Code") to copy,
 * make derivatives, distribute, use and display any portion of the Covered
 * Code in any form, with the right to sublicense such rights; and
 *
 * 2.3. Intel grants Licensee a non-exclusive and non-transferable patent
 * license (with the right to sublicense), under only those claims of Intel
 * patents that are infringed by the Original Intel Code, to make, use, sell,
 * offer to sell, and import the Covered Code and derivative works thereof
 * solely to the minimum extent necessary to exercise the above copyright
 * license, and in no event shall the patent license extend to any additions
 * to or modifications of the Original Intel Code. No other license or right
 * is granted directly or by implication, estoppel or otherwise;
 *
 * The above copyright and patent license is granted only if the following
 * conditions are met:
 *
 * 3. Conditions
 *
 * 3.1. Redistribution of Source with Rights to Further Distribute Source.
 * Redistribution of source code of any substantial portion of the Covered
 * Code or modification with rights to further distribute source must include
 * the above Copyright Notice, the above License, this list of Conditions,
 * and the following Disclaimer and Export Compliance provision. In addition,
 * Licensee must cause all Covered Code to which Licensee contributes to
 * contain a file documenting the changes Licensee made to create that Covered
 * Code and the date of any change. Licensee must include in that file the
 * documentation of any changes made by any predecessor Licensee. Licensee
 * must include a prominent statement that the modification is derived,
 * directly or indirectly, from Original Intel Code.
 *
 * 3.2. Redistribution of Source with no Rights to Further Distribute Source.
 * Redistribution of source code of any substantial portion of the Covered
 * Code or modification without rights to further distribute source must
 * include the following Disclaimer and Export Compliance provision in the
 * documentation and/or other materials provided with distribution. In
 * addition, Licensee may not authorize further sublicense of source of any
 * portion of the Covered Code, and must include terms to the effect that the
 * license from Licensee to its licensee is limited to the intellectual
 * property embodied in the software Licensee provides to its licensee, and
 * not to intellectual property embodied in modifications its licensee may
 * make.
 *
 * 3.3. Redistribution of Executable. Redistribution in executable form of any
 * substantial portion of the Covered Code or modification must reproduce the
 * above Copyright Notice, and the following Disclaimer and Export Compliance
 * provision in the documentation and/or other materials provided with the
 * distribution.
 *
 * 3.4. Intel retains all right, title, and interest in and to the Original
 * Intel Code.
 *
 * 3.5. Neither the name Intel nor any other trademark owned or controlled by
 * Intel shall be used in advertising or otherwise to promote the sale, use or
 * other dealings in products derived from or relating to the Covered Code
 * without prior written authorization from Intel.
 *
 * 4. Disclaimer and Export Compliance
 *
 * 4.1. INTEL MAKES NO WARRANTY OF ANY KIND REGARDING ANY SOFTWARE PROVIDED
 * HERE. ANY SOFTWARE ORIGINATING FROM INTEL OR DERIVED FROM INTEL SOFTWARE
 * IS PROVIDED "AS IS," AND INTEL WILL NOT PROVIDE ANY SUPPORT, ASSISTANCE,
 * INSTALLATION, TRAINING OR OTHER SERVICES. INTEL WILL NOT PROVIDE ANY
 * UPDATES, ENHANCEMENTS OR EXTENSIONS. INTEL SPECIFICALLY DISCLAIMS ANY
 * IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGEMENT AND FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * 4.2. IN NO EVENT SHALL INTEL HAVE ANY LIABILITY TO LICENSEE, ITS LICENSEES
 * OR ANY OTHER THIRD PARTY, FOR ANY LOST PROFITS, LOST DATA, LOSS OF USE OR
 * COSTS OF PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES, OR FOR ANY INDIRECT,
 * SPECIAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THIS AGREEMENT, UNDER ANY
 * CAUSE OF ACTION OR THEORY OF LIABILITY, AND IRRESPECTIVE OF WHETHER INTEL
 * HAS ADVANCE NOTICE OF THE POSSIBILITY OF SUCH DAMAGES. THESE LIMITATIONS
 * SHALL APPLY NOTWITHSTANDING THE FAILURE OF THE ESSENTIAL PURPOSE OF ANY
 * LIMITED REMEDY.
 *
 * 4.3. Licensee shall not export, either directly or indirectly, any of this
 * software or system incorporating such software without first obtaining any
 * required license or other approval from the U. S. Department of Commerce or
 * any other agency or department of the United States Government. In the
 * event Licensee exports any such software from the United States or
 * re-exports any such software from a foreign destination, Licensee shall
 * ensure that the distribution and export/re-export of the software is in
 * compliance with all laws, regulations, orders, or other restrictions of the
 * U.S. Export Administration Regulations. Licensee agrees that neither it nor
 * any of its subsidiaries will export/re-export any technical data, process,
 * software, or service, directly or indirectly, to any country for which the
 * United States government or any agency thereof requires an export license,
 * other governmental approval, or letter of assurance, without first obtaining
 * such license, approval or letter.
 *
 *****************************************************************************/

#include "acpi.h"
#include "accommon.h"
#include "acapps.h"
#include <stdio.h>


static void
DisplayUsage (
    void);

int
AxExtractTables (
    char                    *InputPathname,
    char                    *Signature,
    unsigned int            MinimumInstances);

int
AxListTables (
    char                    *InputPathname);


/* Options */

#define AX_EXTRACT_ALL          0
#define AX_LIST_ALL             1
#define AX_EXTRACT_SIGNATURE    2
#define AX_EXTRACT_AML_TABLES   3

static int          AxAction = AX_EXTRACT_AML_TABLES; /* DSDT & SSDTs */

#define AX_OPTIONAL_TABLES      0
#define AX_REQUIRED_TABLE       1


/******************************************************************************
 *
 * FUNCTION:    DisplayUsage
 *
 * DESCRIPTION: Usage message
 *
 ******************************************************************************/

static void
DisplayUsage (
    void)
{

    ACPI_USAGE_HEADER ("acpixtract [option] <InputFile>");

    ACPI_OPTION ("-a",                  "Extract all tables, not just DSDT/SSDT");
    ACPI_OPTION ("-l",                  "List table summaries, do not extract");
    ACPI_OPTION ("-s <signature>",      "Extract all tables with <signature>");

    printf ("\nExtract binary ACPI tables from text acpidump output\n");
    printf ("Default invocation extracts the DSDT and all SSDTs\n");
}


/******************************************************************************
 *
 * FUNCTION:    main
 *
 * DESCRIPTION: C main function
 *
 ******************************************************************************/

int
main (
    int                     argc,
    char                    *argv[])
{
    char                    *Filename;
    int                     Status;
    int                     j;


    printf (ACPI_COMMON_SIGNON ("ACPI Binary Table Extraction Utility"));

    if (argc < 2)
    {
        DisplayUsage ();
        return (0);
    }

    /* Command line options */

    while ((j = AcpiGetopt (argc, argv, "ahls:")) != EOF) switch (j)
    {
    case 'a':
        AxAction = AX_EXTRACT_ALL;          /* Extract all tables found */
        break;

    case 'l':
        AxAction = AX_LIST_ALL;             /* List tables only, do not extract */
        break;

    case 's':
        AxAction = AX_EXTRACT_SIGNATURE;    /* Extract only tables with this sig */
        break;

    case 'h':
    default:
        DisplayUsage ();
        return (0);
    }

    /* Input filename is always required */

    Filename = argv[AcpiGbl_Optind];
    if (!Filename)
    {
        printf ("Missing required input filename\n");
        return (-1);
    }

    /* Perform requested action */

    switch (AxAction)
    {
    case AX_EXTRACT_ALL:
        Status = AxExtractTables (Filename, NULL, AX_OPTIONAL_TABLES);
        break;

    case AX_LIST_ALL:
        Status = AxListTables (Filename);
        break;

    case AX_EXTRACT_SIGNATURE:
        Status = AxExtractTables (Filename, AcpiGbl_Optarg, AX_REQUIRED_TABLE);
        break;

    default:
        /*
         * Default output is the DSDT and all SSDTs. One DSDT is required,
         * any SSDTs are optional.
         */
        Status = AxExtractTables (Filename, "DSDT", AX_REQUIRED_TABLE);
        if (Status)
        {
            return (Status);
        }

        Status = AxExtractTables (Filename, "SSDT", AX_OPTIONAL_TABLES);
        break;
    }

    return (Status);
}
