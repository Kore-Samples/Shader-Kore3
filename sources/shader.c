#include <kope/graphics5/device.h>

#include <kinc/io/filereader.h>
#include <kinc/system.h>
#include <kinc/window.h>

#include <kong.h>

#include <assert.h>

static kope_g5_device device;
static kope_g5_command_list list;
static vertex_in_buffer vertices;
static kope_g5_buffer indices;

static void update(void *data) {
	kope_g5_texture *framebuffer = kope_g5_device_get_framebuffer(&device);

	kope_g5_render_pass_parameters parameters = {0};
	parameters.color_attachments[0].load_op = KOPE_G5_LOAD_OP_CLEAR;
	kope_g5_color clear_color;
	clear_color.r = 0.2f;
	clear_color.g = 0.0f;
	clear_color.b = 0.0f;
	clear_color.a = 1.0f;
	parameters.color_attachments[0].clear_value = clear_color;
	parameters.color_attachments[0].texture = framebuffer;
	kope_g5_command_list_begin_render_pass(&list, &parameters);

	kong_set_pipeline(&list, &pipeline);

	kong_set_vertex_buffer_vertex_in(&list, &vertices);

	kope_g5_command_list_set_index_buffer(&list, &indices, KOPE_G5_INDEX_FORMAT_UINT16, 0, 3 * sizeof(uint16_t));

	kope_g5_command_list_draw_indexed(&list, 3, 1, 0, 0, 0);

	kope_g5_command_list_end_render_pass(&list);

	kope_g5_command_list_present(&list);

	kope_g5_device_execute_command_list(&device, &list);
}

int kickstart(int argc, char **argv) {
	kinc_init("Shader", 1024, 768, NULL, NULL);
	kinc_set_update_callback(update, NULL);

	kope_g5_device_wishlist wishlist = {0};
	kope_g5_device_create(&device, &wishlist);

	kong_init(&device);

	kope_g5_device_create_command_list(&device, &list);

	kong_create_buffer_vertex_in(&device, 3, &vertices);
	vertex_in *v = kong_vertex_in_buffer_lock(&vertices);

	v[0].pos.x = -1.0f;
	v[0].pos.y = -1.0f;
	v[0].pos.z = 0.5f;

	v[1].pos.x = 1.0f;
	v[1].pos.y = -1.0f;
	v[1].pos.z = 0.5f;

	v[2].pos.x = -1.0f;
	v[2].pos.y = 1.0f;
	v[2].pos.z = 0.5f;

	kong_vertex_in_buffer_unlock(&vertices);

	kope_g5_buffer_parameters params;
	params.size = 3 * sizeof(uint16_t);
	params.usage_flags = KOPE_G5_BUFFER_USAGE_INDEX | KOPE_G5_BUFFER_USAGE_CPU_WRITE;
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
