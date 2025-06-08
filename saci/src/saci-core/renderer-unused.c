#if 0
// Frees opengl related data in sc_renderer
SA_INTERNAL_INLINE void s_Renderer_Free_Opengl(struct sc_renderer* rendr);

// Frees memory related data in sc_renderer (batch and call included)
SA_INTERNAL void s_Renderer_Free_Memory(struct sc_renderer* rendr);

// Resets bound information
SA_INTERNAL void s_Renderer_Reset_Bound(struct sc_renderer* rendr);

// Resets batch information
SA_INTERNAL void s_Renderer_Reset_Batch(struct sc_renderer* rendr);

#  if 0
SA_API void sc_Renderer_Bind_Index_Buffer(struct sc_renderer* rendr,
                                          const sa_dArray* new_indices) {
    if (!new_indices) {
        sa_Log_Warn_Print_m(sa_LOG_SEVERITY_MEDIUM,
                            sa_LOG_CONTEXT_RENDERER,
                            "Indices are NULL and cannot be bound");
        return;
    }

    sa_Log_DebugF_Print_m(sa_LOG_DEBUG_TYPE_RENDERER_FUNCTIONS, sa_LOG_CONTEXT_RENDERER,
                          "Bound %lu indices", sa_DArray_Length(new_indices));
    sa_DArray_Append(rendr->bound.index_array, new_indices);
}

// TODO
#    if 0
SA_API void sc_Renderer_Push_Model_Mesh(struct sc_renderer* rendr, const sc_modelMesh* model_mesh) {
}
#    endif

SA_API void sc_Renderer_Free(struct sc_renderer* rendr) {
    sc_Renderer_Begin(rendr);
    s_Renderer_Free_Memory(rendr);
    s_Renderer_Free_Opengl(rendr);
    sa_Free_m(rendr);
    rendr = NULL;
}

SA_API void sc_Renderer_Free_Opts(struct sc_renderer* rendr, int free_opts) {
    sc_Renderer_Begin(rendr);
    if (free_opts & sc_RENDERER_FREE_OPT_MEMORY) {
        s_Renderer_Free_Memory(rendr);
    }
    if (free_opts & sc_RENDERER_FREE_OPT_OPENGL) {
        s_Renderer_Free_Opengl(rendr);
    }
    sa_Free_m(rendr);
    rendr = NULL;
}
#  endif

#  if 0
SA_INTERNAL_INLINE void s_Renderer_Free_Opengl(struct sc_renderer* rendr) {
#    ifndef SACI_RENDERING_DISABLED
    glDeleteBuffers(1, &(rendr)->ibo);
    glDeleteBuffers(1, &(rendr)->vbo);
    glDeleteBuffers(1, &(rendr)->vbo);
    glDeleteVertexArrays(1, &(rendr)->vao);
    glDeleteProgram((rendr)->shader_program);
#    endif // SACI_RENDERING_DISABLED
}

SA_INTERNAL void s_Renderer_Free_Memory(struct sc_renderer* rendr) {
    if (!rendr) {
        sa_Log_Error_Print_m(sa_LOG_SEVERITY_MEDIUM,
                             sa_LOG_CONTEXT_RENDERER,
                             "Trying to free a NULL renderer");
        return;
    }
    sa_DArray_Free(rendr->bound.uniform_data_array);
    sa_DArray_Free(rendr->bound.index_array);
    sa_Free_m(sa_Scast_To_m(void*)(rendr->instance_batch_address_array));
    ArenaDelete(&rendr->instance_batch_arena);
    // TODO
    // s_Renderer_Free_Batch_Array(rendr);
}

SA_INTERNAL void s_Renderer_Reset_Bound(struct sc_renderer* rendr) {
    sa_DArray_Clear(rendr->bound.uniform_data_array);
    sa_DArray_Clear(rendr->bound.index_array);
    rendr->bound.texture = SC_TEXTURE_INVALID;
}

SA_INTERNAL void s_Renderer_Reset_Batch(struct sc_renderer* rendr) {
    sa_Log_Debug_Print_m(sa_LOG_DEBUG_TYPE_RENDERER_FUNCTIONS,
                         sa_LOG_CONTEXT_RENDERER,
                         "Reseting batches");
    rendr->instance_batch_info.in_use = 0;
    for (sa_u8 i = 0; i < rendr->instance_batch_info.capacity; ++i) {
        struct sc_instanceBatch* batch = rendr->instance_batch_address_array[i];
        batch->texture = SC_TEXTURE_INVALID;
        sa_DArray_Clear(batch->index_array);
        sa_DArray_Clear(batch->model_matrix_array);
        sa_DArray_Clear(batch->vertex_array);
        sa_DArray_Clear(batch->uniform_data);
    }
}

SA_INTERNAL sa_bool s_Renderer_Validate_Before_Push(const struct sc_renderer* rendr,
                                                    const sa_dArray* pos_array,
                                                    const sa_dArray* uv_array,
                                                    const sa_dArray* color_array) {
    if (!pos_array) {
        sa_Log_Error_Print_m(sa_LOG_SEVERITY_HIGH,
                             sa_LOG_CONTEXT_RENDERER,
                             "Vertex array being pushed has length ZERO or is NULL");
        return sa_FALSE;
    }
    if (sa_DArray_Is_Null(pos_array) || !sa_DArray_Length(pos_array)) {
        sa_Log_Error_Print_m(sa_LOG_SEVERITY_HIGH,
                             sa_LOG_CONTEXT_RENDERER,
                             "Vertex array being pushed has length ZERO or is NULL");
        return sa_FALSE;
    }
    if (sa_DArray_Is_Null(rendr->bound.index_array) || !sa_DArray_Length(rendr->bound.index_array)) {
        sa_Log_Error_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_RENDERER, "NULL index array bound");
        return sa_FALSE;
    }

    sa_Log_Debug_Condition_Print_m(uv_array, sa_LOG_DEBUG_TYPE_RENDERER,
                                   sa_LOG_CONTEXT_OPENGL, "NULL uv array param");

    sa_Log_Debug_Condition_Print_m(color_array, sa_LOG_DEBUG_TYPE_RENDERER,
                                   sa_LOG_CONTEXT_OPENGL, "NULL color array param");

    return sa_TRUE;
}




SA_INTERNAL sa_bool s_Renderer_Batch_Can_Push(const struct sc_renderer* rendr, const struct sc_staticBatch* batch) {
#    if 0
    if (batch->vertex_array_length >= rendr->batch_vertex_capacity) {
        sa_Log_Debug_Print_m(sa_LOG_DEBUG_TYPE_RENDERER_BATCH, sa_LOG_CONTEXT_RENDERER,
                             "Batch has reached it's vertex limit, going to next one");
        return sa_FALSE;
    }
    if (batch->index_array_length >= rendr->bound_index_array_capacity) {
        sa_Log_Debug_Print_m(sa_LOG_DEBUG_TYPE_RENDERER_BATCH, sa_LOG_CONTEXT_RENDERER,
                             "Batch has reached it's index limit, going to next one");
        return sa_FALSE;
    }

    if (batch->texture != rendr->bound_texture_id && batch->texture != 0) {
        sa_Log_DebugF_Print_m(sa_LOG_DEBUG_TYPE_RENDERER_BATCH, sa_LOG_CONTEXT_RENDERER,
                              "Batch has different texture (%d) than bound (%d), skipping to next batch",
                              batch->texture, rendr->bound_texture_id);
        return sa_FALSE;
    }
    // TODO
    // if (batch->uniform_struct_block_size != rendr->bound_uniform_struct_size &&
    //     batch->uniform_struct_block_size != 0) {
    //     sa_Log_Debug_Print_m(sa_LOG_DEBUG_TYPE_RENDERER_BATCH, sa_LOG_CONTEXT_RENDERER,
    //                          "Batch uniform block SIZE is different than bound, skipping to next batch");
    //     return sa_FALSE;
    // }
    // if (batch->uniform_struct_block_size != 0 &&
    //     s_Renderer_Uniform_Is_Equal(batch->uniform_struct_block, batch->uniform_struct_block, batch->uniform_struct_block_size)) {
    //     sa_Log_Debug_Print_m(sa_LOG_DEBUG_TYPE_RENDERER_BATCH, sa_LOG_CONTEXT_RENDERER,
    //                          "Batch uniform is different than bound, skipping to next batch");
    //     return sa_FALSE;
    // }
#    endif
    return sa_TRUE;
}

#  endif
#endif
