#include "app.hpp"
#include "exception.hpp"
#include <iostream>
#include <algorithm>
#include <fstream> 
#include <iostream>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

#include <curl/curl.h>

string App::save_path = "";
string App::url = "https://miliogo.com";
//string App::url = "http://localhost/miliogo";

//-----------------------------------------------------------------------------
App::App(const string& window_caption, int width, int heigth) {
    window = NULL;
    last_render_at = NULL;
    this->width = width;
    this->heigth = heigth;
    animate_stack = 0;
    show_login_window = false;
    show_alert_window = false;
    login_wait = false;
    login_status_msg = "";
    alert_message = "";
    login_hash = "";    
    load_login_hash();
    if((login_hash=="")||(!check_login())) {
        login_hash = ""; 
        show_login_window = true;
        login_status_msg = "necessário fazer login";
    }
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

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    static string save_file = App::save_path + "imgui.ini";
    io.IniFilename = save_file.c_str();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(window, window_renderer);
    ImGui_ImplSDLRenderer2_Init(window_renderer); 

}

//-----------------------------------------------------------------------------
App::~App() {

    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    delete_renders();
    SDL_DestroyRenderer(window_renderer);
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
            ImGui_ImplSDL2_ProcessEvent(&e);
            poll_event(&e);
            switch(e.type) {
                case SDL_QUIT:
                    quit = true;
            }  
        }
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame(); 

        render_login_window();
        render_alert_window();

        ImGui::Render();
        render();
    }    
}

//-----------------------------------------------------------------------------
void App::poll_event(SDL_Event *e) {

    if(show_login_window) return;

    switch(e->type) {
        case SDL_MOUSEMOTION: {            
            Render *r = get_render_at(e->motion.x, e->motion.y);            
            if((r) && (last_render_at != r)) {
                r->mouse_over();
            }            
            if((last_render_at) && (last_render_at != r)) {
                last_render_at->mouse_leave();
            }
            last_render_at = r;
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
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), window_renderer);

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
    
    release_last_render_at(NULL);
    for(RendersI i = renders.begin(); i != renders.end(); i++) {
        delete (*i);                
    }
    renders.clear();    
}

//-----------------------------------------------------------------------------
void App::delete_render(const Render *render) {
    
    release_last_render_at(render);
    RendersI position = find(renders.begin(), renders.end(), render);
    if(position != renders.end())  {
        renders.erase(position);
    }
    delete render;  
}

//-----------------------------------------------------------------------------
void App::release_last_render_at(const Render *render) {
    if((!render) || (last_render_at == render)) {
        last_render_at = NULL;
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
int App::get_width(void) {
    return width;
}
//-----------------------------------------------------------------------------    
int App::get_heigth(void) {
    return heigth;
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

//-----------------------------------------------------------------------------
void App::push_mouse_motion(void) {
    SDL_Event e;
    e.type = SDL_MOUSEMOTION;
    SDL_GetMouseState(&e.motion.x, &e.motion.y);    
    SDL_PushEvent(&e);
}

//-----------------------------------------------------------------------------
void App::push_quit(void) {
    SDL_Event e;
    e.type = SDL_QUIT;
    SDL_PushEvent(&e);
}

//-----------------------------------------------------------------------------
size_t App::curl_write_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

//-----------------------------------------------------------------------------
void App::render_login_window(void) {

    if(!show_login_window) return;  

    ImGui::Begin(
        "Login", 
        NULL, 
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove
    );
    const int LOGIN_W = 480;
    const int LOGIN_H = 320;
    ImGui::SetWindowPos(ImVec2(width/2-LOGIN_W/2, heigth/2-LOGIN_H/2));
    ImGui::SetWindowSize(ImVec2(LOGIN_W, LOGIN_H));
    ImGui::GetStyle().Alpha = 1;
    static bool show_password = false;
    static bool login_progress = false;
    

    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), login_status_msg.c_str());

    ImGui::InputText("Username", username, IM_ARRAYSIZE(username));
    ImGui::InputText("Password", password, IM_ARRAYSIZE(password), show_password ? ImGuiInputTextFlags_None : ImGuiInputTextFlags_Password);
    
    ImGui::Checkbox("Show Password", &show_password);
    if(login_progress || login_wait) {
        ImGui::ProgressBar(-1.0f * (float)ImGui::GetTime(), ImVec2(0.0f, 0.0f), "Login, please wait..");        
    } else {
        if (ImGui::Button("Login")) {
            if (strcmp(username, "") == 0 && strcmp(password, "") == 0) {
                login_status_msg = "login como usuário anônimo";
                login_hash = "";
                SDL_CreateThread(
                    [](void *obj) {
                        App *app = (App*)obj;
                        SDL_Delay(3000);
                        app->show_login_window = false;
                        return 0;
                    }, NULL, (void*)this
                );                  
            } else {
                login_progress = true;
                login_status_msg = "login em processo, aguarde...";
                login_hash = "";
                auto login_function = [](void *obj) {
                    App *app = (App*)obj;
                    CURL *curl = curl_easy_init();
                    if(curl) {
                        string u("u: ");
                        u.append(app->username);
                        string p("p: ");
                        p.append(app->password);
                        struct curl_slist *slist = NULL;
                        slist = curl_slist_append(slist, u.c_str());
                        slist = curl_slist_append(slist, p.c_str());                        
                        CURLcode res;
                        string response_buffer;
                        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
                        curl_easy_setopt(curl, CURLOPT_URL, app->make_url("login.php").c_str());
                        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, App::curl_write_callback);
                        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_buffer);
                        res = curl_easy_perform(curl);
                        switch(res) {
                            case CURLE_OK:
                                long response_code;
                                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
                                if(response_code == 201) {
                                    app->login_status_msg = "login realizado com sucesso";
                                    app->login_hash = response_buffer;
                                    app->save_login_hash();
                                    app->login_wait = true;
                                    SDL_CreateThread(
                                        [](void *obj) {
                                            App *app = (App*)obj;
                                            SDL_Delay(1500);
                                            app->show_login_window = false;
                                            app->login_wait = false;
                                            return 0;
                                        }, NULL, (void*)app
                                    );                                    
                                } else if(response_code == 200) {
                                    app->login_status_msg = "servidor respondeu de modo inesperado";
                                } else if(response_code == 401) {
                                    app->login_status_msg = "usuário ou senha inválidos";
                                } else if(response_code == 404) {
                                    app->login_status_msg = "o endpoint não existe";
                                } else if(response_code == 418) {
                                    app->login_status_msg = "eu sou um bule de chá";
                                } else {
                                    app->login_status_msg = "servidor respondeu com erro desconhecido";
                                }
                                break;
                            case CURLE_COULDNT_RESOLVE_HOST:
                                app->login_status_msg = "host não resolvido, verifique sua internet";
                                break;
                            default:
                                app->login_status_msg = "curl erro ";
                                app->login_status_msg.append(to_string((int)res));
                                app->login_status_msg.append(", verifique sua internet");
                        }
                        curl_easy_cleanup(curl);
                        curl_slist_free_all(slist);
                    }

                    login_progress = false;
                    return 0;
                };    
                SDL_CreateThread(login_function, NULL, (void*)this);
            }
        }
    }
    ImGui::Text("");
    ImGui::Text("Crie sua conta em miliogo.com");
    ImGui::Text("É grátis :-)");
    ImGui::End();
 
}

//-----------------------------------------------------------------------------
void App::save_login_hash(void) {
    const string file = App::save_path + "login.dat";
    ofstream f(file.c_str());
    if(f.good()) {
        f.write(login_hash.c_str(), login_hash.size());
    }        
}

//-----------------------------------------------------------------------------
void App::load_login_hash(void) {
    const string file = App::save_path + "login.dat";
    ifstream f(file.c_str());
    if(f.good()) {
        f >> login_hash;
    } else {
        login_hash = "";
    }
}

//-----------------------------------------------------------------------------
bool App::check_login(void) const {
    /*
    // request sincrono!
    */
    bool result = false;
    CURL *curl = curl_easy_init();
    if(curl) {
        string check("check: ");
        check.append(login_hash);
        struct curl_slist *slist = NULL;
        slist = curl_slist_append(slist, check.c_str());                        
        CURLcode res;
        string response_buffer;
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
        curl_easy_setopt(curl, CURLOPT_URL, make_url("login.php").c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, App::curl_write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_buffer);
        res = curl_easy_perform(curl);
        switch(res) {
            case CURLE_OK:
                long response_code;
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
                if(response_code == 202) {
                    result = true;
                }
                break;
        }
        curl_easy_cleanup(curl);
        curl_slist_free_all(slist);
    }
    return result;
}

//-----------------------------------------------------------------------------
void App::request(const string &endpoint, const Headers &headers, const string &body, RequestFunction func) {
    typedef struct { 
        App *app;
        RequestFunction func;
        string endpoint, body;
        Headers headers;
    } Obj;
    Obj *obj = new Obj();
    obj->app = this;
    obj->func = func;
    obj->headers = headers;
    obj->endpoint = endpoint;
    obj->body = body;
    auto request_function = [](void *obj) {
        App *app = ((Obj*)obj)->app;
        RequestFunction func = ((Obj*)obj)->func;
        const string& endpoint = ((Obj*)obj)->endpoint;
        const Headers& headers = ((Obj*)obj)->headers;
        const string& body = ((Obj*)obj)->body;
        CURL *curl = curl_easy_init();
        if(curl) {
            struct curl_slist *slist = NULL;
            for (auto i : headers) {                
                string header(i.first + ": " + i.second);
                slist = curl_slist_append(slist, header.c_str());
            }
            CURLcode res;
            string response_buffer;
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
            curl_easy_setopt(curl, CURLOPT_URL, app->make_url(endpoint).c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, App::curl_write_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_buffer);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
            res = curl_easy_perform(curl);
            switch(res) {
                case CURLE_OK:
                    long response_code;
                    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
                    func(app, response_code, response_buffer);
                    break;
                default:
                    func(app, (int)res, response_buffer);
            }
            curl_easy_cleanup(curl);
            curl_slist_free_all(slist);
        }
        delete ((Obj*)obj);
        return 0;
    };
    SDL_CreateThread(request_function, NULL, (void*)obj);
}

//-----------------------------------------------------------------------------
string App::make_url(const string& endpoint) const {
    return string(App::url + "/" + endpoint);
}

//-----------------------------------------------------------------------------
void App::show_alert(const string &msg) {
    alert_message = msg;
    SDL_CreateThread(
        [](void *obj) {
            App *app = (App*)obj;            
            app->show_alert_window = true;
            SDL_Delay(3000);
            for(int i = 0; i < 100; i++) {
                ImGui::GetStyle().Alpha -= 0.009;
                SDL_Delay(5);
            }            
            app->show_alert_window = false;
            ImGui::GetStyle().Alpha = 1;
            return 0;
        }, NULL, (void*)this
    );      
}

//-----------------------------------------------------------------------------
void App::render_alert_window(void) {
    if(!show_alert_window) return;

    ImGui::Begin(
        "Alerta", 
        NULL, 
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove
    );
    const int LOGIN_W = 480;
    const int LOGIN_H = 320;
    ImGui::SetWindowPos(ImVec2(width/2-LOGIN_W/2, heigth/2-LOGIN_H/2));
    ImGui::SetWindowSize(ImVec2(LOGIN_W, LOGIN_H));
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), alert_message.c_str());
    ImGui::End();

}