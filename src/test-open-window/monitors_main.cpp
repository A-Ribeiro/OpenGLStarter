#include <wayland-client.h>
#include <iostream>
#include "wl_output_monitor.h"
#include "zwlr_output_monitor.h"
#include "xdg_output_monitor.h"

struct MonCtx { WlOutputs wl; ZwlrOutputs zw; XdgOutputs xdg; };

static void registry_handler(void *data, struct wl_registry *registry, uint32_t id,
                             const char *interface, uint32_t version)
{
    std::cout << interface << std::endl;
    MonCtx *ctx = static_cast<MonCtx *>(data);
    // Let each module inspect and bind if interested
    wl_output_registry_handle(&ctx->wl, registry, id, interface, version);
    zwlr_registry_handle(&ctx->zw, registry, id, interface, version);
    xdg_registry_handle(&ctx->xdg, registry, id, interface, version);
}

static void registry_remover(void *data, struct wl_registry *registry, uint32_t id)
{
    (void)data; (void)registry; (void)id;
}

static const struct wl_registry_listener registry_listener = {
    registry_handler,
    registry_remover
};

int run_wayland_queries()
{
    MonCtx ctx;

    wl_display *display = wl_display_connect(nullptr);
    if (!display)
    {
        std::cerr << "Failed to connect to Wayland display" << std::endl;
        return -1;
    }

    wl_registry *registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &registry_listener, &ctx);

    // Roundtrips to gather initial wl_output, zwlr and xdg manager bindings.
    // Run multiple roundtrips to ensure manager/head/mode events are delivered by the compositor.
    wl_display_roundtrip(display);
    wl_display_roundtrip(display);
    wl_display_roundtrip(display);

    // If we have xdg manager, create xdg outputs for each wl_output
    if (ctx.xdg.manager)
    {
        std::cout << "if (ctx.xdg.manager)" << std::endl;
        for (auto &out : ctx.wl.outputs)
        {
            if (out.output){
                xdg_create_for_wl_output(&ctx.xdg, ctx.xdg.manager, out.output);
                std::cout << "xdg_create_for_wl_output" << std::endl;
            }
        }
        wl_display_roundtrip(display);
    }

    // Allow zwlr manager to emit head/mode events (extra roundtrip)
    if (ctx.zw.output_manager)
        wl_display_roundtrip(display);

    // Print collected info
    wl_output_print_and_cleanup(&ctx.wl);
    zwlr_print_and_cleanup(&ctx.zw);
    xdg_print_and_cleanup(&ctx.xdg);

    wl_registry_destroy(registry);
    wl_display_disconnect(display);
    return 0;
}
