local function load_env_table(filename)
	local env = {}
	local file = io.open(filename, "r")
	if not file then
		error("Could not open .env file: " .. filename)
	end

	for line in file:lines() do
		-- Skip empty lines and comments
		if line:match("%S") and not line:match("^%s*#") then
			local key, value = line:match("^%s*([%w_]+)%s*=%s*(.+)%s*$")
			if key and value then
				-- Remove surrounding quotes if present
				value = value:gsub('^"(.*)"$', "%1")
				value = value:gsub("^'(.*)'$", "%1")
				env[key] = value
			end
		end
	end

	file:close()
	return env
end

local function interpolate(str, vars)
	return (str:gsub("%${([%w_]+)}", function(key)
		return vars[key] or ""
	end))
end

local config = load_env_table("../.env")

Saci_base = {
	render_api = {
		api = "OpenGL",
		path = interpolate("${OPENGL_PATH}", config),
	},
	render_loader = {
		api = "GLAD",
		path = interpolate("${GLAD_PATH}", config),
	},
	window_api = {
		api = "GLFW",
		path = interpolate("${GLFW_PATH}", config),
	},
}
