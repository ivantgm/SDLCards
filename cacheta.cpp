#include "cacheta.hpp"

namespace SDLCards {

//-----------------------------------------------------------------------------
Cacheta::Cacheta() : App("Cacheta 1.0", 800, 600) {

    CardGroup *group_1 = add_card_group(Horizontal);    
    group_1->set_xy(0, 100);

    for(int i = 1; i <= 13; i++) {
        for(int j = 1; j <=2; j++) {
            group_1->add_card(0);
        }
    }  

    CardGroup *group_2 = add_card_group(Horizontal);    
    group_2->set_xy(0, 300);


    Texture *btn1 = add_texture_text(
        "28 Days Later.ttf", 
        "MOVE PARA BAIXO", 
        10, 10, {0x00, 0x00, 0xFF, 0xFF}, 18
    );
    void (*btn1_click)(Render*) = [](Render *r) {
        App *app = r->app;
        CardGroup *group_1 = dynamic_cast<CardGroup*>(app->renders[0]);
        CardGroup *group_2 = dynamic_cast<CardGroup*>(app->renders[1]);
        group_1->move_cards(group_1->get_selecteds(), group_2, group_2->get_cards().size());
    };
    btn1->on_mouse_click = btn1_click;

    Texture *btn2 = add_texture_text(
        "28 Days Later.ttf", 
        "MOVE PARA CIMA", 
        180, 10, {0xFF, 0x00, 0x00, 0xFF}, 18
    );
    void (*btn2_click)(Render*) = [](Render *r) {
        App *app = r->app;
        CardGroup *group_1 = dynamic_cast<CardGroup*>(app->renders[1]);
        CardGroup *group_2 = dynamic_cast<CardGroup*>(app->renders[0]);
        Cards cards = group_1->get_selecteds();
        for (auto i: cards) {
            i->set_enabled(false);
            i->set_selected(false);
        }        
        group_1->move_cards(cards, group_2, 0);

    };
    btn2->on_mouse_click = btn2_click;


    Texture *btn3 = add_texture_text(
        "28 Days Later.ttf", 
        "vira cartas", 
        360, 10, {0xFF, 0xFF, 0x00, 0xFF}, 18
    );
    void (*btn3_click)(Render*) = [](Render *r) {
        App *app = r->app;
        CardGroup *group = dynamic_cast<CardGroup*>(app->renders[0]);
        for (auto i: group->get_selecteds()) {
            if(i->get_card_id()) {
                i->set_card_id(0);    
            } else {
                i->set_card_id(Card::rand_card_id());
            }
            i->set_selected(false);            
        }
    };
    btn3->on_mouse_click = btn3_click;    
    


}

//-----------------------------------------------------------------------------
Cacheta::~Cacheta() {

}

//-----------------------------------------------------------------------------
void Cacheta::poll_event(SDL_Event *e) {    
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
        case SDL_MOUSEBUTTONDOWN: {
            break;
        }
        case SDL_MOUSEWHEEL: {
            break;
        }
        case SDL_MOUSEMOTION: {
            break;
        }
    }
    App::poll_event(e);
}

} // namespace SDLCards
