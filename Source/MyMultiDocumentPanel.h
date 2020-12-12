/*
  ==============================================================================

   Heavily based on JUCE's MultiDocumentPanel.

  ==============================================================================
*/

#pragma once

#include "ResizingComponent.h"

class MyMultiDocumentPanel;

class MyConstrainer : public juce::ComponentBoundsConstrainer
{
public:
    void setSnapToGridEnabled(bool enable);
    void setSnapToGridWidth(int width);
    void setSnapToGridHeight(int height);

    void checkBounds(juce::Rectangle<int>& bounds, const juce::Rectangle<int>& previousBounds,
        const juce::Rectangle<int>& limits, bool isStretchingTop, bool isStretchingLeft, bool isStretchingBottom,
        bool isStretchingRight) override;

private:
    bool snapToGrid{false};
    int gridWidth{1};
    int gridHeight{1};
};

//==============================================================================
/**
    This is a derivative of DocumentWindow that is used inside a MyMultiDocumentPanel
    component.

    It's like a normal DocumentWindow but has some extra functionality to make sure
    everything works nicely inside a MyMultiDocumentPanel.

    @see MyMultiDocumentPanel

    @tags{GUI}
*/
class JUCE_API MyMultiDocumentPanelWindow : public juce::DocumentWindow
{
public:
    //==============================================================================
    /**
     */
    MyMultiDocumentPanelWindow(juce::Colour backgroundColour);

    /** Destructor. */
    ~MyMultiDocumentPanelWindow();

    //==============================================================================
    /** @internal */
    void maximiseButtonPressed() override;
    /** @internal */
    void closeButtonPressed() override;
    /** @internal */
    void activeWindowStatusChanged() override;
    /** @internal */
    void broughtToFront() override;

private:
    //==============================================================================
    void updateOrder();
    MyMultiDocumentPanel* getOwner() const noexcept;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MyMultiDocumentPanelWindow)
};

//==============================================================================
/**
    A component that contains a set of other components either in floating windows
    or tabs.

    This acts as a panel that can be used to hold a set of open document windows, with
    different layout modes.

    Use addDocument() and closeDocument() to add or remove components from the
    panel - never use any of the Component methods to access the panel's child
    components directly, as these are managed internally.

    @tags{GUI}
*/
class JUCE_API MyMultiDocumentPanel
    : public juce::Component
    , private juce::ComponentListener
{
public:
    //==============================================================================
    /** Creates an empty panel.

        Use addDocument() and closeDocument() to add or remove components from the
        panel - never use any of the Component methods to access the panel's child
        components directly, as these are managed internally.
    */
    MyMultiDocumentPanel();

    /** Destructor.

        When deleted, this will call closeAllDocuments (false) to make sure all its
        components are deleted. If you need to make sure all documents are saved
        before closing, then you should call closeAllDocuments (true) and check that
        it returns true before deleting the panel.
    */
    ~MyMultiDocumentPanel();

    //==============================================================================
    /** Tries to close all the documents.

        If checkItsOkToCloseFirst is true, then the tryToCloseDocument() method will
        be called for each open document, and any of these calls fails, this method
        will stop and return false, leaving some documents still open.

        If checkItsOkToCloseFirst is false, then all documents will be closed
        unconditionally.

        @see closeDocument
    */
    bool closeAllDocuments(bool checkItsOkToCloseFirst);

    /** Adds a document component to the panel.

        If the number of documents would exceed the limit set by setMaximumNumDocuments() then
        this will fail and return false. (If it does fail, the component passed-in will not be
        deleted, even if deleteWhenRemoved was set to true).

        The MyMultiDocumentPanel will deal with creating a window border to go around your component,
        so just pass in the bare content component here, no need to give it a ResizableWindow
        or DocumentWindow.

        @param component            the component to add
        @param backgroundColour     the background colour to use to fill the component's
                                    window or tab
        @param deleteWhenRemoved    if true, then when the component is removed by closeDocument()
                                    or closeAllDocuments(), then it will be deleted. If false, then
                                    the caller must handle the component's deletion
    */
    bool addDocument(juce::Component* component, juce::Colour backgroundColour, bool deleteWhenRemoved);

    /** Closes one of the documents.

        If checkItsOkToCloseFirst is true, then the tryToCloseDocument() method will
        be called, and if it fails, this method will return false without closing the
        document.

        If checkItsOkToCloseFirst is false, then the documents will be closed
        unconditionally.

        The component will be deleted if the deleteWhenRemoved parameter was set to
        true when it was added with addDocument.

        @see addDocument, closeAllDocuments
    */
    bool closeDocument(juce::Component* component, bool checkItsOkToCloseFirst);

    /** Returns the number of open document windows.

        @see getDocument
    */
    int getNumDocuments() const noexcept;

    /** Returns one of the open documents.

        The order of the documents in this array may change when they are added, removed
        or moved around.

        @see getNumDocuments
    */
    juce::Component* getDocument(int index) const noexcept;

    /** Returns the document component that is currently focused or on top.

        If currently using floating windows, then this will be the component in the currently
        active window, or the top component if none are active.

        If it's currently in tabbed mode, then it'll return the component in the active tab.

        @see setActiveDocument
    */
    juce::Component* getActiveDocument() const noexcept;

    /** Makes one of the components active and brings it to the top.

        @see getActiveDocument
    */
    void setActiveDocument(juce::Component* component);

    /** Callback which gets invoked when the currently-active document changes. */
    virtual void activeDocumentChanged();

    /** Sets a limit on how many windows can be open at once.

        If this is zero or less there's no limit (the default). addDocument() will fail
        if this number is exceeded.
    */
    void setMaximumNumDocuments(int maximumNumDocuments);

    /** Sets an option to make the document fullscreen if there's only one document open.

        If set to true, then if there's only one document, it'll fill the whole of this
        component without tabs or a window border. If false, then tabs or a window
        will always be shown, even if there's only one document. If there's more than
        one document open, then this option makes no difference.
    */
    void useFullscreenWhenOneDocument(bool shouldUseTabs);

    /** Returns the result of the last time useFullscreenWhenOneDocument() was called.
     */
    bool isFullscreenWhenOneDocument() const noexcept;

    //==============================================================================
    /** The different layout modes available. */
    enum LayoutMode
    {
        FloatingWindows, /**< In this mode, there are overlapping DocumentWindow components for each document. */
        MaximisedWindowsWithTabs /**< In this mode, a TabbedComponent is used to show one document at a time. */
    };

    /** Changes the panel's mode.

        @see LayoutMode, getLayoutMode
    */
    void setLayoutMode(LayoutMode newLayoutMode);

    /** Returns the current layout mode. */
    LayoutMode getLayoutMode() const noexcept
    {
        return mode;
    }
    /** Sets the background colour for the whole panel.

        Each document has its own background colour, but this is the one used to fill the areas
        behind them.
    */
    void setBackgroundColour(juce::Colour newBackgroundColour);

    /** Returns the current background colour.

        @see setBackgroundColour
    */
    juce::Colour getBackgroundColour() const noexcept
    {
        return backgroundColour;
    }
    /** If the panel is being used in tabbed mode, this returns the TabbedComponent that's involved. */
    juce::TabbedComponent* getCurrentTabbedComponent() const noexcept
    {
        return tabComponent.get();
    }
    //==============================================================================
    /** A subclass must override this to say whether its currently ok for a document
        to be closed.

        This method is called by closeDocument() and closeAllDocuments() to indicate that
        a document should be saved if possible, ready for it to be closed.

        If this method returns true, then it means the document is ok and can be closed.

        If it returns false, then it means that the closeDocument() method should stop
        and not close.

        Normally, you'd use this method to ask the user if they want to save any changes,
        then return true if the save operation went ok. If the user cancelled the save
        operation you could return false here to abort the close operation.

        If your component is based on the FileBasedDocument class, then you'd probably want
        to call FileBasedDocument::saveIfNeededAndUserAgrees() and return true if this returned
        FileBasedDocument::savedOk

        @see closeDocument, FileBasedDocument::saveIfNeededAndUserAgrees()
    */
    virtual bool tryToCloseDocument(juce::Component* component);

    /** Creates a new window to be used for a document.

        The default implementation of this just returns a basic MyMultiDocumentPanelWindow object,
        but you might want to override it to return a custom component.
    */
    virtual MyMultiDocumentPanelWindow* createNewDocumentWindow();

    void reconfigSnapToGrid(bool snapToGrid, int gridWidth, int gridHeight);

    //==============================================================================
    /** @internal */
    void paint(juce::Graphics&) override;
    /** @internal */
    void resized() override;
    /** @internal */
    void componentNameChanged(juce::Component&) override;

private:
    //==============================================================================
    LayoutMode mode = MaximisedWindowsWithTabs;
    juce::Array<juce::Component*> components;
    std::unique_ptr<juce::TabbedComponent> tabComponent;
    juce::Colour backgroundColour{juce::Colours::lightblue};
    int maximumNumDocuments = 0, numDocsBeforeTabsUsed = 0;

    MyConstrainer m_myConstrainer;

    struct TabbedComponentInternal;
    friend class MyMultiDocumentPanelWindow;

    juce::Component* getContainerComp(juce::Component*) const;
    void updateOrder();
    void addWindow(juce::Component*);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MyMultiDocumentPanel)
};
