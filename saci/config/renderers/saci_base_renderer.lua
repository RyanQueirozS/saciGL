-- TODO have an access type modifier for location
-- TODO set all of the default's
local saci;


--[[

ALL VALIDATION occurs AFTER initialization as some fields might be internal and
required, therefore, having to be setup through C in the initialization phase of
the final config object.

-- readonly, when applied to the config => make the whole config readonly unless a field is marked as `readonly = false`
-- readonly, when applied to a specific field => makes that field only readonly

-- is_array, when set to true creates the <name> as <name_array> and a <name_array_length>, regarding the item itself and it's length respectivelly

]]

saci.Types.custom_type("saciUniformElement", {
  name = {
    data_type = saci.Types.STRING,
    default = nil,
  },

  type = {
    data_type = saci.Types.SACITYPE,
    default = saci.Types.UNKNOWN,
    required = true
  },

  location = {
    data_type = saci.Types.S32,
    default = 0,
    min = 1,
    required = true,
    internal = true,
    readonly = true,
  },
});

saci.Types.custom_type("saciSamplerElement", {
  name = {
    data_type = saci.Types.STRING,
    default = nil,
    required = true
  },

  type = {
    data_type = saci.Types.SACITYPE,
    default = saci.Types.UNKNOWN,
    required = true
  },

  offset = {
    data_type = saci.Types.U64,
    default = 0,
    min = 1,
    required = true
  },

  location = {
    data_type = saci.Types.S32,
    default = 0,
    min = 1,
    required = true,
    internal = true,
    readonly = true,
  },
})

saci.Types.custom_type("saciVertexAttributeElement", {
  name = {
    data_type = saci.Types.STRING,
    default = nil,
    required = true
  },

  type = {
    data_type = saci.Types.SACITYPE,
    default = saci.Types.UNKNOWN,
    required = true
  },

  offset = {
    data_type = saci.Types.U64,
    default = 0,
    min = 1,
    required = true
  },

  location = {
    data_type = saci.Types.S32,
    default = 0,
    min = 1,
    required = true,
    internal = true,
    readonly = true,
  },
})

saci.Types.custom_type("saciInstanceBufferLayout", {
  type = saci.Types.custom_type({
    name = {
      data_type = saci.Types.STRING,
      default = nil,
      required = true
    },

    type = {
      data_type = saci.Types.SACITYPE,
      default = saci.Types.UNKNOWN,
      required = true
    },

    offset = {
      data_type = saci.Types.U64,
      default = 0,
      min = 1,
      required = true
    },

    location = {
      data_type = saci.Types.S32,
      default = 0,
      min = 1,
      required = true
    },
  }),
})

saci_core.register_renderer("saci_base", {
  strings = {
    max_length = 64,
    style_transform = "lower_case",
  },

  fields = {
    readonly = true, -- sets all fields as readonly, can be removed if the field itself has `readonly = false`
    required = true, -- sets all fields as required, can be removed if the field itself has `required = false`

    vertex_attribute = {
      attribute_size = {
        data_type = saci.Types.U64,
        internal = true
      },

      attribute = {
        data_type = saci.Types.custom_type("saciVertexAttributeElement"),
        is_array = true
      },
    },


    uniform = {
      data_type = saci.Types.custom_type("saciUniformElement"),
      required = false,
      is_array = true,
    },

    sampler = {
      data_type = saci.Types.custom_type("saciSamplerElement"),
      required = false,
      is_array = true,
    },

    instance_buffer = {
      name = {
        data_type = saci.Types.STRING,
        default = nil
      },

      size = {
        data_type = saci.Types.U64,
        default = 0,
        min = 1,
      },

      layout_array = {
        data_type = saci.Types.custom_type("saciInstanceBufferLayout"),
        is_array = true,
      },

      is_array = true,
      required = false,
    },

    batch = {
      capacity = { data_type = saci.Types.U64, default = 10, min = 1, },
      index_capacity = { data_type = saci.Types.U64, default = 10000, min = 3, },
      vertex_capacity = { data_type = saci.Types.U64, default = 10000, min = 1, },
      instance_capacity = { data_type = saci.Types.U64, default = 1000, min = 1, },

      required = false,
    },

    bound = {
      vertex_capacity = { data_type = saci.Types.U64, default = 1000, min = 1, },
      index_capacity = { data_type = saci.Types.U64, default = 1000, min = 1, },
      instance_capacity = { data_type = saci.Types.U64, default = 1000, min = 1, },

      required = false,
    },

    draw_opts = {
      primitives = { data_type = saci.Types.PRIMITIVES, default = saci.Primitives.TRIANGLES, required = false },
      cull_mode = { data_type = saci.Types.CULL_MODE, default = saci.CullMode.BACK, required = false },
      front_face = { data_type = saci.Types.FRONT_FACE, default = saci.FrontFace.CCW, required = false },

      required = false,
    },

    pipeline = {
      depth_test = { data_type = saci.Types.BOOLEAN, default = true },
      blend = {
        data_type = saci.Types.custom_type({
          enabled = { data_type = saci.Types.BOOLEAN, default = true, },
          opts = { data_type = saci.Types.U64, default = 1, },
        }),
      },
      default = { blend = { enabled = true, opts = 1 } },
      required = false,
    },

    index_data = {
      element_size = {
        data_type = saci.Types.U64,
        default = 4,
      },
    },
  },
})
