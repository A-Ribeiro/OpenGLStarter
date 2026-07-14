#pragma once
#include <wayland-client.h>
#include <list>
#include <string>
#include <vector>

struct WlOutputInfo
{
    int32_t width = 0;
    int32_t height = 0;
    int32_t scale = 1;
    std::string make;
    std::string model;
    wl_output *output = nullptr;
    int32_t x = 0;
    int32_t y = 0;
    int32_t mwidth = 0;
    int32_t mheight = 0;
    int32_t refresh = 0;
    struct Mode { int32_t width; int32_t height; int32_t refresh; uint32_t flags; };
    std::vector<Mode> modes;
};

struct WlOutputs
{
    std::list<WlOutputInfo> outputs;
};

// Return true if handled the registry event
bool wl_output_registry_handle(WlOutputs *wldata, struct wl_registry *registry, uint32_t id, const char *interface, uint32_t version);
void wl_output_print_and_cleanup(WlOutputs *wldata);
