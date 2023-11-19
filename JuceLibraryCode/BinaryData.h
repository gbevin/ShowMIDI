/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   appiconios_png;
    const int            appiconios_pngSize = 39031;

    extern const char*   appicon_png;
    const int            appicon_pngSize = 51952;

    extern const char*   collapsed_svg;
    const int            collapsed_svgSize = 864;

    extern const char*   expanded_svg;
    const int            expanded_svgSize = 825;

    extern const char*   help_svg;
    const int            help_svgSize = 1676;

    extern const char*   hidden_svg;
    const int            hidden_svgSize = 1148;

    extern const char*   pause_svg;
    const int            pause_svgSize = 919;

    extern const char*   play_svg;
    const int            play_svgSize = 1128;

    extern const char*   settings_svg;
    const int            settings_svgSize = 2022;

    extern const char*   visible_svg;
    const int            visible_svgSize = 1504;

    extern const char*   JetBrainsMonoItalic_ttf;
    const int            JetBrainsMonoItalic_ttfSize = 276840;

    extern const char*   JetBrainsMonoRegular_ttf;
    const int            JetBrainsMonoRegular_ttfSize = 273900;

    extern const char*   JetBrainsMonoSemiBold_ttf;
    const int            JetBrainsMonoSemiBold_ttfSize = 277092;

    extern const char*   JetBrainsMonoSemiBoldItalic_ttf;
    const int            JetBrainsMonoSemiBoldItalic_ttfSize = 279828;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 14;

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
