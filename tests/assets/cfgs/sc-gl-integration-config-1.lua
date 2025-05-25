local batch_vertex_capacity = 250
core_renderer = {
	bound_index_array_capacity = 10000, -- Won't be tested in this one
	batch_index_capacity = batch_vertex_capacity * 6 / 4,
	batch_array_capacity = 10,
	batch_vertex_capacity = batch_vertex_capacity,
}
