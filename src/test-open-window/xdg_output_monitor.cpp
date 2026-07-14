#include "xdg_output_monitor.h"
#include <iostream>

static void handle_xdg_name(void *data, struct zxdg_output_v1 *xdg, const char *name)
{
    XdgOutputInfo *info = static_cast<XdgOutputInfo *>(data);
    info->name = name ? name : "";
}

static void handle_xdg_description(void *data, struct zxdg_output_v1 *xdg, const char *desc)
{
    XdgOutputInfo *info = static_cast<XdgOutputInfo *>(data);
    info->desc = desc ? desc : "";
}
static void handle_xdg_logical_position(void *data, struct zxdg_output_v1 *xdg, int32_t x, int32_t y)
{
    XdgOutputInfo *info = static_cast<XdgOutputInfo *>(data);
    info->logical_x = x; info->logical_y = y;
}

static void handle_xdg_logical_size(void *data, struct zxdg_output_v1 *xdg, int32_t w, int32_t h)
{
    XdgOutputInfo *info = static_cast<XdgOutputInfo *>(data);
    info->logical_width = w; info->logical_height = h;

    std::cout << " *** logical size: " << w << "x" << h << std::endl;
}

static void handle_xdg_done(void *data, struct zxdg_output_v1 *xdg) { (void)data; (void)xdg; }
static void handle_xdg_finished(void *data, struct zxdg_output_v1 *xdg) { ((XdgOutputInfo*)data)->xdg = nullptr; }

static const struct zxdg_output_v1_listener xdg_listener = {
    handle_xdg_logical_position,
    handle_xdg_logical_size,
    handle_xdg_done,
    handle_xdg_name,
    handle_xdg_description
};

bool xdg_registry_handle(XdgOutputs *data, struct wl_registry *registry, uint32_t id, const char *interface, uint32_t version)
{
    if (std::string(interface) == "zxdg_output_manager_v1")
    {
        data->manager = static_cast<zxdg_output_manager_v1 *>(wl_registry_bind(registry, id, &zxdg_output_manager_v1_interface, version));
        return true;
    }
    return false;
}

void xdg_create_for_wl_output(XdgOutputs *data, zxdg_output_manager_v1 *manager, struct wl_output *output)
{
    if (!manager) return;
    data->outputs.emplace_back();
    auto &info = data->outputs.back();
    info.xdg = zxdg_output_manager_v1_get_xdg_output(manager, output);
    zxdg_output_v1_add_listener(info.xdg, &xdg_listener, &info);
}

void xdg_print_and_cleanup(XdgOutputs *data)
{
    if (data->manager)
        zxdg_output_manager_v1_destroy(data->manager);

    for (const auto &o : data->outputs)
    {
        std::cout << "XDG Output: " << o.name << " desc=" << o.desc << " logical=" << o.logical_width << "x" << o.logical_height << " @(" << o.logical_x << "," << o.logical_y << ")\n";
        if (o.xdg)
            zxdg_output_v1_destroy(o.xdg);
    }
}
