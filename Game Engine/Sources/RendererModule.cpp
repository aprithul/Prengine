//
//  Display.cpp
//  Game Engine
//
//  Created by Aniruddha Prithul on 7/22/17.
//  Copyright © 2017 Aniruddha Prithul. All rights reserved.
//
#include "RendererModule.hpp"

namespace Pringine {

    // globally required functions that are related to renderer but shouldn't require access to the renderer
    std::map<std::string, SDL_Texture*> loaded_texture_library;

    SDL_Texture* load_texture(const std::string &file_name, SDL_Renderer *ren)
    {
        if(loaded_texture_library.count(file_name) == 0)
        {
            if(ren == nullptr)
            {
                LOG(LOGTYPE_ERROR, std::string("Renderer provided in load_texture is null"));
                return nullptr;
            }

            SDL_Texture *texture = IMG_LoadTexture(ren, file_name.c_str());
            if (texture == nullptr){
                LOG(LOGTYPE_ERROR, std::string("Couldn't load texture : ").append(file_name));
                return nullptr;
            }
            
            loaded_texture_library[file_name] = texture;
            return texture;
        }
        else
            return loaded_texture_library[file_name];
    }

    //////////////////////////////////////////////////////////////////////
    
    // publics
    Renderer2D::Renderer2D(int width, int height, std::string title, bool vsync, Camera* camera, int world_unit_to_pixels, std::string name, int priority):Module(name,priority)
    {
        // initialize SDL video
        SDL_Init(SDL_INIT_VIDEO);
        
        this->window_width = width;
        this->window_height = height;
        this->title = title;
        this->camera = camera;
        this->world_unit_to_pixels = world_unit_to_pixels;
        // create window and sdl_renderer
        set_vsync(vsync);
        sdl_window = SDL_CreateWindow(this->title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, this->window_width, this->window_height,  SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
        sdl_renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
        set_clear_color(255,0,0,255);
        render_texture = SDL_CreateTexture( sdl_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, this->window_width, this->window_height );
    }

    void Renderer2D::set_clear_color(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
    {
        SDL_SetRenderDrawColor(sdl_renderer, r,g,b,a);
    }
    
    Renderer2D::~Renderer2D()
    {
        std::map<std::string, SDL_Texture*>::iterator ptr;
        for (ptr = loaded_texture_library.begin(); ptr != loaded_texture_library.end(); ptr++) 
        {
            if(ptr->second!=nullptr)
                SDL_DestroyTexture( ptr->second);
            else
            {
                LOG(LOGTYPE_WARNING, "Renderer found texture to be null while cleaning up");
            }
            
        }

        SDL_DestroyRenderer(sdl_renderer);
        SDL_DestroyWindow(sdl_window);
        SDL_Quit();
    }
    
    void Renderer2D::start()
    {
        
    }
    
    void Renderer2D::update()
    {
        if(is_active)
        {
            //set render target to texture 
            SDL_SetRenderTarget(sdl_renderer, render_texture);
            SDL_RenderClear(sdl_renderer);
            draw();
            SDL_RenderPresent(sdl_renderer);

            //reset back to render to screen
            SDL_SetRenderTarget(sdl_renderer, NULL);
            //copy over the render texture to screen
            // before that resize the texture to apply any screen effect ( zoom / rotation)
            int src_w = this->window_width * 1/camera->zoom_amount;
            int src_h = this->window_height * 1/camera->zoom_amount;
            SDL_Rect src_rct;
            src_rct.x = (this->window_width - src_w)/2;
            src_rct.y = (this->window_height - src_h)/2;
            src_rct.w = src_w;
            src_rct.h = src_h;
            SDL_RenderCopy(sdl_renderer, render_texture, &src_rct, NULL);
        }
    }

    void Renderer2D::end()
    {
        LOG(LOGTYPE_GENERAL, "RendererComponent Ended");
    }

        
    void Renderer2D::set_vsync(bool value)
    {
        if(value == 1)
        {
            SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1"); 
        }
        else
        {
            SDL_SetHint(SDL_HINT_RENDER_VSYNC, "0"); 
        }
    }

    int Renderer2D::add_graphics_to_draw(Graphics* graphics)
    {
        if(graphics!=nullptr)
        {
            int next_pos = -1;
            if(!released_positions.empty())
            {
                next_pos = released_positions.front();
                released_positions.pop();
            }
            else
            {
                if(render_array_head < MAX_RENDERED_GRAPHICS_PER_FRAME)
                {
                    render_array_head++;
                    next_pos = render_array_head;
                }
                else
                {
                    LOG(LOGTYPE_ERROR, "Cant add anymore graphics to draw, array full");
                    return -1;
                }
                
            }

            render_list[next_pos] = graphics;
            return next_pos;
            
        }
        else
        {    
            LOG(LOGTYPE_ERROR, "Cant put null graphics in render_list");
            return -1;
        }

    }

    void Renderer2D::remove_graphics(int id)
    {
        if(id<=render_array_head)
        {   
            Graphics* _graphics = render_list[id];    
            if(_graphics!= nullptr)
            {
                render_list[id] = nullptr;
                released_positions.push(id);
            }
            else
            {
                LOG(LOGTYPE_WARNING, std::string("No graphics object with id: ").append(std::to_string(id)).append(" exists"));
            }
        }
        else
        {
            LOG(LOGTYPE_WARNING, std::string("Requested graphics object id: ").append(std::to_string(id)).append(" is out of id range"));
        }
    }


    // private    
    void Renderer2D::draw()
    {
        for(int _i=0; _i<=render_array_head; _i++)
        {
            Graphics* _graphics = render_list[_i];
            if(_graphics != nullptr)
            {
                // dst_dimension has world unit values, convert to screen space
                // also make drawing relative to camera position
                // also make center of screen 0,0
                _graphics->dst_dimension.w *= world_unit_to_pixels;
                _graphics->dst_dimension.h *= world_unit_to_pixels;
                
                _graphics->dst_dimension.x = (((_graphics->dst_dimension.x - camera->transform.position.x)*world_unit_to_pixels) - (_graphics->dst_dimension.w/2) + window_width/2);
                _graphics->dst_dimension.y = ((-(_graphics->dst_dimension.y - camera->transform.position.y)*world_unit_to_pixels) - (_graphics->dst_dimension.h/2) + window_height/2);

                SDL_RenderCopyEx(this->sdl_renderer, _graphics->get_current_frame()->texture , &(_graphics->get_current_frame()->region), 
                                        &_graphics->dst_dimension, _graphics->angle, NULL, SDL_FLIP_NONE) ;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////GRAPHICS CLASS DEFINTION///////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Graphics::Graphics()
    {
        this->current_frame_index = -1;
        this->angle = 0;
    }

    Graphics::~Graphics()
    {
        if(graphics_frames != nullptr)
        {
            /*for(int _i=0; _i < number_of_frames; _i++)
            {
                if(graphics_frames[_i].texture != nullptr)
                    SDL_DestroyTexture(graphics_frames[_i].texture);
            }*/
            LOG(LOGTYPE_GENERAL,"Deleted: ");
            delete[] graphics_frames;
        }
    }
    
    void Graphics::load_graphics(std::string graphics_file, TextureSlicingParameters slicing_params, const Renderer2D& renderer2d, int num_of_frames)
    {
        // load all animation frames
        this->number_of_frames = num_of_frames;
        graphics_frames = new GraphicsFrame[num_of_frames];
        SDL_Texture* texture = Pringine::load_texture(graphics_file, renderer2d.sdl_renderer);
        if(texture != nullptr)
        {
            int w = 0;
            int h = 0;
            SDL_QueryTexture(texture, NULL, NULL, &w, &h);
            //LOG(LOGTYPE_GENERAL, std::to_string(w).append("  ").append(std::to_string(h)));

            int columns = w/(slicing_params.w+slicing_params.x_pad);
            int rows = h/(slicing_params.h+slicing_params.y_pad);
            int frame_no = 0;


            for(int _i= slicing_params.x + slicing_params.x_pad; _i<w; _i += (slicing_params.w+slicing_params.x_pad))
            {
                for(int _j = slicing_params.y + slicing_params.y_pad; _j<h; _j += (slicing_params.h+slicing_params.y_pad))
                {
                    if(frame_no<this->number_of_frames)
                    {
                        GraphicsFrame* frame = &graphics_frames[frame_no];
                        frame->texture = texture;
                        frame->region.x = _i;
                        frame->region.y = _j;
                        
                        if(_i+slicing_params.w > w)
                            frame->region.w = slicing_params.w - (_i+slicing_params.w-w);
                        else
                            frame->region.w = slicing_params.w;

                        if(_j+slicing_params.h > h)
                            frame->region.h = slicing_params.h - (_j+slicing_params.h-h);
                        else
                            frame->region.h = slicing_params.h;

                        frame->region.h = slicing_params.h;

                        //dst_dimension.w = slicing_params.w;
                        //dst_dimension.h = slicing_params.h;

                        
                        

                        frame_no++;
                    }
                }
            }
            current_frame_index = 0;
        }
    }

    GraphicsFrame* Graphics::get_current_frame()
    {
        return &this->graphics_frames[this->current_frame_index];
    }

    GraphicsFrame* Graphics::get_frame_at(int index)
    {
        if(index>=number_of_frames)
            LOG(LOGTYPE_ERROR, "index is greater than frame count");
        return &this->graphics_frames[index];
    }  
}