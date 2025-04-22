#ifndef __GAME_RENDER__
#define __GAME_RENDER__

#include <string>
#include <vector>
#include <map>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

using namespace std;

class App;
class Render;

typedef vector<Render*> Renders;
typedef Renders::iterator RendersI;
typedef Renders::reverse_iterator RendersRI;
typedef Renders::const_reverse_iterator RendersCRI;

void inflate_rect(SDL_Rect &rect, int amount);

class Render {
protected:
    Renders renders;
public:
    Render(App *app);
    virtual ~Render();
public:
    App *app;
    Render *owner;
    Render *link;
    SDL_Color color;
    string name;
    int tag;
    virtual void render(void) = 0;
    virtual void set_x(int x) = 0;
    virtual void set_y(int y) = 0;
    virtual void move(int x, int y) = 0;
    virtual void get_rect(SDL_Rect &rect) const = 0;
    virtual void set_xy(int x, int y);
    void get_inflate_rect(SDL_Rect &rect, int amount) const;
    const Renders& get_renders() const;
    bool rect_contains(int x, int y) const;
    void mouse_over(void);
    void mouse_leave(void);
    void mouse_click(void);
    void mouse_dclick(void);
public:
    void (*on_mouse_over)(Render*);
    void (*on_mouse_leave)(Render*);
    void (*on_mouse_click)(Render*);
    void (*on_mouse_dclick)(Render*);
};

class Texture : public Render {
public:
    Texture(App *app, const string& file_name, int x, int y);
    Texture(App *app, 
            const string& ttf_file_name, 
            const string& text,
            int x, int y, 
            const SDL_Color &color,
            int font_size
    );
    ~Texture();
public:      
    SDL_Rect rect;
    void render(void);      
    void set_x(int x);
    void set_y(int y);
    void move(int x, int y);
    void get_rect(SDL_Rect &rect) const;
    void set_blend(SDL_BlendMode mode);
    void set_alpha(Uint8 alpha);
    void set_color(Uint8 r, Uint8 g, Uint8 b);
    void rotate(double angle);
    void inc_rotate(double inc_angle);
    void change_image(const string& file_name);
    void change_text(const string& text);
private:
    SDL_Texture* texture;
    double rotate_angle;
    string ttf_file_name;
    int font_size;
};

class Rectangle : public Render {
public:
    Rectangle(App *app, const SDL_Rect &rect, const SDL_Color &color, bool fill);
    ~Rectangle();
public:
    SDL_Rect rect;
    void render(void);
    void set_x(int x);
    void set_y(int y);
    void move(int x, int y); 
    void get_rect(SDL_Rect &rect) const;   
private:
    bool fill;    
};

class Rectangles : public Render {
public:
    Rectangles(App *app, const vector<SDL_Rect>&rects, const SDL_Color &color, bool fill);
    ~Rectangles();
public:
    void render(void);
    void set_x(int x);
    void set_y(int y);
    void move(int x, int y);
    void get_xy(int &x, int &y) const;
    void get_rect(SDL_Rect &rect) const;
protected:
    vector<SDL_Rect>rects;
private:
    bool fill;    
};

class Line : public Render {
public:
    Line(App *app, const SDL_Point &point1, const SDL_Point &point2, const SDL_Color &color);
    ~Line();
public:
    SDL_Point point1, point2;
    void render(void);
    void set_x(int x);
    void set_y(int y);
    void move(int x, int y);
    void get_rect(SDL_Rect &rect) const;        
};

class Lines : public Render {
public:
    Lines(App *app, const vector<SDL_Point> &points, const SDL_Color &color);
    ~Lines();
public:
    void render(void);
    void set_x(int x);
    void set_y(int y);
    void move(int x, int y);
    void get_rect(SDL_Rect &rect) const;       
private:
    vector<SDL_Point> points;
};

class Point : public Render {
public:
    Point(App *app, const SDL_Point &point, const SDL_Color &color);
    ~Point();
public:
    SDL_Point point;
    void render(void); 
    void set_x(int x);
    void set_y(int y);
    void move(int x, int y);
    void get_rect(SDL_Rect &rect) const;         
};

class Points : public Render {
public:
    Points(App *app, const vector<SDL_Point>&points, const SDL_Color &color);
    ~Points();
public:
    void render(void);
    void set_x(int x);
    void set_y(int y);
    void move(int x, int y);
    void get_rect(SDL_Rect &rect) const;
private:
    vector<SDL_Point>points;    
};

class Grid : public Rectangles {
public:
    Grid(App *app, 
         int cols, int rows, int col_size, 
         int row_size, int vline_size, int hline_size,
         int vpad, int hpad,
         const SDL_Color &color);
    ~Grid();
public:
    void get_cell_rect(int col, int row, SDL_Rect &rect) const;
    Rectangle *add_retangle(int col, int row, const SDL_Color &color, bool fill);
    Texture *add_texture(int col, int row, const string& file_name);
    Render *remove_render(int col, int row);
    Render *remove_render(Render *render);
    bool get_render_cell(const Render *render, int &col, int &row) const;
    Render *get_render(int col, int row);
    Render *add_render(int col, int row, Render *render);
public:
    void render(void);    
    void set_x(int x);
    void set_y(int y);
    void move(int x, int y);  
    void get_rect(SDL_Rect &rect) const;    
protected:
    map<int, map<int, Render*> > map_renders;
private:
    int cols;
    int rows;
    int col_size;
    int row_size;    
    int vline_size;
    int hline_size;
    int vpad;
    int hpad;        
};

class Spin : public Render {
public:
    Spin(App *app, const string &ttf_file_name, 
        int x, int y, const SDL_Color &color, 
        const SDL_Color &bg_color,
        int font_size,
        const vector<string> &items = {"0","1","2","3","4","5","6","7","8","9"}
    );
    ~Spin();
public:    
    SDL_Rect rect;
    SDL_Rect text_rect;
    bool animate;
    void render(void);
    void set_x(int x);
    void set_y(int y);
    void move(int x, int y);
    void get_rect(SDL_Rect &rect) const;
    int get_value(void);
    void set_value(int value);
private:
    vector<SDL_Texture*> textures;
    vector<SDL_Point> points_up;
    vector<SDL_Point> points_down;
    SDL_Color bg_color;
    int spin;
    void inc_spin(void);
    void dec_spin(void);
};

#endif