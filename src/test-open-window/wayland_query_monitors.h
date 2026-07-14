#include <wayland-client.h>
#include <iostream>
#include <list>
#include <vector>

struct OutputInfo
{
    struct Mode
    {
        int32_t width = 0;
        int32_t height = 0;
        int32_t refresh = 0;
        uint32_t flags = 0;
    };

    int32_t width = 0;
    int32_t height = 0;
    int32_t scale = 1;
    std::string make;
    std::string model;

    wl_output *output;

    int32_t x = 0;
    int32_t y = 0;

    int32_t mwidth = 0;
    int32_t mheight = 0;

    int32_t refresh = 0;
    std::vector<Mode> modes;
};

// Callback for geometry event
static void handle_geometry(void *data, struct wl_output *output,
                            int32_t x, int32_t y, int32_t physical_width,
                            int32_t physical_height, int32_t subpixel,
                            const char *make, const char *model, int32_t transform)
{
    OutputInfo *info = static_cast<OutputInfo *>(data);
    info->make = make;
    info->model = model;

    info->x = x;
    info->y = y;

    info->mwidth = physical_width;
    info->mheight = physical_height;
}

// Callback for mode event
static void handle_mode(void *data, struct wl_output *output,
                        uint32_t flags, int32_t width, int32_t height, int32_t refresh)
{
    OutputInfo *info = static_cast<OutputInfo *>(data);
    info->modes.push_back({width, height, refresh, flags});

    if (flags & WL_OUTPUT_MODE_CURRENT)
    {
        info->width = width;
        info->height = height;
        info->refresh = refresh;
    }
}

// Callback for scale event
static void handle_scale(void *data, struct wl_output *output, int32_t factor)
{
    OutputInfo *info = static_cast<OutputInfo *>(data);
    info->scale = factor;
}

static void handle_done(void *data, struct wl_output *wl_output)
{
    OutputInfo *info = static_cast<OutputInfo *>(data);
}

static void handle_name(void *data,
                        struct wl_output *wl_output,
                        const char *name)
{
    OutputInfo *info = static_cast<OutputInfo *>(data);
}
static void handle_description(void *data,
                               struct wl_output *wl_output,
                               const char *description)
{
    OutputInfo *info = static_cast<OutputInfo *>(data);
}

// Output listener to handle the events
static const struct wl_output_listener output_listener = {
    handle_geometry,
    handle_mode,
    handle_done,
    handle_scale,
    handle_name,
    handle_description
};

struct WaylandData
{
    std::list<OutputInfo> outputs;
};

static void registry_handler(void *data, struct wl_registry *registry, uint32_t id,
                             const char *interface, uint32_t version)
{
    std::cout << "registry_handler" << std::endl;
    WaylandData *wldata = static_cast<WaylandData *>(data);
    if (std::string(interface) == "wl_output")
    {
        wl_output *output = static_cast<wl_output *>(wl_registry_bind(
            registry, id, &wl_output_interface, version));
        wldata->outputs.emplace_back();
        wldata->outputs.back().output = output;
        wl_output_add_listener(output, &output_listener, &wldata->outputs.back());
    }
}

static void registry_remover(void *data, struct wl_registry *registry, uint32_t id)
{
    // Handle object removal, if necessary
}

static const struct wl_registry_listener registry_listener = {
    registry_handler,
    registry_remover};

int main_2()
{
    if (const char* env_p = std::getenv("WAYLAND_DISPLAY")) {
        printf("WAYLAND MONITORS!!!\n");
    }

    WaylandData wldata;

    wl_display *display = wl_display_connect(nullptr);
    if (!display)
    {
        std::cerr << "Failed to connect to Wayland display" << std::endl;
        return -1;
    }

    wl_registry *registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &registry_listener, &wldata);

    std::cout << "wl_display_roundtrip" << wl_display_roundtrip(display) << std::endl; // Process registry events
    std::cout << "wl_display_roundtrip" << wl_display_roundtrip(display) << std::endl; // Process output events

    for (const auto &output : wldata.outputs)
    {
        std::cout << "Monitor: " << output.make << " " << output.model << "\n";
        std::cout << "Position: " << output.x << " " << output.y << "\n";
        std::cout << "Current resolution: " << output.width << "x" << output.height << " " << (float)output.refresh * 1e-3f << "hz" << "\n";
        std::cout << "Scale: " << output.scale << "\n";
        std::cout << "RealSize: " << output.mwidth << "x" << output.mheight << " mm" << "\n";

        std::cout << "Available modes:\n";
        for (const auto &mode : output.modes)
        {
            std::cout << "  " << mode.width << "x" << mode.height << " " << (float)mode.refresh * 1e-3f << "hz";
            if (mode.flags & WL_OUTPUT_MODE_CURRENT)
                std::cout << " [current]";
            if (mode.flags & WL_OUTPUT_MODE_PREFERRED)
                std::cout << " [preferred]";
            std::cout << "\n";
        }

        wl_output_destroy(output.output);
    }

    wl_registry_destroy(registry);

    wl_display_disconnect(display);

    return 0;
}
