#include <nanogui\nanogui.h>

#include "App.h"

int main() {
	nanogui::init();

	nanogui::ref<AStarApp> app = new AStarApp;
	app->setVisible(true);

	nanogui::mainloop();

	nanogui::shutdown();
	exit(EXIT_SUCCESS);
}