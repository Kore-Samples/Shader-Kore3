/*#include <kinc/graphics5/commandlist.h>
#include <kinc/graphics5/graphics.h>
#include <kinc/graphics5/indexbuffer.h>
#include <kinc/graphics5/pipeline.h>
#include <kinc/graphics5/shader.h>
#include <kinc/graphics5/vertexbuffer.h>*/

#include <kope/graphics5/device.h>

#include <kinc/io/filereader.h>
#include <kinc/system.h>
#include <kinc/window.h>

#include <kong.h>

#include <assert.h>

/*#define BUFFER_COUNT 2
static int currentBuffer = -1;
static kinc_g5_render_target_t framebuffers[BUFFER_COUNT];
static kinc_g5_shader_t vertexShader;
static kinc_g5_shader_t fragmentShader;
static kinc_g5_pipeline_t pipeline;
static kinc_g5_vertex_buffer_t vertices;
static kinc_g5_index_buffer_t indices;
static kinc_g5_command_list_t commandList;

#define HEAP_SIZE 1024 * 1024
static uint8_t *heap = NULL;
static size_t heap_top = 0;

static void *allocate(size_t size) {
    size_t old_top = heap_top;
    heap_top += size;
    assert(heap_top <= HEAP_SIZE);
    return &heap[old_top];
}

static bool first_update = true;*/

static kope_g5_device device;
static kope_g5_command_list list;
static kope_g5_buffer indices;

static void update(void *data) {
	/*currentBuffer = (currentBuffer + 1) % BUFFER_COUNT;

	kinc_g5_begin(&framebuffers[currentBuffer], 0);

	kinc_g5_command_list_begin(&commandList);

	if (first_update) {
	    kinc_g5_command_list_upload_vertex_buffer(&commandList, &vertices);
	    kinc_g5_command_list_upload_index_buffer(&commandList, &indices);
	}

	kinc_g5_command_list_framebuffer_to_render_target_barrier(&commandList, &framebuffers[currentBuffer]);

	kinc_g5_render_target_t *p_framebuffer = &framebuffers[currentBuffer];
	kinc_g5_command_list_set_render_targets(&commandList, &p_framebuffer, 1);
	kinc_g5_command_list_clear(&commandList, &framebuffers[currentBuffer], KINC_G5_CLEAR_COLOR, 0, 0.0f, 0);
	kinc_g5_command_list_set_pipeline(&commandList, &pipeline);

	int offset = 0;
	kinc_g5_vertex_buffer_t *p_vertices = &vertices;
	kinc_g5_command_list_set_vertex_buffers(&commandList, &p_vertices, &offset, 1);
	kinc_g5_command_list_set_index_buffer(&commandList, &indices);
	kinc_g5_command_list_draw_indexed_vertices(&commandList);

	kinc_g5_command_list_render_target_to_framebuffer_barrier(&commandList, &framebuffers[currentBuffer]);
	kinc_g5_command_list_end(&commandList);

	kinc_g5_command_list_execute(&commandList);

	kinc_g5_end(0);
	kinc_g5_swap_buffers();*/

	kope_g5_texture *framebuffer = kope_g5_device_get_framebuffer_texture(&device);

	kope_g5_render_pass_parameters parameters = {0};
	parameters.color_attachments[0].load_op = KOPE_G5_LOAD_OP_CLEAR;
	kope_g5_color clear_color;
	clear_color.r = 1.0f;
	clear_color.g = 0.0f;
	clear_color.b = 0.0f;
	clear_color.a = 1.0f;
	parameters.color_attachments[0].clear_value = clear_color;
	parameters.color_attachments[0].texture = framebuffer;
	kope_g5_command_list_begin_render_pass(&list, &parameters);

	kope_g5_command_list_set_index_buffer(&list, &indices, KOPE_G5_INDEX_FORMAT_UINT16, 0, 3 * sizeof(uint16_t));

	kope_g5_command_list_end_render_pass(&list);

	kope_g5_command_list_present(&list);

	kope_g5_device_submit_command_list(&device, &list);
}

/*static void load_shader(const char *filename, kinc_g5_shader_t *shader, kinc_g5_shader_type_t shader_type) {
    kinc_file_reader_t file;
    kinc_file_reader_open(&file, filename, KINC_FILE_TYPE_ASSET);
    size_t data_size = kinc_file_reader_size(&file);
    uint8_t *data = allocate(data_size);
    kinc_file_reader_read(&file, data, data_size);
    kinc_file_reader_close(&file);
    kinc_g5_shader_init(shader, data, data_size, shader_type);
}*/

int kickstart(int argc, char **argv) {
	kinc_init("Shader", 1024, 768, NULL, NULL);
	kinc_set_update_callback(update, NULL);

	kope_g5_device_wishlist wishlist = {0};
	kope_g5_device_create(&device, &wishlist);

	kope_g5_device_create_command_list(&device, &list);

	/*heap = (uint8_t *)malloc(HEAP_SIZE);
	assert(heap != NULL);

	load_shader("shader.vert", &vertexShader, KINC_G5_SHADER_TYPE_VERTEX);
	load_shader("shader.frag", &fragmentShader, KINC_G5_SHADER_TYPE_FRAGMENT);

	kinc_g5_vertex_structure_t structure;
	kinc_g5_vertex_structure_init(&structure);
	kinc_g5_vertex_structure_add(&structure, "pos", KINC_G4_VERTEX_DATA_F32_3X);
	kinc_g5_pipeline_init(&pipeline);
	pipeline.vertexShader = &vertexShader;
	pipeline.fragmentShader = &fragmentShader;
	pipeline.inputLayout[0] = &structure;
	pipeline.inputLayout[1] = NULL;
	kinc_g5_pipeline_compile(&pipeline);

	kinc_g5_command_list_init(&commandList);
	for (int i = 0; i < BUFFER_COUNT; ++i) {
	    // kinc_g5_render_target_init(&framebuffers[i], kinc_window_width(0), kinc_window_height(0), 16, false, KINC_G5_RENDER_TARGET_FORMAT_32BIT, -1,
	    //                          -i - 1);
	}

	kinc_g5_vertex_buffer_init(&vertices, 3, &structure, true, 0);
	{
	    float *v = kinc_g5_vertex_buffer_lock_all(&vertices);
	    int i = 0;

	    v[i++] = -1;
	    v[i++] = -1;
	    v[i++] = 0.5;

	    v[i++] = 1;
	    v[i++] = -1;
	    v[i++] = 0.5;

	    v[i++] = -1;
	    v[i++] = 1;
	    v[i++] = 0.5;

	    kinc_g5_vertex_buffer_unlock_all(&vertices);
	}

	kinc_g5_index_buffer_init(&indices, 3, KINC_G5_INDEX_BUFFER_FORMAT_16BIT, true);
	{
	    uint16_t *i = (uint16_t *)kinc_g5_index_buffer_lock_all(&indices);
	    i[0] = 0;
	    i[1] = 1;
	    i[2] = 2;
	    kinc_g5_index_buffer_unlock_all(&indices);
	}*/

	kope_g5_buffer_parameters params;
	params.size = 3 * sizeof(uint16_t);
	params.usage_flags = KOPE_G5_BUFFER_USAGE_INDEX;
	kope_g5_device_create_buffer(&device, &params, &indices);
	{
		uint16_t *i = (uint16_t *)kope_g5_buffer_lock(&indices);
		i[0] = 0;
		i[1] = 1;
		i[2] = 2;
		kope_g5_buffer_unlock(&indices);
	}

	kinc_start();

	return 0;
}
