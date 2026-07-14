#pragma once
#include "xdg-output-unstable-v1-client-protocol.h"
#include <string>
#include <vector>

struct XdgOutputInfo
{
    std::string name;
    std::string desc;
    int32_t logical_x = 0;
    int32_t logical_y = 0;
    int32_t logical_width = 0;
    int32_t logical_height = 0;
    zxdg_output_v1 *xdg = nullptr;
};

struct XdgOutputs
{
    std::vector<XdgOutputInfo> outputs;
    zxdg_output_manager_v1 *manager = nullptr;
};

bool xdg_registry_handle(XdgOutputs *data, struct wl_registry *registry, uint32_t id, const char *interface, uint32_t version);
void xdg_create_for_wl_output(XdgOutputs *data, zxdg_output_manager_v1 *manager, struct wl_output *output);
void xdg_print_and_cleanup(XdgOutputs *data);
