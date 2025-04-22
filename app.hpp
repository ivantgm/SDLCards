#ifndef __GAME_APP__
#define __GAME_APP__

#include <string>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "render.hpp"
#include "card.hpp"

namespace SDLCards {

using namespace std;

class Animate {
public:
    Card *card;
    int orig_x, orig_y, dest_x, dest_y;
};

typedef vector<Animate> Animates;
typedef map<string, string> Headers;
typedef void (*RequestFunction)(App*, long, string);

class App {
public:
    App(const string& window_caption, int width, int heigth);
    ~App();
public:        
    Renders renders;    
public:
    static string save_path;
    static string url;
    string login_status_msg;
    string login_hash;
    bool show_login_window; 
    bool login_wait;
    bool show_alert_window;
    string alert_message;
    void save_login_hash(void);
    void load_login_hash(void);
    bool check_login(void) const; 
    void request(const string &url, const Headers &headers, const string &body, RequestFunction func);    
    string make_url(const string& endpoint) const;
    void show_alert(const string &msg);
    static size_t curl_write_callback(void *contents, size_t size, size_t nmemb, void *userp);
    SDL_Renderer* get_window_renderer(void);
    void loop(void);
    virtual void poll_event(SDL_Event *e);
    void screen_shot(void);
    int get_width(void);
    int get_heigth(void);
    void release_last_render_at(const Render *render);
    void push_mouse_motion(void);
    void push_quit(void);    
public:    
    void begin_animate(void);
    void end_animate(void);
    bool is_animate(void) const;
    void add_animate(Card *card, int x, int y);
protected: 
    char username[128] = "";
    char password[128] = "";
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
    CardGroup *add_card_group(void);
    Render *get_render_at(int x, int y);
    void delete_renders(void);
    void delete_render(const Render *render);    
private:
    SDL_Window* window; 
    SDL_Renderer* window_renderer;
    int width;
    int heigth;
    Render *last_render_at;
private:
    int animate_stack;
    Animates animates;
private:
    void render_renders(void);    
    void render(void);
private: // IMGUI
    void render_login_window(void);
    void render_alert_window(void);
};

} // namespace SDLCards

#endif
