/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#ifndef BINARYDATA_H_24633736_INCLUDED
#define BINARYDATA_H_24633736_INCLUDED

namespace BinaryData
{
    extern const char*   arrowrightdouble_png;
    const int            arrowrightdouble_pngSize = 1398;

    extern const char*   audiovolumemedium_png;
    const int            audiovolumemedium_pngSize = 1317;

    extern const char*   configure_png;
    const int            configure_pngSize = 1450;

    extern const char*   audioheadphones_png;
    const int            audioheadphones_pngSize = 1184;

    extern const char*   audiovolumemuted_png;
    const int            audiovolumemuted_pngSize = 1677;

    extern const char*   mediaplaybackpause_png;
    const int            mediaplaybackpause_pngSize = 1145;

    extern const char*   mediaplaybackstart_png;
    const int            mediaplaybackstart_pngSize = 1177;

    extern const char*   mediaplaybackstop_png;
    const int            mediaplaybackstop_pngSize = 1165;

    extern const char*   mediaskipbackward_png;
    const int            mediaskipbackward_pngSize = 1211;

    extern const char*   mediaskipforward_png;
    const int            mediaskipforward_pngSize = 1209;

    extern const char*   stop_svg;
    const int            stop_svgSize = 500;

    extern const char*   play_svg;
    const int            play_svgSize = 566;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Number of elements in the namedResourceList array.
    const int namedResourceListSize = 12;

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes) throw();
}

#endif