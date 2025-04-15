#pragma once
#include <map>
#include <string>
#include "texture/Texture.h"

using namespace std;


class AssetManager {
	inline static map<string, unsigned int> textures;


public:

	static int getTexture(string path) {
		auto t = textures.find(path);
		if (t == textures.end())
			return -1;
		else
			return t->second;
	}


	static void addTexture(string path, unsigned int textureId) {
		textures.insert({path,textureId});
	}

};