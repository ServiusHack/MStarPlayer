/*
  ==============================================================================

   Heavily based on JUCE's MultiDocumentPanel.

  ==============================================================================
*/

#include "MyMultiDocumentPanel.h"

void MyConstrainer::setSnapToGridEnabled(bool enable)
{
    snapToGrid = enable;
}

void MyConstrainer::setSnapToGridWidth(int width)
{
    gridWidth = width;
}

void MyConstrainer::setSnapToGridHeight(int height)
{
    gridHeight = height;
}

void MyConstrainer::checkBounds(Rectangle<int>& bounds, const Rectangle<int>& previousBounds,
    const Rectangle<int>& limits, bool isStretchingTop, bool isStretchingLeft, bool isStretchingBottom,
    bool isStretchingRight)
{
    if (snapToGrid)
    {
        if (isStretchingLeft || isStretchingRight)
        {
            bounds.setWidth(bounds.getWidth() / gridWidth * gridWidth);
        }
        else
        {
            bounds.setX(bounds.getX() / gridWidth * gridWidth);
        }
        if (isStretchingTop || isStretchingBottom)
        {
            bounds.setHeight(bounds.getHeight() / gridHeight * gridHeight);
        }
        else
        {
            bounds.setY(bounds.getY() / gridHeight * gridHeight);
        }
    }

    ComponentBoundsConstrainer::checkBounds(
        bounds, previousBounds, limits, isStretchingTop, isStretchingLeft, isStretchingBottom, isStretchingRight);
}

MyMultiDocumentPanelWindow::MyMultiDocumentPanelWindow(Colour backgroundColour)
    : DocumentWindow({}, backgroundColour, DocumentWindow::maximiseButton | DocumentWindow::closeButton, false)
{
}

MyMultiDocumentPanelWindow::~MyMultiDocumentPanelWindow() {}

//==============================================================================
void MyMultiDocumentPanelWindow::maximiseButtonPressed()
{
    if (auto* owner = getOwner())
        owner->setLayoutMode(MyMultiDocumentPanel::MaximisedWindowsWithTabs);
    else
        jassertfalse; // these windows are only designed to be used inside a MultiDocumentPanel!
}

void MyMultiDocumentPanelWindow::closeButtonPressed()
{
    if (auto* owner = getOwner())
        owner->closeDocument(getContentComponent(), true);
    else
        jassertfalse; // these windows are only designed to be used inside a MultiDocumentPanel!
}

void MyMultiDocumentPanelWindow::activeWindowStatusChanged()
{
    DocumentWindow::activeWindowStatusChanged();
    updateOrder();
}

void MyMultiDocumentPanelWindow::broughtToFront()
{
    DocumentWindow::broughtToFront();
    updateOrder();
}

void MyMultiDocumentPanelWindow::updateOrder()
{
    if (auto* owner = getOwner())
        owner->updateOrder();
}

MyMultiDocumentPanel* MyMultiDocumentPanelWindow::getOwner() const noexcept
{
    return findParentComponentOfClass<MyMultiDocumentPanel>();
}

//==============================================================================
struct MyMultiDocumentPanel::TabbedComponentInternal : public TabbedComponent
{
    TabbedComponentInternal()
        : TabbedComponent(TabbedButtonBar::TabsAtTop)
    {
    }

    void currentTabChanged(int, const String&)
    {
        if (auto* const owner = findParentComponentOfClass<MyMultiDocumentPanel>())
            owner->updateOrder();
    }
};

//==============================================================================
MyMultiDocumentPanel::MyMultiDocumentPanel()
{
    setOpaque(true);
}

MyMultiDocumentPanel::~MyMultiDocumentPanel()
{
    closeAllDocuments(false);
}

//==============================================================================
namespace MultiDocHelpers
{
static bool shouldDeleteComp(Component* const c)
{
    return c->getProperties()["mdiDocumentDelete_"];
}
}

bool MyMultiDocumentPanel::closeAllDocuments(const bool checkItsOkToCloseFirst)
{
    while (components.size() > 0)
        if (!closeDocument(components.getLast(), checkItsOkToCloseFirst))
            return false;

    return true;
}

MyMultiDocumentPanelWindow* MyMultiDocumentPanel::createNewDocumentWindow()
{
    return new MyMultiDocumentPanelWindow(backgroundColour);
}

void MyMultiDocumentPanel::addWindow(Component* component)
{
    auto* const dw = createNewDocumentWindow();

    dw->setResizable(true, false);
    dw->setContentNonOwned(component, true);
    dw->setName(component->getName());
    dw->setConstrainer(&m_myConstrainer);

    auto bkg = component->getProperties()["mdiDocumentBkg_"];
    dw->setBackgroundColour(bkg.isVoid() ? backgroundColour : Colour((uint32) static_cast<int>(bkg)));

    int x = 4;

    if (auto* topComp = getChildren().getLast())
        if (topComp->getX() == x && topComp->getY() == x)
            x += 16;

    dw->setTopLeftPosition(x, x);

    auto pos = component->getProperties()["mdiDocumentPos_"];
    if (pos.toString().isNotEmpty())
        dw->restoreWindowStateFromString(pos.toString());

    addAndMakeVisible(dw);
    dw->toFront(true);
}

bool MyMultiDocumentPanel::addDocument(Component* const component, Colour docColour, const bool deleteWhenRemoved)
{
    // If you try passing a full DocumentWindow or ResizableWindow in here, you'll end up
    // with a frame-within-a-frame! Just pass in the bare content component.
    jassert(dynamic_cast<ResizableWindow*>(component) == nullptr);

    if (component == nullptr || (maximumNumDocuments > 0 && components.size() >= maximumNumDocuments))
        return false;

    components.add(component);
    component->getProperties().set("mdiDocumentDelete_", deleteWhenRemoved);
    component->getProperties().set("mdiDocumentBkg_", static_cast<int>(docColour.getARGB()));
    component->addComponentListener(this);

    if (mode == FloatingWindows)
    {
        if (isFullscreenWhenOneDocument())
        {
            if (components.size() == 1)
            {
                addAndMakeVisible(component);
            }
            else
            {
                if (components.size() == 2)
                    addWindow(components.getFirst());

                addWindow(component);
            }
        }
        else
        {
            addWindow(component);
        }
    }
    else
    {
        if (tabComponent == nullptr && components.size() > numDocsBeforeTabsUsed)
        {
            tabComponent = std::make_unique<TabbedComponentInternal>();
            addAndMakeVisible(tabComponent.get());

            auto temp = components;

            for (auto& c : temp)
                tabComponent->addTab(c->getName(), docColour, c, false);

            resized();
        }
        else
        {
            if (tabComponent != nullptr)
                tabComponent->addTab(component->getName(), docColour, component, false);
            else
                addAndMakeVisible(component);
        }

        setActiveDocument(component);
    }

    resized();
    activeDocumentChanged();
    return true;
}

bool MyMultiDocumentPanel::closeDocument(Component* component, const bool checkItsOkToCloseFirst)
{
    if (components.contains(component))
    {
        if (checkItsOkToCloseFirst && !tryToCloseDocument(component))
            return false;

        component->removeComponentListener(this);

        const bool shouldDelete = MultiDocHelpers::shouldDeleteComp(component);
        component->getProperties().remove("mdiDocumentDelete_");
        component->getProperties().remove("mdiDocumentBkg_");

        if (mode == FloatingWindows)
        {
            for (auto* child : getChildren())
            {
                if (auto* dw = dynamic_cast<MyMultiDocumentPanelWindow*>(child))
                {
                    if (dw->getContentComponent() == component)
                    {
                        std::unique_ptr<MyMultiDocumentPanelWindow>(dw)->clearContentComponent();
                        break;
                    }
                }
            }

            if (shouldDelete)
                delete component;

            components.removeFirstMatchingValue(component);

            if (isFullscreenWhenOneDocument() && components.size() == 1)
            {
                for (int i = getNumChildComponents(); --i >= 0;)
                {
                    std::unique_ptr<MyMultiDocumentPanelWindow> dw(
                        dynamic_cast<MyMultiDocumentPanelWindow*>(getChildComponent(i)));

                    if (dw != nullptr)
                        dw->clearContentComponent();
                }

                addAndMakeVisible(components.getFirst());
            }
        }
        else
        {
            jassert(components.indexOf(component) >= 0);

            if (tabComponent != nullptr)
            {
                for (int i = tabComponent->getNumTabs(); --i >= 0;)
                    if (tabComponent->getTabContentComponent(i) == component)
                        tabComponent->removeTab(i);
            }
            else
            {
                removeChildComponent(component);
            }

            if (shouldDelete)
                delete component;

            if (tabComponent != nullptr && tabComponent->getNumTabs() <= numDocsBeforeTabsUsed)
                tabComponent.reset();

            components.removeFirstMatchingValue(component);

            if (components.size() > 0 && tabComponent == nullptr)
                addAndMakeVisible(components.getFirst());
        }

        resized();

        // This ensures that the active tab is painted properly when a tab is closed!
        if (auto* activeComponent = getActiveDocument())
            setActiveDocument(activeComponent);

        activeDocumentChanged();
    }
    else
    {
        jassertfalse;
    }

    return true;
}

int MyMultiDocumentPanel::getNumDocuments() const noexcept
{
    return components.size();
}

Component* MyMultiDocumentPanel::getDocument(const int index) const noexcept
{
    return components[index];
}

Component* MyMultiDocumentPanel::getActiveDocument() const noexcept
{
    if (mode == FloatingWindows)
    {
        for (auto* child : getChildren())
            if (auto* dw = dynamic_cast<MyMultiDocumentPanelWindow*>(child))
                if (dw->isActiveWindow())
                    return dw->getContentComponent();
    }

    return components.getLast();
}

void MyMultiDocumentPanel::setActiveDocument(Component* component)
{
    jassert(component != nullptr);

    if (mode == FloatingWindows)
    {
        component = getContainerComp(component);

        if (component != nullptr)
            component->toFront(true);
    }
    else if (tabComponent != nullptr)
    {
        jassert(components.indexOf(component) >= 0);

        for (int i = tabComponent->getNumTabs(); --i >= 0;)
        {
            if (tabComponent->getTabContentComponent(i) == component)
            {
                tabComponent->setCurrentTabIndex(i);
                break;
            }
        }
    }
    else
    {
        component->grabKeyboardFocus();
    }
}

void MyMultiDocumentPanel::activeDocumentChanged() {}

void MyMultiDocumentPanel::setMaximumNumDocuments(const int newNumber)
{
    maximumNumDocuments = newNumber;
}

void MyMultiDocumentPanel::useFullscreenWhenOneDocument(const bool shouldUseTabs)
{
    numDocsBeforeTabsUsed = shouldUseTabs ? 1 : 0;
}

bool MyMultiDocumentPanel::isFullscreenWhenOneDocument() const noexcept
{
    return numDocsBeforeTabsUsed != 0;
}

//==============================================================================
void MyMultiDocumentPanel::setLayoutMode(const LayoutMode newLayoutMode)
{
    if (mode != newLayoutMode)
    {
        mode = newLayoutMode;

        if (mode == FloatingWindows)
        {
            tabComponent.reset();
        }
        else
        {
            for (int i = getNumChildComponents(); --i >= 0;)
            {
                std::unique_ptr<MyMultiDocumentPanelWindow> dw(
                    dynamic_cast<MyMultiDocumentPanelWindow*>(getChildComponent(i)));

                if (dw != nullptr)
                {
                    dw->getContentComponent()->getProperties().set("mdiDocumentPos_", dw->getWindowStateAsString());
                    dw->clearContentComponent();
                }
            }
        }

        resized();

        auto tempComps = components;
        components.clear();

        for (auto* c : tempComps)
            addDocument(c,
                Colour((uint32) static_cast<int>(
                    c->getProperties().getWithDefault("mdiDocumentBkg_", (int)Colours::white.getARGB()))),
                MultiDocHelpers::shouldDeleteComp(c));
    }
}

void MyMultiDocumentPanel::setBackgroundColour(Colour newBackgroundColour)
{
    if (backgroundColour != newBackgroundColour)
    {
        backgroundColour = newBackgroundColour;
        setOpaque(newBackgroundColour.isOpaque());
        repaint();
    }
}

//==============================================================================
void MyMultiDocumentPanel::paint(Graphics& g)
{
    g.fillAll(backgroundColour);
}

void MyMultiDocumentPanel::resized()
{
    if (mode == MaximisedWindowsWithTabs || components.size() == numDocsBeforeTabsUsed)
    {
        for (auto* child : getChildren())
            child->setBounds(getLocalBounds());
    }

    setWantsKeyboardFocus(components.size() == 0);
}

Component* MyMultiDocumentPanel::getContainerComp(Component* c) const
{
    if (mode == FloatingWindows)
    {
        for (auto* child : getChildren())
            if (auto* dw = dynamic_cast<MyMultiDocumentPanelWindow*>(child))
                if (dw->getContentComponent() == c)
                    return dw;
    }

    return c;
}

void MyMultiDocumentPanel::componentNameChanged(Component&)
{
    if (mode == FloatingWindows)
    {
        for (auto* child : getChildren())
            if (auto* dw = dynamic_cast<MyMultiDocumentPanelWindow*>(child))
                dw->setName(dw->getContentComponent()->getName());
    }
    else if (tabComponent != nullptr)
    {
        for (int i = tabComponent->getNumTabs(); --i >= 0;)
            tabComponent->setTabName(i, tabComponent->getTabContentComponent(i)->getName());
    }
}

void MyMultiDocumentPanel::updateOrder()
{
    auto oldList = components;

    if (mode == FloatingWindows)
    {
        components.clear();

        for (auto* child : getChildren())
            if (auto* dw = dynamic_cast<MyMultiDocumentPanelWindow*>(child))
                components.add(dw->getContentComponent());
    }
    else
    {
        if (tabComponent != nullptr)
        {
            if (auto* current = tabComponent->getCurrentContentComponent())
            {
                components.removeFirstMatchingValue(current);
                components.add(current);
            }
        }
    }

    if (components != oldList)
        activeDocumentChanged();
}

bool MyMultiDocumentPanel::tryToCloseDocument(Component* /*component*/)
{
    return true;
}

void MyMultiDocumentPanel::reconfigSnapToGrid(bool snapToGrid, int gridWidth, int gridHeight)
{
    m_myConstrainer.setSnapToGridEnabled(snapToGrid);
    m_myConstrainer.setSnapToGridWidth(gridWidth);
    m_myConstrainer.setSnapToGridHeight(gridHeight);
}
