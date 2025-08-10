-- These should be loaded by default by the api, disconsider them:
saci_base = {
	render_api = {
		api = "OpenGL",
		path = "/usr/lib/libGL.so",
	},
	render_loader = {
		api = "GLAD",
		path = "/path/to/glad.so",
	},
	window_api = {
		api = "GLFW",
		path = "/path/to/libglfw.so",
	},
}

saci_init = {
	window = {
		sdl_flags = nil,
		gl_major = 3,
		gl_minor = 3,
		profile = "CORE",
	},
}
