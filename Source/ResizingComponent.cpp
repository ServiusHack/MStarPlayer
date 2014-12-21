#include "ResizingComponent.h"

void ResizingComponent::childBoundsChanged(Component *child)
{
	int maxWidth = 0;
	int maxHeight = 0;
	for (int i = 0; i < getNumChildComponents(); ++i) {
		maxWidth = std::max(maxWidth, getChildComponent(i)->getX() + getChildComponent(i)->getWidth());
		maxHeight = std::max(maxHeight, getChildComponent(i)->getY() + getChildComponent(i)->getHeight());
	}

	setBounds(getX(), getY(), maxWidth, maxHeight);
}