#ifndef __GAME_CARD__
#define __GAME_CARD__

#include "render.hpp"
#include "thread.hpp"

enum CardGroupDirection {
    Vertical,
    Horizontal
};

class Card: public Texture {
public:
    Card(App *app, int card_id, int x, int y);
public:    
    void set_animated(bool animated);
    bool get_animated(void) const;
    void set_enabled(bool enabled);
    bool get_enabled(void) const;
    void set_selected(bool selected);
    bool get_selected(void) const;
    int get_card_id(void) const;
    void set_card_id(int card_id);
    void set_xy_animate(int x, int y);
    void set_xy(int x, int y);
public:
    static int rand_card_id(void);
private:
    int card_id;
    bool animated;
    bool enabled;
    bool selected;
    static string determine_file_name(int card_id);
};

typedef vector<Card*> Cards;

class CardGroup: public Grid {
public:
    CardGroup(App *app, CardGroupDirection direction);
public:
    Card *add_card(int card_id);
    Card *add_card(Card *card);
    Card *remove_card(int card_id);
    Card *remove_card(Card *card);
    Card *insert_card(int card_id, int index);
    Card *insert_card(Card *card, int index);
    Cards get_cards(void) const;
    Cards get_selecteds(bool selecteds = true) const;
    void select_all(bool select = true);
    void move_cards(const Cards &cards, CardGroup *source_group, int index);
private:
    CardGroupDirection direction; 
    void inc_col_row(int &col, int &row, SDL_Rect &rect);
    Card *add(Card *card, int col, int row);
};

class ThreadRotate360: public Thread {
public:
    ThreadRotate360(const Cards &cards, int duration_miliseconds);
    ThreadRotate360(Card *card, int duration_miliseconds);
    ~ThreadRotate360(void);
protected:
    int on_execute(void);
private:
    Cards *cards; 
    int duration_miliseconds;   
};

class ThreadMove: public Thread {
public:
    ThreadMove(Card *card, int duration_miliseconds, int x, int y);
protected:
    int on_execute(void);
    void on_terminate(void);
private:
    Card *card; 
    int duration_miliseconds;   
    int xf, yf, xi, yi;
};

#endif