#include "wl_output_monitor.h"
#include <iostream>
#include <cstring>

// Internal callbacks
static void handle_geometry(void *data, struct wl_output *output,
                            int32_t x, int32_t y, int32_t physical_width,
                            int32_t physical_height, int32_t subpixel,
                            const char *make, const char *model, int32_t transform)
{
    WlOutputInfo *info = static_cast<WlOutputInfo *>(data);
    info->make = make ? make : "";
    info->model = model ? model : "";
    info->x = x;
    info->y = y;
    info->mwidth = physical_width;
    info->mheight = physical_height;
}

static void handle_mode(void *data, struct wl_output *output,
                        uint32_t flags, int32_t width, int32_t height, int32_t refresh)
{
    WlOutputInfo *info = static_cast<WlOutputInfo *>(data);
    info->modes.push_back({width, height, refresh, flags});
    if (flags & WL_OUTPUT_MODE_CURRENT)
    {
        info->width = width;
        info->height = height;
        info->refresh = refresh;
    }
}

static void handle_scale(void *data, struct wl_output *output, int32_t factor)
{
    WlOutputInfo *info = static_cast<WlOutputInfo *>(data);
    info->scale = factor;
}

static void handle_done(void *data, struct wl_output *wl_output)
{
    (void)data; (void)wl_output;
}

static void handle_name(void *data,
                        struct wl_output *wl_output,
                        const char *name)
{
    (void)data; (void)wl_output; (void)name;
}

static void handle_description(void *data,
                               struct wl_output *wl_output,
                               const char *description)
{
    (void)data; (void)wl_output; (void)description;
}

static const struct wl_output_listener output_listener = {
    handle_geometry,
    handle_mode,
    handle_done,
    handle_scale,
    handle_name,
    handle_description
};

bool wl_output_registry_handle(WlOutputs *wldata, struct wl_registry *registry, uint32_t id, const char *interface, uint32_t version)
{
    if (std::string(interface) == "wl_output")
    {
        wl_output *output = static_cast<wl_output *>(wl_registry_bind(
            registry, id, &wl_output_interface, version));
        wldata->outputs.emplace_back();
        wldata->outputs.back().output = output;
        wl_output_add_listener(output, &output_listener, &wldata->outputs.back());
        return true;
    }
    return false;
}

void wl_output_print_and_cleanup(WlOutputs *wldata)
{
    for (const auto &output : wldata->outputs)
    {
        std::cout << "Monitor (wl_output): " << output.make << " " << output.model << "\n";
        std::cout << "  Position: " << output.x << " " << output.y << "\n";
        std::cout << "  Current resolution: " << output.width << "x" << output.height << " " << (float)output.refresh * 1e-3f << "hz" << "\n";
        std::cout << "  Scale: " << output.scale << "\n";
        std::cout << "  Physical: " << output.mwidth << "x" << output.mheight << " mm" << "\n";
        std::cout << "  Available modes:\n";
        for (const auto &mode : output.modes)
        {
            std::cout << "    " << mode.width << "x" << mode.height << " " << (float)mode.refresh * 1e-3f << "hz";
            if (mode.flags & WL_OUTPUT_MODE_CURRENT)
                std::cout << " [current]";
            if (mode.flags & WL_OUTPUT_MODE_PREFERRED)
                std::cout << " [preferred]";
            std::cout << "\n";
        }
        if (output.output)
            wl_output_destroy(output.output);
    }
}
