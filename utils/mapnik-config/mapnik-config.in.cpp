#include <iostream>
#include <ranges>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <unicode/putil.h>

#include "mapnik/version.hpp"

#define MAPNIK_LIB_NAME "mapnik"

void print_usage(int exit_code)
{
    printf("Usage: mapnik-config [OPTION]\n\n");
    printf("Known values for OPTION are:\n\n");
    printf("  -h --help         display this help and exit\n");
    printf("  -v --version      version information (MAPNIK_VERSION_STRING)\n");
    printf("  --version-number  version number (MAPNIK_VERSION) (new in 2.2.0)\n");
    printf("  --git-revision    git hash from \"git rev-list --max-count=1 HEAD\"\n");
    printf("  --git-describe    git describe output (new in 2.2.0)\n");
    printf("  --fonts           default fonts directory\n");
    printf("  --input-plugins   default input plugins directory\n");
    printf("  --defines         pre-processor defines for Mapnik build (new in 2.2.0)\n");
    printf("  --prefix          Mapnik prefix [default @CMAKE_INSTALL_PREFIX@]\n");
    printf("  --lib-name        Mapnik library name\n");
    printf("  --libs            library linking information\n");
    // printf("  --dep-libs        library linking information for Mapnik dependencies\n");
    // printf("  --ldflags         library paths (-L) information\n");
    // printf("  --includes        include paths (-I) for Mapnik headers (new in 2.2.0)\n");
    // printf("  --dep-includes    include paths (-I) for Mapnik dependencies (new in 2.2.0)\n");
    // printf("  --cxxflags        c++ compiler flags and pre-processor defines (new in 2.2.0)\n");
    // printf("  --cflags          all include paths, compiler flags, and pre-processor defines (for back-compatibility)\n");
    printf("  --cxx             c++ compiler used to build mapnik (new in 2.2.0)\n");
    // printf("  --all-flags       all compile and link flags (new in 2.2.0)\n");
    printf("  --gdal-data       path to GDAL_DATA directory, if detected at build time (new in 3.0.16)\n");
    printf("  --proj-lib        path to PROJ_LIB directory, if detected at build time (new in 3.0.16)\n");
    printf("  --icu-data        path to ICU_DATA directory, if detected at build time (new in 3.0.16)\n");
    exit(exit_code);
}

template<typename... Args>
void print(Args... args)
{
    (std::cout << ... << args) << std::endl;
}

std::string icu_data() {
    std::string icu_data = u_getDataDirectory();
    return icu_data.empty() ? "@ICU_DATA_DIR@" : icu_data;
}

std::string defines(const std::string str, const std::string delimiter)
{
    auto view = str | std::views::split(delimiter);
    std::string defines;
    std::vector<std::string> tokens;

    for (const auto& token : view)
    {
        tokens.emplace_back(token.begin(), token.end());
    }

    for (const auto& token : tokens)
    {
        defines.append("-D" + token + " ");
    }

    return defines;
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        print_usage(1);
    }

    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
    {
        print_usage(0);
    }
    else if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0)
    {
        print(MAPNIK_VERSION_STRING);
    }
    else if (strcmp(argv[1], "--version-number") == 0)
    {
        print(MAPNIK_VERSION);
    }
    else if (strcmp(argv[1], "--git-revision") == 0)
    {
        print("@GIT_REVISION@");
    }
    else if (strcmp(argv[1], "--git-describe") == 0)
    {
        print("@GIT_DESCRIBE@");
    }
    else if (strcmp(argv[1], "--fonts") == 0)
    {
        print("@CMAKE_INSTALL_PREFIX@/@FONTS_INSTALL_DIR@");
    }
    else if (strcmp(argv[1], "--input-plugins") == 0)
    {
        print("@CMAKE_INSTALL_PREFIX@/@PLUGINS_INSTALL_DIR@");
    }
    else if (strcmp(argv[1], "--defines") == 0)
    {
        print(defines("@MAPNIK_COMPILE_DEFS@", ";"));
    }
    else if (strcmp(argv[1], "--prefix") == 0)
    {
        print("@CMAKE_INSTALL_PREFIX@");
    }
    else if (strcmp(argv[1], "--lib-name") == 0)
    {
        print(MAPNIK_LIB_NAME);
    }
    else if (strcmp(argv[1], "--libs") == 0)
    {
        print("@MAPNIK_CONFIG_LIBS@");
    }
    else if (strcmp(argv[1], "--cflags") == 0)
    {
        print("@MAPNIK_CONFIG_CFLAGS@");
    }
    else if (strcmp(argv[1], "--cxx") == 0)
    {
        print(basename("@CMAKE_CXX_COMPILER@"));
    }
    else if (strcmp(argv[1], "--gdal-data") == 0)
    {
        print("@GDAL_DATA_DIR@");
    }
    else if (strcmp(argv[1], "--proj-lib") == 0)
    {
        print("@PROJ_LIB_DIR@");
    }
    else if (strcmp(argv[1], "--icu-data") == 0)
    {
        print(icu_data());
    }
    else
    {
        printf("Unknown option: %s\n", argv[1]);
        print_usage(1);
    }

    return 0;
}
