# using popen through a system command line is totally valid...

https://gitlab.gnome.org/GNOME/mutter/-/blob/main/tools/gdctl?ref_type=heads

https://github.com/fraz0815/gnome-randr

https://github.com/jadahl/gnome-monitor-config

https://browse.dgit.debian.org/mutter.git/tree/data/dbus-interfaces/org.gnome.Mutter.DisplayConfig.xml

gdbus-codegen --interface-prefix org.gnome.Mutter. \
              --generate-c-code generated_display_config \
              org.gnome.Mutter.DisplayConfig.xml

# 1. Install necessary dependencies (Debian/Ubuntu example)
sudo apt install libglibmm-2.4-dev libgiomm-2.4-dev gdbus-codegen

# 2. Extract or download the XML to your local project directory
# org.gnome.Mutter.DisplayConfig.xml

# 3. Generate the proxy bindings 
gdbus-codegen --interface-prefix org.gnome.Mutter. \
              --generate-c-code generated_display_config \
              org.gnome.Mutter.DisplayConfig.xml


#include <iostream>
#include <memory>
#include <gio/gio.h>
#include "generated_display_config.h" // Include your generated D-Bus proxy header

// RAII cleanup helper for glib structures
struct GObjectDeleter {
    void operator()(void* ptr) const { g_object_unref(ptr); }
};

int main(int argc, char* argv[]) {
    // Initialize the GType system required by GLib/GIO
    #if !GLIB_CHECK_VERSION(2, 36, 0)
    g_type_init();
    #endif

    GError* error = nullptr;

    // Connect to the user's D-Bus Session Bus where Mutter lives
    std::unique_ptr<GDBusConnection, GObjectDeleter> connection(
        g_bus_get_sync(G_BUS_TYPE_SESSION, nullptr, &error), 
        GObjectDeleter()
    );

    if (error) {
        std::cerr << "Failed to connect to D-Bus Session Bus: " << error->message << std::endl;
        g_error_free(error);
        return 1;
    }

    // Instantiate the auto-generated Mutter proxy client object
    // Destination: "org.gnome.Mutter.DisplayConfig"
    // Object Path: "/org/gnome/Mutter/DisplayConfig"
    std::unique_ptr<OrgGnomeMutterDisplayConfig, GObjectDeleter> proxy(
        org_gnome_mutter_display_config_proxy_new_sync(
            connection.get(),
            G_DBUS_PROXY_FLAGS_NONE,
            "org.gnome.Mutter.DisplayConfig",
            "/org/gnome/Mutter/DisplayConfig",
            nullptr,
            &error
        ),
        GObjectDeleter()
    );

    if (error) {
        std::cerr << "Failed to create Mutter D-Bus Proxy: " << error->message << std::endl;
        g_error_free(error);
        return 1;
    }

    std::cout << "Successfully established MIT C++ boundary to Mutter DisplayConfig over D-Bus!" << std::endl;

    // --- EXAMPLE API CALL ---
    // This executes the 'GetCurrentState' method declared in your XML file
    guint serial = 0;
    GVariant* monitors = nullptr;
    GVariant* logical_monitors = nullptr;
    GVariant* properties = nullptr;

    gboolean success = org_gnome_mutter_display_config_call_get_current_state_sync(
        proxy.get(),
        &serial,
        &monitors,
        &logical_monitors,
        &properties,
        nullptr,
        &error
    );

    if (!success || error) {
        std::cerr << "Mutter API Execution error: " << (error ? error->message : "Unknown error") << std::endl;
        if (error) g_error_free(error);
        return 1;
    }

    // Output state configuration metadata safely extracted into your C++ binary
    std::cout << "Current Mutter Display Configuration Serial Sequence: " << serial << std::endl;

    // Clean up variables unpacked from the D-Bus serialization layer
    if (monitors) g_variant_unref(monitors);
    if (logical_monitors) g_variant_unref(logical_monitors);
    if (properties) g_variant_unref(properties);

    return 0;
}


g++ -std=c++17 main.cpp generated_display_config.c \
    $(pkg-config --cflags --libs glib-2.0 gio-2.0) \
    -o mit_display_util


    // --- EXECUTING AND PARSING THE APIS ---
    guint serial = 0;
    GVariant* monitors = nullptr;
    GVariant* logical_monitors = nullptr;
    GVariant* properties = nullptr;

    gboolean success = org_gnome_mutter_display_config_call_get_current_state_sync(
        proxy.get(),
        &serial,
        &monitors,
        &logical_monitors,
        &properties,
        nullptr,
        &error
    );

    if (!success || error) {
        std::cerr << "Mutter API Execution error: " << (error ? error->message : "Unknown") << std::endl;
        if (error) g_error_free(error);
        return 1;
    }

    std::cout << "\n=== Current Layout State (Serial: " << serial << ") ===" << std::endl;

    // Iterate through the array of logical monitors: a(iiduba(ssss)a{sv})
    GVariantIter iter;
    g_variant_iter_init(&iter, logical_monitors);

    gint x, y;
    gdouble scale;
    gboolean is_primary;
    GVariant* transforms = nullptr;
    GVariant* opt_properties = nullptr;

    int monitor_index = 0;
    // Unpack each logical monitor structure tuple step by step
    while (g_variant_iter_loop(&iter, "(iidub@a(ssss)@a{sv})", 
                               &x, &y, &scale, &is_primary, &transforms, &opt_properties)) {
        
        std::cout << "\nLogical Monitor #" << monitor_index++ << (is_primary ? " [PRIMARY]" : "") << std::endl;
        std::cout << "  Position Window: X=" << x << ", Y=" << y << std::endl;
        std::cout << "  Interface Scale Factor: " << scale << std::endl;

        // Dig down into the sub-array of physical monitor specifications assigned to this layout
        GVariantIter sub_iter;
        g_variant_iter_init(&sub_iter, transforms);
        
        const gchar* connector_id;
        const gchar* vendor;
        const gchar* product;
        const gchar* serial_num;
        
        while (g_variant_iter_loop(&sub_iter, "(&s&s&s&s)", &connector_id, &vendor, &product, &serial_num)) {
            std::cout << "  Connected Hardware Port: " << connector_id << std::endl;
            std::cout << "  Device Info: " << vendor << " " << product << " (S/N: " << serial_num << ")" << std::endl;
        }
    }

    // Always release structural memory tracking allocations to prevent leaks
    if (monitors) g_variant_unref(monitors);
    if (logical_monitors) g_variant_unref(logical_monitors);
    if (properties) g_variant_unref(properties);


#include <gio/gio.h>
#include <iostream>
#include "generated_display_config.h"

// Assume 'proxy' and 'serial' are initialized from the previous step

void apply_new_scale_configuration(OrgGnomeMutterDisplayConfig* proxy, guint serial) {
    GError* error = nullptr;

    // 1. Initialize the builder for the 'logical_monitors' array
    // Signature type: a(iiduba(ssss)a{sv})
    GVariantBuilder logical_monitors_builder;
    g_variant_builder_init(&logical_monitors_builder, G_VARIANT_TYPE("a(iiduba(ssss)a{sv})"));

    // 2. Begin constructing a single logical monitor tuple
    g_variant_builder_open(&logical_monitors_builder, G_VARIANT_TYPE("(iiduba(ssss)a{sv})"));
    
    gint x = 0;
    gint y = 0;
    gdouble scale = 2.0; // Target change: set scale to 200%
    gboolean is_primary = TRUE;

    g_variant_builder_add(&logical_monitors_builder, "i", x);
    g_variant_builder_add(&logical_monitors_builder, "i", y);
    g_variant_builder_add(&logical_monitors_builder, "d", scale);
    g_variant_builder_add(&logical_monitors_builder, "b", is_primary);

    // 3. Build the nested array of physical monitor structural strings: a(ssss)
    // This tells Mutter which hardware port is mapped to this logical desktop space
    GVariantBuilder hardware_monitors_builder;
    g_variant_builder_init(&hardware_monitors_builder, G_VARIANT_TYPE("a(ssss)"));
    
    // Replace these strings with your actual hardware info discovered from GetCurrentState
    g_variant_builder_add(&hardware_monitors_builder, "(ssss)", 
                          "eDP-1",    // Connector ID (e.g. HDMI-1, eDP-1)
                          "DEL",      // Vendor string
                          "DELL P24", // Product string
                          "123456"    // Serial number string
    );
    
    g_variant_builder_add_value(&logical_monitors_builder, g_variant_builder_end(&hardware_monitors_builder));

    // 4. Build the optional properties dictionary for this monitor: a{sv}
    GVariantBuilder monitor_props_builder;
    g_variant_builder_init(&monitor_props_builder, G_VARIANT_TYPE("a{sv}"));
    // Leaving it empty is standard unless you are configuring specific features like variable refresh rates
    g_variant_builder_add_value(&logical_monitors_builder, g_variant_builder_end(&monitor_props_builder));

    // Close the logical monitor tuple entry
    g_variant_builder_close(&logical_monitors_builder);

    // Finalize the complete logical monitors array container
    GVariant* logical_monitors_array = g_variant_builder_end(&logical_monitors_builder);

    // 5. Build global properties dictionary: a{sv}
    GVariantBuilder global_props_builder;
    g_variant_builder_init(&global_props_builder, G_VARIANT_TYPE("a{sv}"));
    GVariant* global_properties = g_variant_builder_end(&global_props_builder);

    // 6. Make the synchronous D-Bus configuration call
    // 1 = ask to confirm...
    guint execution_method = 2; // 2 = Persistent save configuration instantly
    
    gboolean success = org_gnome_mutter_display_config_call_apply_monitors_config_sync(
        proxy,
        serial,
        execution_method,
        logical_monitors_array, // Passes ownership safely
        global_properties,       // Passes ownership safely
        nullptr,
        &error
    );

    if (!success || error) {
        std::cerr << "Failed to apply new monitor scale setting: " 
                  << (error ? error->message : "Unknown D-Bus Error") << std::endl;
        if (error) g_error_free(error);
        return;
    }

    std::cout << "Successfully updated the display scaling configuration parameters!" << std::endl;
}
