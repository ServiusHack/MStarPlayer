/*
  ==============================================================================

   Heavily based on JUCE's MultiDocumentPanel.

  ==============================================================================
*/

#include "MyMultiDocumentPanel.h"

MyMultiDocumentPanelWindow::MyMultiDocumentPanelWindow (Colour backgroundColour)
    : DocumentWindow (String::empty, backgroundColour,
                      DocumentWindow::maximiseButton | DocumentWindow::closeButton, false)
{
}

MyMultiDocumentPanelWindow::~MyMultiDocumentPanelWindow()
{
}

//==============================================================================
void MyMultiDocumentPanelWindow::maximiseButtonPressed()
{
    if (MyMultiDocumentPanel* const owner = getOwner())
        owner->setLayoutMode (MyMultiDocumentPanel::MaximisedWindowsWithTabs);
    else
        jassertfalse; // these windows are only designed to be used inside a MultiDocumentPanel!
}

void MyMultiDocumentPanelWindow::closeButtonPressed()
{
    if (MyMultiDocumentPanel* const owner = getOwner())
        owner->closeDocument (getContentComponent(), true);
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
    if (MyMultiDocumentPanel* const owner = getOwner())
        owner->updateOrder();
}

MyMultiDocumentPanel* MyMultiDocumentPanelWindow::getOwner() const noexcept
{
    return findParentComponentOfClass<MyMultiDocumentPanel>();
}


//==============================================================================
class MyMultiDocumentPanel::TabbedComponentInternal   : public TabbedComponent
{
public:
    TabbedComponentInternal()
        : TabbedComponent (TabbedButtonBar::TabsAtTop)
    {
    }

    void currentTabChanged (int, const String&)
    {
        if (MyMultiDocumentPanel* const owner = findParentComponentOfClass<MyMultiDocumentPanel>())
            owner->updateOrder();
    }
};


//==============================================================================
MyMultiDocumentPanel::MyMultiDocumentPanel()
    : mode (MaximisedWindowsWithTabs),
      backgroundColour (Colours::lightblue),
      maximumNumDocuments (0),
      numDocsBeforeTabsUsed (0),
      viewport(new Viewport()),
      resizingComponent(new ResizingComponent())
{
    setOpaque (true);
    addAndMakeVisible(viewport);
    viewport->setViewedComponent(resizingComponent, false);
}

MyMultiDocumentPanel::~MyMultiDocumentPanel()
{
    closeAllDocuments (false);
}

//==============================================================================
namespace MultiDocHelpers
{
    static bool shouldDeleteComp (Component* const c)
    {
        return c->getProperties() ["mdiDocumentDelete_"];
    }
}

bool MyMultiDocumentPanel::closeAllDocuments (const bool checkItsOkToCloseFirst)
{
    while (components.size() > 0)
        if (! closeDocument (components.getLast(), checkItsOkToCloseFirst))
            return false;

    return true;
}

MyMultiDocumentPanelWindow* MyMultiDocumentPanel::createNewDocumentWindow()
{
    return new MyMultiDocumentPanelWindow (backgroundColour);
}

void MyMultiDocumentPanel::addWindow (Component* component)
{
    MyMultiDocumentPanelWindow* const dw = createNewDocumentWindow();

    dw->setResizable (true, false);
    dw->setContentNonOwned (component, true);
    dw->setName (component->getName());

    const var bkg (component->getProperties() ["mdiDocumentBkg_"]);
    dw->setBackgroundColour (bkg.isVoid() ? backgroundColour : Colour ((uint32) static_cast <int> (bkg)));

    int x = 4;

    if (Component* const topComp = resizingComponent->getChildComponent (resizingComponent->getNumChildComponents() - 1))
        if (topComp->getX() == x && topComp->getY() == x)
            x += 16;

    dw->setTopLeftPosition (x, x);

    const var pos (component->getProperties() ["mdiDocumentPos_"]);
    if (pos.toString().isNotEmpty())
        dw->restoreWindowStateFromString (pos.toString());

    resizingComponent->addAndMakeVisible (dw);
    dw->toFront (true);
}

bool MyMultiDocumentPanel::addDocument (Component* const component,
                                      Colour docColour,
                                      const bool deleteWhenRemoved)
{
    // If you try passing a full DocumentWindow or ResizableWindow in here, you'll end up
    // with a frame-within-a-frame! Just pass in the bare content component.
    jassert (dynamic_cast <ResizableWindow*> (component) == nullptr);

    if (component == nullptr || (maximumNumDocuments > 0 && components.size() >= maximumNumDocuments))
        return false;

    components.add (component);
    component->getProperties().set ("mdiDocumentDelete_", deleteWhenRemoved);
    component->getProperties().set ("mdiDocumentBkg_", (int) docColour.getARGB());
    component->addComponentListener (this);

    if (mode == FloatingWindows)
    {
        if (isFullscreenWhenOneDocument())
        {
            if (components.size() == 1)
            {
                resizingComponent->addAndMakeVisible (component);
            }
            else
            {
                if (components.size() == 2)
                    addWindow (components.getFirst());

                addWindow (component);
            }
        }
        else
        {
           addWindow (component);
        }
    }
    else
    {
        if (tabComponent == nullptr && components.size() > numDocsBeforeTabsUsed)
        {
            addAndMakeVisible (tabComponent = new TabbedComponentInternal());

            Array <Component*> temp (components);

            for (int i = 0; i < temp.size(); ++i)
                tabComponent->addTab (temp[i]->getName(), docColour, temp[i], false);

            resized();
        }
        else
        {
            if (tabComponent != nullptr)
                tabComponent->addTab (component->getName(), docColour, component, false);
            else
                resizingComponent->addAndMakeVisible (component);
        }

        setActiveDocument (component);
    }

    resized();
    activeDocumentChanged();
    return true;
}

bool MyMultiDocumentPanel::closeDocument (Component* component,
                                        const bool checkItsOkToCloseFirst)
{
    if (components.contains (component))
    {
        if (checkItsOkToCloseFirst && ! tryToCloseDocument (component))
            return false;

        component->removeComponentListener (this);

        const bool shouldDelete = MultiDocHelpers::shouldDeleteComp (component);
        component->getProperties().remove ("mdiDocumentDelete_");
        component->getProperties().remove ("mdiDocumentBkg_");

        if (mode == FloatingWindows)
        {
            for (int i = resizingComponent->getNumChildComponents(); --i >= 0;)
            {
                if (MyMultiDocumentPanelWindow* const dw = dynamic_cast <MyMultiDocumentPanelWindow*> (resizingComponent->getChildComponent (i)))
                {
                    if (dw->getContentComponent() == component)
                    {
                        ScopedPointer<MyMultiDocumentPanelWindow> (dw)->clearContentComponent();
                        break;
                    }
                }
            }

            components.removeFirstMatchingValue (component);

            if (shouldDelete)
                delete component;

            if (isFullscreenWhenOneDocument() && components.size() == 1)
            {
                for (int i = resizingComponent->getNumChildComponents(); --i >= 0;)
                {
                    ScopedPointer<MyMultiDocumentPanelWindow> dw (dynamic_cast <MyMultiDocumentPanelWindow*> (resizingComponent->getChildComponent (i)));

                    if (dw != nullptr)
                        dw->clearContentComponent();
                }

                addAndMakeVisible (components.getFirst());
            }
        }
        else
        {
            jassert (components.indexOf (component) >= 0);

            if (tabComponent != nullptr)
            {
                for (int i = tabComponent->getNumTabs(); --i >= 0;)
                    if (tabComponent->getTabContentComponent (i) == component)
                        tabComponent->removeTab (i);
            }
            else
            {
                removeChildComponent (component);
            }

            if (tabComponent != nullptr && tabComponent->getNumTabs() <= numDocsBeforeTabsUsed)
                tabComponent = nullptr;

            components.removeFirstMatchingValue (component);

            if (shouldDelete)
                delete component;

            if (components.size() > 0 && tabComponent == nullptr)
                addAndMakeVisible (components.getFirst());
        }

        resized();

        // This ensures that the active tab is painted properly when a tab is closed!
        if (Component* activeComponent = getActiveDocument())
            setActiveDocument (activeComponent);

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

Component* MyMultiDocumentPanel::getDocument (const int index) const noexcept
{
    return components [index];
}

Component* MyMultiDocumentPanel::getActiveDocument() const noexcept
{
    if (mode == FloatingWindows)
    {
        for (int i = resizingComponent->getNumChildComponents(); --i >= 0;)
            if (MyMultiDocumentPanelWindow* const dw = dynamic_cast <MyMultiDocumentPanelWindow*> (resizingComponent->getChildComponent (i)))
                if (dw->isActiveWindow())
                    return dw->getContentComponent();
    }

    return components.getLast();
}

void MyMultiDocumentPanel::setActiveDocument (Component* component)
{
    jassert (component != nullptr);

    if (mode == FloatingWindows)
    {
        component = getContainerComp (component);

        if (component != nullptr)
            component->toFront (true);
    }
    else if (tabComponent != nullptr)
    {
        jassert (components.indexOf (component) >= 0);

        for (int i = tabComponent->getNumTabs(); --i >= 0;)
        {
            if (tabComponent->getTabContentComponent (i) == component)
            {
                tabComponent->setCurrentTabIndex (i);
                break;
            }
        }
    }
    else
    {
        component->grabKeyboardFocus();
    }
}

void MyMultiDocumentPanel::activeDocumentChanged()
{
}

void MyMultiDocumentPanel::setMaximumNumDocuments (const int newNumber)
{
    maximumNumDocuments = newNumber;
}

void MyMultiDocumentPanel::useFullscreenWhenOneDocument (const bool shouldUseTabs)
{
    numDocsBeforeTabsUsed = shouldUseTabs ? 1 : 0;
}

bool MyMultiDocumentPanel::isFullscreenWhenOneDocument() const noexcept
{
    return numDocsBeforeTabsUsed != 0;
}

//==============================================================================
void MyMultiDocumentPanel::setLayoutMode (const LayoutMode newLayoutMode)
{
    if (mode != newLayoutMode)
    {
        mode = newLayoutMode;

        if (mode == FloatingWindows)
        {
            tabComponent = nullptr;
            viewport->setScrollBarsShown(true, true);
        }
        else
        {
            viewport->setScrollBarsShown(false, false);
            for (int i = resizingComponent->getNumChildComponents(); --i >= 0;)
            {
                ScopedPointer<MyMultiDocumentPanelWindow> dw (dynamic_cast <MyMultiDocumentPanelWindow*> (resizingComponent->getChildComponent (i)));

                if (dw != nullptr)
                {
                    dw->getContentComponent()->getProperties().set ("mdiDocumentPos_", dw->getWindowStateAsString());
                    dw->clearContentComponent();
                }
            }
        }

        resized();

        const Array <Component*> tempComps (components);
        components.clear();

        for (int i = 0; i < tempComps.size(); ++i)
        {
            Component* const c = tempComps.getUnchecked(i);

            addDocument (c,
                         Colour ((uint32) static_cast <int> (c->getProperties().getWithDefault ("mdiDocumentBkg_", (int) Colours::white.getARGB()))),
                         MultiDocHelpers::shouldDeleteComp (c));
        }
    }
}

void MyMultiDocumentPanel::setBackgroundColour (Colour newBackgroundColour)
{
    if (backgroundColour != newBackgroundColour)
    {
        backgroundColour = newBackgroundColour;
        setOpaque (newBackgroundColour.isOpaque());
        repaint();
    }
}

//==============================================================================
void MyMultiDocumentPanel::paint (Graphics& g)
{
    g.fillAll (backgroundColour);
}

void MyMultiDocumentPanel::resized()
{
    if (mode == MaximisedWindowsWithTabs || components.size() == numDocsBeforeTabsUsed)
    {
        for (int i = getNumChildComponents(); --i >= 0;)
            getChildComponent (i)->setBounds (getLocalBounds());
    }
	else
	{
		viewport->setBounds(getLocalBounds());
	}

    setWantsKeyboardFocus (components.size() == 0);
}

Component* MyMultiDocumentPanel::getContainerComp (Component* c) const
{
    if (mode == FloatingWindows)
    {
        for (int i = 0; i < resizingComponent->getNumChildComponents(); ++i)
            if (MyMultiDocumentPanelWindow* const dw = dynamic_cast <MyMultiDocumentPanelWindow*> (resizingComponent->getChildComponent (i)))
                if (dw->getContentComponent() == c)
                    return dw;
    }

    return c;
}

void MyMultiDocumentPanel::componentNameChanged (Component&)
{
    if (mode == FloatingWindows)
    {
        for (int i = 0; i < resizingComponent->getNumChildComponents(); ++i)
            if (MyMultiDocumentPanelWindow* const dw = dynamic_cast <MyMultiDocumentPanelWindow*> (resizingComponent->getChildComponent (i)))
                dw->setName (dw->getContentComponent()->getName());
    }
    else if (tabComponent != nullptr)
    {
        for (int i = tabComponent->getNumTabs(); --i >= 0;)
            tabComponent->setTabName (i, tabComponent->getTabContentComponent (i)->getName());
    }
}

void MyMultiDocumentPanel::updateOrder()
{
    const Array <Component*> oldList (components);

    if (mode == FloatingWindows)
    {
        components.clear();

        for (int i = 0; i < resizingComponent->getNumChildComponents(); ++i)
            if (MyMultiDocumentPanelWindow* const dw = dynamic_cast <MyMultiDocumentPanelWindow*> (resizingComponent->getChildComponent (i)))
                components.add (dw->getContentComponent());
    }
    else
    {
        if (tabComponent != nullptr)
        {
            if (Component* const current = tabComponent->getCurrentContentComponent())
            {
                components.removeFirstMatchingValue (current);
                components.add (current);
            }
        }
    }

    if (components != oldList)
        activeDocumentChanged();
}

bool MyMultiDocumentPanel::tryToCloseDocument(Component* /*component*/) {
    return true;
}
