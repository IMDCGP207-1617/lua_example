#include <iostream>

#include <lua.hpp>

const char *luaScript[] = {
"x = 8 "
"return (x > 7)" ,

"x = GetLockpickLevel() "
"return (x > 7) ",

"function game_door_check(context) "
"    local x = GetLockpickLevelFromClass(context) "
"    return (x > 7) "
"end " 
};

class Skills
{
public:
	Skills(int lockPick) : lockpickLevel(lockPick) {}
	int lockpickLevel;
};

int lua_GetLockpickLevel(lua_State *luaState)
{
	lua_pushinteger(luaState, 6);
	return 1;
}

int lua_GetLockpickLevelFromClass(lua_State *luaState)
{
	int args = lua_gettop(luaState);
	if (args != 1)
	{
		lua_pushstring(luaState, "Incorrect arguments for GetLockpickLevelFromClass");
		lua_error(luaState);
	}

	Skills* data = (Skills*)lua_touserdata(luaState, 1);
	if (!data)
	{
		lua_pushstring(luaState, "Invalid skills context");
		lua_error(luaState);
	}

	lua_pushinteger(luaState, data->lockpickLevel);

	return 1;
}

const int SCRIPT = 3;

int main(int argc, char* args)
{
	lua_State *lState;

	lState = luaL_newstate();
	luaL_openlibs(lState);

	lua_register(lState, "GetLockpickLevel", lua_GetLockpickLevel);
	lua_register(lState, "GetLockpickLevelFromClass", lua_GetLockpickLevelFromClass);

	int status = luaL_loadstring(lState, luaScript[SCRIPT-1]);
	if (status)
	{
		std::cout << "Error: " << lua_tostring(lState, -1) << std::endl;
		return 1;
	}

	if (SCRIPT == 3)
	{
		status = lua_pcall(lState, 0, 0, 0);

		lua_getglobal(lState, "game_door_check");

		Skills data(8);
		lua_pushlightuserdata(lState, (void*)&data);

		status = lua_pcall(lState, 1, 1, 0);
	}
	else {
		status = lua_pcall(lState, 0, 1, 0);
	}

	int ret = lua_toboolean(lState, -1);

	std::cout << "lua returned: " << ret << std::endl;

	lua_close(lState);

	std::getchar();
}