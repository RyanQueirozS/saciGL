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
		instance = {
			vertex = {
				layout = {
					{ name = "position", type = Saci_Types.VEC3, offset = 0, location = 0 },
					{ name = "color", type = Saci_Types.COLOR, offset = 12, location = 1 },
					{ name = "uv", type = Saci_Types.VEC2, offset = 28, location = 2 },
				},
			},
			index = {
				element_type = Saci_Types.U32,
			},

			shaders = {
				frag = [[
                #version 330 core

                in vec4 v_color;
                in vec2 v_texcoord;

                uniform sampler2D u_texture;
                uniform bool u_use_texture;

                out vec4 frag_color;

                void main()
                {
                    if (u_use_texture) {
                        vec4 texcolor = texture(u_texture, v_texcoord);
                        frag_color = texcolor * v_color;
                    } else {
                        frag_color = v_color;
                    }
                }]],
				vert = [[
                #version 330 core

                layout (location = 0) in vec3 a_pos;
                layout (location = 1) in vec4 a_color;
                layout (location = 2) in vec2 a_texcoord;
                layout (location = 3) in mat4 i_model_matrix;
                layout (location = 7) in vec4 i_color;

                uniform mat4 u_model_matrix;
                uniform mat4 u_view_matrix;
                uniform mat4 u_projection_matrix;
                uniform int u_flags;
                uniform vec4 u_lighting;

                out vec4 v_color;
                out vec2 v_texcoord;\
                void main()
                {
                    gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * i_model_matrix * vec4(a_pos, 1.0);
                    v_color = a_color + i_color;
                    v_texcoord = a_texcoord;
                }]],
				geom = nil,
			},
			uniforms = {
				{
					name = "u_texture",
					type = Saci_Types.SAMPLER_2D,
					location = 0,
				},
				{
					name = "u_use_texture",
					type = Saci_Types.BOOL,
					location = 1,
				},
				{
					name = "u_model_matrix",
					type = Saci_Types.MAT4,
					location = 2,
				},
				{
					name = "u_view_matrix",
					type = Saci_Types.MAT4,
					location = 3,
				},
				{
					name = "u_projection_matrix",
					type = Saci_Types.MAT4,
					location = 4,
				},
				{
					name = "u_flags",
					type = Saci_Types.S32,
					location = 5,
				},
				{
					name = "u_lighting",
					type = Saci_Types.VEC4,
					location = 6,
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
				},
				vertex = {
					capacity = 1000,
					fixed_capacity = true,
				},
				instances = {
					capacity = 10,
					fixed_capacity = true,
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
