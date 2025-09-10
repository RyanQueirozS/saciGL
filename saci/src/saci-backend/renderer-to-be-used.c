#if 0
SA_INTERNAL struct sb_StaticRenderer* sb__renderer_static_new(void) {
    struct sb_StaticRenderer* rendr = su_MALLOC_M(sizeof(struct sb_StaticRenderer));
    su_String* render_name = su_string_create("static", su_REALLOCATION_KIND_FIXED_SIZE);
    struct sb_RendererConfig cfg = sb_cfg_manager_get_renderer(render_name);
    sb__renderer_static_fill_default(rendr);
    sb__renderer_init_gl(rendr,&rendr->common, cfg);
    sb__renderer_init_static_batch(rendr, cfg);
    return rendr;
}
SA_INTERNAL void sb__renderer_instanced_new(sb_Renderer* self) {
    struct sb_InstanceRenderer* rendr = su_MALLOC_M(sizeof(struct sb_InstanceRenderer));
    self->rendr.instance_renderer = rendr;
    struct sb_RendererConfig cfg =
        sb_cfg_manager_get_renderer(su_string_create("instance", su_REALLOCATION_KIND_FIXED_SIZE));
    sb__renderer_instance_fill_default(rendr);
    sb__renderer_init_gl(&rendr->common, cfg);
    sb__renderer_init_instance_batch(rendr);
    sb__renderer_init_bound(&rendr->common, cfg);

    // todo send to a separate function
    // rendr->instance_transform_vbo = sb_gl_create_vertex_buffer(
    //     sizeof(su_Mat4) * 1024, NULL, GL_DYNAMIC_DRAW);
    // rendr->instance_color_vbo = sb_gl_create_vertex_buffer(
    //     sizeof(su_Color) * 1024, NULL, GL_DYNAMIC_DRAW);
    // sb_gl_bind_vertex_buffer(rendr->instance_transform_vbo);
    // for (su_U32 i = 0; i < 4; ++i) {
    //     sb_gl_set_vertex_attrib_pointer(3 + i, 4, GL_FLOAT, GL_FALSE,
    //                                     sizeof(su_Mat4), (void*)(sizeof(float) * i * 4));
    //     sb_gl_enable_vertex_attrib_array(3 + i);
    //     sb_gl_funcs.vertex_attrib_divisor(3 + i, 1);
    // }
    // sb_gl_bind_vertex_buffer(rendr->instance_color_vbo);
    // sb_gl_set_vertex_attrib_pointer(7, 4, GL_FLOAT, GL_FALSE,
    //                                 sizeof(su_Vec4),
    //                                 (void*)(0));
    // sb_gl_enable_vertex_attrib_array(7);
    // sb_gl_funcs.vertex_attrib_divisor(7, 1);
}

SA_INTERNAL void sb__renderer_init_gl(union sb_GFXInfo* info_out, const struct sb_RendererConfig rendr_cfg) {
    sb_gfx_create(info_out, rendr_cfg);
}

SA_INTERNAL void sb__renderer_init_static_batch(struct sb_StaticRenderer* rendr, struct sb_RendererConfig cfg) {
    const struct sb_StaticBatchInfo info = rendr->batch_info;
    const struct sb_RendererBoundInfo bound_info = rendr->bound_info;
    su_U64 index_size = info.index_array_capacity * sizeof(su_U32);
    su_U64 vertex_size = info.vertex_array_capacity * sizeof(struct sb_Vertex);
    su_U64 uniform_size = bound_info.uniform_array_capacity * sizeof(struct sb_RendererUniformData);
    su_U64 batch_arena_element_size = index_size + vertex_size + uniform_size + sizeof(struct sb_StaticBatch);
    rendr->batch_info.batch_struct_allocation_size = batch_arena_element_size;

    rendr->batch_ptr_array = su_MALLOC_M(sizeof(struct sb_StaticBatch*) * info.batch_capacity);

    ArenaInit(
        &rendr->batch_arena,
        batch_arena_element_size * info.batch_capacity + 1024);

    for (su_U8 i = 0; i < info.batch_capacity; ++i) {
        void* dyn_batch_mem = ArenaPush(&rendr->batch_arena, sizeof(struct sb_StaticBatch));
        void* index_mem = ArenaPush(&rendr->batch_arena, index_size);
        void* vertex_mem = ArenaPush(&rendr->batch_arena, vertex_size);
        void* uniform_mem = ArenaPush(&rendr->batch_arena, uniform_size);

        struct sb_StaticBatch* batch =
            su_SCAST_TO_M(struct sb_StaticBatch*)(dyn_batch_mem);
        batch->texture = sb_TEXTURE_INVALID;
        batch->index_array = su_darray_create_ctx(
            index_mem,
            index_size + su_SIZE_OF_DARRAY,
            info.index_array_capacity,
            sizeof(su_U32),
            su_TRUE);
        batch->vertex_array = su_darray_create_ctx(
            vertex_mem,
            vertex_size + su_SIZE_OF_DARRAY,
            info.vertex_array_capacity,
            sizeof(struct sb_Vertex),
            su_TRUE);
        batch->uniform_data = su_darray_create_ctx(
            uniform_mem,
            uniform_size + su_SIZE_OF_DARRAY,
            bound_info.uniform_array_capacity,
            sizeof(struct sb_RendererUniformData),
            su_TRUE);
        rendr->batch_ptr_array[i] = batch;
    }
}

SA_INTERNAL void sb__renderer_set_uniform_from_uniform_data(const struct sb_RendererUniformData uniform_data) {
    const void* value_ptr = NULL;

    switch (uniform_data.type) {
    case su_TYPE_U8:
        value_ptr = &uniform_data.value.u8;
        break;
    case su_TYPE_U16:
        value_ptr = &uniform_data.value.u16;
        break;
    case su_TYPE_U32:
    case su_TYPE_SHADERID:
    case su_TYPE_TEXTUREID:
    case su_TYPE_BUFFERID:
        value_ptr = &uniform_data.value.u32;
        break;
    case su_TYPE_U64:
        value_ptr = &uniform_data.value.u64;
        break;

    case su_TYPE_S8:
        value_ptr = &uniform_data.value.s8;
        break;
    case su_TYPE_S16:
        value_ptr = &uniform_data.value.s16;
        break;
    case su_TYPE_S32:
        value_ptr = &uniform_data.value.s32;
        break;
    case su_TYPE_S64:
        value_ptr = &uniform_data.value.s64;
        break;

    case su_TYPE_BOOL:
        value_ptr = &uniform_data.value.boolean;
        break;

    case su_TYPE_UV:
        value_ptr = &uniform_data.value.uv;
        break;
    case su_TYPE_VEC2:
        value_ptr = &uniform_data.value.vec2;
        break;
    case su_TYPE_VEC3:
        value_ptr = &uniform_data.value.vec3;
        break;
    case su_TYPE_VEC4:
        value_ptr = &uniform_data.value.vec4;
        break;

    case su_TYPE_COLOR:
        value_ptr = &uniform_data.value.color;
        break;

    case su_TYPE_MAT4:
        value_ptr = &uniform_data.value.mat4;
        break;

    // TODO NEED TO BE ADDED
    case su_TYPE_MAT2:
    case su_TYPE_MAT3:
    case su_TYPE_MAT2X3:
    case su_TYPE_MAT2X4:
    case su_TYPE_MAT3X2:
    case su_TYPE_MAT3X4:
    case su_TYPE_MAT4X2:
    case su_TYPE_MAT4X3:
        break;

    default:
        su_LOG_ERRORF_PRINT_M(su_LOG_SEVERITY_MEDIUM, su_LOG_CONTEXT_OPENGL,
                              "Invalid type %d for uniform", uniform_data.type);
        return;
    }

    sb_gl_uniform_set_value(uniform_data.location, uniform_data.type, value_ptr);
}

SA_INTERNAL void sb__renderer_static_fill_default(struct sb_StaticRenderer* rendr) {
    rendr->batch_info.uniform_array_capacity = sb_RENDERER_DEFAULT_UNIFORM_CAPACITY;
    rendr->batch_info.batch_capacity = sb_RENDERER_DEFAULT_BATCH_CAPACITY;
    rendr->batch_info.in_use = 0;
    rendr->batch_info.vertex_array_capacity = sb_RENDERER_DEFAULT_BATCH_VERTEX_CAPACITY;

    rendr->common.bound_info.index_array_capacity = sb_RENDERER_DEFAULT_BOUND_INDEX_CAPACITY;
    rendr->common.bound_info.uniform_array_capacity = sb_RENDERER_DEFAULT_UNIFORM_CAPACITY;
    rendr->common.bound.texture = sb_TEXTURE_INVALID;
}

SA_INTERNAL struct sb_RendererConfig sb__renderer_get_static_config(const char* const path) {
    struct sb_RendererConfig cfg = {0};
    if (!path) {
        su_LOG_WARN_PRINT_M(su_LOG_SEVERITY_MEDIUM, su_LOG_CONTEXT_RENDERER,
                            "Could not open renderer config");
        return cfg;
    }
    sb_ConfigState* state = sb_config_load(path);
    if (!state) {
        su_LOG_ERROR_PRINT_M(su_LOG_SEVERITY_HIGH, su_LOG_CONTEXT_RENDERER,
                             "Couldn't load config state");
        return cfg;
    }
    if (!sb_config_load_table(state, "static_renderer")) {
        su_LOG_ERRORF_PRINT_M(su_LOG_SEVERITY_HIGH, su_LOG_CONTEXT_RENDERER,
                              "Couldn't find 'static' renderer in config: %s", path);
        return cfg;
    }

    {
        su_S8 fixed_size = sb_config_get_bool(state, "bound.index.fixed_size");
        SET_FIXED_SIZE(cfg.bound_cfg.index_info.is_fixed_size, sb_RENDERER_DEFAULT_BATCH_IS_FIXED_SIZE, fixed_size);

        su_U64 capacity = sb_config_get_uint32(state, "bound.index.capacity");
        SET_CAPACITY(cfg.bound_cfg.index_info.capacity, sb_RENDERER_DEFAULT_BATCH_INDEX_CAPACITY, capacity);
    }

    {
        su_S8 uniform_fixed_size = sb_config_get_bool(state, "bound.uniform.fixed_size");
        SET_FIXED_SIZE(cfg.bound_cfg.uniform_info.is_fixed_size, sb_RENDERER_DEFAULT_UNIFORM_IS_FIXED_SIZE, uniform_fixed_size);

        su_U64 uniform_capacity = sb_config_get_uint32(state, "bound.uniform.capacity");
        SET_CAPACITY(cfg.bound_cfg.uniform_info.capacity, sb_RENDERER_DEFAULT_UNIFORM_CAPACITY, uniform_capacity);
    }

    {
        su_S8 batch_index_fixed_size = sb_config_get_bool(state, "batch.index.fixed_size");
        SET_FIXED_SIZE(cfg.instance_batch_cfg.index_info.is_fixed_size, sb_RENDERER_DEFAULT_INDEX_IS_FIXED_SIZE, batch_index_fixed_size);

        su_U64 batch_index_capacity = sb_config_get_uint32(state, "batch.index.capacity");
        SET_CAPACITY(cfg.instance_batch_cfg.index_info.capacity, sb_RENDERER_DEFAULT_BATCH_INDEX_CAPACITY, batch_index_capacity);
    }

    {
        su_S8 batch_vertex_fixed_size = sb_config_get_bool(state, "batch.vertex.fixed_size");
        SET_FIXED_SIZE(cfg.instance_batch_cfg.vertex_info.is_fixed_size, sb_RENDERER_DEFAULT_VERTEX_IS_FIXED_SIZE, batch_vertex_fixed_size);

        su_U64 batch_vertex_capacity = sb_config_get_uint32(state, "batch.vertex.capacity");
        SET_CAPACITY(cfg.instance_batch_cfg.vertex_info.capacity, sb_RENDERER_DEFAULT_BATCH_VERTEX_CAPACITY, batch_vertex_capacity);
    }

    {
        su_S8 batch_fixed_size = sb_config_get_bool(state, "batch.fixed_size");
        SET_FIXED_SIZE(cfg.instance_batch_cfg.is_fixed_size, sb_RENDERER_DEFAULT_BATCH_IS_FIXED_SIZE, batch_fixed_size);

        su_U8 batch_capacity = sb_config_get_uint8(state, "batch.capacity");
        SET_CAPACITY(cfg.instance_batch_cfg.capacity, sb_RENDERER_DEFAULT_BATCH_CAPACITY, batch_capacity);
    }

    sb_config_close(state);

    return cfg;
}

SA_INTERNAL void sb__renderer_draw_static_batch(const struct sb_StaticRenderer* rendr) {
    su_LOG_DEBUGF_PRINT_M(su_LOG_DEBUG_TYPE_RENDERER_FUNCTIONS,
                          su_LOG_CONTEXT_RENDERER,
                          "Flushing %d batches", rendr->batch_info.in_use);

    struct sb_PreviousBatch { // Will get bigger later
        su_TextureId texture;
    } previous_batch = {0};

    for (su_U8 i = 0; i < rendr->batch_info.in_use; ++i) {
        struct sb_StaticBatch* batch = rendr->batch_ptr_array[i];

        sb_gl_funcs.use_program(rendr->common.shader_program);

        sb_gl_funcs.bind_vertex_array(rendr->common.vao);

        sb_gl_funcs.bind_buffer(GL_ARRAY_BUFFER, rendr->common.vbo);
        sb_gl_funcs.buffer_subdata(GL_ARRAY_BUFFER, 0,
                                   su_SCAST_TO_M(long int)(sizeof(struct sb_Vertex) * su_darray_length(batch->vertex_array)),
                                   su_darray_get_ptr(batch->vertex_array, 0));

        sb_gl_funcs.bind_buffer(GL_ELEMENT_ARRAY_BUFFER, rendr->common.ibo);
        sb_gl_funcs.buffer_subdata(GL_ELEMENT_ARRAY_BUFFER, 0,
                                   su_SCAST_TO_M(long int)(sizeof(su_U32) * su_darray_length(batch->index_array)),
                                   su_darray_get_ptr(batch->index_array, 0));

        { // Uniforms
            {
                // TODO should be removed
                sb_gl_funcs.enable(GL_DEPTH_TEST);
            }
            for (su_U32 j = 0; j < su_darray_length(rendr->common.bound.uniform_data_array); ++j) {
                struct sb_RendererUniformData uniform_data = {0};
                su_darray_get(batch->uniform_data, j, &uniform_data);
                sb__renderer_set_uniform_from_uniform_data(uniform_data);
            }
        }

        if (batch->texture != previous_batch.texture) {
            sb_gl_funcs.uniform1i(SC_U_USE_TEXTURE_LOC, su_TRUE);
            sb_gl_funcs.active_texture(GL_TEXTURE0);
            sb_gl_funcs.bind_texture(GL_TEXTURE_2D, batch->texture);
            previous_batch.texture = batch->texture;
        }

        sb_gl_funcs.draw_elements(GL_TRIANGLES, su_SCAST_TO_M(int)(su_darray_length(batch->index_array)), GL_UNSIGNED_INT, 0);
        sb_gl_funcs.bind_buffer(GL_UNIFORM_BUFFER, 0);
    }
}

#endif
