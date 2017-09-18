#include <nanogui\nanogui.h>

#include "App.h"

int main() {
	nanogui::init();

	AStarApp app;
	app.setVisible(true);

	nanogui::mainloop();

	nanogui::shutdown();
	exit(EXIT_SUCCESS);
}