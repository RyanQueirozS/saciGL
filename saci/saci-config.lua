local Saci_Types = {
	U8 = 1,
	U16 = 2,
	U32 = 3,
	U64 = 4,
	S8 = 5,
	S16 = 6,
	S32 = 7,
	S64 = 8,

	BOOL = 9,

	SHADERID = 10,
	TEXTUREID = 11,
	BUFFERID = 12,

	UV = 13,
	VEC2 = 14,
	VEC3 = 15,
	VEC4 = 16,

	COLOR = 17,

	MAT2 = 18,
	MAT3 = 19,
	MAT4 = 20,

	MAT2X3 = 21,
	MAT2X4 = 22,
	MAT3X2 = 23,
	MAT3X4 = 24,
	MAT4X2 = 25,
	MAT4X3 = 26,

	SAMPLER_2D = 27,
}

local Saci_Primites = {
	TRIANGLES = 1,
}

local Saci_CullMode = {
	BACK = 1,
}

local Saci_FrontFace = {
	CCW = 1,
}

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
		path = config.OPENGL_PATH,
	},
	render_loader = {
		api = "GLAD",
		path = config.GLAD_PATH,
	},
	window_api = {
		api = "GLFW",
		path = config.GLFW_PATH,
	},
}

Saci_Backend = {
	renderers = {
		static = {
			shaders = {
				frag = "",
				vert = "",
				geom = nil,
			},
			uniforms = {
				{
					name = "name",
					type = Saci_Types.VEC3,
					location = 0,
				},
			},
			batch = {
				capacity = 10,
				fixed_capacity = true,

				index = {
					capacity = 1000,
					fixed_capacity = true,
					element_byte_size = 4,
				},
				vertex = {
					capacity = 1000,
					fixed_capacity = true,
					element_byte_size = 32,
				},
			},
		},

		instance = {
			shaders = {
				frag = "",
				vert = "",
				geom = nil,
			},
			uniforms = {
				{
					name = "name",
					type = Saci_Types.VEC3, -- Should have a Type enum
					location = 0,
				},
			},
			samplers = {
				{ name = "", type = Saci_Types.SAMPLER_2D, binding = 0, unit = 0 },
			},
			batch = {
				capacity = 10,
				fixed_capacity = true,

				index = {
					capacity = 1000,
					fixed_capacity = true,
					element_byte_size = 4,
				},
				vertex = {
					capacity = 1000,
					fixed_capacity = true,
					element_byte_size = 32,
					layout = {
						{ name = "position", type = Saci_Types.VEC3, offset = 0, location = 0 },
						{ name = "normal", type = Saci_Types.VEC3, offset = 12, location = 1 },
						{ name = "uv", type = Saci_Types.VEC2, offset = 24, location = 2 },
					},
				},
				instances = {
					capacity = 10,
					fixed_capacity = true,
					element_byte_size = 123123,
					buffers = {
						{
							name = "buffer 1",
							layout = {
								{
									name = "modelMatrix",
									type = Saci_Types.MAT4, -- 64 bytes
									offset = 0,
									location = 3,
								},
								{
									name = "color",
									type = Saci_Types.VEC4, -- 16 bytes
									offset = 64,
									location = 7,
								},
							},
						},
					},
				},
			},
			draw = {
				primitive = Saci_Primites.TRIANGLES,
				cull_mode = Saci_CullMode.BACK,
				front_face = Saci_FrontFace.CCW,
			},
			pipeline = {
				depth_test = true,
				blend = { enabled = true },
			},
		},
	},
}
