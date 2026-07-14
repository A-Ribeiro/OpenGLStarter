#include <wayland-client.h>
#include "wlr-output-management-unstable-v1-client-protocol.h"
#include <iostream>
#include <list>
#include <vector>

struct OutputModeInfo
{
    int32_t width = 0;
    int32_t height = 0;
    int32_t refresh = 0;
    bool preferred = false;
    bool current = false;
    zwlr_output_mode_v1 *mode = nullptr;
};

struct OutputHeadInfo
{
    std::string name;
    std::string description;
    std::string make;
    std::string model;
    std::string serial;

    int32_t physical_width = 0;
    int32_t physical_height = 0;
    bool enabled = false;
    int32_t x = 0;
    int32_t y = 0;
    int32_t transform = 0;
    wl_fixed_t scale = 0;

    zwlr_output_head_v1 *head = nullptr;
    std::vector<OutputModeInfo> modes;
};

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

static void handle_output_done(void *data, struct wl_output *wl_output)
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
    handle_output_done,
    handle_scale,
    handle_name,
    handle_description
};

static void handle_manager_head(void *data,
                                struct zwlr_output_manager_v1 *manager,
                                struct zwlr_output_head_v1 *head);
static void handle_manager_done(void *data,
                                struct zwlr_output_manager_v1 *manager,
                                uint32_t serial);
static void handle_manager_finished(void *data,
                                    struct zwlr_output_manager_v1 *manager);

static void handle_head_name(void *data,
                             struct zwlr_output_head_v1 *head,
                             const char *name);
static void handle_head_description(void *data,
                                    struct zwlr_output_head_v1 *head,
                                    const char *description);
static void handle_head_physical_size(void *data,
                                      struct zwlr_output_head_v1 *head,
                                      int32_t width,
                                      int32_t height);
static void handle_head_mode(void *data,
                             struct zwlr_output_head_v1 *head,
                             struct zwlr_output_mode_v1 *mode);
static void handle_head_enabled(void *data,
                                struct zwlr_output_head_v1 *head,
                                int32_t enabled);
static void handle_head_current_mode(void *data,
                                     struct zwlr_output_head_v1 *head,
                                     struct zwlr_output_mode_v1 *mode);
static void handle_head_position(void *data,
                                 struct zwlr_output_head_v1 *head,
                                 int32_t x,
                                 int32_t y);
static void handle_head_transform(void *data,
                                  struct zwlr_output_head_v1 *head,
                                  int32_t transform);
static void handle_head_scale(void *data,
                              struct zwlr_output_head_v1 *head,
                              wl_fixed_t scale);
static void handle_head_finished(void *data,
                                 struct zwlr_output_head_v1 *head);
static void handle_head_make(void *data,
                             struct zwlr_output_head_v1 *head,
                             const char *make);
static void handle_head_model(void *data,
                              struct zwlr_output_head_v1 *head,
                              const char *model);
static void handle_head_serial_number(void *data,
                                      struct zwlr_output_head_v1 *head,
                                      const char *serial_number);

static void handle_mode_size(void *data,
                             struct zwlr_output_mode_v1 *mode,
                             int32_t width,
                             int32_t height);
static void handle_mode_refresh(void *data,
                                struct zwlr_output_mode_v1 *mode,
                                int32_t refresh);
static void handle_mode_preferred(void *data,
                                  struct zwlr_output_mode_v1 *mode);
static void handle_mode_finished(void *data,
                                 struct zwlr_output_mode_v1 *mode);

static const struct zwlr_output_manager_v1_listener output_manager_listener = {
    handle_manager_head,
    handle_manager_done,
    handle_manager_finished
};

static const struct zwlr_output_head_v1_listener output_head_listener = {
    handle_head_name,
    handle_head_description,
    handle_head_physical_size,
    handle_head_mode,
    handle_head_enabled,
    handle_head_current_mode,
    handle_head_position,
    handle_head_transform,
    handle_head_scale,
    handle_head_finished,
    handle_head_make,
    handle_head_model,
    handle_head_serial_number
};

static const struct zwlr_output_mode_v1_listener output_mode_listener = {
    handle_mode_size,
    handle_mode_refresh,
    handle_mode_preferred,
    handle_mode_finished
};

struct WaylandData
{
    std::list<OutputInfo> outputs;
    std::list<OutputHeadInfo> heads;
    zwlr_output_manager_v1 *output_manager = nullptr;
};

static void registry_handler(void *data, struct wl_registry *registry, uint32_t id,
                             const char *interface, uint32_t version)
{
    std::cout << "registry_handler" << "(protocol: " << std::string(interface) << ")" << std::endl;
    WaylandData *wldata = static_cast<WaylandData *>(data);
    if (std::string(interface) == "wl_output")
    {
        wl_output *output = static_cast<wl_output *>(wl_registry_bind(
            registry, id, &wl_output_interface, version));
        wldata->outputs.emplace_back();
        wldata->outputs.back().output = output;
        wl_output_add_listener(output, &output_listener, &wldata->outputs.back());
    }
    else if (std::string(interface) == "zwlr_output_manager_v1")
    {
        wldata->output_manager = static_cast<zwlr_output_manager_v1 *>(
            wl_registry_bind(registry, id, &zwlr_output_manager_v1_interface, version));
        zwlr_output_manager_v1_add_listener(wldata->output_manager, &output_manager_listener, wldata);
    }
}

static void registry_remover(void *data, struct wl_registry *registry, uint32_t id)
{
    // Handle object removal, if necessary
}

static const struct wl_registry_listener registry_listener = {
    registry_handler,
    registry_remover};

static OutputModeInfo *find_mode_info(OutputHeadInfo *headInfo, struct zwlr_output_mode_v1 *mode)
{
    for (auto &m : headInfo->modes)
    {
        if (m.mode == mode)
            return &m;
    }
    return nullptr;
}

static void handle_manager_head(void *data,
                                struct zwlr_output_manager_v1 *manager,
                                struct zwlr_output_head_v1 *head)
{
    WaylandData *wldata = static_cast<WaylandData *>(data);
    wldata->heads.emplace_back();
    auto &headInfo = wldata->heads.back();
    headInfo.head = head;
    zwlr_output_head_v1_set_user_data(head, &headInfo);
    zwlr_output_head_v1_add_listener(head, &output_head_listener, &headInfo);
}

static void handle_manager_done(void *data,
                                struct zwlr_output_manager_v1 *manager,
                                uint32_t serial)
{
    (void)data;
    (void)manager;
    (void)serial;
}

static void handle_manager_finished(void *data,
                                    struct zwlr_output_manager_v1 *manager)
{
    WaylandData *wldata = static_cast<WaylandData *>(data);
    if (wldata->output_manager == manager)
        wldata->output_manager = nullptr;
}

static void handle_head_name(void *data,
                             struct zwlr_output_head_v1 *head,
                             const char *name)
{
    OutputHeadInfo *info = static_cast<OutputHeadInfo *>(data);
    info->name = name;
}

static void handle_head_description(void *data,
                                    struct zwlr_output_head_v1 *head,
                                    const char *description)
{
    OutputHeadInfo *info = static_cast<OutputHeadInfo *>(data);
    info->description = description;
}

static void handle_head_physical_size(void *data,
                                      struct zwlr_output_head_v1 *head,
                                      int32_t width,
                                      int32_t height)
{
    OutputHeadInfo *info = static_cast<OutputHeadInfo *>(data);
    info->physical_width = width;
    info->physical_height = height;
}

static void handle_head_mode(void *data,
                             struct zwlr_output_head_v1 *head,
                             struct zwlr_output_mode_v1 *mode)
{
    OutputHeadInfo *info = static_cast<OutputHeadInfo *>(data);
    info->modes.emplace_back();
    auto &modeInfo = info->modes.back();
    modeInfo.mode = mode;
    zwlr_output_mode_v1_set_user_data(mode, &modeInfo);
    zwlr_output_mode_v1_add_listener(mode, &output_mode_listener, &modeInfo);
}

static void handle_head_enabled(void *data,
                                struct zwlr_output_head_v1 *head,
                                int32_t enabled)
{
    OutputHeadInfo *info = static_cast<OutputHeadInfo *>(data);
    info->enabled = enabled != 0;
}

static void handle_head_current_mode(void *data,
                                     struct zwlr_output_head_v1 *head,
                                     struct zwlr_output_mode_v1 *mode)
{
    OutputHeadInfo *info = static_cast<OutputHeadInfo *>(data);
    if (OutputModeInfo *modeInfo = find_mode_info(info, mode))
    {
        modeInfo->current = true;
    }
}

static void handle_head_position(void *data,
                                 struct zwlr_output_head_v1 *head,
                                 int32_t x,
                                 int32_t y)
{
    OutputHeadInfo *info = static_cast<OutputHeadInfo *>(data);
    info->x = x;
    info->y = y;
}

static void handle_head_transform(void *data,
                                  struct zwlr_output_head_v1 *head,
                                  int32_t transform)
{
    OutputHeadInfo *info = static_cast<OutputHeadInfo *>(data);
    info->transform = transform;
}

static void handle_head_scale(void *data,
                              struct zwlr_output_head_v1 *head,
                              wl_fixed_t scale)
{
    OutputHeadInfo *info = static_cast<OutputHeadInfo *>(data);
    info->scale = scale;
}

static void handle_head_finished(void *data,
                                 struct zwlr_output_head_v1 *head)
{
    OutputHeadInfo *info = static_cast<OutputHeadInfo *>(data);
    info->head = nullptr;
}

static void handle_head_make(void *data,
                             struct zwlr_output_head_v1 *head,
                             const char *make)
{
    OutputHeadInfo *info = static_cast<OutputHeadInfo *>(data);
    info->make = make;
}

static void handle_head_model(void *data,
                              struct zwlr_output_head_v1 *head,
                              const char *model)
{
    OutputHeadInfo *info = static_cast<OutputHeadInfo *>(data);
    info->model = model;
}

static void handle_head_serial_number(void *data,
                                      struct zwlr_output_head_v1 *head,
                                      const char *serial_number)
{
    OutputHeadInfo *info = static_cast<OutputHeadInfo *>(data);
    info->serial = serial_number;
}

static void handle_mode_size(void *data,
                             struct zwlr_output_mode_v1 *mode,
                             int32_t width,
                             int32_t height)
{
    OutputModeInfo *info = static_cast<OutputModeInfo *>(data);
    info->width = width;
    info->height = height;
}

static void handle_mode_refresh(void *data,
                                struct zwlr_output_mode_v1 *mode,
                                int32_t refresh)
{
    OutputModeInfo *info = static_cast<OutputModeInfo *>(data);
    info->refresh = refresh;
}

static void handle_mode_preferred(void *data,
                                  struct zwlr_output_mode_v1 *mode)
{
    OutputModeInfo *info = static_cast<OutputModeInfo *>(data);
    info->preferred = true;
}

static void handle_mode_finished(void *data,
                                 struct zwlr_output_mode_v1 *mode)
{
    OutputModeInfo *info = static_cast<OutputModeInfo *>(data);
    info->mode = nullptr;
}

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
    std::cout << "wl_display_roundtrip" << wl_display_roundtrip(display) << std::endl; // Process output + manager events
    std::cout << "wl_display_roundtrip" << wl_display_roundtrip(display) << std::endl; // Ensure head/mode events are delivered

    for (const auto &output : wldata.outputs)
    {
        std::cout << "Monitor: " << output.make << " " << output.model << "\n";
        std::cout << "Position: " << output.x << " " << output.y << "\n";
        std::cout << "Current resolution: " << output.width << "x" << output.height << " " << (float)output.refresh * 1e-3f << "hz" << "\n";
        std::cout << "Scale: " << output.scale << "\n";
        std::cout << "RealSize: " << output.mwidth << "x" << output.mheight << " mm" << "\n";

        std::cout << "Available wl_output modes:\n";
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

    if (wldata.output_manager)
    {
        std::cout << "\nUsing zwlr_output_manager_v1:\n";
        for (const auto &head : wldata.heads)
        {
            std::cout << "Head: " << head.name << "\n";
            if (!head.description.empty())
                std::cout << "Description: " << head.description << "\n";
            if (!head.make.empty() || !head.model.empty())
                std::cout << "Make/Model: " << head.make << " " << head.model << "\n";
            if (!head.serial.empty())
                std::cout << "Serial: " << head.serial << "\n";
            std::cout << "Position: " << head.x << " " << head.y << "\n";
            std::cout << "Physical size: " << head.physical_width << "x" << head.physical_height << " mm\n";
            std::cout << "Enabled: " << (head.enabled ? "yes" : "no") << "\n";
            std::cout << "Scale: " << wl_fixed_to_double(head.scale) << "\n";
            std::cout << "Available zwlr modes:\n";
            for (const auto &mode : head.modes)
            {
                std::cout << "  " << mode.width << "x" << mode.height << " " << (float)mode.refresh * 1e-3f << "hz";
                if (mode.current)
                    std::cout << " [current]";
                if (mode.preferred)
                    std::cout << " [preferred]";
                std::cout << "\n";
            }
        }
    }

    if (wldata.output_manager)
        zwlr_output_manager_v1_destroy(wldata.output_manager);

    wl_registry_destroy(registry);
    wl_display_disconnect(display);

    return 0;
}
