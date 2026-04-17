local default_renderer_backend_api = "OpenGL"
local default_renderer_backend_version = "4"

platform.backend = {
  api = default_renderer_backend_api,
  version = default_renderer_backend_version,

  loader = {
    api = "Glad",
  },
}

platform.windowing = {
  api = "GLFW",
  version = "3",
}
