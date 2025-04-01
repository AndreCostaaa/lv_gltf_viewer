#include "lvgl/lvgl.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "lvgl/src/draw/opengles/lv_draw_opengles.h"
#include "lvgl/src/drivers/glfw/lv_opengles_debug.h"
#include "lvgl/src/drivers/glfw/lv_opengles_texture.h"
#include "lvgl/src/drivers/glfw/lv_opengles_driver.h"

#define JSMN_STRICT
#define JSMN_STATIC
#include "lib/jsmn/jsmn.h"

#include "lib/b64/b64.h"

#include <stdlib.h> /* exit, strtoul, atof */
#include <math.h> /* fabsf, fmaxf */
#include <stdio.h> /* file IO */

#define BUFFER_URI_START "data:application/octet-stream;base64,"

#define STRINGIFY(x) #x

typedef struct {
    float * vertex_array;
    uint32_t vertex_count;
    float vertex_max[3];
    float vertex_min[3];
    uint16_t * index_array;
    uint32_t index_count;
} gltf_data_t;

static jsmntok_t * search_obj(const char * json, const char * search_str, jsmntok_t * obj_tok, jsmntok_t * past_end_tok) {
    LV_ASSERT(obj_tok->type == JSMN_OBJECT);
	jsmntok_t * tok = obj_tok + 1;
    int search_str_len = lv_strlen(search_str);
	while(tok < past_end_tok) {
		LV_ASSERT(tok->type == JSMN_STRING);
		jsmntok_t * value_tok = tok + 1;
		int key_len = tok->end - tok->start;
		if(key_len == search_str_len && 0 == lv_memcmp(search_str, json + tok->start, search_str_len)) {
			return value_tok;
		}
		tok += 2;
		while(tok < past_end_tok && tok->start < value_tok->end) {
			tok++;
		};
	}
	return NULL;
}

static jsmntok_t * array_index(uint32_t index, jsmntok_t * array_tok, jsmntok_t * past_end_tok)
{
    LV_ASSERT(array_tok->type == JSMN_ARRAY);
	jsmntok_t * tok = array_tok + 1;
	while(tok < past_end_tok && tok->start < array_tok->end) {
        jsmntok_t * element_token = tok;
        if(index-- == 0) {
            return element_token;
        }
		tok++;
		while(tok < past_end_tok && tok->start < element_token->end) {
			tok++;
		};
	}
	return NULL;
}

static jsmntok_t * unwrap(jsmntok_t * x) {
	LV_ASSERT(x);
	return x;
}

static gltf_data_t * load_gltf(const char * gltf_path)
{
    gltf_data_t * data = lv_malloc(sizeof(gltf_data_t));

    FILE * f = fopen(gltf_path, "r");
    if(f == NULL) {
        perror("fopen");
        fprintf(stderr, "could not open '%s'\n", gltf_path);
        exit(1);
    }

    fseek(f, 0, SEEK_END);
    int gltf_json_len = ftell(f);
    fseek(f, 0, SEEK_SET);

    char * gltf_json = lv_malloc(gltf_json_len);
    fread(gltf_json, 1, gltf_json_len, f);
    fclose(f);

    jsmntok_t * tokens = NULL;
    int token_count;
    
    int token_max = 1024;
    while(1) {
        tokens = lv_malloc(token_max * sizeof(jsmntok_t));
        jsmn_parser parser;
        jsmn_init(&parser);
        token_count = jsmn_parse(&parser, gltf_json, gltf_json_len, tokens, token_max);
        if(token_count >= 0) {
            break;
        }
        LV_ASSERT(token_count == JSMN_ERROR_NOMEM);
        token_max *= 2;
        lv_free(tokens);
    }

    LV_ASSERT(token_count > 0 && tokens[0].type == JSMN_OBJECT);

    jsmntok_t * past_end_token = tokens + token_count;

	jsmntok_t * asset = unwrap(search_obj(gltf_json, "asset", tokens, past_end_token));
	jsmntok_t * version = unwrap(search_obj(gltf_json, "version", asset, past_end_token));
	int version_len = version->end - version->start;
    char * version_str = gltf_json + version->start;
	if(version_len != 3 || 0 != lv_memcmp(gltf_json + version->start, "2.0", 3)) {
        fprintf(stderr, "gltf version '%.*s' not supported. Only 2.0 supported.\n", version_len, version_str);
        exit(1);
    }

	jsmntok_t * buffers = unwrap(search_obj(gltf_json, "buffers", tokens, past_end_token));
	LV_ASSERT(buffers->type == JSMN_ARRAY);
	jsmntok_t * buffer0 = buffers + 1;
	LV_ASSERT(buffer0->type == JSMN_OBJECT);
	jsmntok_t * uri = unwrap(search_obj(gltf_json, "uri", buffer0, past_end_token));
	uint32_t uri_len = uri->end - uri->start;
	LV_ASSERT(uri_len >= sizeof(BUFFER_URI_START) - 1);
	LV_ASSERT(0 == lv_memcmp(BUFFER_URI_START, gltf_json + uri->start, sizeof(BUFFER_URI_START) - 1));
	char * b64 = gltf_json + (uri->start + (sizeof(BUFFER_URI_START) - 1));
	uint32_t b64_len = uri_len - (sizeof(BUFFER_URI_START) - 1);

	uint32_t decoded_len = B64_REV(b64_len);
	uint8_t * decoded = lv_malloc(decoded_len);

	b64Decode(decoded, (uint8_t *) b64, b64_len);

	jsmntok_t * byteLength = unwrap(search_obj(gltf_json, "byteLength", buffer0, past_end_token));
	LV_ASSERT(strtoul(gltf_json + byteLength->start, NULL, 10) <= decoded_len);

    jsmntok_t * meshes = unwrap(search_obj(gltf_json, "meshes", tokens, past_end_token));
    jsmntok_t * meshes0 = unwrap(array_index(0, meshes, past_end_token));
    jsmntok_t * primitives = unwrap(search_obj(gltf_json, "primitives", meshes0, past_end_token));
    jsmntok_t * primitives0 = unwrap(array_index(0, primitives, past_end_token));
    jsmntok_t * indices = unwrap(search_obj(gltf_json, "indices", primitives0, past_end_token));
    uint32_t indices_accessor_index = strtoul(gltf_json + indices->start, NULL, 10);
    jsmntok_t * attributes = unwrap(search_obj(gltf_json, "attributes", primitives0, past_end_token));
    jsmntok_t * position = unwrap(search_obj(gltf_json, "POSITION", attributes, past_end_token));
    uint32_t vertices_accessor_index = strtoul(gltf_json + position->start, NULL, 10);

    jsmntok_t * accessors = unwrap(search_obj(gltf_json, "accessors", tokens, past_end_token));
    jsmntok_t * accessor;
    jsmntok_t * accessor_bufferview;
    jsmntok_t * accessor_component_type;
    jsmntok_t * accessor_count;
    jsmntok_t * accessor_type;

    accessor = unwrap(array_index(vertices_accessor_index, accessors, past_end_token));
    accessor_bufferview = unwrap(search_obj(gltf_json, "bufferView", accessor, past_end_token));
    uint32_t vertices_bufferview_index = strtoul(gltf_json + accessor_bufferview->start, NULL, 10);
    accessor_component_type = unwrap(search_obj(gltf_json, "componentType", accessor, past_end_token));
	LV_ASSERT(strtoul(gltf_json + accessor_component_type->start, NULL, 10) == 5126);
    accessor_count = unwrap(search_obj(gltf_json, "count", accessor, past_end_token));
    data->vertex_count = strtoul(gltf_json + accessor_count->start, NULL, 10);
    jsmntok_t * accessor_max = unwrap(search_obj(gltf_json, "max", accessor, past_end_token));
    data->vertex_max[0] = atof(gltf_json + unwrap(array_index(0, accessor_max, past_end_token))->start);
    data->vertex_max[1] = atof(gltf_json + unwrap(array_index(1, accessor_max, past_end_token))->start);
    data->vertex_max[2] = atof(gltf_json + unwrap(array_index(2, accessor_max, past_end_token))->start);
    jsmntok_t * accessor_min = unwrap(search_obj(gltf_json, "min", accessor, past_end_token));
    data->vertex_min[0] = atof(gltf_json + unwrap(array_index(0, accessor_min, past_end_token))->start);
    data->vertex_min[1] = atof(gltf_json + unwrap(array_index(1, accessor_min, past_end_token))->start);
    data->vertex_min[2] = atof(gltf_json + unwrap(array_index(2, accessor_min, past_end_token))->start);
    accessor_type = unwrap(search_obj(gltf_json, "type", accessor, past_end_token));
    LV_ASSERT(accessor_type->end - accessor_type->start == 4 && 0 == lv_memcmp("VEC3", gltf_json + accessor_type->start, 4));

    accessor = unwrap(array_index(indices_accessor_index, accessors, past_end_token));
    accessor_bufferview = unwrap(search_obj(gltf_json, "bufferView", accessor, past_end_token));
    uint32_t indices_bufferview_index = strtoul(gltf_json + accessor_bufferview->start, NULL, 10);
    accessor_component_type = unwrap(search_obj(gltf_json, "componentType", accessor, past_end_token));
	LV_ASSERT(strtoul(gltf_json + accessor_component_type->start, NULL, 10) == 5123);
    accessor_count = unwrap(search_obj(gltf_json, "count", accessor, past_end_token));
    data->index_count = strtoul(gltf_json + accessor_count->start, NULL, 10);
    accessor_type = unwrap(search_obj(gltf_json, "type", accessor, past_end_token));
    LV_ASSERT(accessor_type->end - accessor_type->start == 6 && 0 == lv_memcmp("SCALAR", gltf_json + accessor_type->start, 6));

    jsmntok_t * bufferviews = unwrap(search_obj(gltf_json, "bufferViews", tokens, past_end_token));
    jsmntok_t * bufferview;
    jsmntok_t * bufferview_buffer;
    jsmntok_t * bufferview_bytelength;
    jsmntok_t * bufferview_byteoffset;
    uint32_t length;
    uint32_t offset;

    bufferview = unwrap(array_index(vertices_bufferview_index, bufferviews, past_end_token));
    bufferview_buffer = unwrap(search_obj(gltf_json, "buffer", bufferview, past_end_token));
	LV_ASSERT(strtoul(gltf_json + bufferview_buffer->start, NULL, 10) == 0);
    bufferview_bytelength = unwrap(search_obj(gltf_json, "byteLength", bufferview, past_end_token));
    length = strtoul(gltf_json + bufferview_bytelength->start, NULL, 10);
    bufferview_byteoffset = unwrap(search_obj(gltf_json, "byteOffset", bufferview, past_end_token));
    offset = strtoul(gltf_json + bufferview_byteoffset->start, NULL, 10);
    data->vertex_array = lv_malloc(length);
    lv_memcpy(data->vertex_array, decoded + offset, length);

    bufferview = unwrap(array_index(indices_bufferview_index, bufferviews, past_end_token));
    bufferview_buffer = unwrap(search_obj(gltf_json, "buffer", bufferview, past_end_token));
	LV_ASSERT(strtoul(gltf_json + bufferview_buffer->start, NULL, 10) == 0);
    bufferview_bytelength = unwrap(search_obj(gltf_json, "byteLength", bufferview, past_end_token));
    length = strtoul(gltf_json + bufferview_bytelength->start, NULL, 10);
    bufferview_byteoffset = unwrap(search_obj(gltf_json, "byteOffset", bufferview, past_end_token));
    offset = strtoul(gltf_json + bufferview_byteoffset->start, NULL, 10);
    data->index_array = lv_malloc(length);
    lv_memcpy(data->index_array, decoded + offset, length);

	lv_free(decoded);
    lv_free(tokens);
    lv_free(gltf_json);

    return data;
}

static void free_gltf_data(gltf_data_t * gltf_data)
{
    lv_free(gltf_data->index_array);
    lv_free(gltf_data->vertex_array);
    lv_free(gltf_data);
}

unsigned int render_gltf_model_to_opengl_texture(const char * gltf_path, uint32_t texture_w,
                                           uint32_t texture_h, lv_color_t color)
{
    unsigned int texture;

    gltf_data_t * gltf_data = load_gltf(gltf_path);

    GL_CALL(glGenTextures(1, &texture));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, texture));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_w, texture_h, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL));

    GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 20));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST));

    GL_CALL(glBindTexture(GL_TEXTURE_2D, texture));

    unsigned int renderbuffer;
    GL_CALL(glGenRenderbuffers(1, &renderbuffer));
    GL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer));
    GL_CALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, texture_w, texture_h));

    unsigned framebuffer;
    GL_CALL(glGenFramebuffers(1, &framebuffer));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer));

    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0));
    GL_CALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer));

    /* normalize */
    float * tp = gltf_data->vertex_array;
    unsigned tpl = gltf_data->vertex_count * 3;
    float scale_factor = 1.1f * fmaxf(fabsf(gltf_data->vertex_min[0]),
                                fmaxf(fabsf(gltf_data->vertex_min[1]),
                                fmaxf(fabsf(gltf_data->vertex_min[2]),
                                fmaxf(fabsf(gltf_data->vertex_max[0]),
                                fmaxf(fabsf(gltf_data->vertex_max[1]),
                                fabsf(gltf_data->vertex_max[2]))))));
    float scale_factor_neg = -scale_factor;
    for(unsigned i = 0; i < tpl; i++) {
        if(i % 3 == 1) tp[i] /= scale_factor_neg; /* flip y coordinate */
        else           tp[i] /= scale_factor;
    }

    // The following are GLSL shaders for rendering a triangle on the screen
    static const char *vertexShaderCode =
            "#version 300 es\n"
        STRINGIFY(
            precision lowp float;
            in vec3 pos;
            out float zpos;
            void main() {
                gl_Position = vec4(pos, 1.0);
                zpos = pos.z;
            }
        );

    static const char *fragmentShaderCode =
            "#version 300 es\n"
        STRINGIFY(
            precision lowp float;
            uniform vec4 color;
            in float zpos;
            out vec4 color_out;
            void main() {
                float z2 = -(zpos * 0.5f - 0.5f);
                color_out = vec4(z2, z2, z2, 1.0f) * color;
            }
        );

    // Set GL Viewport size, always needed!
    GL_CALL(glViewport(0, 0, texture_w, texture_h));

    int shader_compiled;

    // Create a shader program
    unsigned int program;
    GL_CALL(program = glCreateProgram());

    unsigned int vert;
    GL_CALL(vert = glCreateShader(GL_VERTEX_SHADER));
    GL_CALL(glShaderSource(vert, 1, &vertexShaderCode, NULL));
    GL_CALL(glCompileShader(vert));

    GL_CALL(glGetShaderiv(vert, GL_COMPILE_STATUS, &shader_compiled));
    if (!shader_compiled)
    {
        //error
        GLchar InfoLog[512];
        GL_CALL(glGetShaderInfoLog(vert, sizeof(InfoLog), NULL, InfoLog));
        LV_LOG_ERROR("%s", InfoLog);
        return 1;
    }

    unsigned int frag;
    GL_CALL(frag = glCreateShader(GL_FRAGMENT_SHADER));
    GL_CALL(glShaderSource(frag, 1, &fragmentShaderCode, NULL));
    GL_CALL(glCompileShader(frag));

    GL_CALL(glGetShaderiv(frag, GL_COMPILE_STATUS, &shader_compiled));
    if (!shader_compiled)
    {
        //error
        GLchar InfoLog[512];
        GL_CALL(glGetShaderInfoLog(frag, sizeof(InfoLog), NULL, InfoLog));
        LV_LOG_ERROR("%s", InfoLog);
        return 1;
    }

    GL_CALL(glAttachShader(program, frag));
    GL_CALL(glAttachShader(program, vert));
    GL_CALL(glLinkProgram(program));
    GL_CALL(glUseProgram(program));

    // Create Vertex Buffer Object
    unsigned int vbo;
    GL_CALL(glGenBuffers(1, &vbo));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, gltf_data->vertex_count * (3 * sizeof(float)), gltf_data->vertex_array, GL_STATIC_DRAW));

    // Get vertex attribute and uniform locations
    int pos_loc;
    GL_CALL(pos_loc = glGetAttribLocation(program, "pos"));

    // Set our vertex data
    GL_CALL(glEnableVertexAttribArray(pos_loc));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    GL_CALL(glVertexAttribPointer(pos_loc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL));

    int color_loc;
    GL_CALL(color_loc = glGetUniformLocation(program, "color"));

    // Set the desired color of the triangle
    GL_CALL(glUniform4f(color_loc,
                        ((float) color.red) / 255.0f,
                        ((float) color.green) / 255.0f,
                        ((float) color.blue) / 255.0f,
                        1.0));

    unsigned int ebo;
    GL_CALL(glGenBuffers(1, &ebo));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, gltf_data->index_count * sizeof(uint16_t), gltf_data->index_array, GL_STATIC_DRAW));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));

    // Enable depth test
    GL_CALL(glEnable(GL_DEPTH_TEST));
    // glDepthMask(GL_FALSE);
    GL_CALL(glDepthFunc(GL_LESS));

    // Clear whole screen (front buffer)
    GL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
    GL_CALL(glClearDepth(1.0f));
    GL_CALL(glDepthMask(true));
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    GL_CALL(glDrawElements(GL_TRIANGLES, gltf_data->index_count, GL_UNSIGNED_SHORT, 0));

    GL_CALL(glDisable(GL_DEPTH_TEST));

    // unbind everything
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, 0));
    GL_CALL(glUseProgram(0));

    // delete everything except the texture
    GL_CALL(glDeleteBuffers(1, &ebo));
    GL_CALL(glDeleteBuffers(1, &vbo));
    GL_CALL(glDeleteRenderbuffers(1, &renderbuffer));
    GL_CALL(glDeleteFramebuffers(1, &framebuffer));
    GL_CALL(glDeleteProgram(program));
    GL_CALL(glDeleteShader(vert));
    GL_CALL(glDeleteShader(frag));

    free_gltf_data(gltf_data);

    return texture;
}
