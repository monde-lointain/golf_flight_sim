#include "AssetStore.h"
#include <iostream>

AssetStore::AssetStore() {
  std::cout << "Asset store constructor called."
            << "\n";
}

AssetStore::~AssetStore() {
  clear_assets();
  std::cout << "Asset store destructor called."
            << "\n";
}

void AssetStore::clear_assets() {

  //for (auto &texture : textures) {
  //  SDL_DestroyTexture(texture.second);
  //}
  //textures.clear();

  for (auto &font : fonts) {
    TTF_CloseFont(font.second);
  }
  fonts.clear();

}

//void AssetStore::add_texture(SDL_Renderer *renderer,
//                             const std::string &asset_id,
//                             const std::string &file_path) {
//
//  SDL_Surface *surface = IMG_Load(file_path.c_str());
//  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
//  SDL_FreeSurface(surface);
//
//  // Add the texture to the map
//  textures.emplace(asset_id, texture);
//
//  std::cout << "Added texture " << asset_id << " to the asset store."
//            << "\n";
//
//}
//
//SDL_Texture *AssetStore::get_texture(const std::string &asset_id) {
//  return textures[asset_id];
//}

void AssetStore::add_font(const std::string &asset_id,
                          const std::string &file_path, int font_size) {

  TTF_Font *font = TTF_OpenFont(file_path.c_str(), font_size);

  if (font == nullptr) {
    std::cout << "Error opening font " << asset_id << ": " << TTF_GetError()
              << "\n";
  } else {
    fonts.emplace(asset_id, TTF_OpenFont(file_path.c_str(), font_size));
    std::cout << "Added font " << asset_id << " to the asset store."
              << "\n";
  }

}

TTF_Font *AssetStore::get_font(const std::string &asset_id) {
  return fonts[asset_id];
}
