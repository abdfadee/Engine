#pragma once
#include <functional>

using namespace std;


template <typename T>
class Listener {
protected:
	vector<std::function<T>> listeners;
public:
	void addListener(std::function<T> listener) {
		listeners.push_back(listener);
	}
};


class MouseListener : public Listener<void(double xpos, double ypos)> {
public:
	void notifyListeners(double xpos, double ypos) {
		for (size_t i = 0; i < listeners.size(); ++i)
			listeners[i](xpos, ypos);
	}
};


class ScrollListener : public Listener<void(double, double)> {

public:
	void notifyListeners(double xoffset, double yoffset) {
		for (size_t i = 0; i < listeners.size(); ++i)
			listeners[i](xoffset, yoffset);
	}
};


class KeyListener : public Listener<void(int,int,int,int)> {
public:
	void notifyListeners(int key, int scancode, int action, int mods) {
		for (size_t i = 0; i < listeners.size(); ++i)
			listeners[i](key,scancode,action,mods);
	}
};