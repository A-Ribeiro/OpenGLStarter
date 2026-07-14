#pragma once
#include "wlr-output-management-unstable-v1-client-protocol.h"
#include <vector>
#include <string>

struct ZwlrModeInfo
{
    int32_t width = 0;
    int32_t height = 0;
    int32_t refresh = 0;
    bool preferred = false;
    bool current = false;
    zwlr_output_mode_v1 *mode = nullptr;
};

struct ZwlrHeadInfo
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
    std::vector<ZwlrModeInfo> modes;
};

struct ZwlrOutputs
{
    std::vector<ZwlrHeadInfo> heads;
    zwlr_output_manager_v1 *output_manager = nullptr;
};

bool zwlr_registry_handle(ZwlrOutputs *data, struct wl_registry *registry, uint32_t id, const char *interface, uint32_t version);
void zwlr_print_and_cleanup(ZwlrOutputs *data);
