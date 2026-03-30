-- TODO set all of the default's

--[[
--ALL VALIDATION occurs AFTER initialization as some fields might be internal
and required, therefore, having to be setup through C in the initialization
phase of the final config object.

-- readonly, when applied to the config => make the whole config readonly unless
a field is marked as `readonly = false`
-- readonly, when applied to a specific field => makes that field only readonly

-- is_array, when set to true creates the <name> as <name_array> and a
<name_array_length>, regarding the item itself and it's length respectivelly
]]

return function(saci)
  return {
    name = "saci_base_renderer",

    init = function()
      saci.register_custom_type("SaciUniformElement", {
        name = {
          data_type = saci.types.STRING,
          default = nil,
        },

        type = {
          data_type = saci.types.SACITYPE,
          default = saci.types.UNKNOWN,
          required = true
        },

        location = {
          data_type = saci.types.S32,
          default = 0,
          min = 1,
          required = true,
          internal = true,
          readonly = true,
        },
      });

      saci.register_custom_type("SaciSamplerElement", {
        name = {
          data_type = saci.types.STRING,
          default = nil,
          required = true
        },

        type = {
          data_type = saci.types.SACITYPE,
          default = saci.types.UNKNOWN,
          required = true
        },

        offset = {
          data_type = saci.types.U64,
          default = 0,
          min = 1,
          required = true
        },

        location = {
          data_type = saci.types.S32,
          default = 0,
          min = 1,
          required = true,
          internal = true,
          readonly = true,
        },
      })

      saci.register_custom_type("SaciVertexAttributeElement", {
        name = {
          data_type = saci.types.STRING,
          default = nil,
          required = true
        },

        type = {
          data_type = saci.types.SACITYPE,
          default = saci.types.UNKNOWN,
          required = true
        },

        offset = {
          data_type = saci.types.U64,
          default = 0,
          min = 1,
          required = true
        },

        location = {
          data_type = saci.types.S32,
          default = 0,
          min = 1,
          required = true,
          internal = true,
          readonly = true,
        },
      })

      saci.register_custom_type("SaciInstanceBufferLayout", {
        name = {
          data_type = saci.types.STRING,
          default = nil,
          required = true
        },

        type = {
          data_type = saci.types.SACITYPE,
          default = saci.types.UNKNOWN,
          required = true
        },

        offset = {
          data_type = saci.types.U64,
          default = 0,
          min = 1,
          required = true
        },

        location = {
          data_type = saci.types.S32,
          default = 0,
          min = 1,
          required = true
        },
        is_array = true,
      })

      saci.platform.register_config("saci_base_renderer", {
        backend = {
          api = "OpenGL",
          api_version = "4",
        },

        strings = {
          max_length = 64,
          style_transform = "lower_case",
        },

        fields = {
          readonly = true, -- sets all fields as readonly, can be removed if the field itself has `readonly = false`
          required = true, -- sets all fields as required, can be removed if the field itself has `required = false`

          vertex_attribute = {
            attribute_size = {
              data_type = saci.types.U64,
              internal = true
            },

            attribute = {
              data_type = saci.get_custom_type("SaciVertexAttributeElement"),
              is_array = true
            },
          },


          uniform = {
            data_type = saci.get_custom_type("SaciUniformElement"),
            required = false,
            is_array = true,
          },

          sampler = {
            data_type = saci.get_custom_type("SaciSamplerElement"),
            required = false,
            is_array = true,
          },

          instance_buffer = {
            name = {
              data_type = saci.types.STRING,
              default = nil
            },

            size = {
              data_type = saci.types.U64,
              default = 0,
              min = 1,
            },

            layout = {
              data_type = saci.get_custom_type("SaciInstanceBufferLayout"),
              is_array = true,
            },

            is_array = true,
            required = false,
          },

          batch = {
            capacity = { data_type = saci.types.U64, default = 10, min = 1, },
            index_capacity = { data_type = saci.types.U64, default = 10000, min = 3, },
            vertex_capacity = { data_type = saci.types.U64, default = 10000, min = 1, },
            instance_capacity = { data_type = saci.types.U64, default = 1000, min = 1, },

            required = false,
          },

          bound = {
            vertex_capacity = { data_type = saci.types.U64, default = 1000, min = 1, },
            index_capacity = { data_type = saci.types.U64, default = 1000, min = 1, },
            instance_capacity = { data_type = saci.types.U64, default = 1000, min = 1, },

            required = false,
          },

          draw_opts = {
            primitives = { data_type = saci.types.PRIMITIVES, default = saci.Primitives.TRIANGLES, required = false },
            cull_mode = { data_type = saci.types.CULL_MODE, default = saci.CullMode.BACK, required = false },
            front_face = { data_type = saci.types.FRONT_FACE, default = saci.FrontFace.CCW, required = false },

            required = false,
          },

          pipeline = {
            depth_test = { data_type = saci.types.BOOLEAN, default = true },
            blend = {
              data_type = saci.get_custom_type({
                enabled = { data_type = saci.types.BOOLEAN, default = true, },
                opts = { data_type = saci.types.U64, default = 1, },
              }),
            },
            default = { blend = { enabled = true, opts = 1 } },
            required = false,
          },

          index_data = {
            element_size = {
              data_type = saci.types.U64,
              default = 4,
            },
          },
        },
      })
    end
  }
end
