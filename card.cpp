#include "card.hpp"
#include "app.hpp"

namespace SDLCards {

//-----------------------------------------------------------------------------
Card::Card(App *app, int card_id, int x, int y) 
     :Texture(app, determine_file_name(card_id), x, y) {
    
    void (*event_click)(Render*) = [](Render *r) {
        Card *c = dynamic_cast<Card*>(r);
        if(c) {
            if(c->get_enabled()) {
                bool can_select;            
                c->before_select(can_select);
                if(can_select) {
                    c->set_selected(!c->get_selected());
                }
            }
        }
    };
    this->card_id = card_id;
    if(card_id) {
        card_face = FACE_UP;
    } else {
        card_face = FACE_DOWN;
    }
    on_mouse_click = event_click;
    set_animated(false);
    set_enabled(true);
    set_selected(false);    
}

//-----------------------------------------------------------------------------
string Card::determine_file_name(int card_id) {
    char s[32];
    sprintf(s, "%03d", card_id);
    return "./img/100x150/" + string(s) + ".png";
}

//-----------------------------------------------------------------------------
int Card::rand_card_id(void) {
    return (rand()%13+1)*10 + (rand()%4+1);
}

//-----------------------------------------------------------------------------
void Card::set_animated(bool animated) {
    this->animated = animated;
}

//-----------------------------------------------------------------------------
bool Card::get_animated(void) const {
    return animated;
}

//-----------------------------------------------------------------------------
void Card::set_card_face(CardFace card_face) {
    if(get_card_id()) {
        if(card_face == FACE_DOWN) {
            change_image(determine_file_name(0));
        } else {
            change_image(determine_file_name(get_card_id()));
        }        
        this->card_face = card_face;
    }    
}

//-----------------------------------------------------------------------------
CardFace Card::get_card_face(void) const {
    return card_face;
}


//-----------------------------------------------------------------------------
void Card::set_enabled(bool enabled) {
    this->enabled = enabled;
}

//-----------------------------------------------------------------------------
bool Card::get_enabled(void) const {
    return enabled;
}

//-----------------------------------------------------------------------------
void Card::set_selected(bool selected) {
    this->selected = selected;
    if(selected) {
        set_color(0xFF, 0xFF, 0x77);
    } else {
        set_color(0xFF, 0xFF, 0xFF);
    }
}

//-----------------------------------------------------------------------------
bool Card::get_selected(void) const {
    return selected;
}

//-----------------------------------------------------------------------------
int Card::get_card_id(void) const {
    return card_id;
}

//-----------------------------------------------------------------------------
void Card::set_card_id(int card_id) {
    change_image(determine_file_name(card_id));
    set_selected(get_selected());
    this->card_id = card_id;
    if(card_id) {
        card_face = FACE_UP;
    } else {
        card_face = FACE_DOWN;
    }    
}

//-----------------------------------------------------------------------------
void Card::set_xy_animate(int x, int y) {
    if(animated) {
        ThreadMove *move = new ThreadMove(this, 250, x, y);
        move->execute(); 
        ThreadRotate360 *rotate = new ThreadRotate360(this, 250);
        rotate->execute();
        animated = false;
    } else {
        set_xy(x, y);
    }
}

//-----------------------------------------------------------------------------
void Card::set_xy(int x, int y) {
    if (!app->is_animate()) {
        Render::set_xy(x, y);
    } else {
        app->add_animate(this, x, y);
    }  
}

//-----------------------------------------------------------------------------
void Card::before_select(bool &can_select) {
    can_select = true;
}

//-----------------------------------------------------------------------------
bool Card::ouros(void) const {
    return naipe(OUROS);
}

//-----------------------------------------------------------------------------
bool Card::espadas(void) const {
    return naipe(ESPADAS);
}

//-----------------------------------------------------------------------------
bool Card::copas(void) const {
    return naipe(COPAS);
}

//-----------------------------------------------------------------------------
bool Card::paus(void) const {
    return naipe(PAUS);
}

//-----------------------------------------------------------------------------
bool Card::naipe(Naipe naipe) const {
    int id = get_card_id();
    if(id) {
        return (id%10)==naipe;
    } else {
        return false;
    }
}

//-----------------------------------------------------------------------------
Naipe Card::get_naipe(void) const {
    switch(get_card_id()%10) {
        case 1: return OUROS;
        case 2: return ESPADAS;
        case 3: return COPAS;
        case 4: return PAUS;
        default: return NONE;
    }
}

//-----------------------------------------------------------------------------
int Card::seq(void) const {
    return get_card_id()/10;
}

//-----------------------------------------------------------------------------
//- CardGroup -----------------------------------------------------------------
//-----------------------------------------------------------------------------
CardGroup::CardGroup(App *app) 
    : Grid(app, 0, 0, 0, 0, 0, 0, 0, 0, SDL_Color()) {
    this->direction = Vertical;
}
//-----------------------------------------------------------------------------
CardGroup::CardGroup(App *app, CardGroupDirection direction) 
    : Grid(app, 0, 0, 18, 24, 0, 0, 0, 0, SDL_Color()) {
    this->direction = direction;
}

//-----------------------------------------------------------------------------
void CardGroup::inc_col_row(int &col, int &row, SDL_Rect &rect) {
    switch(direction) {
        case Vertical: {
            col = 0;
            row = get_renders().size();
            break;
        }
        case Horizontal: {
            row = 0;
            col = get_renders().size();
            break;
        }
    }
    get_cell_rect(col, row, rect);
}

//-----------------------------------------------------------------------------
Card *CardGroup::add_card(int card_id) {
    int col, row;
    SDL_Rect rect;
    inc_col_row(col, row, rect);
    Card *card = new Card(app, card_id, rect.x, rect.y);
    return add(card, col, row);    
}

//-----------------------------------------------------------------------------
Card *CardGroup::add_card(Card *card) {
    int col, row;
    SDL_Rect rect;
    inc_col_row(col, row, rect);    
    card->set_xy(rect.x, rect.y);    
    return add(card, col, row);
}

//-----------------------------------------------------------------------------
Card *CardGroup::add(Card *card, int col, int row) {
    renders.push_back(card);
    map_renders[col][row] = card;
    card->owner = this; 
    return card;   
}

//-----------------------------------------------------------------------------
Card *CardGroup::remove_card(int card_id)  {
    Card *result = nullptr;
    for (auto &i : renders) {
        Card *c = dynamic_cast<Card*>(i);
        if(c) {
            if(c->get_card_id() == card_id) {
                result = c;
                break;
            }
        }
    }
    return remove_card(result);   
}

//-----------------------------------------------------------------------------
Card *CardGroup::remove_card(Card *card) {
    if(!card) {
        return nullptr;
    }
    int col, row;
    get_render_cell(card, col, row);
    remove_render(card);
    
    Cards cards = get_cards();

    switch(direction) {
        case Vertical: {
            for(int i = row; i < cards.size(); i++) {
                SDL_Rect rect;
                get_cell_rect(col, i, rect);
                Card *c = cards[i];
                if(c) {
                    c->set_xy(rect.x, rect.y);
                    map_renders[col][i] = c;
                    map_renders[col][i+1] = nullptr;
                }
            }
            break;
        }
        case Horizontal: {
            for(int i = col; i < cards.size(); i++) {
                SDL_Rect rect;
                get_cell_rect(i, row, rect);
                Card *c = cards[i];
                if(c) {
                    c->set_xy(rect.x, rect.y);
                    map_renders[i][row] = c;
                    map_renders[i+1][row] = nullptr;
                }
            }
            break;
        }
    }
    return card;
}

//-----------------------------------------------------------------------------
Cards CardGroup::get_cards(void) const {
    Cards cards;
    for (auto &i : renders) {
        Card *c = dynamic_cast<Card*>(i);
        if(c) {
            cards.push_back(c);
        }
    }
    return cards;
}

//-----------------------------------------------------------------------------
Cards CardGroup::get_selecteds(bool selecteds) const {
    Cards cards;
    for (auto &i : renders) {
        Card *c = dynamic_cast<Card*>(i);
        if(c) {
            if(c->get_selected() == selecteds) {
                cards.push_back(c);
            }            
        }
    }
    return cards;
}

//-----------------------------------------------------------------------------
Card *CardGroup::insert_card(int card_id, int index) {    
    Card *card = new Card(app, card_id, 0, 0);    
    return insert_card(card, index);
}

//-----------------------------------------------------------------------------
Card *CardGroup::insert_card(Card *card, int index) {
    int col = direction==Vertical ? 0 : index;
    int row = direction==Horizontal ? 0 : index;
    SDL_Rect rect;
    get_cell_rect(col, row, rect);
    card->set_xy(rect.x, rect.y);
    renders.insert(renders.begin() + index, card);
    map_renders[col][row] = card;

    Cards cards = get_cards();
    for(int i = index; i < cards.size(); i++) {
        col = direction==Vertical ? 0 : i;
        row = direction==Horizontal ? 0 : i;
        map_renders[col][row] = cards[i];
        get_cell_rect(col, row, rect);
        cards[i]->set_xy(rect.x, rect.y);
    }    
    card->owner = this; 
    return card;   
}

//-----------------------------------------------------------------------------
void CardGroup::select_all(bool select) {
    for (auto &i : renders) {
        Card *c = dynamic_cast<Card*>(i);
        if(c) {
            c->set_selected(select);
        }
    }
}
//-----------------------------------------------------------------------------
void CardGroup::move_cards(const Cards &cards, CardGroup *target_group, int index, bool reverse) {    
    if(index==-1) {
        index = target_group->get_cards().size();
    }    
    app->begin_animate();
    if(!reverse) {
        for (Cards::const_iterator i = cards.begin(); i != cards.end(); i++) {
            remove_card(*i);
            (*i)->set_animated(true);
            target_group->insert_card(*i, index++);        
        }
    } else {
        for (Cards::const_reverse_iterator i = cards.rbegin(); i != cards.rend(); i++) {
            remove_card(*i);
            (*i)->set_animated(true);
            target_group->insert_card(*i, index++);        
        }
    }
    app->end_animate();  
}


//-----------------------------------------------------------------------------
//- ThreadRotate360 -----------------------------------------------------------
//-----------------------------------------------------------------------------
ThreadRotate360::ThreadRotate360(const Cards &cards, int duration_miliseconds) 
    : Thread("ThreadRotate360", true) {
    this->cards = new Cards(cards);
    this->duration_miliseconds = duration_miliseconds;
}
//-----------------------------------------------------------------------------
ThreadRotate360::ThreadRotate360(Card *card, int duration_miliseconds)
    : Thread("ThreadRotate360", true) {
    this->cards = new Cards();
    this->cards->push_back(card);
    this->duration_miliseconds = duration_miliseconds;
}

//-----------------------------------------------------------------------------
ThreadRotate360::~ThreadRotate360(void) {
    delete cards;
}

//-----------------------------------------------------------------------------
int ThreadRotate360::on_execute(void) {
    const int delay = 10;
    const double final_rotate = 360;
    const double power = 2;
    int steps = ceil((double)duration_miliseconds/(double)delay);
    int mid_steps = steps/2+steps%2;
    if(!steps) {
        return 0;
    }

    vector<double>progress;
    double value = 0;
    for(int i = 0; i < mid_steps; i++) {
        value += pow(i+1, power);
        progress.push_back(value);
    }
    for(int i = steps-1; i >= mid_steps; i--) {
        value += pow((i-mid_steps)+1, power);
        progress.push_back(value);
    }    

    for(int i = 0; i < steps; i++) {
        double rotate = final_rotate/value*progress[i];
        SDL_Delay(delay);
        for(int j = 0; j < cards->size(); j++) {                    
            (*cards)[j]->rotate(rotate);            
        }        
    }
    for(int i = 0; i < cards->size(); i++) {                    
        (*cards)[i]->rotate(0);            
    } 

        
    return 0;
}

//-----------------------------------------------------------------------------
//- ThreadMove ----------------------------------------------------------------
//-----------------------------------------------------------------------------
ThreadMove::ThreadMove(Card *card, int duration_miliseconds, int x, int y) 
    : Thread("ThreadMove", true) {
    this->card = card;
    this->duration_miliseconds = duration_miliseconds;
    SDL_Rect rect;
    card->get_rect(rect);
    this->xi = rect.x;
    this->yi = rect.y;
    this->xf = x;
    this->yf = y;
}

//-----------------------------------------------------------------------------
int ThreadMove::on_execute(void) {
    const int delay = 10;
    const double x_e = xf - xi;
    const double y_e = yf - yi;
    const double power = 2;
    int steps = ceil((double)duration_miliseconds/(double)delay);
    int mid_steps = steps/2+steps%2;
    if(!steps) {
        return 0;
    }

    vector<double>progress;
    double value = 0;
    for(int i = 0; i < mid_steps; i++) {
        value += pow(i+1, power);
        progress.push_back(value);
    }
    for(int i = steps-1; i >= mid_steps; i--) {
        value += pow((i-mid_steps)+1, power);
        progress.push_back(value);
    }    

    for(int i = 0; i < steps; i++) {
        double x = x_e/value*progress[i];
        double y = y_e/value*progress[i];
        SDL_Delay(delay);
        card->set_xy(ceil(x)+xi, ceil(y)+yi);      
    }

        
    return 0;
}

//-----------------------------------------------------------------------------
void ThreadMove::on_terminate(void) {
    card->set_xy(xf, yf);
}

} // namespace SDLCards
