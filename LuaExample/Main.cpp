#include <iostream>

#include <lua.hpp>

// array of lua scripts, 0 is first example and so on
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

// skills class with lock pick level variable
class Skills
{
public:
	Skills(int lockPick) : lockpickLevel(lockPick) {}
	int lockpickLevel;
};

// basic getlockpicklevel function called from lua - just returns 6
int lua_GetLockpickLevel(lua_State *luaState)
{
	// push 6 onto the stack
	lua_pushinteger(luaState, 6);
	return 1;
}

// advanved getlockpicklevel function called from lua, accesses data from context pointer
int lua_GetLockpickLevelFromClass(lua_State *luaState)
{
	// gettop returns the number of items on the stack (so how many arguments have been provided)
	int args = lua_gettop(luaState);
	// if that is anything but 1, we bail
	if (args != 1)
	{
		lua_pushstring(luaState, "Incorrect arguments for GetLockpickLevelFromClass");
		lua_error(luaState);
	}

	// convert the context pointer back into a pointer to a skills object
	Skills* data = (Skills*)lua_touserdata(luaState, 1);
	if (!data)
	{
		lua_pushstring(luaState, "Invalid skills context");
		lua_error(luaState);
	}

	// push the lock pick level from the skills object onto the lua stack
	lua_pushinteger(luaState, data->lockpickLevel);

	return 1;
}

// change this to load earlier scripts
const int SCRIPT = 3;

int main(int argc, char* args)
{
	lua_State *lState;

	// instantiate lua
	lState = luaL_newstate();
	luaL_openlibs(lState);

	// register our two getlockpick level functions
	lua_register(lState, "GetLockpickLevel", lua_GetLockpickLevel);
	lua_register(lState, "GetLockpickLevelFromClass", lua_GetLockpickLevelFromClass);

	// load the lua scripts from strings
	int status = luaL_loadstring(lState, luaScript[SCRIPT-1]);
	if (status)
	{
		std::cout << "Error: " << lua_tostring(lState, -1) << std::endl;
		return 1;
	}

	// only run this section if we're using the class example
	if (SCRIPT == 3)
	{
		// initialise the context object with this pcall
		status = lua_pcall(lState, 0, 0, 0);

		// get the game_door_check function and push it to the stack
		lua_getglobal(lState, "game_door_check");

		// instantiate a skills object
		Skills data(8);
		// store a pointer to the object as light user data on the lua stack
		lua_pushlightuserdata(lState, (void*)&data);

		// trigger pcall, which will call the game_door_check function with the context pointer
		status = lua_pcall(lState, 1, 1, 0);
	}
	else {
		// call the lua code - no arguments, 1 return value, no message handlers
		status = lua_pcall(lState, 0, 1, 0);
	}

	// pull the returned vale off the stack
	int ret = lua_toboolean(lState, -1);

	std::cout << "lua returned: " << ret << std::endl;

	// shutdown lua
	lua_close(lState);

	// wait for input to let us see the output
	std::getchar();
}