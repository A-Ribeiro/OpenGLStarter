git clone https://gitlab.freedesktop.org/wayland/wayland-protocols.git
cd wayland-protocols/stable/fractional-scale
wayland-scanner client-header fractional-scale-v1.xml fractional-scale-v1-client-protocol.h
wayland-scanner private-code fractional-scale-v1.xml fractional-scale-v1-client-protocol.c




#include <wayland-client.h>
#include <fractional-scale-v1-client-protocol.h> // Make sure to have this protocol header
#include <iostream>
#include <list>

struct OutputInfo {
    int32_t width = 0;
    int32_t height = 0;
    double fractional_scale = 1.0;
    std::string make;
    std::string model;

    wl_output *output;
    wp_fractional_scale_v1 *fractional_scale_obj;
};

// Fractional scale callback
static void handle_preferred_scale(void *data, struct wp_fractional_scale_v1 *fractional_scale, uint32_t scale_numerator, uint32_t scale_denominator) {
    OutputInfo *info = static_cast<OutputInfo *>(data);
    info->fractional_scale = static_cast<double>(scale_numerator) / scale_denominator;
    std::cout << "Fractional Scale: " << info->fractional_scale << std::endl;
}

// wl_output callbacks
static void handle_geometry(void *data, struct wl_output *output, int32_t x, int32_t y, int32_t physical_width, int32_t physical_height, int32_t subpixel, const char *make, const char *model, int32_t transform) {
    OutputInfo *info = static_cast<OutputInfo *>(data);
    info->make = make;
    info->model = model;
}

static void handle_mode(void *data, struct wl_output *output, uint32_t flags, int32_t width, int32_t height, int32_t refresh) {
    OutputInfo *info = static_cast<OutputInfo *>(data);
    if (flags & WL_OUTPUT_MODE_CURRENT) {
        info->width = width;
        info->height = height;
    }
}

static void handle_scale(void *data, struct wl_output *output, int32_t factor) {
    // Integer scaling - handled here for backward compatibility
}

static const struct wl_output_listener output_listener = {
    handle_geometry,
    handle_mode,
    nullptr, // handle_done (not used in this example)
    handle_scale,
    nullptr, // handle_name
    nullptr  // handle_description
};

// Registry callbacks
static void registry_handler(void *data, struct wl_registry *registry, uint32_t id, const char *interface, uint32_t version) {
    WaylandData *wldata = static_cast<WaylandData *>(data);
    if (std::string(interface) == "wl_output") {
        wl_output *output = static_cast<wl_output *>(wl_registry_bind(registry, id, &wl_output_interface, version));
        wldata->outputs.emplace_back();
        auto &output_info = wldata->outputs.back();
        output_info.output = output;
        wl_output_add_listener(output, &output_listener, &output_info);
    } else if (std::string(interface) == "wp_fractional_scale_manager_v1") {
        if (version >= 1) {
            wldata->fractional_scale_manager = static_cast<wp_fractional_scale_manager_v1 *>(
                wl_registry_bind(registry, id, &wp_fractional_scale_manager_v1_interface, version));
        }
    }
}

static void registry_remover(void *data, struct wl_registry *registry, uint32_t id) {
    // Handle object removal, if necessary
}

static const struct wl_registry_listener registry_listener = {
    registry_handler,
    registry_remover};

struct WaylandData {
    std::list<OutputInfo> outputs;
    wp_fractional_scale_manager_v1 *fractional_scale_manager = nullptr;
};

int main() {
    WaylandData wldata;

    wl_display *display = wl_display_connect(nullptr);
    if (!display) {
        std::cerr << "Failed to connect to Wayland display" << std::endl;
        return -1;
    }

    wl_registry *registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &registry_listener, &wldata);
    wl_display_roundtrip(display); // Process registry events

    if (wldata.fractional_scale_manager) {
        for (auto &output : wldata.outputs) {
            output.fractional_scale_obj = wp_fractional_scale_manager_v1_get_fractional_scale(
                wldata.fractional_scale_manager, output.output);
            wp_fractional_scale_v1_add_listener(output.fractional_scale_obj, &fractional_scale_listener, &output);
        }
        wl_display_roundtrip(display); // Process fractional scale events
    }

    for (const auto &output : wldata.outputs) {
        std::cout << "Monitor: " << output.make << " " << output.model << "\n";
        std::cout << "Resolution: " << output.width << "x" << output.height << "\n";
        std::cout << "Fractional Scale: " << output.fractional_scale << "\n";

        wp_fractional_scale_v1_destroy(output.fractional_scale_obj);
        wl_output_destroy(output.output);
    }

    if (wldata.fractional_scale_manager) {
        wp_fractional_scale_manager_v1_destroy(wldata.fractional_scale_manager);
    }

    wl_display_disconnect(display);

    return 0;
}


