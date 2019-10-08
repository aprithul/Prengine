//
//  TimeComponent.hpp
//  Game Engine
//
//  Created by Aniruddha Prithul on 7/26/17.
//  Copyright © 2017 Aniruddha Prithul. All rights reserved.
//

#ifndef TimeComponent_hpp
#define TimeComponent_hpp

#include <iostream>
#include "Component.hpp"
#include "SDL2/SDL.h"

namespace Pringine {
    class Time: public Component
    {
    public:
        Time();
        ~Time();
        void update() override;
        void start() override;
        void end() override;
        static double get_time();
        static double get_frame_start_time();
        static double Frame_time;
        
    private:
        static double current_frame_start_time;
        static double last_frame_start_time;
    };
}

#endif /* TimeComponent_hpp */
