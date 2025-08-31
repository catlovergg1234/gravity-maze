#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_mixer.h"
#include <string>
#include <iomanip>
#include <sstream>

void draw_map(SDL_Renderer* renderer, int map[20][20]) {
    int enum_x;
    int enum_y;
    Uint8 r, g, b, a;
    SDL_Rect rect;
    rect.w = rect.h = 30;
    SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);
    for (int y = 0; y < 20; ++y) {
        for (int x = 0; x < 20; ++x) {
            if (map[y][x] == 1) {
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                rect.x = 30 * x;
                rect.y = 30 * y;
                SDL_RenderFillRect(renderer, &rect);
            }
            if (map[y][x] == 2) {
                SDL_SetRenderDrawColor(renderer, 255, 69, 0, 255);
                rect.x = 30 * x;
                rect.y = 30 * y;
                SDL_RenderFillRect(renderer, &rect);
            }        
        }
    }
    
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

SDL_Point get_center(SDL_Rect rect) {
    return {rect.x + (rect.w / 2), rect.y + (rect.h / 2)};
}

/*
This function sets a rect's center. Mainly used for centering. Mostly used with get_center
*/
void set_center(SDL_Rect* rect, SDL_Point center) {
    rect->x = center.x - (rect->w / 2);
    rect->y = center.y - (rect->h / 2);
}


/*
The struct of a button. It is literally a rect but you get my point.
*/
typedef struct {
    SDL_Rect rect;
    SDL_Color color;
    SDL_Color text_color;
    TTF_Font* font;
    char* text;
} Button;

/*
Draw a button, all color + position info will come from the button.

\param renderer The renderer for drawing.
\param button The button that is drawn.

\returns 0 on success, -1 on failure.
*/
int draw(SDL_Renderer* renderer, Button button) {
    int result = 0;
    SDL_Surface* text = TTF_RenderUTF8_Blended(button.font, button.text, button.text_color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, text);
    SDL_Rect rect = {0, 0, text->w, text->h};
    set_center(&rect, get_center(button.rect));
    result |= text == NULL ? 1 : 0;
    result |= texture == NULL ? 1 : 0;
    result |= SDL_SetRenderDrawColor(renderer, button.color.r, button.color.g, button.color.b, button.color.a);
    result |= SDL_RenderFillRect(renderer, &(button.rect));
    result |= SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_FreeSurface(text);
    SDL_DestroyTexture(texture);
    return result;
}

/*
Check if a button is hovered.
This is mostly a helper function for is_clicked but it can be used for special effects like the button getting bigger on hover. 

\param button The target button.

\returns 0 if not hovered, 1 if is hovered.
*/
int is_hovered(Button button) {
    int x, y;
    SDL_GetMouseState(&x, &y);
    SDL_Point m_pos = {x, y};
    return SDL_PointInRect(&m_pos, &(button.rect));
} 

/*
Check if a button is clicked. 

\param button The target button.

\returns 0 if not clicked, 1 if is clicked.
*/
int is_clicked(Button button) {
    Uint32 state = SDL_GetMouseState(NULL, NULL);
    return is_hovered(button) && (state & SDL_BUTTON(SDL_BUTTON_LEFT));
}


int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Couldn't init SDL: %s", SDL_GetError());
        return 1;
    }
    if (TTF_Init() != 0) {
        SDL_Log("Couldn't init TTF: %s", TTF_GetError());
        return 1;
    }
    if (Mix_Init(MIX_INIT_OGG) == 0) {
        SDL_Log("Mix init error: %s", Mix_GetError());
        return 1;
    }
    if (IMG_Init(IMG_INIT_PNG) == 0) {
        SDL_Log("IMG Init error: %s", IMG_GetError());
        return 1;
    }
    SDL_Window* window = SDL_CreateWindow(
        "Maze", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        600, 
        600,
        SDL_WINDOW_SHOWN
    );
    /*
    This map is generated with ChatGPT.
    */
    int map[20][20] = {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,1},
        {1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1,1,0,1},
        {1,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,1,0,1},
        {1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,1,0,1,0,1},
        {1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,1,0,0,0,1},
        {1,1,1,1,1,0,1,1,1,0,1,1,1,1,0,1,1,1,1,1},
        {1,0,0,0,1,0,0,0,1,0,0,0,0,1,0,0,0,0,1,1},
        {1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,0,0,1},
        {1,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,1,1},
        {1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,0,1},
        {1,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,1,1,0,1},
        {1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,0,0,1},
        {1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,1,1},
        {1,0,1,1,1,0,1,1,1,1,1,0,1,0,1,0,1,0,0,1},
        {1,0,0,0,1,0,0,0,0,0,1,0,1,0,1,0,0,0,1,1},
        {1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,0,0,1},
        {1,0,0,0,0,0,0,0,1,0,0,0,0,0,1,2,0,0,1,1},
        {1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,0,0,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        SDL_Log("Couldn't open audio: %s", Mix_GetError());
        return 1;
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font* comic_sans = TTF_OpenFont("fonts/comic.ttf", 50);
    TTF_Font* comic_sans_small = TTF_OpenFont("fonts/comic.ttf", 30);
    TTF_Font* button_text_font = TTF_OpenFont("fonts/comic.ttf", 40);
    TTF_Font* button_text_small =  TTF_OpenFont("fonts/comic.ttf", 35);
    Button play = {{250, 240, 100, 50}, {255, 0, 0}, {0, 0, 0}, button_text_font, "Play"};
    Button main_menu = {{50, 200, 200, 50}, {0, 255, 0}, {0, 0, 0}, button_text_small, "Main Menu"};
    Button exit = {{400, 200, 100, 50}, {255, 0, 0}, {0, 0, 0}, button_text_small, "Exit"};
    SDL_Surface* you_win = TTF_RenderUTF8_Blended(comic_sans, "You Win!", {0, 0, 0});
    SDL_Surface* trophy = IMG_Load("assets/trophy.png");
    SDL_Surface* title = TTF_RenderUTF8_Blended(comic_sans, "Gravity Maze", {0, 0, 0});
    SDL_Rect title_dst = {0, 0, title->w, title->h};
    set_center(&title_dst, {300, 80});
    SDL_Texture* title_texture = SDL_CreateTextureFromSurface(renderer, title);
    SDL_FreeSurface(title);
    SDL_Texture* trophy_text = SDL_CreateTextureFromSurface(renderer, trophy);
    SDL_Surface* spritesheet = IMG_Load("assets/spritesheet.png");
    SDL_Texture* character = SDL_CreateTextureFromSurface(renderer, spritesheet);
    SDL_FreeSurface(spritesheet);
    int win_width = you_win->w;
    int win_height = you_win->h;
    int trophy_height = trophy->h;
    int trophy_width = trophy->w;
    SDL_Rect trophy_rect = {0, 0, trophy_width, trophy_width};
    SDL_Rect win_rect = {0, 0, win_width, win_height};
    set_center(&win_rect, {300, 75});
    set_center(&trophy_rect, {300, 450});
    SDL_Texture* win_texture = SDL_CreateTextureFromSurface(renderer, you_win);
    SDL_FreeSurface(you_win);
    SDL_FreeSurface(trophy);
    bool run = 1;
    int player_x = 30;
    int player_y = 30;
    SDL_Event event;
    const Uint8* state;
    int new_x;
    int new_y;
    bool had_intersection_y, had_intersection_x;
    SDL_Rect player_rect_x, player_rect_y, player_rect;
    SDL_Rect wall, old;
    SDL_Rect character_srcrect;
    Mix_Music* background = Mix_LoadMUS("assets/background.ogg");
    Mix_Chunk* hooray = Mix_LoadWAV("assets/hooray.ogg");
    while (run) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    SDL_DestroyTexture(win_texture);
                    SDL_DestroyTexture(trophy_text);
                    SDL_DestroyTexture(character);
                    SDL_DestroyTexture(title_texture);
                    SDL_DestroyWindow(window);
                    SDL_DestroyRenderer(renderer);
                    Mix_HaltChannel(-1);
                    Mix_FreeMusic(background);
                    Mix_FreeChunk(hooray);
                    Mix_CloseAudio();
                    TTF_CloseFont(button_text_small);
                    TTF_CloseFont(comic_sans);
                    TTF_CloseFont(comic_sans_small);
                    TTF_CloseFont(button_text_font);
                    IMG_Quit();
                    Mix_Quit();
                    TTF_Quit();
                    SDL_Quit();
                    return 0;
            }
        }
        SDL_SetRenderDrawColor(renderer, 175, 238, 238, 255);
        SDL_RenderClear(renderer);
        draw(renderer, play);
        SDL_RenderCopy(renderer, title_texture, NULL, &title_dst);
        SDL_RenderPresent(renderer);
        if (is_clicked(play)) {
            Mix_PlayMusic(background, -1);
            Uint32 start = SDL_GetTicks();
            while (run) {
                while (SDL_PollEvent(&event)) {
                    switch (event.type) {
                        case SDL_QUIT:
                            SDL_DestroyTexture(win_texture);
                            SDL_DestroyTexture(trophy_text);
                            SDL_DestroyTexture(character);
                            SDL_DestroyTexture(title_texture);
                            SDL_DestroyWindow(window);
                            SDL_DestroyRenderer(renderer);
                            Mix_HaltChannel(-1);
                            Mix_HaltMusic();
                            Mix_FreeMusic(background);
                            Mix_FreeChunk(hooray);
                            Mix_CloseAudio();
                            TTF_CloseFont(comic_sans);
                            TTF_CloseFont(comic_sans_small);
                            TTF_CloseFont(button_text_font);
                            TTF_CloseFont(button_text_small);
                            IMG_Quit();
                            Mix_Quit();
                            TTF_Quit();
                            SDL_Quit();
                            return 0;
                    }
                }
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderClear(renderer);
                draw_map(renderer, map);
                state = SDL_GetKeyboardState(NULL);
                new_x = player_x;
                new_y = player_y;
                if (state[SDL_SCANCODE_UP]) {
                    new_y -= 1;
                }
                if (state[SDL_SCANCODE_DOWN]) {
                    new_y += 1;
                }
                if (state[SDL_SCANCODE_RIGHT]) {
                    new_x += 1;
                }
                if (state[SDL_SCANCODE_LEFT]) {
                    new_x -= 1;
                }
                player_rect_x = {new_x, player_y, 20, 20};
                player_rect_y = {player_x, new_y, 20, 20};
                had_intersection_x = had_intersection_y = false;
                for (int y = 0; y < 20; ++y) {
                    for (int x = 0; x < 20; ++x) {
                        if (map[y][x] == 1) {
                            wall = {x * 30, y * 30, 30, 30};
                            if (SDL_HasIntersection(&wall, &player_rect_x)) {
                                had_intersection_x = true;
                            }
                            if (SDL_HasIntersection(&wall, &player_rect_y)) {
                                had_intersection_y = true;
                            }
                        }
                        if (map[y][x] == 2) {
                            wall = {x * 30, y * 30, 30, 30};
                            player_rect = {new_x, new_y, 20, 20};
                            if (SDL_HasIntersection(&wall, &player_rect)) {
                                Mix_HaltMusic();
                                bool wait = true;
                                Uint32 end = SDL_GetTicks();
                                double time_taken = (end - start) / 1000.0;
                                std::ostringstream oss;
                                oss << "Time: " << std::fixed << std::setprecision(3) << time_taken << " seconds";
                                std::string time = oss.str();
                                SDL_Surface* time_surf = TTF_RenderUTF8_Blended(comic_sans_small, time.c_str(), {0, 0, 0});
                                SDL_Texture* time_texture = SDL_CreateTextureFromSurface(renderer, time_surf);
                                SDL_Rect time_rect = {0, 0, time_surf->w, time_surf->h};
                                set_center(&time_rect, {300, 300});
                                SDL_FreeSurface(time_surf);
                                Mix_PlayChannel(-1, hooray, 0);
                                while (wait) {
                                    while (SDL_PollEvent(&event)) {
                                        switch (event.type) {
                                            case SDL_QUIT: 
                                                SDL_DestroyTexture(win_texture);
                                                SDL_DestroyTexture(trophy_text);
                                                SDL_DestroyTexture(time_texture);
                                                SDL_DestroyTexture(character);
                                                SDL_DestroyTexture(title_texture);
                                                SDL_DestroyWindow(window);
                                                SDL_DestroyRenderer(renderer);
                                                Mix_HaltChannel(-1);
                                                Mix_FreeMusic(background);
                                                Mix_FreeChunk(hooray);
                                                TTF_CloseFont(comic_sans);
                                                TTF_CloseFont(comic_sans_small);
                                                TTF_CloseFont(button_text_font);
                                                TTF_CloseFont(button_text_small);
                                                Mix_CloseAudio();
                                                Mix_Quit();
                                                IMG_Quit();
                                                TTF_Quit();
                                                SDL_Quit();
                                                return 0;
                                        }
                                    }
                                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                                    SDL_RenderClear(renderer);
                                    draw(renderer, main_menu);
                                    draw(renderer, exit);
                                    if (is_clicked(exit)) {
                                        SDL_DestroyTexture(win_texture);
                                        SDL_DestroyTexture(trophy_text);
                                        SDL_DestroyTexture(time_texture);
                                        SDL_DestroyTexture(character);
                                        SDL_DestroyTexture(title_texture);
                                        SDL_DestroyWindow(window);
                                        SDL_DestroyRenderer(renderer);
                                        Mix_HaltChannel(-1);
                                        Mix_FreeMusic(background);
                                        Mix_FreeChunk(hooray);
                                        TTF_CloseFont(comic_sans);
                                        TTF_CloseFont(comic_sans_small);
                                        TTF_CloseFont(button_text_font);
                                        TTF_CloseFont(button_text_small);
                                        Mix_CloseAudio();
                                        Mix_Quit();
                                        IMG_Quit();
                                        TTF_Quit();
                                        SDL_Quit();
                                        return 0;
                                    }
                                    SDL_RenderCopy(renderer, trophy_text, NULL, &trophy_rect);
                                    SDL_RenderCopy(renderer, win_texture, NULL, &win_rect);
                                    SDL_RenderCopy(renderer, time_texture, NULL, &time_rect);
                                    SDL_RenderPresent(renderer);
                                }
                            }
                        }
                    }
                }
                if (!had_intersection_x) {
                    player_x = new_x;
                }
                if (!had_intersection_y) {
                    player_y = new_y;
                }
                player_rect = {player_x, player_y, 20, 20};
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                if (state[SDL_SCANCODE_UP] && !(state[SDL_SCANCODE_RIGHT]) && !(state[SDL_SCANCODE_DOWN]) && !(state[SDL_SCANCODE_LEFT])) {
                    character_srcrect = {20, 0, 20, 20};
                } else if (!(state[SDL_SCANCODE_UP]) && state[SDL_SCANCODE_RIGHT] && !(state[SDL_SCANCODE_DOWN]) && !(state[SDL_SCANCODE_LEFT])) {
                    character_srcrect = {40, 20, 20, 20};
                } else if (!(state[SDL_SCANCODE_UP]) && !(state[SDL_SCANCODE_RIGHT]) && state[SDL_SCANCODE_DOWN] && !(state[SDL_SCANCODE_LEFT])) {
                    character_srcrect = {20, 40, 20, 20};
                } else if (!(state[SDL_SCANCODE_UP]) && !(state[SDL_SCANCODE_RIGHT]) && !(state[SDL_SCANCODE_DOWN]) && state[SDL_SCANCODE_LEFT]) {
                    character_srcrect = {0, 20, 20, 20};
                } else if (!(state[SDL_SCANCODE_UP]) && !(state[SDL_SCANCODE_RIGHT]) && state[SDL_SCANCODE_DOWN] && state[SDL_SCANCODE_LEFT]) {
                    character_srcrect = {0, 40, 20, 20};
                } else if (!(state[SDL_SCANCODE_UP]) && state[SDL_SCANCODE_RIGHT] && state[SDL_SCANCODE_DOWN] && !(state[SDL_SCANCODE_LEFT])) {
                    character_srcrect = {40, 40, 20, 20};
                } else if (state[SDL_SCANCODE_UP] && state[SDL_SCANCODE_RIGHT] && !(state[SDL_SCANCODE_DOWN]) && !(state[SDL_SCANCODE_LEFT])) {
                    character_srcrect = {40, 0, 20, 20};
                } else if (state[SDL_SCANCODE_UP] && !(state[SDL_SCANCODE_RIGHT]) && !(state[SDL_SCANCODE_DOWN]) && state[SDL_SCANCODE_LEFT]) {
                    character_srcrect = {0, 0, 20, 20};
                } else {
                    character_srcrect = {20, 20, 20, 20};
                }
                SDL_RenderCopy(renderer, character, &character_srcrect, &player_rect);
                SDL_RenderPresent(renderer);
                SDL_Delay(10);
            }
        }
    }
}