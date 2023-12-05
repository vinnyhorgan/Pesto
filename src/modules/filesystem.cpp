#include "api.h"

#include <filesystem>

static int changeDirectory(lua_State* L)
{
    const char* dir = luaL_checkstring(L, 1);
    bool result = ChangeDirectory(dir);
    lua_pushboolean(L, result);

    return 1;
}

static int createDirectory(lua_State* L)
{
    const char* dirpath = luaL_checkstring(L, 1);

    try {
        bool result = std::filesystem::create_directory(dirpath);
        lua_pushboolean(L, result);
    } catch (const std::exception& e) {
        TraceLog(LOG_WARNING, e.what());
        lua_pushboolean(L, false);
    }

    return 1;
}

static int exists(lua_State* L)
{
    const char* filename = luaL_checkstring(L, 1);

    if (FileExists(filename) || DirectoryExists(filename)) {
        lua_pushboolean(L, true);
    } else {
        lua_pushboolean(L, false);
    }

    return 1;
}

static int getApplicationDirectory(lua_State* L)
{
    const char* result = GetApplicationDirectory();
    lua_pushstring(L, result);

    return 1;
}

static int getDirectoryItems(lua_State* L)
{
    const char* basepath = luaL_checkstring(L, 1);
    const char* filter = luaL_checkstring(L, 2);
    bool scanSubdirs = lua_toboolean(L, 3);
    FilePathList result = LoadDirectoryFilesEx(basepath, filter, scanSubdirs);

    if (result.count > 0) {
        lua_newtable(L);

        for (int i = 0; i < (int)result.count; i++) {
            lua_pushstring(L, result.paths[i]);
            lua_rawseti(L, -2, i + 1);
        }

        UnloadDirectoryFiles(result);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

static int getDroppedItems(lua_State* L)
{
    FilePathList result = LoadDroppedFiles();

    if (result.count > 0) {
        lua_newtable(L);

        for (int i = 0; i < (int)result.count; i++) {
            lua_pushstring(L, result.paths[i]);
            lua_rawseti(L, -2, i + 1);
        }

        UnloadDroppedFiles(result);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

static int getLastModified(lua_State* L)
{
    const char* filename = luaL_checkstring(L, 1);
    long result = GetFileModTime(filename);
    lua_pushinteger(L, result);

    return 1;
}

static int getRealDirectory(lua_State* L)
{
    const char* filepath = luaL_checkstring(L, 1);
    const char* result = GetDirectoryPath(filepath);
    lua_pushstring(L, result);

    return 1;
}

static int getSize(lua_State* L)
{
    const char* filename = luaL_checkstring(L, 1);
    int result = GetFileLength(filename);
    lua_pushinteger(L, result);

    return 1;
}

static int getWorkingDirectory(lua_State* L)
{
    const char* result = GetWorkingDirectory();
    lua_pushstring(L, result);

    return 1;
}

static int isDirectory(lua_State* L)
{
    const char* path = luaL_checkstring(L, 1);
    bool result = IsPathFile(path);

    if (result) {
        lua_pushboolean(L, false);
    } else {
        bool directoryExists = DirectoryExists(path);
        lua_pushboolean(L, directoryExists);
    }

    return 1;
}

static int isFile(lua_State* L)
{
    const char* path = luaL_checkstring(L, 1);
    bool result = IsPathFile(path);
    lua_pushboolean(L, result);

    return 1;
}

static int isFileDropped(lua_State* L)
{
    bool result = IsFileDropped();
    lua_pushboolean(L, result);

    return 1;
}

static int read(lua_State* L)
{
    const char* filename = luaL_checkstring(L, 1);
    char* result = LoadFileText(filename);

    if (result != NULL) {
        lua_pushstring(L, result);
        UnloadFileText(result);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

static int remove(lua_State* L)
{
    const char* filename = luaL_checkstring(L, 1);

    try {
        bool result = std::filesystem::remove(filename);
        lua_pushboolean(L, result);
    } catch (const std::exception& e) {
        TraceLog(LOG_WARNING, e.what());
        lua_pushboolean(L, false);
    }

    return 1;
}

static int write(lua_State* L)
{
    const char* filename = luaL_checkstring(L, 1);
    const char* text = luaL_checkstring(L, 2);
    bool result = SaveFileText(filename, (char*)text);
    lua_pushboolean(L, result);

    return 1;
}

static const luaL_Reg functions[] = {
    { "changeDirectory", changeDirectory },
    { "createDirectory", createDirectory },
    { "exists", exists },
    { "getApplicationDirectory", getApplicationDirectory },
    { "getDirectoryItems", getDirectoryItems },
    { "getDroppedItems", getDroppedItems },
    { "getLastModified", getLastModified },
    { "getRealDirectory", getRealDirectory },
    { "getSize", getSize },
    { "getWorkingDirectory", getWorkingDirectory },
    { "isDirectory", isDirectory },
    { "isFile", isFile },
    { "isFileDropped", isFileDropped },
    { "read", read },
    { "remove", remove },
    { "write", write },
    { NULL, NULL }
};

int luaopen_filesystem(lua_State* L)
{
    lua_getglobal(L, "pesto");

    lua_newtable(L);
    luaL_setfuncs(L, functions, 0);
    lua_setfield(L, -2, "filesystem");

    lua_pop(L, 1);

    return 1;
}
