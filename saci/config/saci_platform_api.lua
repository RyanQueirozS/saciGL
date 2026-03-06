local default_renderer_backend_api = "OpenGL"
local default_renderer_backend_version = "4"

SaciPlatform = {
  renderer = {
    configs = {},
  },

  backend = {
    api = default_renderer_backend_api,
    version = default_renderer_backend_version,

    loader = {
      api = "Glad",
    },
  },

  windowing = {
    api = "GLFW",
    version = "3",
  },
}
