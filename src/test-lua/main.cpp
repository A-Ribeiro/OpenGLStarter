
extern "C"
{
#include <lualib.h>
#include <lauxlib.h>
}

#if defined(_WIN32)
#pragma warning(disable : 4996)
#endif

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

    luaL_dostring(L,
                  "print('Hello from Lua!')\n"
                  "function add(a, b)\n"
                  "    return a + b\n"
                  "end\n");

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

    luaL_dostring(L,
                  "local function list_functions(prefix, tbl)\n"
                  "-- Initialize the tracking table on the first run\n"
                  "visited = visited or {}\n"
                  "-- If we already scanned this table, skip it to prevent loops\n"
                  "if visited[tbl] then return end\n"
                  "visited[tbl] = true\n"
                  "    for key, value in pairs(tbl) do\n"
                  "        -- Check if the value is a function\n"
                  "        if type(value) == \"function\" then\n"
                  "            print(prefix .. key .. \"()\")\n"
                  "        elseif type(value) == \"table\" and key ~= \"_G\" and key ~= \"_ENV\" then\n"
                  "            list_functions(prefix .. key .. \".\", value)\n"
                  "        end\n"
                  "    end\n"
                  "end\n"
                  "-- Start scanning from the root global table\n"
                  "list_functions(\"\", _G)");

    lua_close(L);

    return 0;
}
