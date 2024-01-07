#ifndef __PACIENCIA_APP__
#define __PACIENCIA_APP__

#include "app.hpp"

typedef vector<CardGroup*> Cols;

class Paciencia: public App {
public:
    Paciencia();
    ~Paciencia();
    void poll_event(SDL_Event *e);
private:
    const int cw = 100;
    const int ch = 150;
    const int dist = 12;
    Cols cols;
    CardGroup *casa_ouros;
    Card *casa_ouros_ghost;
    CardGroup *casa_espadas;
    Card *casa_espadas_ghost;
    CardGroup *casa_copas;
    Card *casa_copas_ghost;
    CardGroup *casa_paus;
    Card *casa_paus_ghost;
    void pega_monte(void);
    void casa_ouros_click(void);
    void casa_espadas_click(void);
    void casa_copas_click(void);
    void casa_paus_click(void);
    void casa_click(Naipe naipe);
    int calc_col_x(int col) const;
    int calc_row_y(int row) const;
    CardGroup *create_col(int col);    
};

class PacienciaCard: public Card {
public:
    PacienciaCard(Paciencia *app, int card_id, int x, int y);
};

#endif