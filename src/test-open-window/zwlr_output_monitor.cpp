#include "zwlr_output_monitor.h"
#include <iostream>
#include <cstring>

static ZwlrModeInfo *find_mode(ZwlrHeadInfo *head, zwlr_output_mode_v1 *mode)
{
    for (auto &m : head->modes)
        if (m.mode == mode)
            return &m;
    return nullptr;
}

// mode listeners
static void mode_size(void *data, struct zwlr_output_mode_v1 *mode, int32_t w, int32_t h) { ((ZwlrModeInfo*)data)->width = w; ((ZwlrModeInfo*)data)->height = h; }
static void mode_refresh(void *data, struct zwlr_output_mode_v1 *mode, int32_t r) { ((ZwlrModeInfo*)data)->refresh = r; }
static void mode_preferred(void *data, struct zwlr_output_mode_v1 *mode) { ((ZwlrModeInfo*)data)->preferred = true; }
static void mode_finished(void *data, struct zwlr_output_mode_v1 *mode) { ((ZwlrModeInfo*)data)->mode = nullptr; }
static const struct zwlr_output_mode_v1_listener mode_listener = { mode_size, mode_refresh, mode_preferred, mode_finished };

// head listeners
static void head_name(void *data, struct zwlr_output_head_v1 *head, const char *name) { ((ZwlrHeadInfo*)data)->name = name ? name : ""; }
static void head_description(void *data, struct zwlr_output_head_v1 *head, const char *desc) { ((ZwlrHeadInfo*)data)->description = desc ? desc : ""; }
static void head_physical_size(void *data, struct zwlr_output_head_v1 *head, int32_t w, int32_t h) { ((ZwlrHeadInfo*)data)->physical_width = w; ((ZwlrHeadInfo*)data)->physical_height = h; }
static void head_mode(void *data, struct zwlr_output_head_v1 *head, struct zwlr_output_mode_v1 *mode)
{
    ZwlrHeadInfo *info = static_cast<ZwlrHeadInfo *>(data);
    info->modes.emplace_back();
    auto &mi = info->modes.back();
    mi.mode = mode;
    zwlr_output_mode_v1_set_user_data(mode, &mi);
    zwlr_output_mode_v1_add_listener(mode, &mode_listener, &mi);
}
static void head_enabled(void *data, struct zwlr_output_head_v1 *head, int32_t enabled) { ((ZwlrHeadInfo*)data)->enabled = enabled != 0; }
static void head_current_mode(void *data, struct zwlr_output_head_v1 *head, struct zwlr_output_mode_v1 *mode)
{
    ZwlrHeadInfo *info = static_cast<ZwlrHeadInfo *>(data);
    if (ZwlrModeInfo *mi = find_mode(info, mode)) mi->current = true;
}
static void head_position(void *data, struct zwlr_output_head_v1 *head, int32_t x, int32_t y) { ((ZwlrHeadInfo*)data)->x = x; ((ZwlrHeadInfo*)data)->y = y; }
static void head_transform(void *data, struct zwlr_output_head_v1 *head, int32_t t) { ((ZwlrHeadInfo*)data)->transform = t; }
static void head_scale(void *data, struct zwlr_output_head_v1 *head, wl_fixed_t s) { ((ZwlrHeadInfo*)data)->scale = s; }
static void head_finished(void *data, struct zwlr_output_head_v1 *head) { ((ZwlrHeadInfo*)data)->head = nullptr; }
static void head_make(void *data, struct zwlr_output_head_v1 *head, const char *make) { ((ZwlrHeadInfo*)data)->make = make ? make : ""; }
static void head_model(void *data, struct zwlr_output_head_v1 *head, const char *model) { ((ZwlrHeadInfo*)data)->model = model ? model : ""; }
static void head_serial(void *data, struct zwlr_output_head_v1 *head, const char *serial) { ((ZwlrHeadInfo*)data)->serial = serial ? serial : ""; }

static const struct zwlr_output_head_v1_listener head_listener = {
    head_name, head_description, head_physical_size, head_mode,
    head_enabled, head_current_mode, head_position, head_transform,
    head_scale, head_finished, head_make, head_model, head_serial
};

// manager listeners
static void handle_manager_head(void *data,
                                struct zwlr_output_manager_v1 *manager,
                                struct zwlr_output_head_v1 *head)
{
    ZwlrOutputs *wldata = static_cast<ZwlrOutputs *>(data);
    wldata->heads.emplace_back();
    auto &headInfo = wldata->heads.back();
    headInfo.head = head;
    zwlr_output_head_v1_set_user_data(head, &headInfo);
    zwlr_output_head_v1_add_listener(head, &head_listener, &headInfo);
}

static void handle_manager_done(void *data,
                                struct zwlr_output_manager_v1 *manager,
                                uint32_t serial)
{
    (void)data; (void)manager; (void)serial;
}

static void handle_manager_finished(void *data,
                                    struct zwlr_output_manager_v1 *manager)
{
    ZwlrOutputs *wldata = static_cast<ZwlrOutputs *>(data);
    if (wldata->output_manager == manager)
        wldata->output_manager = nullptr;
}

static const struct zwlr_output_manager_v1_listener manager_listener = { handle_manager_head, handle_manager_done, handle_manager_finished };

bool zwlr_registry_handle(ZwlrOutputs *data, struct wl_registry *registry, uint32_t id, const char *interface, uint32_t version)
{
    if (std::string(interface) == "zwlr_output_manager_v1")
    {
        data->output_manager = static_cast<zwlr_output_manager_v1 *>(wl_registry_bind(registry, id, &zwlr_output_manager_v1_interface, version));
        zwlr_output_manager_v1_add_listener(data->output_manager, &manager_listener, data);
        return true;
    }
    return false;
}

void zwlr_print_and_cleanup(ZwlrOutputs *data)
{
    if (!data->heads.empty())
    {
        std::cout << "\nUsing zwlr_output_manager_v1:\n";
        for (const auto &head : data->heads)
        {
            std::cout << "Head: " << head.name << "\n";
            if (!head.description.empty()) std::cout << "  Description: " << head.description << "\n";
            if (!head.make.empty() || !head.model.empty()) std::cout << "  Make/Model: " << head.make << " " << head.model << "\n";
            if (!head.serial.empty()) std::cout << "  Serial: " << head.serial << "\n";
            std::cout << "  Position: " << head.x << " " << head.y << "\n";
            std::cout << "  Physical size: " << head.physical_width << "x" << head.physical_height << " mm\n";
            std::cout << "  Enabled: " << (head.enabled ? "yes" : "no") << "\n";
            std::cout << "  Scale: " << wl_fixed_to_double(head.scale) << "\n";
            std::cout << "  Available zwlr modes:\n";
            for (const auto &m : head.modes)
            {
                std::cout << "    " << m.width << "x" << m.height << " " << (float)m.refresh * 1e-3f << "hz";
                if (m.current) std::cout << " [current]";
                if (m.preferred) std::cout << " [preferred]";
                std::cout << "\n";
            }
        }
    }

    if (data->output_manager)
        zwlr_output_manager_v1_destroy(data->output_manager);
}
