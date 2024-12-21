#pragma once

#include <vector>
#include <atomic>
#include "IGUIWindow.h"


class SelectFileWindow : public IGUIWindow
{
    std::atomic<bool> m_atomicIsOpen{};

public:
    SelectFileWindow(const std::string& name);

	// override open file controlling functions with their atomic counterparts, as this window may be multithreaded
    void Open(bool open) override { m_atomicIsOpen.store(open); }
    bool IsOpen() const override { return m_atomicIsOpen.load(); }

    void Draw() override;
};