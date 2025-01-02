#pragma once


//#include <vector>
#include "IGUIWindow.h"
//#include "../../../core/loader/Loader.h"


class AddCodeWindow : public IGUIWindow
{

public:
    AddCodeWindow(const std::string& name);

    void Draw() override;
};