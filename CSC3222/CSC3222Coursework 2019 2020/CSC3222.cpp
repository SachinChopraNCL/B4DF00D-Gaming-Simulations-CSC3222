#include "../../Common/Window.h"
#include "BadFoodGame.h"

#include "../../Common/Vector3.h"
#include "../../Common/Vector4.h"
#include "../../Common/Matrix4.h"
using namespace NCL;
using namespace CSC3222;

int main() {
	Window*w = Window::CreateGameWindow("CSC3222 - B4DF00D!", 1120, 768);

	if (!w->HasInitialised()) {
		return -1;
	}

	Matrix4 test = Matrix4::Orthographic(-1, 1, 800, 0, 0, 600);

	BadFoodGame* theGame = new BadFoodGame();
	w->GetTimer()->GetTimeDeltaSeconds(); //Reset so we don't get a big dt in the first frame!
	while (w->UpdateWindow() && !Window::GetKeyboard()->KeyDown(KeyboardKeys::ESCAPE)) {
		float time = w->GetTimer()->GetTimeDeltaSeconds();

		theGame->Update(time);

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::PRIOR)) {
			w->ShowConsole(true);
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NEXT)) {
			w->ShowConsole(false);
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::HOME)) {
			w->SetFullScreen(true);
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::END)) {
			w->SetFullScreen(false);
		}

		w->SetTitle(std::to_string(time));
	}

	delete theGame;

	Window::DestroyGameWindow();
}