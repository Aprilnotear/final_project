#include "../nclgl/window.h"
#include "Renderer.h"
#include "Deferred.h"

int main() {
	Window w("Project! Switch renderer in main.cpp to choose Forward+ or Deferred rendering", 1920, 1080, false);
	if (!w.HasInitialised()) {
		return -1;
	}

	Renderer renderer(w);       //Forward+ renderer
	//Deferred renderer(w);       //Deferred renderer
	if (!renderer.HasInitialised()) {
		return -1;
	}

	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {
		renderer.UpdateScene(w.GetTimer()->GetTimeDeltaSeconds());
		renderer.RenderScene();
		//cout << 1.0f / w.GetTimer()->GetTimeDeltaSeconds() << endl;
		renderer.SwapBuffers();
	}

	return 0;
}