#include "paciencia.hpp"

//-----------------------------------------------------------------------------
Paciencia::Paciencia() : App("Paciência 1.0", 800, 600) {    

    for (int i = 1; i <= 7; i++) {
        create_col(i);
    }

    //====================
    void (*monte_click)(Render*) = [](Render *r) {
        Paciencia *paciencia = dynamic_cast<Paciencia*>(r->app);
        paciencia->pega_monte();
    };
    Card *monte = add_card(0, calc_col_x(1), calc_row_y(1));
    monte->set_enabled(false);
    monte->on_mouse_click = monte_click;
    
    //====================
    void (*casa_ouros_click)(Render*) = [](Render *r) {
        Paciencia *paciencia = dynamic_cast<Paciencia*>(r->app);
        paciencia->casa_ouros_click();
    };
    
    casa_ouros = add_card_group();
    casa_ouros->set_xy(calc_col_x(2), calc_row_y(1));    
    casa_ouros_ghost = casa_ouros->add_card(11);
    casa_ouros_ghost->set_alpha(32);
    casa_ouros_ghost->set_enabled(false);
    casa_ouros_ghost->on_mouse_click = casa_ouros_click;

    //====================
    void (*casa_espadas_click)(Render*) = [](Render *r) {
        Paciencia *paciencia = dynamic_cast<Paciencia*>(r->app);
        paciencia->casa_espadas_click();
    };
    

    casa_espadas = add_card_group();
    casa_espadas->set_xy(calc_col_x(3), calc_row_y(1));    
    casa_espadas_ghost = casa_espadas->add_card(12);
    casa_espadas_ghost->set_alpha(32);
    casa_espadas_ghost->set_enabled(false);
    casa_espadas_ghost->on_mouse_click = casa_espadas_click;    


    //====================
    void (*casa_copas_click)(Render*) = [](Render *r) {
        Paciencia *paciencia = dynamic_cast<Paciencia*>(r->app);
        paciencia->casa_copas_click();
    };
    casa_copas = add_card_group();
    casa_copas->set_xy(calc_col_x(4), calc_row_y(1));    
    casa_copas_ghost = casa_copas->add_card(13);
    casa_copas_ghost->set_alpha(32);
    casa_copas_ghost->set_enabled(false);
    casa_copas_ghost->on_mouse_click = casa_copas_click;    

    //====================
    void (*casa_paus_click)(Render*) = [](Render *r) {
        Paciencia *paciencia = dynamic_cast<Paciencia*>(r->app);
        paciencia->casa_paus_click();
    };
    casa_paus = add_card_group();
    casa_paus->set_xy(calc_col_x(5), calc_row_y(1));    
    casa_paus_ghost = casa_paus->add_card(14);
    casa_paus_ghost->set_alpha(32);
    casa_paus_ghost->set_enabled(false);
    casa_paus_ghost->on_mouse_click = casa_paus_click;


}

//-----------------------------------------------------------------------------
Paciencia::~Paciencia() {

}

//-----------------------------------------------------------------------------
void Paciencia::poll_event(SDL_Event *e) {    
    switch(e->type) {
        case SDL_KEYDOWN: {
            if(e->key.keysym.sym == SDLK_ESCAPE) {
               e->type = SDL_QUIT; 
            }
            if(e->key.keysym.sym == SDLK_p) {
               screen_shot(); 
            }
            break;
        }
    }
    App::poll_event(e);
}

//-----------------------------------------------------------------------------
void Paciencia::pega_monte(void) {    

}

//-----------------------------------------------------------------------------
void Paciencia::casa_click(Naipe naipe) {
    
    CardGroup *casa;
    Card *casa_ghost;
    switch(naipe) {
        case OUROS:
            casa = casa_ouros;
            casa_ghost = casa_ouros_ghost;
            break;
        case ESPADAS: 
            casa = casa_espadas;
            casa_ghost = casa_espadas_ghost;
            break;
        case COPAS: 
            casa = casa_copas;
            casa_ghost = casa_copas_ghost;
            break;
        case PAUS: 
            casa = casa_paus;
            casa_ghost = casa_paus_ghost;
            break;
    };    
    
    for(auto col : cols) {
        Cards selecteds = col->get_selecteds();
        for(auto card : selecteds) {
            int id = card->get_card_id();
            if(!((card->naipe(naipe)) && ((id/10) == casa->get_cards().size()))) {
                return;
            }
        }
        for(auto card : selecteds) {
            card->set_selected(false);
            card->on_mouse_click = casa_ghost->on_mouse_click;
        }

        col->move_cards(selecteds, casa, casa->get_cards().size());
        
        if(!selecteds.empty()) {
            Cards cards = col->get_cards();
            if(!cards.empty()) {
                Card *c = *cards.rbegin();
                c->set_card_id(Card::rand_card_id());
                c->set_enabled(true);
            }
        }
    }
}

//-----------------------------------------------------------------------------
void Paciencia::casa_ouros_click(void) {
    casa_click(OUROS);
}

//-----------------------------------------------------------------------------
void Paciencia::casa_espadas_click(void) {
    casa_click(ESPADAS);
}

//-----------------------------------------------------------------------------
void Paciencia::casa_copas_click(void) {
    casa_click(COPAS);
}

//-----------------------------------------------------------------------------
void Paciencia::casa_paus_click(void) {    
    casa_click(PAUS);
}

//-----------------------------------------------------------------------------
int Paciencia::calc_col_x(int col) const {
    return (dist*col) + (cw*(col-1));
}

//-----------------------------------------------------------------------------
int Paciencia::calc_row_y(int row) const {
    return (dist*row) + (ch*(row-1));
}

//-----------------------------------------------------------------------------
CardGroup *Paciencia::create_col(int col) {

    Card *empty_ghost = add_card(0, calc_col_x(col), calc_row_y(2));
    empty_ghost->set_alpha(16);
    empty_ghost->set_enabled(false);
    //empty_ghost->on_mouse_click = empty_ghost_click;

    CardGroup *group = add_card_group(Vertical);    
    group->set_xy(calc_col_x(col), calc_row_y(2)); 
    for (int i = 1; i <= col; i++) {
        Card *c = group->add_card(0);
        c->set_enabled(false);
    }    
    group->add_card(new PacienciaCard(this, Card::rand_card_id(), 0, 0)); 
    cols.push_back(group);
    return group;   
}

//-----------------------------------------------------------------------------
//- PacienciaCard -------------------------------------------------------------
//-----------------------------------------------------------------------------
PacienciaCard::PacienciaCard(Paciencia *app, int card_id, int x, int y) 
    : Card(app, card_id, x, y) {

}


