#include "app.hpp"
#include "exception.hpp"
#include <iostream>


//-----------------------------------------------------------------------------
App::App(const string& window_caption, int width, int heigth) {
    window = NULL;
    this->width = width;
    this->heigth = heigth;
    animate_stack = 0;    
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw Exception("SDL não inicializou", SDL_GetError());            
    } 
    window = SDL_CreateWindow(
        window_caption.c_str(), 
        SDL_WINDOWPOS_UNDEFINED, 
        SDL_WINDOWPOS_UNDEFINED, 
        width, 
        heigth, 
        SDL_WINDOW_SHOWN
    ); 
    if(!window) {
        throw Exception("SDL não criou a janela", SDL_GetError());
    }                        
    int imgFlags = IMG_INIT_PNG;
    if(!(IMG_Init(imgFlags) & imgFlags)) {
        throw Exception("SDL_Image não inicializou", IMG_GetError());
    }
    if(TTF_Init() == -1) {
        throw Exception("SDL_ttf não inicializou", TTF_GetError());
    }    
    window_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(!window_renderer) {
        throw Exception("Não foi possível criar o renderizador da janela", SDL_GetError());
    }  
    SDL_RenderSetLogicalSize(window_renderer, width, heigth);  
}

//-----------------------------------------------------------------------------
App::~App() {
    delete_renders();
    SDL_DestroyWindow(window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

//-----------------------------------------------------------------------------
SDL_Renderer* App::get_window_renderer(void) {
    return window_renderer;
}

//-----------------------------------------------------------------------------
void App::loop(void) {
    SDL_Event e; 
    bool quit = false; 
    while(!quit) { 
        while(SDL_PollEvent(&e)) {
            poll_event(&e);
            switch(e.type) {
                case SDL_QUIT:
                    quit = true;
            }  
        }
        render();        
    }    
}

//-----------------------------------------------------------------------------
void App::poll_event(SDL_Event *e) {
    switch(e->type) {
        case SDL_MOUSEMOTION: {            
            static Render *last_render = NULL;
            Render *r = get_render_at(e->motion.x, e->motion.y);            
            if((r) && (last_render != r)) {
                r->mouse_over();
            }            
            if((last_render) && (last_render != r)) {
                last_render->mouse_leave();
            }
            last_render = r;
            break;
        }
        case SDL_MOUSEBUTTONDOWN: {
            switch(e->button.button) {
                case SDL_BUTTON_LEFT: {
                    Render *r = get_render_at(e->button.x, e->button.y);
                    if(r) {
                        if(e->button.clicks == 2) {
                            r->mouse_dclick();
                        }
                        r->mouse_click();
                    }
                    break;
                }
            }
            break;
        }
    }
}

//-----------------------------------------------------------------------------
void App::render(void) {
    SDL_SetRenderDrawColor(window_renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(window_renderer);

    render_renders();

    SDL_RenderPresent(window_renderer);
}

//-----------------------------------------------------------------------------
void App::render_renders(void) {
    for(RendersI i = renders.begin(); i != renders.end(); i++) {
        (*i)->render();                
    }     
}

//-----------------------------------------------------------------------------
void App::delete_renders(void) {
    for(RendersI i = renders.begin(); i != renders.end(); i++) {
        delete (*i);                
    }     
}

//-----------------------------------------------------------------------------
Texture *App::add_texture(const string& file_name, int x, int y) { 
    Texture *p = new Texture(this, file_name, x, y);
    renders.push_back(p);
    return p;
}

//-----------------------------------------------------------------------------
Texture *App::add_texture_text(const string& ttf_file_name, const string& text,
        int x, int y, const SDL_Color &color, int font_size) {
    Texture *p = new Texture(this, ttf_file_name, text, x, y, color, font_size);
    renders.push_back(p);
    return p;    
}

//-----------------------------------------------------------------------------
Rectangle *App::add_rectangle(const SDL_Rect &rect, const SDL_Color &color, bool fill) {
    Rectangle *p = new Rectangle(this, rect, color, fill);
    renders.push_back(p);
    return p;
}
//-----------------------------------------------------------------------------
Rectangles *App::add_rectangles(const vector<SDL_Rect> &rects, const SDL_Color &color, bool fill) {
    Rectangles *p = new Rectangles(this, rects, color, fill);
    renders.push_back(p);
    return p;
}

//-----------------------------------------------------------------------------
Line *App::add_line(const SDL_Point &point1, const SDL_Point &point2, const SDL_Color &color) {
    Line *p = new Line(this, point1, point2, color);
    renders.push_back(p);
    return p;
}

Lines *App::add_lines(const vector<SDL_Point> &points, const SDL_Color &color) {
    Lines *p = new Lines(this, points, color);
    renders.push_back(p);    
    return p;
}

//-----------------------------------------------------------------------------
Point *App::add_point(const SDL_Point &point, const SDL_Color &color) {
    Point *p = new Point(this, point, color);
    renders.push_back(p);
    return p;
}

//-----------------------------------------------------------------------------
Points *App::add_points(const vector<SDL_Point> &points, const SDL_Color &color) {
    Points *p = new Points(this, points, color);
    renders.push_back(p);
    return p;
}

//-----------------------------------------------------------------------------
Grid *App::add_grid(int cols, int rows, int col_size, 
                   int row_size, int vline_size, int hline_size, 
                   int vpad, int hpad, 
                   const SDL_Color &color) {
    Grid *p = new Grid(
        this, 
        cols, 
        rows, 
        col_size, 
        row_size, 
        vline_size, 
        hline_size,
        vpad,
        hpad, 
        color
    );
    renders.push_back(p);
    return p;

}

//-----------------------------------------------------------------------------
Card *App::add_card(int card_id, int x, int y) {
    Card *p = new Card(this, card_id, x, y);
    renders.push_back(p);
    return p;    
}

//-----------------------------------------------------------------------------
CardGroup *App::add_card_group(CardGroupDirection direction) {
    CardGroup *p = new CardGroup(this, direction);
    renders.push_back(p);
    return p;    
}
//-----------------------------------------------------------------------------
CardGroup *App::add_card_group(void) {
    CardGroup *p = new CardGroup(this);
    renders.push_back(p);
    return p;    
}

//-----------------------------------------------------------------------------
void App::screen_shot(void) {
    // https://stackoverflow.com/questions/22315980/sdl2-c-taking-a-screenshot
    SDL_Surface *ss = SDL_CreateRGBSurface(0, width, heigth, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    SDL_RenderReadPixels(window_renderer, NULL, SDL_PIXELFORMAT_ARGB8888, ss->pixels, ss->pitch);
    SDL_SaveBMP(ss, "screenshots/screenshot.bmp");
    SDL_FreeSurface(ss);
}

//-----------------------------------------------------------------------------
Render *App::get_render_at(int x, int y) {
    for (RendersCRI r = renders.rbegin(); r != renders.rend(); r++) {
        const Renders &rs = (*r)->get_renders();
        for (RendersCRI i = rs.rbegin(); i != rs.rend(); i++) {
            if((*i)->rect_contains(x, y)) {
                return *i;
            }
        }
        if((*r)->rect_contains(x, y)) {
            return *r;
        }        
    }
    return nullptr;
}

//-----------------------------------------------------------------------------
void App::begin_animate(void) {
    animate_stack++;
}

//-----------------------------------------------------------------------------
void App::end_animate(void) {
    animate_stack--;
    if(!is_animate()) {
        for (auto &i : animates) {
            i.card->set_xy_animate(i.dest_x, i.dest_y);
        }
        animates.clear();
    }
}

//-----------------------------------------------------------------------------
bool App::is_animate(void) const {
    return animate_stack;
}

//-----------------------------------------------------------------------------
void App::add_animate(Card *card, int x, int y) {
    for (auto &i : animates) {
        if(i.card == card) {
            i.dest_x = x;
            i.dest_y = y;
            return;
        }
    }
    Animate animate;
    SDL_Rect rect;
    card->get_rect(rect);
    animate.card = card;
    animate.orig_x = rect.x;
    animate.orig_y = rect.y;
    animate.dest_x = x;
    animate.dest_y = y;
    animates.push_back(animate);
}