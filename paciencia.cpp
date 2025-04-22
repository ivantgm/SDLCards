#include "paciencia.hpp"
#include "exception.hpp"
#include <algorithm>
#include <fstream> 

namespace SDLCards {

const string GAME_PACIENCIA = "1";
const string TIPO_NEW_GAME  = "1";
const string TIPO_PEGAMONTE = "2";
const string TIPO_MOVE      = "3";
const string TIPO_TERMINOU  = "4";

//-----------------------------------------------------------------------------
Paciencia::Paciencia() : App("Paciência 1.0", 910, 700) {
    
    config_file = App::save_path + "config.dat";

    ifstream f(config_file.c_str());
    if(f.good()) {
        f.read((char*)&save_data, sizeof(SaveData));
    } else {
        save_data.seed = 42;
        save_data.dificult = 0;
    }    
    
    new_game();    

}

//-----------------------------------------------------------------------------
Paciencia::~Paciencia() {
    ofstream f(config_file.c_str());
    if(f.good()) {
        f.write((char*)&save_data, sizeof(SaveData));
    }
}

//-----------------------------------------------------------------------------
void Paciencia::menu(void) {
    const int LINHAS = 30;
    const int COLUNAS = 50;
    Grid *form = add_grid( 
         COLUNAS, LINHAS, (get_width()-8) / COLUNAS, (get_heigth()-8) / LINHAS, 
         0, 0, 0, 0, {255, 0, 0}
    );
    form->move(4, 4);
    for(int c = 0; c < COLUNAS; c++) {
        for(int r = 0; r < LINHAS; r++) {
            form->add_retangle(c, r, {16,16,16}, true);
        }
    }
    Render *btn_quit = form->add_render(1, 0, create_paciencia_button("SAIR", 640, 10, 96));
    btn_quit->on_mouse_click = [](Render *r) {
        Paciencia *paciencia = dynamic_cast<Paciencia*>(r->app);
        paciencia->push_quit();
    };  

    Render *btn_cancel = form->add_render(16, 0, create_paciencia_button("VOLTAR PRO JOGO", 640, 10, 96));
    btn_cancel->on_mouse_click = [](Render *r) {
        Paciencia *paciencia = dynamic_cast<Paciencia*>(r->app);
        paciencia->release_last_render_at(r);
        paciencia->delete_render(r->owner);
        paciencia->push_mouse_motion();
    }; 
    
    Render *btn_logoff = form->add_render(30, 13, create_paciencia_button("LOGOFF", 640, 10, 96));
    btn_logoff->on_mouse_click = [](Render *r) {
        Paciencia *paciencia = dynamic_cast<Paciencia*>(r->app);
        
        paciencia->login_hash = "";
        paciencia->show_login_window = true;
        paciencia->login_status_msg = "logoff realizado";        
        paciencia->save_login_hash();

        paciencia->release_last_render_at(r);
        paciencia->delete_render(r->owner);
        paciencia->push_mouse_motion();
    }; 


    form->add_render(5, 8, new Texture(this, FONTFILE, "Random Seed:", 0, 0, {127, 127, 127}, 48));

    spins_rand_seed.clear();
    spins_rand_seed.push_back((Spin*) form->add_render(5, 10, new Spin(this, FONTFILE, 0, 0, {255,255,255}, {64,64,64}, 72)));
    spins_rand_seed.push_back((Spin*) form->add_render(7, 10, new Spin(this, FONTFILE, 0, 0, {255,255,255}, {64,64,64}, 72)));
    spins_rand_seed.push_back((Spin*) form->add_render(9, 10, new Spin(this, FONTFILE, 0, 0, {255,255,255}, {64,64,64}, 72)));

    spins_rand_seed.push_back((Spin*) form->add_render(12, 10, new Spin(this, FONTFILE, 0, 0, {255,255,255}, {64,64,64}, 72)));
    spins_rand_seed.push_back((Spin*) form->add_render(14, 10, new Spin(this, FONTFILE, 0, 0, {255,255,255}, {64,64,64}, 72)));
    spins_rand_seed.push_back((Spin*) form->add_render(16, 10, new Spin(this, FONTFILE, 0, 0, {255,255,255}, {64,64,64}, 72)));

    spins_rand_seed.push_back((Spin*) form->add_render(19, 10, new Spin(this, FONTFILE, 0, 0, {255,255,255}, {64,64,64}, 72)));
    spins_rand_seed.push_back((Spin*) form->add_render(21, 10, new Spin(this, FONTFILE, 0, 0, {255,255,255}, {64,64,64}, 72)));
    spins_rand_seed.push_back((Spin*) form->add_render(23, 10, new Spin(this, FONTFILE, 0, 0, {255,255,255}, {64,64,64}, 72)));
    
    
    string s = to_string(save_data.seed);
    while(s.length()<9) s = "0" + s;
    int l = 0;
    for(auto i : spins_rand_seed) {
        char c = s[l++]-'0';
        i->set_value(c);
    }   

    form->add_render(5, 17, new Texture(this, FONTFILE, "Dificuldade:", 0, 0, {127, 127, 127}, 48));
    spin_dificult = (Spin*)form->add_render(5, 19, 
        new Spin(this, FONTFILE, 0, 0, 
            {255,255,255}, {64,64,64}, 72, {"FACIL","MEDIO","DIFICIL"}
        )
    ); 
    spin_dificult->set_value(save_data.dificult);

    Render *btn_apply_seed = form->add_render(12, 25, create_paciencia_button("Aplicar e Reiniciar", 0, 0, 72));    
    btn_apply_seed->on_mouse_click = [](Render *r) {
        Paciencia *paciencia = dynamic_cast<Paciencia*>(r->app);        
        paciencia->release_last_render_at(r);
        int seed = 0;
        int ex = 8;
        for(auto i : paciencia->spins_rand_seed) {
            int value = i->get_value();
            seed += value * pow(10, ex);
            ex--;
        }
        paciencia->save_data.dificult = paciencia->spin_dificult->get_value();
        paciencia->delete_render(r->owner);
        paciencia->push_mouse_motion();        
        paciencia->save_data.seed = seed;
        paciencia->new_game();
    };     

    push_mouse_motion();


}

//-----------------------------------------------------------------------------
void Paciencia::delete_render(const Render *render) {
    App::delete_render(render);
}

//-----------------------------------------------------------------------------
Texture *Paciencia::add_paciencia_button(const string& text, int x, int y, int font_size) {
    
    Texture *p = create_paciencia_button(text, x, y, font_size);
    renders.push_back(p);
    return p;    
}

//-----------------------------------------------------------------------------
Texture *Paciencia::create_paciencia_button(const string& text, int x, int y, int font_size) {
    
    Texture *button = new Texture(this, FONTFILE, text, x, y, {127, 127, 127}, font_size);

    button->on_mouse_over = [](Render *r) {
        Texture *button = dynamic_cast<Texture*>(r);
        button->set_color(255, 0, 0);
    };
    button->on_mouse_leave = [](Render *r) {
        Texture *button = dynamic_cast<Texture*>(r);
        button->set_color(127, 127, 127);
    };    
    button->set_color(127, 127, 127);
    return button;
}

//-----------------------------------------------------------------------------
void Paciencia::new_game(void) {
    
    move_count = 0;
    cols.clear();
    baralho.clear();

    for(int v = 13; v >= 1; v--) {
        for(int n = 1; n <= 4; n++) {
            baralho.push_back(v*10+n);
        }
    }    
    srand(save_data.seed);
    random_shuffle(baralho.begin(), baralho.end());    

    delete_renders();

    send_new_game();

    score = new Texture(this, FONTFILE, "000", 600, 10, {255, 255, 255}, 64);
    score->tag = 0;
    renders.push_back(score);

    Texture *button = add_paciencia_button("MENU", 780, 10, 64);
    button->on_mouse_click = [](Render *r) {
        Paciencia *paciencia = dynamic_cast<Paciencia*>(r->app);
        paciencia->menu();
    };    
    const int DIFICIL = 2;
    const int NUMERO_COLUNAS = (save_data.dificult==DIFICIL) ? 7 : 8;
    for (int i = 1; i <= NUMERO_COLUNAS; i++) {
        create_col(i);
    }

    //====================
    void (*monte_click)(Render*) = [](Render *r) {
        Paciencia *paciencia = dynamic_cast<Paciencia*>(r->app);
        paciencia->pega_monte(r);
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
    casa_ouros_ghost->set_alpha(ghost_alpha);
    casa_ouros_ghost->set_enabled(false);
    casa_ouros_ghost->on_mouse_click = casa_ouros_click;
    casa_ouros->name = "ouros";

    //====================
    void (*casa_espadas_click)(Render*) = [](Render *r) {
        Paciencia *paciencia = dynamic_cast<Paciencia*>(r->app);
        paciencia->casa_espadas_click();
    };
    

    casa_espadas = add_card_group();
    casa_espadas->set_xy(calc_col_x(3), calc_row_y(1));    
    casa_espadas_ghost = casa_espadas->add_card(12);
    casa_espadas_ghost->set_alpha(ghost_alpha);
    casa_espadas_ghost->set_enabled(false);
    casa_espadas_ghost->on_mouse_click = casa_espadas_click;
    casa_espadas->name = "espadas";


    //====================
    void (*casa_copas_click)(Render*) = [](Render *r) {
        Paciencia *paciencia = dynamic_cast<Paciencia*>(r->app);
        paciencia->casa_copas_click();
    };
    casa_copas = add_card_group();
    casa_copas->set_xy(calc_col_x(4), calc_row_y(1));    
    casa_copas_ghost = casa_copas->add_card(13);
    casa_copas_ghost->set_alpha(ghost_alpha);
    casa_copas_ghost->set_enabled(false);
    casa_copas_ghost->on_mouse_click = casa_copas_click;
    casa_copas->name = "copas";

    //====================
    void (*casa_paus_click)(Render*) = [](Render *r) {
        Paciencia *paciencia = dynamic_cast<Paciencia*>(r->app);
        paciencia->casa_paus_click();
    };
    casa_paus = add_card_group();
    casa_paus->set_xy(calc_col_x(5), calc_row_y(1));    
    casa_paus_ghost = casa_paus->add_card(14);
    casa_paus_ghost->set_alpha(ghost_alpha);
    casa_paus_ghost->set_enabled(false);
    casa_paus_ghost->on_mouse_click = casa_paus_click;
    casa_paus->name = "paus";

}

//-----------------------------------------------------------------------------
void Paciencia::poll_event(SDL_Event *e) {    
    switch(e->type) {
        case SDL_KEYDOWN: {
            if(e->key.keysym.sym == SDLK_p) {
               screen_shot(); 
            }
            break;
        }
    }
    App::poll_event(e);
}

//-----------------------------------------------------------------------------
void Paciencia::pega_monte(Render *r) {
    Card *monte = dynamic_cast<Card *>(r);    
    for(auto col : cols) {
        Cards selecteds = col->get_selecteds();
        for(auto card : selecteds) {
            if(card->get_selected()) {
                card->set_selected(false);
            }
        }
        if(baralho.size()) {
            col->add_card(new PacienciaCard(this, pop_baralho(), 0, 0)); 
        } 
        if(!baralho.size()) {
            monte->set_alpha(ghost_alpha);
        }        
    }
    send_pega_monte();
}

//-----------------------------------------------------------------------------
int Paciencia::pop_baralho(void) {
    int result = baralho.back();
    baralho.pop_back();
    return result;
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
        case NONE:
            throw Exception("Não tem um naipe.", "Paciencia::casa_click");
    };    
    
    for(auto col : cols) {
        Cards selecteds = col->get_selecteds();
        if(!selecteds.empty()) {

            bool mover = false;
            // todo - nao precisava ser for, pois testa somente a ultima carta, analisar
            for(Cards::reverse_iterator i = selecteds.rbegin(); i != selecteds.rend(); i++) {
                Card *card = (*i);
                int id = card->get_card_id();
                if(!((card->naipe(naipe)) && ((id/10) == casa->get_cards().size()))) {
                    return;
                } else {
                    break;
                }
            }
            for(auto card : selecteds) {
                card->set_selected(false);
                card->on_mouse_click = casa_ghost->on_mouse_click;
            }

            col->move_cards(selecteds, casa, -1, true);
            score->tag += pow(selecteds.size(), 2) * (save_data.dificult+1);
            string score_string = to_string(score->tag);
            score_string.insert(0, 3-score_string.size(), '0');
            score->change_text(score_string);

            send_move(col, selecteds, casa); 

            Cards cards = col->get_cards();
            if(!cards.empty()) {
                Card *c = *cards.rbegin();
                if(!c->get_enabled()) {
                    c->set_card_face(FACE_UP);
                    c->set_enabled(true);
                }
            }
        }
    }
    bool terminou = true;
    for(auto col : cols) {
        if(!col->get_cards().empty()) {
            terminou = false;
            break;
        }
    }
    if(terminou) {
        send_terminou();
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

    void (*empty_ghost_click)(Render*) = [](Render *r) {
        CardGroup *group = dynamic_cast<CardGroup *>(r->link);
        Paciencia *paciencia = dynamic_cast<Paciencia*>(r->app);
        for(auto col : paciencia->cols) {
            if(col != group) {
                Cards selecteds = col->get_selecteds();
                col->move_cards(selecteds, group, -1, false);
                for(auto card : selecteds) {
                    card->set_selected(false);
                }  
                Cards cards = col->get_cards();
                if(!cards.empty()) {
                    Card *c = *cards.rbegin();
                    if(!c->get_enabled()) {
                        c->set_card_face(FACE_UP);
                        c->set_enabled(true);
                    }
                }
            }
        }

    };    
    
    Card *empty_ghost = add_card(0, calc_col_x(col), calc_row_y(2));
    empty_ghost->set_alpha(ghost_alpha);
    empty_ghost->set_enabled(false);
    empty_ghost->on_mouse_click = empty_ghost_click;

    CardGroup *group = add_card_group(Vertical); 
    empty_ghost->link = group;   
    group->set_xy(calc_col_x(col), calc_row_y(2)); 
    group->name = string("Col") + to_string(col);

    const int FACIL = 0;
    const int DIFICIL = 2;
    vector<int> cols_values;
    if (save_data.dificult == DIFICIL) {
        cols_values = {3, 3, 3, 3, 4, 4, 4};
    } else {
        if (save_data.dificult == FACIL) {
            cols_values = {2, 2, 2, 2, 3, 3, 3, 3};
        } else {
            cols_values = {3, 3, 3, 3, 4, 4, 4, 4};
        }
    }
    
    for (int i = 0; i < cols_values[col-1]; i++) {
        Card *c = group->add_card(new PacienciaCard(this, pop_baralho(), 0, 0));
        c->set_card_face(FACE_DOWN);
        c->set_enabled(false);
    }    
    const int cartas_abertas = 1;
    for (int i = 0; i < cartas_abertas; i++) {
        group->add_card(new PacienciaCard(this, pop_baralho(), 0, 0)); 
    }    
    cols.push_back(group);
    return group;   
}

//-----------------------------------------------------------------------------
string Paciencia::baralho_str(void) const {
    string result("[");
    for(size_t i = 0; i < baralho.size(); i++) {
        result.append(to_string(baralho[i]));
        if(i < baralho.size()-1) {
            result.append(",");
        }
    }
    result.append("]");
    return result;
}

//-----------------------------------------------------------------------------
string Paciencia::cards_to_string(const Cards &cards) const {
    string result("[");
    for(size_t i = 0; i < cards.size(); i++) {
        result.append(to_string(cards[i]->get_card_id()));
        if(i < cards.size()-1) {
            result.append(",");
        }
    }
    result.append("]");
    return result;
}

//-----------------------------------------------------------------------------
void Paciencia::send_new_game(void) {
    if(login_hash != "") {
        Headers headers;
        headers["hash"] = login_hash;
        headers["seed"] = to_string(save_data.seed);
        headers["dificult"] = to_string(save_data.dificult);
        headers["tipo"] = TIPO_NEW_GAME;
        headers["game"] = GAME_PACIENCIA;
        string body = baralho_str();
        request("game.php", headers, body, 
            [](App *app, long response_code, string response) {
                if(response_code!=201) {
                    string msg("falha ao acessar servidor: ");
                    msg.append(to_string(response_code));
                    app->show_alert(msg);
                } 
            }
        );
    }    
}

//-----------------------------------------------------------------------------
void Paciencia::send_pega_monte(void) {
    if(login_hash != "") {
        Headers headers;
        headers["hash"] = login_hash;
        headers["seed"] = to_string(save_data.seed);
        headers["dificult"] = to_string(save_data.dificult);
        headers["tipo"] = TIPO_PEGAMONTE;
        headers["game"] = GAME_PACIENCIA;
        request("game.php", headers, "", 
            [](App *app, long response_code, string response) {
                if(response_code!=201) {
                    string msg("falha ao acessar servidor: ");
                    msg.append(to_string(response_code));
                    app->show_alert(msg);
                } 
            }
        );
    }      
}

//-----------------------------------------------------------------------------
void Paciencia::send_move(const CardGroup *source, const Cards &selecteds, const CardGroup *dest) {
    move_count++;    
    if(login_hash != "") {
        Headers headers;
        headers["hash"] = login_hash;
        headers["seed"] = to_string(save_data.seed);
        headers["dificult"] = to_string(save_data.dificult);
        headers["tipo"] = TIPO_MOVE;
        headers["game"] = GAME_PACIENCIA;
        string body = 
          to_string(move_count) + "," + 
          source->name + "," + 
          dest->name + ":" + 
          cards_to_string(selecteds)
        ;
        request("game.php", headers, body, 
            [](App *app, long response_code, string response) {
                if(response_code!=201) {
                    string msg("falha ao acessar servidor: ");
                    msg.append(to_string(response_code));
                    app->show_alert(msg);
                } 
            }
        );
    }    
}

//-----------------------------------------------------------------------------
void Paciencia::send_terminou(void) {
    if(login_hash != "") {
        Headers headers;
        headers["hash"] = login_hash;
        headers["seed"] = to_string(save_data.seed);
        headers["dificult"] = to_string(save_data.dificult);
        headers["tipo"] = TIPO_TERMINOU;
        headers["game"] = GAME_PACIENCIA;
        request("game.php", headers, to_string(score->tag),
            [](App *app, long response_code, string response) {
                if(response_code!=201) {
                    string msg("falha ao acessar servidor: ");
                    msg.append(to_string(response_code));
                    app->show_alert(msg);
                } 
            }
        );
    }      
}

//-----------------------------------------------------------------------------
//- PacienciaCard -------------------------------------------------------------
//-----------------------------------------------------------------------------
PacienciaCard::PacienciaCard(Paciencia *app, int card_id, int x, int y) 
    : Card(app, card_id, x, y) {

}

//-----------------------------------------------------------------------------
void PacienciaCard::before_select(bool &can_select) { 
    
    can_select = true;
    Paciencia *paciencia = dynamic_cast<Paciencia*>(app);
    CardGroup *this_group = dynamic_cast<CardGroup*>(owner);
    if(paciencia && this_group) {

        if(this->get_selected()) {
            Cards this_cards = this_group->get_cards();
            for(auto card : this_cards) {
                if(card != this) {
                    card->set_selected(false);
                }                    
            }
            can_select = true;
            return;
        }
        
        Cards this_cards = this_group->get_cards();
        CardsI it = find(this_cards.begin(), this_cards.end(), this);
        int index = it - this_cards.begin();
        int count = this_cards.size();

        if(index < (count-1)) {
            Cards temp_cards;            
            for(int i = index; i < (count-1); i++) {
                int s1 = this_cards[i]->seq();
                int s2 = this_cards[i+1]->seq(); 
                Naipe n1 = this_cards[i]->get_naipe();
                Naipe n2 = this_cards[i+1]->get_naipe();
                if((s1-1 != s2) || (n1 != n2)) {
                    can_select = false;
                    return;                
                }
                temp_cards.push_back(this_cards[i]);
                temp_cards.push_back(this_cards[i+1]);
            }
            if(!temp_cards.empty()) {
                for(auto card : temp_cards) {
                    if(card != this) {
                        card->set_selected(true);
                    }                    
                }
                for(auto col : paciencia->cols) {
                    if(col != this_group) {
                        Cards selecteds = col->get_selecteds();
                        for(auto card : selecteds) {
                            card->set_selected(false);
                        }
                    }
                }
                can_select = true;
                return; 
            }
        } 

        for(auto col : paciencia->cols) {
            if(col != this_group) {
                Cards selecteds = col->get_selecteds();
                for(auto card : selecteds) {
                    if(card->seq()+1 == seq()) {

                        col->move_cards(selecteds, this_group, -1, false);
                        paciencia->send_move(col, selecteds, this_group);                        

                        for(auto card : selecteds) {
                            card->set_selected(false);
                        }

                        Cards cards = col->get_cards();
                        if(!cards.empty()) {
                            Card *c = *cards.rbegin();
                            if(!c->get_enabled()) {
                                c->set_card_face(FACE_UP);
                                c->set_enabled(true);
                            }
                        }


                        can_select = false;
                        return;

                    } else {
                        for(auto card : selecteds) {
                            card->set_selected(false);
                        }
                        can_select = true;
                        return;
                    }
                }
            }
        }
    }
    
}

} // namespace SDLCards
