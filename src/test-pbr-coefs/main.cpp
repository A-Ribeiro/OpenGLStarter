
#if defined(_WIN32)
#pragma warning(disable : 4996)
#endif

// #include <appkit-gl-engine/Engine.h>

#include <InteractiveToolkit/ITKCommon/Path.h>
#include <InteractiveToolkit/Platform/Thread.h>
#include <InteractiveToolkit/Platform/Process.h>

#include <math.h>

#include "pbr.h"
#include "polar.h"
#include "linear_depth.h"
#include "generation.h"

// #include "FileIterator.h"

// #include "Date.h"
// #include "FileSystem/Directory.h"

#include <InteractiveToolkit/ITKCommon/FileSystem/Directory.h>

#include <regex>

static double smartToDouble(float v)
{
    char aux[32];
    snprintf(aux, 32, "%.6e\n", v);
    double result;
    if (sscanf(aux, "%le", &result) != 1)
        result = v;
    return result;
}

struct OptionKey
{
    const char *key_name;
    std::vector<const char*> options;
    const char *default_option;
};
struct OptionsGroup
{
    const char *group_name;
    std::vector<OptionKey> keys;
};

// typedef std::unordered_map<const char*, std::vector<OptionKey>> OptionsMap;

void complex_string_hierarchy_as_parameter(
    const std::vector<OptionsGroup> &groups)
{
    for (const auto &group : groups)
    {
        printf("\"%s\"\n", group.group_name);
        for (const auto &key : group.keys)
        {
            printf("  \"%s\" -> \"%s\"\n", key.key_name, key.default_option);
            printf("      ");
            for (const auto &value : key.options)
                printf("\"%s\" ", value);
            printf("\n");
        }
    }
}

int main(int argc, char *argv[])
{
    ITKCommon::Path::setWorkingPath(ITKCommon::Path::getExecutablePath(argv[0]));
    // initialize self referencing of the main thread.
    Platform::Thread::staticInitialization();

    // example of several groups with keys, options and default values, all passed as a single parameter to a function.
    complex_string_hierarchy_as_parameter({{"Control",
                                            {
                                                {"Input", {"Steam 1", "Keyboard", "Steam 1 + Keyboard"}, "Steam 1 + Keyboard"},
                                                {"Movement", {"Fluid", "Legacy"}, "Fluid"},
                                            }},
                                           {"Audio",
                                            {
                                                {"EffectsVolume", {"0", "10", "20", "30", "40", "50", "60", "70", "80", "90", "100"}, "100"},
                                                {"MusicVolume", {"0", "10", "20", "30", "40", "50", "60", "70", "80", "90", "100"}, "100"},
                                            }},
                                           {"Video",
                                            {
                                                {"WindowMode", {"Window", "Borderless", "Fullscreen"}, "Window"},
                                                {"Resolution", {"-"}, "-"},
                                                {"Aspect", {"16:9", "16:10"}, "16:9"},
                                                {"AntiAliasing", {"MSAA", "OFF"}, "MSAA"},
                                                {"VSync", {"ON", "OFF"}, "ON"},
                                            }},
                                           {"Extra",
                                            {
                                                {"Language", {"English", "Português (BR)"}, "English"},
                                                {"ColorScheme", {"Blush", "Purple", "Orange", "Green", "Blue", "Dark"}, "Blush"},
                                                {"ButtonAppearance", {"Bend Up", "Bend Down", "Round", "Tip Front", "Tip Back", "Tip Up", "Tip Down", "Square"}, "Bend Up"},
                                                {"UiSize", {"Extra Small", "Small", "Medium", "Large", "Extra Large"}, "Medium"},
                                                {"MeshCrusher", {"ON", "OFF"}, "OFF"},
                                                {"Particles", {"Low", "Medium", "High", "Ultra"}, "High"},
                                                {"OnGameStats", {"OFF", "FPS"}, "OFF"},
                                            }}});

    // Platform::Signal::Set(&on_signal);

    // testPBR_Values();
    // testPolar_Values();
    // linear_depth_Values();
    // linear_ortho_Values();
    // sunFlowerValues();

    /*auto s = "test \\\\\" ";
    printf("original: %s\n", s);
    auto v = ITKCommon::StringUtil::quote_cmd(s);
    printf("quote_cmd: %s\n", v.c_str());

    auto argv_ = ITKCommon::StringUtil::parseArgv("\"Another\\\\ Space\\\\ String\"");
    printf("parsed argv: %s\n", argv_[0].c_str());

    printf("trim: \'%s\'\n",  ITKCommon::StringUtil::trim("   a").c_str() );
    printf("trim: \'%s\'\n",  ITKCommon::StringUtil::trim("a   ").c_str() );
    printf("trim: \'%s\'\n",  ITKCommon::StringUtil::trim("    ").c_str() );
    printf("trim: \'%s\'\n",  ITKCommon::StringUtil::trim("").c_str() );


    argv_.push_back("");
    argv_.push_back("AnotherString");
    argv_.push_back("Another\\ Space\\ String");
    printf("cmd: \'%s\'\n",  ITKCommon::StringUtil::argvToCmd(argv_).c_str() );*/

    // ITKCommon::FileSystem::Directory dir(ITKCommon::Path::getWorkingPath());

    // for (auto & file : dir) {
    //     printf("file: %s\n", file.full_path.c_str());
    //     printf("      Creation: %s\n", file.creationTime.toString(true).c_str());
    //     printf("      Modification: %s\n", file.lastWriteTime.toString(true).c_str());
    //     printf("      size: %" PRIu64 " KB\n", file.size / 1024);
    // }

    // auto date = ITKCommon::Date::NowUTC();
    // printf("Now: %s\n", date.toISOString().c_str());
    // printf("Now(local): %s\n", date.toLocal().toString().c_str());
    // printf("ns %u\n", date.nanoseconds);

    //  Platform::Time tim;
    //  int64_t t = 0;
    //  while (t < INT64_C(5000000)) {
    //      tim.update();
    //      t += tim.deltaTimeMicro;
    //      printf("t: %" PRIi64 "\n", t);
    //  }

    float v = 1234567e-10f;
    printf("%.6e\n", v);

    double vd = smartToDouble(v); // 1234567.891234567e-10;
    printf("%.15e\n", vd);

    return 0;
}
