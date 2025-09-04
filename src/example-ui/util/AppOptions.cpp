#include "AppOptions.h"

namespace AppOptions
{
    void generate_binary_data_v1(const AppOptions_v1 &options, Platform::ObjectBuffer *output)
    {
        ITKExtension::IO::Writer writer;
        writer.writeInt32(0x01); // version
        writer.writeRaw(&options, sizeof(AppOptions_v1));
        writer.writeToBuffer(output);
    }

    bool parse_binary_data_v1(const Platform::ObjectBuffer &data, AppOptions_v1 *out_options)
    {
        ITKExtension::IO::Reader reader;

        if (!reader.readFromBuffer(data))
            return false;
        if (reader.readInt32() != 0x01)
            return false;
        reader.readRaw(out_options, sizeof(AppOptions_v1));

        return true;
    }

    void create_default_binary_data_v1(AppOptions_v1 *options)
    {
        memset(options, 0, sizeof(AppOptions_v1));
        snprintf(options->Control.Input, sizeof(options->Control.Input), "%s", options->ControlInput[2]);
        snprintf(options->Control.Movement, sizeof(options->Control.Movement), "%s", options->ControlMovement[0]);

        snprintf(options->Audio.EffectsVolume, sizeof(options->Audio.EffectsVolume), "%s", options->AudioVolume[10]);
        snprintf(options->Audio.MusicVolume, sizeof(options->Audio.MusicVolume), "%s", options->AudioVolume[10]);

        snprintf(options->Video.WindowMode, sizeof(options->Video.WindowMode), "%s", options->VideoWindowMode[1]);
        snprintf(options->Video.Resolution, sizeof(options->Video.Resolution), "%s", "");
        snprintf(options->Video.Aspect, sizeof(options->Video.Aspect), "%s", options->VideoAspect[0]);
        snprintf(options->Video.AntiAliasing, sizeof(options->Video.AntiAliasing), "%s", options->VideoAntiAliasing[0]);
        snprintf(options->Video.VSync, sizeof(options->Video.VSync), "%s", options->VideoVSync[0]);

        snprintf(options->Extra.Language, sizeof(options->Extra.Language), "%s", options->ExtraLanguage[0]);
        snprintf(options->Extra.ColorScheme, sizeof(options->Extra.ColorScheme), "%s", options->ExtraColorScheme[0]);
        snprintf(options->Extra.MeshCrusher, sizeof(options->Extra.MeshCrusher), "%s", options->ExtraMeshCrusher[0]);
        snprintf(options->Extra.Particles, sizeof(options->Extra.Particles), "%s", options->ExtraParticles[2]);
        snprintf(options->Extra.OnGameStats, sizeof(options->Extra.OnGameStats), "%s", options->ExtraOnGameStats[0]);

    }
}