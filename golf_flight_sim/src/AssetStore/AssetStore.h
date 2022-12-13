#pragma once

#include <map>
#include <string>
#include <SDL.h>
#include <SDL_ttf.h>

class AssetStore {
private:
  //std::map<std::string, SDL_Texture *> textures;
  std::map<std::string, TTF_Font *> fonts;

public:
  AssetStore();
  ~AssetStore();

  void clear_assets();

  //void add_texture(SDL_Renderer *renderer, const std::string &asset_id,
  //                const std::string &file_path);
  //SDL_Texture *get_texture(const std::string &asset_id);

  void add_font(const std::string &asset_id, const std::string &file_path,
               int font_size);
  TTF_Font *get_font(const std::string &asset_id);
};
