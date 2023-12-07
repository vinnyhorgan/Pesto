#include "api.h"

#include <string.h>

#include "../util.h"

Color currentColor = WHITE;

static int indexTexture(lua_State* L)
{
    Texture2D img = *(Texture2D*)luaL_checkudata(L, 1, "Texture");
    const char* key = luaL_checkstring(L, 2);

    if (TextIsEqual(key, "width"))
        lua_pushinteger(L, img.width);
    else if (TextIsEqual(key, "height"))
        lua_pushinteger(L, img.height);
    else if (TextIsEqual(key, "mipmaps"))
        lua_pushinteger(L, img.mipmaps);
    else if (TextIsEqual(key, "format"))
        lua_pushinteger(L, img.format);
    else if (TextIsEqual(key, "id"))
        lua_pushinteger(L, img.id);
    else
        return 0;

    return 1;
}

static int gcTexture(lua_State* L)
{
    Texture2D img = *(Texture2D*)luaL_checkudata(L, 1, "Texture");

    UnloadTexture(img);

    return 0;
}

static int clear(lua_State* L)
{
    unsigned char r = (unsigned char)luaL_checkinteger(L, 1);
    unsigned char g = (unsigned char)luaL_checkinteger(L, 2);
    unsigned char b = (unsigned char)luaL_checkinteger(L, 3);
    unsigned char a = (unsigned char)luaL_checkinteger(L, 4);
    ClearBackground({ r, g, b, a });

    return 0;
}

static int setColor(lua_State* L)
{
    unsigned char r = (unsigned char)luaL_checkinteger(L, 1);
    unsigned char g = (unsigned char)luaL_checkinteger(L, 2);
    unsigned char b = (unsigned char)luaL_checkinteger(L, 3);
    unsigned char a = (unsigned char)luaL_checkinteger(L, 4);
    currentColor = { r, g, b, a };

    return 0;
}

static int getColor(lua_State* L)
{
    lua_pushinteger(L, currentColor.r);
    lua_pushinteger(L, currentColor.g);
    lua_pushinteger(L, currentColor.b);
    lua_pushinteger(L, currentColor.a);

    return 4;
}

static int pixel(lua_State* L)
{
    int x = (int)luaL_checkinteger(L, 1);
    int y = (int)luaL_checkinteger(L, 2);
    DrawPixel(x, y, currentColor);

    return 0;
}

static int line(lua_State* L)
{
    int x1 = (int)luaL_checkinteger(L, 1);
    int y1 = (int)luaL_checkinteger(L, 2);
    int x2 = (int)luaL_checkinteger(L, 3);
    int y2 = (int)luaL_checkinteger(L, 4);
    DrawLine(x1, y1, x2, y2, currentColor);

    return 0;
}

static int circle(lua_State* L)
{
    int x = (int)luaL_checkinteger(L, 1);
    int y = (int)luaL_checkinteger(L, 2);
    float radius = (float)luaL_checknumber(L, 3);
    DrawCircle(x, y, radius, currentColor);

    return 0;
}

static int circleLines(lua_State* L)
{
    int x = (int)luaL_checkinteger(L, 1);
    int y = (int)luaL_checkinteger(L, 2);
    float radius = (float)luaL_checknumber(L, 3);
    DrawCircleLines(x, y, radius, currentColor);

    return 0;
}

static int rectangle(lua_State* L)
{
    int x = (int)luaL_checkinteger(L, 1);
    int y = (int)luaL_checkinteger(L, 2);
    int width = (int)luaL_checkinteger(L, 3);
    int height = (int)luaL_checkinteger(L, 4);
    DrawRectangle(x, y, width, height, currentColor);

    return 0;
}

static int rectangleLines(lua_State* L)
{
    int x = (int)luaL_checkinteger(L, 1);
    int y = (int)luaL_checkinteger(L, 2);
    int width = (int)luaL_checkinteger(L, 3);
    int height = (int)luaL_checkinteger(L, 4);
    DrawRectangleLines(x, y, width, height, currentColor);

    return 0;
}

static int polygon(lua_State* L)
{
    int x = (int)luaL_checkinteger(L, 1);
    int y = (int)luaL_checkinteger(L, 2);
    int sides = (int)luaL_checkinteger(L, 3);
    float radius = (float)luaL_checknumber(L, 4);
    float rotation = (float)luaL_checknumber(L, 5);
    DrawPoly({ (float)x, (float)y }, sides, radius, rotation, currentColor);

    return 0;
}

static int polygonLines(lua_State* L)
{
    int x = (int)luaL_checkinteger(L, 1);
    int y = (int)luaL_checkinteger(L, 2);
    int sides = (int)luaL_checkinteger(L, 3);
    float radius = (float)luaL_checknumber(L, 4);
    float rotation = (float)luaL_checknumber(L, 5);
    DrawPolyLines({ (float)x, (float)y }, sides, radius, rotation, currentColor);

    return 0;
}

static int text(lua_State* L)
{
    const char* text = luaL_checkstring(L, 1);
    int x = (int)luaL_checkinteger(L, 2);
    int y = (int)luaL_checkinteger(L, 3);
    DrawTextEx(currentFont, text, Vector2 { (float)x, (float)y }, (float)currentFont.baseSize, 0, currentColor);

    return 0;
}

static int wrappedText(lua_State* L)
{
    const char* text = luaL_checkstring(L, 1);
    int x = (int)luaL_checkinteger(L, 2);
    int y = (int)luaL_checkinteger(L, 3);
    int w = (int)luaL_checkinteger(L, 4);
    int h = (int)luaL_checkinteger(L, 5);
    DrawTextBoxedSelectable(currentFont, text, { (float)x, (float)y, (float)w, (float)h }, (float)currentFont.baseSize, 0, true, currentColor, 0, 0, WHITE, WHITE);

    return 0;
}

static int load(lua_State* L)
{
    const char* filename = luaL_checkstring(L, 1);
    Texture2D result = LoadTexture(filename);

    void* ud = lua_newuserdata(L, sizeof(Texture2D));
    memcpy(ud, &result, sizeof(Texture2D));
    luaL_setmetatable(L, "Texture");

    return 1;
}

static int draw(lua_State* L)
{
    Texture2D img = *(Texture2D*)luaL_checkudata(L, 1, "Texture");
    int x = (int)luaL_checknumber(L, 2);
    int y = (int)luaL_checknumber(L, 3);
    DrawTexture(img, x, y, currentColor);

    return 0;
}

static int drawRenderTexturePro(lua_State* L)
{
    RenderTexture2D img = *(RenderTexture2D*)luaL_checkudata(L, 1, "RenderTexture");
    float srcX = (float)luaL_checknumber(L, 2);
    float srcY = (float)luaL_checknumber(L, 3);
    float srcW = (float)luaL_checknumber(L, 4);
    float srcH = (float)luaL_checknumber(L, 5);
    float destX = (float)luaL_checknumber(L, 6);
    float destY = (float)luaL_checknumber(L, 7);
    float destW = (float)luaL_checknumber(L, 8);
    float destH = (float)luaL_checknumber(L, 9);
    float originX = (float)luaL_checknumber(L, 10);
    float originY = (float)luaL_checknumber(L, 11);
    float rotation = (float)luaL_checknumber(L, 12);
    DrawTexturePro(img.texture, { srcX, srcY, srcW, srcH }, { destX, destY, destW, destH }, { originX, originY }, rotation, currentColor);

    return 0;
}

static int drawPro(lua_State* L)
{
    Texture2D img = *(Texture2D*)luaL_checkudata(L, 1, "Texture");
    float srcX = (float)luaL_checknumber(L, 2);
    float srcY = (float)luaL_checknumber(L, 3);
    float srcW = (float)luaL_checknumber(L, 4);
    float srcH = (float)luaL_checknumber(L, 5);
    float destX = (float)luaL_checknumber(L, 6);
    float destY = (float)luaL_checknumber(L, 7);
    float destW = (float)luaL_checknumber(L, 8);
    float destH = (float)luaL_checknumber(L, 9);
    float originX = (float)luaL_checknumber(L, 10);
    float originY = (float)luaL_checknumber(L, 11);
    float rotation = (float)luaL_checknumber(L, 12);
    DrawTexturePro(img, { srcX, srcY, srcW, srcH }, { destX, destY, destW, destH }, { originX, originY }, rotation, currentColor);

    return 0;
}

static int loadRenderTexture(lua_State* L)
{
    int width = (int)luaL_checkinteger(L, 1);
    int height = (int)luaL_checkinteger(L, 2);
    RenderTexture result = LoadRenderTexture(width, height);

    void* ud = lua_newuserdata(L, sizeof(RenderTexture));
    memcpy(ud, &result, sizeof(RenderTexture));
    luaL_setmetatable(L, "RenderTexture");

    return 1;
}

static int beginTextureMode(lua_State* L)
{
    RenderTexture2D target = *(RenderTexture2D*)luaL_checkudata(L, 1, "RenderTexture");
    BeginTextureMode(target);

    return 0;
}

static int endTextureMode(lua_State* L)
{
    EndTextureMode();

    return 0;
}

static int beginDrawing(lua_State* L)
{
    BeginDrawing();

    return 0;
}

static int endDrawing(lua_State* L)
{
    EndDrawing();

    return 0;
}

static int getDelta(lua_State* L)
{
    float result = GetFrameTime();
    lua_pushnumber(L, result);

    return 1;
}

static const luaL_Reg functions[] = {
    { "clear", clear },
    { "setColor", setColor },
    { "getColor", getColor },
    { "pixel", pixel },
    { "line", line },
    { "circle", circle },
    { "circleLines", circleLines },
    { "rectangle", rectangle },
    { "rectangleLines", rectangleLines },
    { "polygon", polygon },
    { "polygonLines", polygonLines },
    { "text", text },
    { "wrappedText", wrappedText },
    { "load", load },
    { "draw", draw },
    { "drawPro", drawPro },
    { "loadRenderTexture", loadRenderTexture },
    { "beginTextureMode", beginTextureMode },
    { "endTextureMode", endTextureMode },
    { "drawRenderTexturePro", drawRenderTexturePro },
    { "beginDrawing", beginDrawing },
    { "endDrawing", endDrawing },
    { "getDelta", getDelta },
    { NULL, NULL }
};

int luaopen_graphics(lua_State* L)
{
    luaL_newmetatable(L, "Texture");
    lua_pushcfunction(L, &indexTexture);
    lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, &gcTexture);
    lua_setfield(L, -2, "__gc");
    lua_pop(L, 1);

    luaL_newmetatable(L, "RenderTexture");
    lua_pop(L, 1);

    lua_getglobal(L, "pesto");

    lua_newtable(L);
    luaL_setfuncs(L, functions, 0);
    lua_setfield(L, -2, "graphics");

    lua_pop(L, 1);

    return 1;
}
