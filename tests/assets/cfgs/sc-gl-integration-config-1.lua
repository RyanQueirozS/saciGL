local batch_vertex_capacity = 250
core_renderer = {
	bound_index_array_capacity = 10000, -- Won't be tested in this one
	batch_index_capacity = batch_vertex_capacity * 6 / 4,
	batch_array_capacity = 10,
	batch_vertex_capacity = batch_vertex_capacity,
}

static_renderer = {
	bound = {
		index = {
			fixed_size = true,
			capacity = 10000,
		},
		uniform = {
			fixed_size = true,
			capacity = 1024,
		},
	},
	batch = {
		index = {
			capacity = 10000,
			fixed_size = true,
		},
		vertex = {
			capacity = 10000,
			fixed_size = true,
		},
		capacity = 10,
		fixed_size = true,
	},
}
