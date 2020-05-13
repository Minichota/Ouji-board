#include "traces.hpp"
#include "temp.hpp"
#include "instance-view.hpp"

enum Selection
{
	TRACE = 0,
	TEMP,
	VIEW,
	SELECTION_COUNT
};

class DebugManager
{
	public:
	DebugManager();
	~DebugManager();

	void handle_event(const SDL_Event& event);
	void render();

	private:
	// represents which one is currently being worked with
	Selection active;
	// represents which one is currently being moved
	Selection moving;

	// a toggle boolean of visibility outside of debug mode
	bool visible[SELECTION_COUNT];

	// all the rects of each debug window
	Ivec debug_positions[SELECTION_COUNT];
	SDL_Rect debug_sizes[SELECTION_COUNT];
};

