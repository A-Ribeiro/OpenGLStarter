
#if defined(_WIN32)
#pragma warning( disable : 4996)
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

int main(int argc, char *argv[])
{
    ITKCommon::Path::setWorkingPath(ITKCommon::Path::getExecutablePath(argv[0]));
    // initialize self referencing of the main thread.
    Platform::Thread::staticInitialization();

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

    double vd = smartToDouble(v);//1234567.891234567e-10;
    printf("%.15e\n", vd);

    return 0;
}
