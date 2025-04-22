#ifndef __PACIENCIA_APP__
#define __PACIENCIA_APP__

#include "app.hpp"

typedef vector<CardGroup*> Cols;
typedef vector<int> Baralho;

typedef struct {
    int seed;
    int dificult;
} SaveData;

class Paciencia: public App {
public:
    Paciencia();
    ~Paciencia();
    void poll_event(SDL_Event *e);    
public:    
    Cols cols;
    vector<Spin*> spins_rand_seed;
    Spin *spin_dificult;
    SaveData save_data;
    Texture *score;
    const string FONTFILE = "./VT323-Regular.ttf";
    int pop_baralho(void);
    void new_game(void);
    void menu(void);
    void delete_render(const Render *render); 
    string baralho_str(void) const;
    string cards_to_string(const Cards &cards) const;
    void send_new_game(void);
    void send_pega_monte(void);
    void send_move(const CardGroup *source, const Cards &selecteds, const CardGroup *dest);
    void send_terminou(void);
private:
    const int cw = 100;
    const int ch = 150;
    const int dist = 12;
    const int ghost_alpha = 32;
    int move_count;    
    string config_file;
    Baralho baralho;   
    CardGroup *casa_ouros;
    Card *casa_ouros_ghost;
    CardGroup *casa_espadas;
    Card *casa_espadas_ghost;
    CardGroup *casa_copas;
    Card *casa_copas_ghost;
    CardGroup *casa_paus;
    Card *casa_paus_ghost;    
    void pega_monte(Render *r);
    void casa_ouros_click(void);
    void casa_espadas_click(void);
    void casa_copas_click(void);
    void casa_paus_click(void);
    void casa_click(Naipe naipe);
    int calc_col_x(int col) const;
    int calc_row_y(int row) const;
    CardGroup *create_col(int col); 
    Texture *add_paciencia_button(const string& text,
        int x, int y, int font_size);
    Texture *create_paciencia_button(const string& text,
        int x, int y, int font_size);
};

class PacienciaCard: public Card {
public:
    PacienciaCard(Paciencia *app, int card_id, int x, int y);
protected:
    void before_select(bool &can_select);
};

#endif