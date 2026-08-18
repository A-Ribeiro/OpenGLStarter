
extern "C"
{
#include <lualib.h>
#include <lauxlib.h>
}

#if defined(_WIN32)
#pragma warning(disable : 4996)
#endif

// Function exposed to Lua
int print_values(lua_State *L)
{
    // Lua arguments are 1-indexed
    int number = luaL_checkinteger(L, 1);
    const char *text = luaL_checkstring(L, 2);

    printf(" - Integer: %i\n", number);
    printf(" - String: %s\n", text);

    return 0; // Number of return values pushed onto the Lua stack
}

// This function creates the Lua module.
// You can use in a lua script:
//
//     local mypackage = require("mypackage")
//     mypackage.print_values(42, "Hello from Lua!")
//
extern "C" int luaopen_mypackage(lua_State *L)
{
    // Create the module table
    lua_newtable(L);

    // mypackage.print_values = print_values
    lua_pushcfunction(L, print_values);
    lua_setfield(L, -2, "print_values");

    return 1; // return the module table
}

int main(int argc, char *argv[])
{
    lua_State *L;
    L = luaL_newstate();

    if (L == NULL)
    {
        fprintf(stderr, "Lua: cannot initialize\n");
        return -1;
    }

    luaL_openlibs(L);

    // register global function
    lua_register(L, "print_values_global", print_values);

    {
        // create a global package
        lua_newtable(L);

        lua_pushcfunction(L, print_values);
        lua_setfield(L, -2, "print_values");

        lua_setglobal(L, "custom_print");
    }

    {
        // preload C package definition
        lua_getglobal(L, "package");
        lua_getfield(L, -1, "preload");

        lua_pushcfunction(L, luaopen_mypackage);
        lua_setfield(L, -2, "mypackage");

        lua_pop(L, 2);
    }

    luaL_dostring(L, R"literal(
                    print('registring add function...')
                    function add(a, b)
                        local mypackage = require("mypackage")
                        custom_print.print_values(42, "Hello from Lua!")
                        mypackage.print_values(42, "Hello from Lua!")
                        return a + b
                    end
                  )literal");

    lua_getglobal(L, "add");
    if (lua_isfunction(L, -1))
    {
        lua_pushnumber(L, 3);
        lua_pushnumber(L, 4);
        if (lua_pcall(L, 2, 1, 0) == LUA_OK)
        {
            double result = lua_tonumber(L, -1);
            printf("Result: %f\n", result);
        }
        else
        {
            fprintf(stderr, "Error: %s\n", lua_tostring(L, -1));
        }
    }

    luaL_dostring(L, R"literal(
        local function list_functions(prefix, tbl)
        -- Initialize the tracking table on the first run
        visited = visited or {}
        -- If we already scanned this table, skip it to prevent loops
        if visited[tbl] then return end
        visited[tbl] = true
            for key, value in pairs(tbl) do
                -- Check if the value is a function
                if type(value) == "function" then
                    print(prefix .. key .. "()")
                elseif type(value) == "table" and key ~= "_G" and key ~= "_ENV" then
                    list_functions(prefix .. key .. ".", value)
                end
            end
        end
        -- Start scanning from the root global table
        list_functions("", _G)
    )literal");

    lua_close(L);

    return 0;
}
