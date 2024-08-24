includes("**/xmake.lua")

add_rules("plugin.compile_commands.autoupdate", {outputdir = "."})

set_languages("c++17")
add_includedirs(os.projectdir())

set_optimize("fastest")
add_cxflags("-Wall", "-Wextra", "-Werror")
add_cxflags("-fPIC")
add_cxflags("-g", "-DNDEBUG", "-gz", "-gdwarf-4")
add_ldflags("-Wl,--export-dynamic", "-Wl,--build-id")
if not is_arch("arm.*") then add_ldflags("-flto=thin") end
