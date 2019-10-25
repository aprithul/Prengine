#ifndef GUI_HPP
#define GUI_HPP

#include <string>
#include <dirent.h>
#include <map>
#include "Vector2.hpp"
#include "InputModule.hpp"
#include "Utils.hpp"
#include "RendererModule.hpp"
namespace Pringine
{

    enum Font_ID
    {
        FONT_SAMPLE_TTF_16,
        FONT_SAMPLE_TTF_24,
        FONT_SAMPLE_TTF_32,
        FONT_SAMPLE_TTF_48,
        FONT_ID_MAX
    };

    enum GUI_ID
    {
        GUI_ID_PANEL,
        GUI_ID_BUTTON,
        GUI_ID_SLIDER_BASE,
        GUI_ID_SLIDER_KNOB
    };

    struct UI
    {
        uintptr_t id;
    };

    extern bool initialize_gui(Mouse* mouse, Renderer2D* renderer);
    extern void show_file_structure(std::string root, std::string prefix);
    extern void draw_panel(uintptr_t id, SDL_Rect draw_region);
    extern bool do_button(uintptr_t id, SDL_Rect& draw_region, SDL_FRect& anchor, const std::string& text, const TextJustification& justification);
    extern float get_slider(uintptr_t id, float value, SDL_Rect& slide_region, SDL_Rect& cursor_region, SDL_FRect& anchor, float min = 0.0f, float max =1.0f, bool horizontal=true);
    extern void show_text(uintptr_t id, std::string& text, SDL_Rect& draw_region, SDL_FRect& anchor,Font_ID font_id, SDL_Color& c, float scale = 1.f );
    extern bool inside(Vector2<int> pos, SDL_Rect rect);
    extern void process_rect_with_anchor(SDL_Rect& rect, SDL_FRect& anchor, int width, int height);
    extern Mouse* mouse;
    extern Renderer2D* renderer;
    extern Graphics gui_graphics[];
    extern UI hot;
    extern UI active;
    extern int canvas_width;
    extern int canvas_height;
    extern void initalize_text_resources();

    extern std::map<uintptr_t,SDL_Texture*> rendered_texts;
    extern TTF_Font* fonts[];

}


#endif