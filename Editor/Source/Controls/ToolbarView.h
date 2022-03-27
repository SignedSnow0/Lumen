#pragma once
#include <Lumen.h>

#include "../Utils.h"

class ToolbarView
{
public:
	ToolbarView() = default;
	~ToolbarView() = default;

	void Render();

private:
	void CreateDialog();

	b8 mFileOpen{ false };
	FileExplorer mFileExplorer{};
};
