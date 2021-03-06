/******************************************************************************
 *
 * Module Name: nsprepkg - Validation of package objects for predefined names
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
#include "acnamesp.h"
#include "acpredef.h"


#define _COMPONENT          ACPI_NAMESPACE
        ACPI_MODULE_NAME    ("nsprepkg")


/* Local prototypes */

static ACPI_STATUS
AcpiNsCheckPackageList (
    ACPI_PREDEFINED_DATA        *Data,
    const ACPI_PREDEFINED_INFO  *Package,
    ACPI_OPERAND_OBJECT         **Elements,
    UINT32                      Count);

static ACPI_STATUS
AcpiNsCheckPackageElements (
    ACPI_PREDEFINED_DATA        *Data,
    ACPI_OPERAND_OBJECT         **Elements,
    UINT8                       Type1,
    UINT32                      Count1,
    UINT8                       Type2,
    UINT32                      Count2,
    UINT32                      StartIndex);


/*******************************************************************************
 *
 * FUNCTION:    AcpiNsCheckPackage
 *
 * PARAMETERS:  Data                - Pointer to validation data structure
 *              ReturnObjectPtr     - Pointer to the object returned from the
 *                                    evaluation of a method or object
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Check a returned package object for the correct count and
 *              correct type of all sub-objects.
 *
 ******************************************************************************/

ACPI_STATUS
AcpiNsCheckPackage (
    ACPI_PREDEFINED_DATA        *Data,
    ACPI_OPERAND_OBJECT         **ReturnObjectPtr)
{
    ACPI_OPERAND_OBJECT         *ReturnObject = *ReturnObjectPtr;
    const ACPI_PREDEFINED_INFO  *Package;
    ACPI_OPERAND_OBJECT         **Elements;
    ACPI_STATUS                 Status = AE_OK;
    UINT32                      ExpectedCount;
    UINT32                      Count;
    UINT32                      i;


    ACPI_FUNCTION_NAME (NsCheckPackage);


    /* The package info for this name is in the next table entry */

    Package = Data->Predefined + 1;

    ACPI_DEBUG_PRINT ((ACPI_DB_NAMES,
        "%s Validating return Package of Type %X, Count %X\n",
        Data->Pathname, Package->RetInfo.Type, ReturnObject->Package.Count));

    /*
     * For variable-length Packages, we can safely remove all embedded
     * and trailing NULL package elements
     */
    AcpiNsRemoveNullElements (Data, Package->RetInfo.Type, ReturnObject);

    /* Extract package count and elements array */

    Elements = ReturnObject->Package.Elements;
    Count = ReturnObject->Package.Count;

    /* The package must have at least one element, else invalid */

    if (!Count)
    {
        ACPI_WARN_PREDEFINED ((AE_INFO, Data->Pathname, Data->NodeFlags,
            "Return Package has no elements (empty)"));

        return (AE_AML_OPERAND_VALUE);
    }

    /*
     * Decode the type of the expected package contents
     *
     * PTYPE1 packages contain no subpackages
     * PTYPE2 packages contain sub-packages
     */
    switch (Package->RetInfo.Type)
    {
    case ACPI_PTYPE1_FIXED:

        /*
         * The package count is fixed and there are no sub-packages
         *
         * If package is too small, exit.
         * If package is larger than expected, issue warning but continue
         */
        ExpectedCount = Package->RetInfo.Count1 + Package->RetInfo.Count2;
        if (Count < ExpectedCount)
        {
            goto PackageTooSmall;
        }
        else if (Count > ExpectedCount)
        {
            ACPI_DEBUG_PRINT ((ACPI_DB_REPAIR,
                "%s: Return Package is larger than needed - "
                "found %u, expected %u\n",
                Data->Pathname, Count, ExpectedCount));
        }

        /* Validate all elements of the returned package */

        Status = AcpiNsCheckPackageElements (Data, Elements,
                    Package->RetInfo.ObjectType1, Package->RetInfo.Count1,
                    Package->RetInfo.ObjectType2, Package->RetInfo.Count2, 0);
        break;


    case ACPI_PTYPE1_VAR:

        /*
         * The package count is variable, there are no sub-packages, and all
         * elements must be of the same type
         */
        for (i = 0; i < Count; i++)
        {
            Status = AcpiNsCheckObjectType (Data, Elements,
                        Package->RetInfo.ObjectType1, i);
            if (ACPI_FAILURE (Status))
            {
                return (Status);
            }
            Elements++;
        }
        break;


    case ACPI_PTYPE1_OPTION:

        /*
         * The package count is variable, there are no sub-packages. There are
         * a fixed number of required elements, and a variable number of
         * optional elements.
         *
         * Check if package is at least as large as the minimum required
         */
        ExpectedCount = Package->RetInfo3.Count;
        if (Count < ExpectedCount)
        {
            goto PackageTooSmall;
        }

        /* Variable number of sub-objects */

        for (i = 0; i < Count; i++)
        {
            if (i < Package->RetInfo3.Count)
            {
                /* These are the required package elements (0, 1, or 2) */

                Status = AcpiNsCheckObjectType (Data, Elements,
                            Package->RetInfo3.ObjectType[i], i);
                if (ACPI_FAILURE (Status))
                {
                    return (Status);
                }
            }
            else
            {
                /* These are the optional package elements */

                Status = AcpiNsCheckObjectType (Data, Elements,
                            Package->RetInfo3.TailObjectType, i);
                if (ACPI_FAILURE (Status))
                {
                    return (Status);
                }
            }
            Elements++;
        }
        break;


    case ACPI_PTYPE2_REV_FIXED:

        /* First element is the (Integer) revision */

        Status = AcpiNsCheckObjectType (Data, Elements,
                    ACPI_RTYPE_INTEGER, 0);
        if (ACPI_FAILURE (Status))
        {
            return (Status);
        }

        Elements++;
        Count--;

        /* Examine the sub-packages */

        Status = AcpiNsCheckPackageList (Data, Package, Elements, Count);
        break;


    case ACPI_PTYPE2_PKG_COUNT:

        /* First element is the (Integer) count of sub-packages to follow */

        Status = AcpiNsCheckObjectType (Data, Elements,
                    ACPI_RTYPE_INTEGER, 0);
        if (ACPI_FAILURE (Status))
        {
            return (Status);
        }

        /*
         * Count cannot be larger than the parent package length, but allow it
         * to be smaller. The >= accounts for the Integer above.
         */
        ExpectedCount = (UINT32) (*Elements)->Integer.Value;
        if (ExpectedCount >= Count)
        {
            goto PackageTooSmall;
        }

        Count = ExpectedCount;
        Elements++;

        /* Examine the sub-packages */

        Status = AcpiNsCheckPackageList (Data, Package, Elements, Count);
        break;


    case ACPI_PTYPE2:
    case ACPI_PTYPE2_FIXED:
    case ACPI_PTYPE2_MIN:
    case ACPI_PTYPE2_COUNT:
    case ACPI_PTYPE2_FIX_VAR:

        /*
         * These types all return a single Package that consists of a
         * variable number of sub-Packages.
         *
         * First, ensure that the first element is a sub-Package. If not,
         * the BIOS may have incorrectly returned the object as a single
         * package instead of a Package of Packages (a common error if
         * there is only one entry). We may be able to repair this by
         * wrapping the returned Package with a new outer Package.
         */
        if (*Elements && ((*Elements)->Common.Type != ACPI_TYPE_PACKAGE))
        {
            /* Create the new outer package and populate it */

            Status = AcpiNsWrapWithPackage (Data, ReturnObject, ReturnObjectPtr);
            if (ACPI_FAILURE (Status))
            {
                return (Status);
            }

            /* Update locals to point to the new package (of 1 element) */

            ReturnObject = *ReturnObjectPtr;
            Elements = ReturnObject->Package.Elements;
            Count = 1;
        }

        /* Examine the sub-packages */

        Status = AcpiNsCheckPackageList (Data, Package, Elements, Count);
        break;


    default:

        /* Should not get here if predefined info table is correct */

        ACPI_WARN_PREDEFINED ((AE_INFO, Data->Pathname, Data->NodeFlags,
            "Invalid internal return type in table entry: %X",
            Package->RetInfo.Type));

        return (AE_AML_INTERNAL);
    }

    return (Status);


PackageTooSmall:

    /* Error exit for the case with an incorrect package count */

    ACPI_WARN_PREDEFINED ((AE_INFO, Data->Pathname, Data->NodeFlags,
        "Return Package is too small - found %u elements, expected %u",
        Count, ExpectedCount));

    return (AE_AML_OPERAND_VALUE);
}


/*******************************************************************************
 *
 * FUNCTION:    AcpiNsCheckPackageList
 *
 * PARAMETERS:  Data            - Pointer to validation data structure
 *              Package         - Pointer to package-specific info for method
 *              Elements        - Element list of parent package. All elements
 *                                of this list should be of type Package.
 *              Count           - Count of subpackages
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Examine a list of subpackages
 *
 ******************************************************************************/

static ACPI_STATUS
AcpiNsCheckPackageList (
    ACPI_PREDEFINED_DATA        *Data,
    const ACPI_PREDEFINED_INFO  *Package,
    ACPI_OPERAND_OBJECT         **Elements,
    UINT32                      Count)
{
    ACPI_OPERAND_OBJECT         *SubPackage;
    ACPI_OPERAND_OBJECT         **SubElements;
    ACPI_STATUS                 Status;
    UINT32                      ExpectedCount;
    UINT32                      i;
    UINT32                      j;


    /*
     * Validate each sub-Package in the parent Package
     *
     * NOTE: assumes list of sub-packages contains no NULL elements.
     * Any NULL elements should have been removed by earlier call
     * to AcpiNsRemoveNullElements.
     */
    for (i = 0; i < Count; i++)
    {
        SubPackage = *Elements;
        SubElements = SubPackage->Package.Elements;
        Data->ParentPackage = SubPackage;

        /* Each sub-object must be of type Package */

        Status = AcpiNsCheckObjectType (Data, &SubPackage,
                    ACPI_RTYPE_PACKAGE, i);
        if (ACPI_FAILURE (Status))
        {
            return (Status);
        }

        /* Examine the different types of expected sub-packages */

        Data->ParentPackage = SubPackage;
        switch (Package->RetInfo.Type)
        {
        case ACPI_PTYPE2:
        case ACPI_PTYPE2_PKG_COUNT:
        case ACPI_PTYPE2_REV_FIXED:

            /* Each subpackage has a fixed number of elements */

            ExpectedCount = Package->RetInfo.Count1 + Package->RetInfo.Count2;
            if (SubPackage->Package.Count < ExpectedCount)
            {
                goto PackageTooSmall;
            }

            Status = AcpiNsCheckPackageElements (Data, SubElements,
                        Package->RetInfo.ObjectType1,
                        Package->RetInfo.Count1,
                        Package->RetInfo.ObjectType2,
                        Package->RetInfo.Count2, 0);
            if (ACPI_FAILURE (Status))
            {
                return (Status);
            }
            break;


        case ACPI_PTYPE2_FIX_VAR:
            /*
             * Each subpackage has a fixed number of elements and an
             * optional element
             */
            ExpectedCount = Package->RetInfo.Count1 + Package->RetInfo.Count2;
            if (SubPackage->Package.Count < ExpectedCount)
            {
                goto PackageTooSmall;
            }

            Status = AcpiNsCheckPackageElements (Data, SubElements,
                        Package->RetInfo.ObjectType1,
                        Package->RetInfo.Count1,
                        Package->RetInfo.ObjectType2,
                        SubPackage->Package.Count - Package->RetInfo.Count1, 0);
            if (ACPI_FAILURE (Status))
            {
                return (Status);
            }
            break;


        case ACPI_PTYPE2_FIXED:

            /* Each sub-package has a fixed length */

            ExpectedCount = Package->RetInfo2.Count;
            if (SubPackage->Package.Count < ExpectedCount)
            {
                goto PackageTooSmall;
            }

            /* Check the type of each sub-package element */

            for (j = 0; j < ExpectedCount; j++)
            {
                Status = AcpiNsCheckObjectType (Data, &SubElements[j],
                            Package->RetInfo2.ObjectType[j], j);
                if (ACPI_FAILURE (Status))
                {
                    return (Status);
                }
            }
            break;


        case ACPI_PTYPE2_MIN:

            /* Each sub-package has a variable but minimum length */

            ExpectedCount = Package->RetInfo.Count1;
            if (SubPackage->Package.Count < ExpectedCount)
            {
                goto PackageTooSmall;
            }

            /* Check the type of each sub-package element */

            Status = AcpiNsCheckPackageElements (Data, SubElements,
                        Package->RetInfo.ObjectType1,
                        SubPackage->Package.Count, 0, 0, 0);
            if (ACPI_FAILURE (Status))
            {
                return (Status);
            }
            break;


        case ACPI_PTYPE2_COUNT:

            /*
             * First element is the (Integer) count of elements, including
             * the count field (the ACPI name is NumElements)
             */
            Status = AcpiNsCheckObjectType (Data, SubElements,
                        ACPI_RTYPE_INTEGER, 0);
            if (ACPI_FAILURE (Status))
            {
                return (Status);
            }

            /*
             * Make sure package is large enough for the Count and is
             * is as large as the minimum size
             */
            ExpectedCount = (UINT32) (*SubElements)->Integer.Value;
            if (SubPackage->Package.Count < ExpectedCount)
            {
                goto PackageTooSmall;
            }
            if (SubPackage->Package.Count < Package->RetInfo.Count1)
            {
                ExpectedCount = Package->RetInfo.Count1;
                goto PackageTooSmall;
            }
            if (ExpectedCount == 0)
            {
                /*
                 * Either the NumEntries element was originally zero or it was
                 * a NULL element and repaired to an Integer of value zero.
                 * In either case, repair it by setting NumEntries to be the
                 * actual size of the subpackage.
                 */
                ExpectedCount = SubPackage->Package.Count;
                (*SubElements)->Integer.Value = ExpectedCount;
            }

            /* Check the type of each sub-package element */

            Status = AcpiNsCheckPackageElements (Data, (SubElements + 1),
                        Package->RetInfo.ObjectType1,
                        (ExpectedCount - 1), 0, 0, 1);
            if (ACPI_FAILURE (Status))
            {
                return (Status);
            }
            break;


        default: /* Should not get here, type was validated by caller */

            return (AE_AML_INTERNAL);
        }

        Elements++;
    }

    return (AE_OK);


PackageTooSmall:

    /* The sub-package count was smaller than required */

    ACPI_WARN_PREDEFINED ((AE_INFO, Data->Pathname, Data->NodeFlags,
        "Return Sub-Package[%u] is too small - found %u elements, expected %u",
        i, SubPackage->Package.Count, ExpectedCount));

    return (AE_AML_OPERAND_VALUE);
}


/*******************************************************************************
 *
 * FUNCTION:    AcpiNsCheckPackageElements
 *
 * PARAMETERS:  Data            - Pointer to validation data structure
 *              Elements        - Pointer to the package elements array
 *              Type1           - Object type for first group
 *              Count1          - Count for first group
 *              Type2           - Object type for second group
 *              Count2          - Count for second group
 *              StartIndex      - Start of the first group of elements
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Check that all elements of a package are of the correct object
 *              type. Supports up to two groups of different object types.
 *
 ******************************************************************************/

static ACPI_STATUS
AcpiNsCheckPackageElements (
    ACPI_PREDEFINED_DATA        *Data,
    ACPI_OPERAND_OBJECT         **Elements,
    UINT8                       Type1,
    UINT32                      Count1,
    UINT8                       Type2,
    UINT32                      Count2,
    UINT32                      StartIndex)
{
    ACPI_OPERAND_OBJECT         **ThisElement = Elements;
    ACPI_STATUS                 Status;
    UINT32                      i;


    /*
     * Up to two groups of package elements are supported by the data
     * structure. All elements in each group must be of the same type.
     * The second group can have a count of zero.
     */
    for (i = 0; i < Count1; i++)
    {
        Status = AcpiNsCheckObjectType (Data, ThisElement,
                    Type1, i + StartIndex);
        if (ACPI_FAILURE (Status))
        {
            return (Status);
        }
        ThisElement++;
    }

    for (i = 0; i < Count2; i++)
    {
        Status = AcpiNsCheckObjectType (Data, ThisElement,
                    Type2, (i + Count1 + StartIndex));
        if (ACPI_FAILURE (Status))
        {
            return (Status);
        }
        ThisElement++;
    }

    return (AE_OK);
}
