#include "render.hpp"
#include "exception.hpp"
#include "app.hpp"
#include <string>
#include <algorithm>
#include <limits>
#include <functional>

namespace SDLCards {

using namespace std;

void inflate_rect(SDL_Rect &rect, int amount) {
    rect.x -= amount;
    rect.y -= amount;
    rect.w += (amount*2);
    rect.h += (amount*2);
}

//-----------------------------------------------------------------------------
//- Render --------------------------------------------------------------------
//-----------------------------------------------------------------------------
Render::Render(App *app) {
    on_mouse_over = nullptr;
    on_mouse_leave = nullptr;
    on_mouse_click = nullptr;
    on_mouse_dclick = nullptr;
    owner = nullptr;
    link = nullptr;
    this->app = app;
    name = "";
}

//-----------------------------------------------------------------------------
Render::~Render() {
    on_mouse_over = nullptr;
    on_mouse_leave = nullptr;
    on_mouse_click = nullptr;
    on_mouse_dclick = nullptr;
    owner = nullptr;
}

//-----------------------------------------------------------------------------
void Render::set_xy(int x, int y) {
    set_x(x);
    set_y(y);
}

//-----------------------------------------------------------------------------
void Render::get_inflate_rect(SDL_Rect &rect, int amount) const {
    get_rect(rect);
    inflate_rect(rect, amount);
}

//-----------------------------------------------------------------------------
const Renders& Render::get_renders(void) const {
    return renders;
}

//-----------------------------------------------------------------------------
bool Render::rect_contains(int x, int y) const {
    SDL_Rect rect;
    get_rect(rect);
    return (rect.x <= x) && 
           (rect.y <= y) && 
           ((rect.x+rect.w) >= x) &&
           ((rect.y+rect.h) >= y);
}

//-----------------------------------------------------------------------------
void Render::mouse_over(void) {
    if(on_mouse_over) {
        on_mouse_over(this);
    }

}

//-----------------------------------------------------------------------------
void Render::mouse_leave(void) {
    if(on_mouse_leave) {
        on_mouse_leave(this);    
    }
}

//-----------------------------------------------------------------------------
void Render::mouse_click(void) {
    if(on_mouse_click) {
        on_mouse_click(this);    
    }
}

//-----------------------------------------------------------------------------
void Render::mouse_dclick(void) {
    if(on_mouse_dclick) {
        on_mouse_dclick(this);    
    }
}



//-----------------------------------------------------------------------------
//- Texture -------------------------------------------------------------------
//-----------------------------------------------------------------------------
Texture::Texture(App *app, const string& file_name, int x, int y) 
    : Render(app) {    
    rotate_angle = 0;
    SDL_Surface* temp_surface = IMG_Load(file_name.c_str());
    if(!temp_surface) {
        throw Exception("Não foi possível carregar a imagem " + file_name, IMG_GetError());
    }
    SDL_GetClipRect(temp_surface, &rect);
    rect.x = x;
    rect.y = y;
    texture = SDL_CreateTextureFromSurface(app->get_window_renderer(), temp_surface);
    if(!texture) {
        throw Exception("Não foi possível criar a textura " + file_name, SDL_GetError());
    }
    SDL_FreeSurface(temp_surface);
}

//-----------------------------------------------------------------------------
Texture::Texture(App *app, 
        const string& ttf_file_name, const string& text,
        int x, int y, const SDL_Color &color, int font_size) 
    : Render(app) {
    rotate_angle = 0;
    this->ttf_file_name = ttf_file_name;
    this->color = color;
    this->font_size = font_size;
    TTF_Font* font = TTF_OpenFont(ttf_file_name.c_str(), font_size);
    if(!font) {
        throw Exception("Não foi possível carregar a fonte " + ttf_file_name, TTF_GetError());    
    }
    SDL_Surface* temp_surface = TTF_RenderText_Solid(font, text.c_str(), color);
    if(!temp_surface) {
        throw Exception("Não foi possível renderizar o texto " + text, TTF_GetError());
    }
    SDL_GetClipRect(temp_surface, &rect);
    rect.x = x;
    rect.y = y;
    texture = SDL_CreateTextureFromSurface(app->get_window_renderer(), temp_surface);
    if(!texture) {
        throw Exception("Não foi possível criar a textura do texto " + text, SDL_GetError());
    }
    SDL_FreeSurface(temp_surface);
    TTF_CloseFont(font);
}

//-----------------------------------------------------------------------------
Texture::~Texture() {
    SDL_DestroyTexture(texture);
}

//-----------------------------------------------------------------------------
void Texture::render() {
     SDL_RenderCopyEx(app->get_window_renderer(), texture, NULL, &rect, rotate_angle, NULL, SDL_FLIP_NONE);
}

//-----------------------------------------------------------------------------
void Texture::set_x(int x) {
    rect.x = x;
}

//-----------------------------------------------------------------------------
void Texture::set_y(int y) {
    rect.y = y;
}

//-----------------------------------------------------------------------------
void Texture::move(int x, int y) {
    rect.x += x;
    rect.y += y;
}

//-----------------------------------------------------------------------------
void Texture::get_rect(SDL_Rect &rect) const {
    rect.x = this->rect.x;
    rect.y = this->rect.y;
    rect.w = this->rect.w;
    rect.h = this->rect.h;
}

//-----------------------------------------------------------------------------
void Texture::set_blend(SDL_BlendMode mode) {
    SDL_SetTextureBlendMode(texture, mode);
}

//-----------------------------------------------------------------------------
void Texture::set_alpha(Uint8 alpha) {
    SDL_SetTextureAlphaMod(texture, alpha);
}

//-----------------------------------------------------------------------------
void Texture::set_color(Uint8 r, Uint8 g, Uint8 b) {
    SDL_SetTextureColorMod(texture, r, g, b);
}

//-----------------------------------------------------------------------------
void Texture::rotate(double angle) {
    rotate_angle = angle;
}

//-----------------------------------------------------------------------------
void Texture::inc_rotate(double inc_angle) {
    rotate_angle += inc_angle;
}

//-----------------------------------------------------------------------------
void Texture::change_image(const string& file_name) {
    SDL_Surface* temp_surface = IMG_Load(file_name.c_str());
    if(!temp_surface) {
        throw Exception("Não foi possível carregar a imagem " + file_name, IMG_GetError());
    }
    SDL_Rect temp_rect;
    get_rect(temp_rect);
    SDL_GetClipRect(temp_surface, &rect);
    rect.x = temp_rect.x;
    rect.y = temp_rect.y;

    SDL_DestroyTexture(texture);
    texture = SDL_CreateTextureFromSurface(app->get_window_renderer(), temp_surface);
    if(!texture) {
        throw Exception("Não foi possível criar a textura " + file_name, SDL_GetError());
    }
    SDL_FreeSurface(temp_surface);    
}

//-----------------------------------------------------------------------------
void Texture::change_text(const string& text) {
    TTF_Font* font = TTF_OpenFont(ttf_file_name.c_str(), font_size);
    if(!font) {
        throw Exception("Não foi possível carregar a fonte " + ttf_file_name, TTF_GetError());    
    }
    SDL_Surface* temp_surface = TTF_RenderText_Solid(font, text.c_str(), color);
    if(!temp_surface) {
        throw Exception("Não foi possível renderizar o texto " + text, TTF_GetError());
    }
    texture = SDL_CreateTextureFromSurface(app->get_window_renderer(), temp_surface);
    if(!texture) {
        throw Exception("Não foi possível criar a textura do texto " + text, SDL_GetError());
    }
    SDL_FreeSurface(temp_surface);
    TTF_CloseFont(font);
}

//-----------------------------------------------------------------------------
//- Rectangle -----------------------------------------------------------------
//-----------------------------------------------------------------------------
Rectangle::Rectangle(App *app, const SDL_Rect &rect, const SDL_Color &color, bool fill) 
    : Render(app) {        
    this->rect = rect;
    this->color = color;
    this->fill = fill;
}

//-----------------------------------------------------------------------------
Rectangle::~Rectangle() {
    
}

//-----------------------------------------------------------------------------
void Rectangle::render() {
    SDL_SetRenderDrawColor(app->get_window_renderer(), color.r, color.g, color.b, color.a);
    if(fill) {
        SDL_RenderFillRect(app->get_window_renderer(), &rect);   
    } else {
        SDL_RenderDrawRect(app->get_window_renderer(), &rect);   
    }
    
}

//-----------------------------------------------------------------------------
void Rectangle::set_x(int x) {
    rect.x = x;
}

//-----------------------------------------------------------------------------
void Rectangle::set_y(int y) {
    rect.y = y;
}

//-----------------------------------------------------------------------------
void Rectangle::move(int x, int y) {
    rect.x += x;
    rect.y += y;
}

//-----------------------------------------------------------------------------
void Rectangle::get_rect(SDL_Rect &rect) const {
    rect.x = this->rect.x;
    rect.y = this->rect.y;
    rect.w = this->rect.w;
    rect.h = this->rect.h;
}


//-----------------------------------------------------------------------------
//- Line ----------------------------------------------------------------------
//-----------------------------------------------------------------------------
Line::Line(App *app, const SDL_Point &point1, const SDL_Point &point2, const SDL_Color &color) 
    : Render(app) {
    this->point1 = point1;
    this->point2 = point2;
    this->color = color;
}

//-----------------------------------------------------------------------------
Line::~Line() {

}
//-----------------------------------------------------------------------------
void Line::render(void) {
    SDL_SetRenderDrawColor(app->get_window_renderer(), color.r, color.g, color.b, color.a);      
    SDL_RenderDrawLine(app->get_window_renderer(), point1.x, point1.y, point2.x, point2.y); 
}
//-----------------------------------------------------------------------------
void Line::set_x(int x) {
    point2.x += (x - point1.x);
    point1.x = x;
}

//-----------------------------------------------------------------------------
void Line::set_y(int y) {
    point2.y += (y - point1.y);
    point1.y = y;
}

//-----------------------------------------------------------------------------
void Line::move(int x, int y) {
    point1.x += x;
    point1.y += y;
    point2.x += x;
    point2.y += y;
}

//-----------------------------------------------------------------------------
void Line::get_rect(SDL_Rect &rect) const {
    rect.x = this->point1.x;
    rect.y = this->point1.y;
    rect.w = this->point2.x - this->point1.x + 1;
    rect.h = this->point2.y - this->point1.y + 1;
}

//-----------------------------------------------------------------------------
//- Lines ---------------------------------------------------------------------
//-----------------------------------------------------------------------------
Lines::Lines(App *app, const vector<SDL_Point> &points, const SDL_Color &color) 
    : Render(app) {
    this->points = points;
    this->color = color;
}

//-----------------------------------------------------------------------------
Lines::~Lines() {

}

//-----------------------------------------------------------------------------
void Lines::render(void) {
    SDL_SetRenderDrawColor(app->get_window_renderer(), color.r, color.g, color.b, color.a);      
    SDL_RenderDrawLines(app->get_window_renderer(), points.data(), points.size()); 
}

//-----------------------------------------------------------------------------
void Lines::set_x(int x) {
    int m = numeric_limits<int>::max();
    for (auto& point : points) {
        m = min(m, point.x);
    }
    move(x-m, 0);
}

//-----------------------------------------------------------------------------
void Lines::set_y(int y) {
    int m = numeric_limits<int>::max();
    for (auto& point : points) {
        m = min(m, point.y);
    }
    move(0, y-m);
}

//-----------------------------------------------------------------------------
void Lines::move(int x, int y) {
    for (auto& point : points) {
        point.x += x;
        point.y += y;
    }
}

//-----------------------------------------------------------------------------
void Lines::get_rect(SDL_Rect &rect) const {

    int min_x = numeric_limits<int>::max();
    int min_y = numeric_limits<int>::max();
    int max_x = numeric_limits<int>::min();
    int max_y = numeric_limits<int>::min();

    for (auto& point : points) {
        min_x = min(min_x, point.x);
        min_y = min(min_y, point.y);
        max_x = max(max_x, point.x);
        max_y = max(max_y, point.y);
    }    

    rect.x = min_x;
    rect.y = min_y;
    rect.w = max_x - min_x + 1;
    rect.h = max_y - min_y + 1;
}


//-----------------------------------------------------------------------------
//- Point ---------------------------------------------------------------------
//-----------------------------------------------------------------------------
Point::Point(App *app, const SDL_Point &point, const SDL_Color &color) 
    : Render(app) {
    this->point = point;
    this->color = color;
}

//-----------------------------------------------------------------------------
Point::~Point() {

}
//-----------------------------------------------------------------------------
void Point::render(void) {
    SDL_SetRenderDrawColor(app->get_window_renderer(), color.r, color.g, color.b, color.a);      
    SDL_RenderDrawPoint(app->get_window_renderer(), point.x, point.y);  
}

//-----------------------------------------------------------------------------
void Point::set_x(int x) {
    point.x = x;
}

//-----------------------------------------------------------------------------
void Point::set_y(int y) {
    point.y = y;
}

//-----------------------------------------------------------------------------
void Point::move(int x, int y) {
    point.x += x;
    point.y += y;
}

//-----------------------------------------------------------------------------
void Point::get_rect(SDL_Rect &rect) const {
    rect.x = point.x;
    rect.y = point.y;
    rect.w = 1;
    rect.h = 1;
}

//-----------------------------------------------------------------------------
//- Rectangles ----------------------------------------------------------------
//-----------------------------------------------------------------------------
Rectangles::Rectangles(App *app, const vector<SDL_Rect>&rects, const SDL_Color &color, bool fill) 
    : Render(app) {
    this->rects = rects;
    this->color = color;
    this->fill = fill;
}
//-----------------------------------------------------------------------------
Rectangles::~Rectangles() {

}
//-----------------------------------------------------------------------------
void Rectangles::render(void) {
    SDL_SetRenderDrawColor(app->get_window_renderer(), color.r, color.g, color.b, color.a);      
    if(fill) {
        SDL_RenderFillRects(app->get_window_renderer(), rects.data(), rects.size());   
    } else {
        SDL_RenderDrawRects(app->get_window_renderer(), rects.data(), rects.size());   
    }
}

//-----------------------------------------------------------------------------
void Rectangles::set_x(int x) {
    int m = numeric_limits<int>::max();
    for (auto& rect : rects) {
        m = min(m, rect.x);
    }
    move(x-m, 0);
}

//-----------------------------------------------------------------------------
void Rectangles::set_y(int y) {
    int m = numeric_limits<int>::max();
    for (auto& rect : rects) {
        m = min(m, rect.y);
    }
    move(0, y-m);
}

//-----------------------------------------------------------------------------
void Rectangles::move(int x, int y) {
    for (auto& rect : rects) {
        rect.x += x;
        rect.y += y;
    }
}

//-----------------------------------------------------------------------------
void Rectangles::get_xy(int &x, int &y) const {
    x = numeric_limits<int>::max();
    y = numeric_limits<int>::max();
    for (auto& rect : rects) {
        x = min(x, rect.x);
        y = min(y, rect.y);    
    } 
}

//-----------------------------------------------------------------------------
void Rectangles::get_rect(SDL_Rect &rect) const {

    int min_x = numeric_limits<int>::max();
    int min_y = numeric_limits<int>::max();
    int max_x = numeric_limits<int>::min();
    int max_y = numeric_limits<int>::min();

    for (auto& rect : rects) {
        min_x = min(min_x, rect.x);
        min_y = min(min_y, rect.y);  
        max_x = max(max_x, rect.x + rect.w);
        max_y = max(max_y, rect.y + rect.y);
    } 

    rect.x = min_x;
    rect.y = min_y;
    rect.w = max_x - min_x;
    rect.h = max_y - min_y;
}

//-----------------------------------------------------------------------------
//- Points --------------------------------------------------------------------
//-----------------------------------------------------------------------------
Points::Points(App *app, const vector<SDL_Point>&points, const SDL_Color &color) 
    : Render(app) {
    this->points = points;
    this->color = color;
}

//-----------------------------------------------------------------------------
Points::~Points() {
    
}

//-----------------------------------------------------------------------------
void Points::render(void) {
    SDL_SetRenderDrawColor(app->get_window_renderer(), color.r, color.g, color.b, color.a);      
    SDL_RenderDrawPoints(app->get_window_renderer(), points.data(), points.size());  
}

//-----------------------------------------------------------------------------
void Points::set_x(int x) {
    int m = numeric_limits<int>::max();
    for (auto& point : points) {
        m = min(m, point.x);
    }
    move(x-m, 0);    
}

//-----------------------------------------------------------------------------
void Points::set_y(int y) {
    int m = numeric_limits<int>::max();
    for (auto& point : points) {
        m = min(m, point.y);
    }
    move(0, y-m);    
}

//-----------------------------------------------------------------------------
void Points::move(int x, int y) {
    for (auto& point : points) {
        point.x += x;
        point.y += y;
    }
}

//-----------------------------------------------------------------------------
void Points::get_rect(SDL_Rect &rect) const {

    int min_x = numeric_limits<int>::max();
    int min_y = numeric_limits<int>::max();
    int max_x = numeric_limits<int>::min();
    int max_y = numeric_limits<int>::min();    

    for (auto& point : points) {
        min_x = min(min_x, point.x);
        min_y = min(min_y, point.y);
        max_x = max(max_x, point.x);
        max_y = max(max_y, point.y);
    }

    rect.x = min_x;
    rect.y = min_y;
    rect.w = max_x - min_x + 1;
    rect.h = max_y - min_y + 1;
}

//-----------------------------------------------------------------------------
//- Grid ----------------------------------------------------------------------
//-----------------------------------------------------------------------------
Grid::Grid(App *app, 
           int cols, int rows, int col_size, 
           int row_size, int vline_size, int hline_size,
           int vpad, int hpad,
           const SDL_Color &color)
    : Rectangles(app, vector<SDL_Rect>(), color, true) {

    this->cols = cols;
    this->rows = rows;
    this->col_size = col_size;
    this->row_size = row_size;    
    this->vline_size = vline_size;
    this->hline_size = hline_size;
    this->vpad = vpad;
    this->hpad = hpad;

    for(int i = 0; i <= cols; i++) {
        rects.push_back({i*col_size, 0, vline_size, rows*row_size+hline_size});            
    }
    
    for(int i = 0; i <= rows; i++) {
        rects.push_back({0, i*row_size, cols*col_size+vline_size, hline_size});            
    }
          
}  

//-----------------------------------------------------------------------------        
Grid::~Grid() {
    for(RendersI i = renders.begin(); i != renders.end(); i++) {
        delete (*i);                
    }   
}

//-----------------------------------------------------------------------------
void Grid::get_cell_rect(int col, int row, SDL_Rect &rect) const { 
    int x, y;
    get_xy(x, y);
    rect.x = ((col_size * col) + vpad + vline_size) + x;
    rect.y = ((row_size * row) + hpad + hline_size) + y;
    rect.w = col_size - (vpad*2) - vline_size;
    rect.h = row_size - (hpad*2) - hline_size;
}

//-----------------------------------------------------------------------------
Rectangle *Grid::add_retangle(int col, int row, const SDL_Color &color, bool fill) {
    SDL_Rect rect;
    get_cell_rect(col, row, rect);
    Rectangle *p = new Rectangle(app, rect, color, fill);
    renders.push_back(p);
    map_renders[col][row] = p;
    p->owner = this;
    return p;
}

//-----------------------------------------------------------------------------
Texture *Grid::add_texture(int col, int row, const string& file_name) { 
    SDL_Rect rect;
    get_cell_rect(col, row, rect);    
    Texture *p = new Texture(app, file_name, rect.x, rect.y);
    renders.push_back(p);
    map_renders[col][row] = p;
    p->owner = this;
    return p;
}    


//-----------------------------------------------------------------------------
void Grid::render(void) {

    Rectangles::render();

    for (auto i : renders) {
        i->render();    
    }
}

//-----------------------------------------------------------------------------
void Grid::set_x(int x) {

    Rectangles::set_x(x); 

    int m = numeric_limits<int>::max();
    for (auto& rect : rects) {
        m = min(m, rect.x);
    }     

    for (auto i : renders) {
        i->move(x-m, 0);
    }
}

//-----------------------------------------------------------------------------
void Grid::set_y(int y) {

    Rectangles::set_y(y); 

    int m = numeric_limits<int>::max();
    for (auto& rect : rects) {
        m = min(m, rect.y);
    }     

    for (auto i : renders) {
        i->move(0, y-m);
    }    
}

//-----------------------------------------------------------------------------
void Grid::move(int x, int y) {

    Rectangles::move(x, y);

    for (auto i : renders) {
        Rectangle *r = dynamic_cast<Rectangle*>(i);
        if(r) {
            r->rect.x += x;
            r->rect.y += y;
        } else {
            Texture *t = dynamic_cast<Texture*>(i);
            if(t) {
                t->rect.x += x;
                t->rect.y += y;
            }
        }
    }
}

//-----------------------------------------------------------------------------
void Grid::get_rect(SDL_Rect &rect) const {

    int min_x = numeric_limits<int>::max();
    int min_y = numeric_limits<int>::max();
    int max_x = numeric_limits<int>::min();
    int max_y = numeric_limits<int>::min();    
    
    for (auto& rect : rects) {
        min_x = min(min_x, rect.x);
        min_y = min(min_y, rect.y);
        max_x = max(max_x, rect.x + rect.w);
        max_y = max(max_y, rect.y + rect.h);
    }   

    rect.x = min_x;
    rect.y = min_y;
    rect.w = max_x - min_x;
    rect.h = max_y - min_y;
}

//-----------------------------------------------------------------------------
Render *Grid::get_render(int col, int row) {
    Render *result = map_renders[col][row];
    return result;
}

//-----------------------------------------------------------------------------
Render *Grid::remove_render(int col, int row) {
    Render *result = get_render(col, row);
    map_renders[col][row] = nullptr;
    RendersI position = find(renders.begin(), renders.end(), result);
    if(position != renders.end())  {
        renders.erase(position);
    }
    return result;
}

//-----------------------------------------------------------------------------
Render *Grid::remove_render(Render *render) {
    int col, row;
    get_render_cell(render, col, row);
    return remove_render(col, row);
}

//-----------------------------------------------------------------------------
bool Grid::get_render_cell(const Render *render, int &col, int &row) const {
    for (auto &i : map_renders) {
        for (auto j: i.second) {
            if(j.second == render) {
                col = i.first;
                row = j.first;
                return true;
            }
        }
    }
    return false;
}

//-----------------------------------------------------------------------------
Render *Grid::add_render(int col, int row, Render *render) {
    SDL_Rect rect;
    get_cell_rect(col, row, rect);
    render->set_xy(rect.x, rect.y);
    renders.push_back(render);
    map_renders[col][row] = render;
    render->owner = this;
    return render;
}

static constexpr int HH = 12;

//-----------------------------------------------------------------------------
//- Spin ----------------------------------------------------------------------
//-----------------------------------------------------------------------------
Spin::Spin(App *app, const string &ttf_file_name, int x, int y, 
    const SDL_Color &color, const SDL_Color &bg_color, int font_size,
    const vector<string> &items) 
: Render(app) {
    animate = false;
    this->color = color;
    this->bg_color = bg_color;
    spin = 0;
    TTF_Font* font = TTF_OpenFont(ttf_file_name.c_str(), font_size);
    if(!font) {
        throw Exception("Não foi possível carregar a fonte " + ttf_file_name, TTF_GetError());    
    }

    int max_w = 0;
    for(auto &text : items) {        
        SDL_Surface* temp_surface = TTF_RenderText_Solid(font, text.c_str(), color);
        if(!temp_surface) {
            throw Exception("Não foi possível renderizar o texto " + text, TTF_GetError());
        }
        SDL_GetClipRect(temp_surface, &text_rect);
        max_w = max(text_rect.w, max_w);

        SDL_Texture *texture;
        texture = SDL_CreateTextureFromSurface(app->get_window_renderer(), temp_surface);
        if(!texture) {
            throw Exception("Não foi possível criar a textura do texto " + text, SDL_GetError());
        }
        textures.push_back(texture);
        SDL_FreeSurface(temp_surface);

    }
    TTF_CloseFont(font);

    text_rect.w = max_w;

    points_up = {
        {0, HH}, 
        {text_rect.w/2, 0}, 
        {text_rect.w, HH}, 
        {0, HH}
    };
    points_down = {
        {0, text_rect.h+HH}, 
        {text_rect.w/2, text_rect.h+(HH*2)}, 
        {text_rect.w, text_rect.h+HH}, 
        {0, text_rect.h+HH}
    }; 
    rect = text_rect;
    rect.x = x;
    rect.y = y;
    rect.h += (HH*2);
    text_rect.x = x;
    text_rect.y += HH;

    on_mouse_click = [](Render *r) {
        auto up_function = [](void *obj) {
            Spin *spin = (Spin*)obj;
            spin->animate = true;
            for(int i = 0; i < 100; i++) {
                spin->dec_spin();
                SDL_Delay(3);
            }
            spin->animate = false;
            return 0;
        };
        auto down_function = [](void *obj) {
            Spin *spin = (Spin*)obj;
            spin->animate = true;
            for(int i = 0; i < 100; i++) {
                spin->inc_spin();
                SDL_Delay(3);
            }
            spin->animate = false;
            return 0;
        };
        Spin *spin = dynamic_cast<Spin*>(r);
        if(!spin->animate) {            
            int x, y;
            SDL_Rect rect;
            spin->get_rect(rect);
            SDL_GetMouseState(&x, &y);
            if(y > (rect.y+rect.h-HH)) {
                SDL_CreateThread(up_function, NULL, (void*)r);
            } else if (y < (rect.y+HH)) {
                SDL_CreateThread(down_function, NULL, (void*)r);
            }

        }
    };


}

//-----------------------------------------------------------------------------
Spin::~Spin() {
    for (auto i : textures) {
        SDL_DestroyTexture(i);
    }    
}

//-----------------------------------------------------------------------------
void Spin::render(void) {

    SDL_SetRenderDrawColor(app->get_window_renderer(), bg_color.r, bg_color.g, bg_color.b, bg_color.a);
    SDL_RenderFillRect(app->get_window_renderer(), &rect); 

    if((spin<0)||(spin>(textures.size()*100-1))) {
        throw Exception("Valor de spin alem dos limites", "Spin::render()");    
    }
    int index = get_value();
    SDL_Texture *texture0 = *(textures.begin()+index);
    SDL_Texture *texture1 = (index!=(textures.size()-1)) ? *(textures.begin()+index+1) : *textures.begin();

    const float h = text_rect.h;
    const int s = h/100 * (spin%100);

    SDL_Rect sr0 = text_rect;
    SDL_Rect dr0 = text_rect;
    sr0.x = 0;
    sr0.y = s; 
    sr0.h -= s;       
    dr0.h -= s;
    SDL_RenderCopyEx(app->get_window_renderer(), texture0, &sr0, &dr0, 0, NULL, SDL_FLIP_NONE);

    SDL_Rect sr1 = text_rect;
    SDL_Rect dr1 = text_rect;
    sr1.x = 0;
    sr1.y = 0; 
    sr1.h = s;
    dr1.h = s;
    dr1.y += h-s;
    SDL_RenderCopyEx(app->get_window_renderer(), texture1, &sr1, &dr1, 0, NULL, SDL_FLIP_NONE);

    SDL_SetRenderDrawColor(app->get_window_renderer(), color.r, color.g, color.b, color.a);
    SDL_RenderDrawLines(app->get_window_renderer(), points_up.data(), points_up.size()); 
    SDL_RenderDrawLines(app->get_window_renderer(), points_down.data(), points_down.size()); 
}

//-----------------------------------------------------------------------------
void Spin::set_x(int x) {
    int desloc = x - rect.x;
    rect.x = x;
    text_rect.x += desloc;
    for (auto& point : points_up) {
        point.x += desloc;
    }
    for (auto& point : points_down) {
        point.x += desloc;
    }

}

//-----------------------------------------------------------------------------
void Spin::set_y(int y) {
    int desloc = y - rect.y;
    rect.y = y;
    text_rect.y += desloc;
    for (auto& point : points_up) {
        point.y += desloc;
    }
    for (auto& point : points_down) {
        point.y += desloc;
    }
}

//-----------------------------------------------------------------------------
void Spin::move(int x, int y) {
    rect.x += x;
    rect.y += y;
    text_rect.x += x;
    text_rect.y += y;
}

//-----------------------------------------------------------------------------
void Spin::get_rect(SDL_Rect &rect) const {
    rect.x = this->rect.x;
    rect.y = this->rect.y;
    rect.w = this->rect.w;
    rect.h = this->rect.h;
}

//-----------------------------------------------------------------------------
void Spin::inc_spin(void) {
    if(spin == (textures.size()*100-1)) {
        spin = 0;
    } else {
        spin++;
    }
}

//-----------------------------------------------------------------------------
void Spin::dec_spin(void) {
    if(spin == 0) {
        spin=(textures.size()*100-1);
    } else {
        spin--;
    }
}

//-----------------------------------------------------------------------------
int Spin::get_value(void) {
    return spin/100;
}

//-----------------------------------------------------------------------------
void Spin::set_value(int value) {
    spin = value*100;
}

} // namespace SDLCards
