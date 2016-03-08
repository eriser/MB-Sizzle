#ifndef ICrossoverControl_H
#define ICrossoverControl_H

#include "IControl.h"
#include <vector>
#include <algorithm>

class Handle {
public:
    unsigned int uid;
    double x;
    bool operator < (const Handle& Handle) const { return (this->x < Handle.x); };
    bool operator > (const Handle& Handle) const {	return (this->x > Handle.x); };
};

class ICrossoverControl : public IControl {
protected:
    std::vector<Handle> Handles;
    Handle selected;
    bool isDragging;
    unsigned int counter;
    
    double convertToGraphicX(double value) {
        double min = (double) this->mRECT.L;
        double distance = (double) this->mRECT.W();
        return value * distance + min;
    };
    double convertToPercentX(double value) {
        double min = (double) this->mRECT.L;
        double position = value - min;
        double distance = (double) this->mRECT.W();
        return position / distance;
    };

    double convertToPercentY(double value) {
        double min = (double) this->mRECT.T;
        double position = value - min;
        double distance = (double) this->mRECT.H();
        // We return the 1 - distance as the value 1 is located on top of graphics and not bottom
        return 1 - position / distance;
    };
    
public:
    ICrossoverControl(IPlugBase *pPlug, IRECT pR) : IControl(pPlug, pR) {};
    ~ICrossoverControl() {};
    
    bool IsDirty() { return true; };
    
    bool Draw(IGraphics *pGraphics) {
        IColor color(255, 50, 200, 20);
        Handle previous;
        // Little trick, no "real" Handles got uid = 0, so we know it's
        // not a real Handle, and we should avoid doing line with it...
        previous.uid = 0;
        
        for (std::vector<Handle>::iterator it = Handles.begin(); it != Handles.end(); ++it) {
            Handle current = *it;
            // We draw the Handle
            pGraphics->DrawCircle(&color, convertToGraphicX(current.x), convertToGraphicY(current.y), 3, 0, true);
            
            // The previous Handle is a valid Handle, we draw line also
            if (previous.uid > 0) {
                pGraphics->DrawLine(&color,
                                    convertToGraphicX(previous.x), convertToGraphicY(previous.y),
                                    convertToGraphicX(current.x), convertToGraphicY(current.y),
                                    0, true);
            }
            
            // We update the previous Handle
            previous = current;
        }
        
        /* TODO:
         I didn't put it here, to make you think
         But you need few more cases to handle:
         - when there is no Handles on the graphic
         - when there is a single Handle on the graphic
         - what to draw BEFORE the first Handle
         - what to draw AFTER the last Handle
         */
        return true;
    };
    
    Handle getHandle(double x, double y, double epsilon) {
        for (std::vector<Handle>::iterator it = Handles.begin(); it != Handles.end(); ++it) {
            Handle Handle = *it;
            double xGraphic = convertToGraphicX(Handle.x);
            double yGraphic = convertToGraphicY(Handle.y);
            
            if (
                // X check
                (xGraphic - epsilon < x && xGraphic + epsilon > x) &&
                // Y check
                (yGraphic - epsilon < y && yGraphic + epsilon > y)
                ) {
                return Handle;
            }
        }
        
        // Nothing found, we return a "blank" Handle
        Handle none;
        none.uid = 0;
        return none;
    };
    
    void OnMouseDblClick(int x, int y, IMouseMod* pMouseMod) {
        Handle imHere = getHandle(x, y, 6);
        
        // As we said, the uid = 0 means no Handle
        if (imHere.uid == 0) {
            Handle newHandle;
            newHandle.x = convertToPercentX(x);
            newHandle.y = convertToPercentY(y);
            newHandle.uid = counter++;
            Handles.push_back(newHandle);
            // And we sort it!
            std::sort(Handles.begin(), Handles.end());
            SetDirty();
        } else {
            // We delete the Handle
            Handles.erase(std::remove(Handles.begin(), Handles.end(), imHere), Handles.end());
            SetDirty();
        }
    };
    
    void OnMouseUp(int x, int y, IMouseMod* pMouseMod) {
        isDragging = false;
    };
    
    void OnMouseDown(int x, int y, IMouseMod* pMouseMod) {
        Handle imHere = getHandle(x, y, 6);
        
        if (imHere.uid == 0) {
            // We erase selected
            Handle none;
            none.uid = 0;
            selected = none;
            // Not needed, but who knows.
            isDragging = false;
            SetDirty();
        } else {
            selected = imHere;
            isDragging = true;
            SetDirty();
        }
    };
    
    void OnMouseDrag(int x, int y, int dX, int dY, IMouseMod* pMouseMod) {
        if (selected.uid == 0 || isDragging == false) {
            // Nothing to do
            return;
        }
        
        // We search for our Handles
        // As selected is not a Handleer
        // If we modify it, it will be meaningless...
        std::vector<Handle>::iterator it = std::find(Handles.begin(), Handles.end(), selected);
        
        if (it != Handles.end()) {
            (&(*it))->x = convertToPercentX(x);
            (&(*it))->y = convertToPercentY(y);
        }
        
        // And we ask to render
        SetDirty();
    };
};

#endif