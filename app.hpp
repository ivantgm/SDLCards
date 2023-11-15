#ifndef __GAME_APP__
#define __GAME_APP__

#include <string>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "render.hpp"
#include "card.hpp"

using namespace std;

class App {
public:
    App(const string& window_caption, int width, int heigth);
    ~App();
public:    
    void loop(void);
    virtual void poll_event(SDL_Event *e);
    void screen_shot(void);
protected:
    SDL_Renderer* window_renderer;
    Renders renders;    
    Texture *add_texture(const string& file_name, int x, int y);
    Texture *add_texture_text(const string& ttf_file_name, const string& text,
            int x, int y, const SDL_Color &color, int font_size
        );
    Rectangle *add_rectangle(const SDL_Rect &rect, const SDL_Color &color, bool fill);
    Rectangles *add_rectangles(const vector<SDL_Rect> &rects, const SDL_Color &color, bool fill);
    Line *add_line(const SDL_Point &point1, const SDL_Point &point2, const SDL_Color &color);
    Lines *add_lines(const vector<SDL_Point> &points, const SDL_Color &color);
    Point *add_point(const SDL_Point &point, const SDL_Color &color);
    Points *add_points(const vector<SDL_Point> &points, const SDL_Color &color);
    Grid *add_grid(
            int cols, int rows, 
            int col_size, int row_size, 
            int vline_size, int hline_size, 
            int vpad, int hpad, 
            const SDL_Color &color
        );
    Card *add_card(int card_id, int x, int y);
    CardGroup *add_card_group(CardGroupDirection direction);
    Render *get_render_at(int x, int y);
private:
    SDL_Window* window; 
    int width;
    int heigth;
private:
    void render_renders(void);
    void delete_renders(void);
    void render(void);    
};

#endif
