#include "display.h"

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

static uint32_t *color_buffer = NULL;
static float *z_buffer = NULL;

static SDL_Texture *color_buffer_texture = NULL;
static int window_width = 800;
static int window_height = 600;

static int render_method = 0;
static int cull_method = 0;

bool initialize_window(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }
    // Query fullscreen width and height
    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    int fullscreen_width = display_mode.w;
    int fullscreen_height = display_mode.h;

    // Simulating low resolution displays
    window_width = fullscreen_width / 1;
    window_height= fullscreen_height / 1;

    window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, fullscreen_width,
                              fullscreen_height, SDL_WINDOW_BORDERLESS);
    if (!window) {
        fprintf(stderr, "Error creating SDL window.\n");
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        fprintf(stderr, "Error creating SDL renderer.\n");
        return false;
    }
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);

    color_buffer =
        (uint32_t *)malloc(sizeof(uint32_t) * window_width * window_height);
    z_buffer = (float *)malloc(sizeof(float) * window_width * window_height);
    // Windows OS uses little endian, bytes in uint32_t are reversed
    // SDL_PIXELFORMAT_RGBA32 is SDL_PIXELFORMAT_ABGR8888
    // For 0xFF112233, FF is Alpha, 11 is Blue, 22 is Green, 33 is Red
    color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             window_width, window_height);

    return true;
}

void draw_grid(void) {
    int n = 10, m = 10;
    for (int y = 0; y < window_height; y += m) {
        for (int x = 0; x < window_width; x += m) {
            if (y % n == 0 || x % n == 0) {
                color_buffer[window_width * y + x] = 0xFF333333;
            }
        }
    }
}

void draw_pixel(int x, int y, uint32_t color) {
    if (x < 0 || x >= window_width || y < 0 || y >= window_height) {
        return;
    }
    color_buffer[window_width * y + x] = color;
}

void draw_line(int x0, int y0, int x1, int y1, uint32_t color) {
    // Digital Differential Analyzer
    int delta_x = (x1 - x0);
    int delta_y = (y1 - y0);
    int longest_side_length =
        (abs(delta_x) >= abs(delta_y)) ? abs(delta_x) : abs(delta_y);
    float x_inc = delta_x / (float)longest_side_length;
    float y_inc = delta_y / (float)longest_side_length;
    float current_x = x0;
    float current_y = y0;
    for (int i = 0; i <= longest_side_length; i++) {
        draw_pixel(round(current_x), round(current_y), color);
        current_x += x_inc;
        current_y += y_inc;
    }
}

void draw_rect(int x, int y, int width, int height, uint32_t color) {
    for (int j = y; j < y + height; j++) {
        for (int i = x; i < x + width; i++) {
            draw_pixel(i, j, color);
        }
    }
}

void clear_color_buffer(uint32_t color) {
    for (int i = 0; i < window_width * window_height; i++) {
        color_buffer[i] = color;
    }
}

void clear_z_buffer() {
    for (int i = 0; i < window_width * window_height; i++) {
        // After applied perspective projection, value of z has been between
        // 0 and 1, 0 is znear, 1 is zfar, smaller z is, closer to screen
        // the pixel is
        z_buffer[i] = 1.0f;
    }
}

float get_zbuffer_at(int x, int y) {
    // No clipping, y maybe greater than window_height, then segmentation fault
    // when access z-buffer
    // After clipping, y still can be window_height
    if (x < 0 || x >= window_width || y < 0 || y >= window_height) {
        // Prevent segmentation fault when access z-buffer
        return 1.0f;
    }
    return z_buffer[y * window_width + x];
}

void update_zbuffer_at(int x, int y, float value) {
    if (x < 0 || x >= window_width || y < 0 || y >= window_height) {
        return;
    }
    z_buffer[y * window_width + x] = value;
}

void render_color_buffer(void) {
    SDL_UpdateTexture(color_buffer_texture, NULL, color_buffer,
                      (int)sizeof(uint32_t) * window_width);
    SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void destroy_window(void) {
    free(color_buffer);
    free(z_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

bool should_cull_backface(void) { return cull_method == CULL_BACKFACE; }

bool should_render_filled_triangle(void) {
    return render_method == RENDER_FILL_TRIANGLE ||
           render_method == RENDER_FILL_TRIANGLE_WIRE;
}

bool should_render_textured_triangle(void) {
    return render_method == RENDER_TEXTURED ||
           render_method == RENDER_TEXTURED_WIRE;
}

bool should_render_wireframe(void) {
    return render_method == RENDER_WIRE ||
           render_method == RENDER_WIRE_VERTEX ||
           render_method == RENDER_FILL_TRIANGLE_WIRE ||
           render_method == RENDER_TEXTURED_WIRE;
}

bool should_render_wire_vertex(void) {
    return render_method == RENDER_WIRE_VERTEX;
}

int get_window_width(void) { return window_width; }

int get_window_height(void) { return window_height; }

void set_render_method(int method) { render_method = method; }

void set_cull_method(int method) { cull_method = method; }
