/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   appicon_png;
    const int            appicon_pngSize = 62644;

    extern const char*   JetBrainsMonoItalic_ttf;
    const int            JetBrainsMonoItalic_ttfSize = 276840;

    extern const char*   JetBrainsMonoMedium_ttf;
    const int            JetBrainsMonoMedium_ttfSize = 273860;

    extern const char*   JetBrainsMonoMediumItalic_ttf;
    const int            JetBrainsMonoMediumItalic_ttfSize = 276804;

    extern const char*   JetBrainsMonoRegular_ttf;
    const int            JetBrainsMonoRegular_ttfSize = 273900;

    extern const char*   JetBrainsMonoSemiBold_ttf;
    const int            JetBrainsMonoSemiBold_ttfSize = 277092;

    extern const char*   JetBrainsMonoSemiBoldItalic_ttf;
    const int            JetBrainsMonoSemiBoldItalic_ttfSize = 279828;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 7;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
