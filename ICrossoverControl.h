//
//  ICrossoverControl.h
//
//  Created by Michael Donovan on 3/9/16.
//  Inspired by MyGraphicControl class by deisss (https://github.com/Deisss/IPlug-basic-graphic-example)
//

#ifndef ICrossoverControl_H
#define ICrossoverControl_H
#include "IControl.h"

#include "IControl.h"
#include <vector>
#include <algorithm>
#include <sstream>

class CrossoverHandle {
public:
    double x;
    int uid;
    bool operator < (const CrossoverHandle& handle) const { return (this->x < handle.x); };
    bool operator > (const CrossoverHandle& handle) const {	return (this->x > handle.x); };
};

class ICrossoverControl : public IControl {
protected:
    IColor* mColor;
    IColor* mColor2;
    IColor* mColor3; 
    CrossoverHandle handles[4];
    CrossoverHandle selected;
    double minFreq;
    double maxFreq;
    bool isDragging;
    
    double percentToCoordinates(double value) {
        double min = (double) this->mRECT.L;
        double distance = (double) this->mRECT.W();
        return value * distance + min;
    };
    double convertToPercent(double value) {
        double min = (double) this->mRECT.L;
        double position = value - min;
        double distance = (double) this->mRECT.W();
        return position / distance;
    };

    
public:
    ICrossoverControl(IPlugBase *pPlug, IRECT pR, IColor *c1, IColor *c2, IColor *c3) : IControl(pPlug, pR),mColor(c1), mColor2(c2), mColor3(c3),isDragging(false),minFreq(20.),maxFreq(20000.) {
        for (int i=0; i<4; i++) {
            handles[i].uid=i+1;
            handles[i].x=.2*(i+1);
        }
    };
    ~ICrossoverControl() {};
    
    bool Draw(IGraphics *pGraphics){
        int y = mRECT.T+mRECT.H()/2;
        for (int i=0; i<4; i++) {
            CrossoverHandle* current = &handles[i];
            
     
            if(i==selected.uid-1){
                pGraphics->DrawVerticalLine(mColor3, percentToCoordinates(current->x), this->mRECT.B, this->mRECT.T);
  
                pGraphics->FillCircle(mColor3, percentToCoordinates(current->x), y, 4);
            }
            else{
                pGraphics->DrawVerticalLine(mColor, percentToCoordinates(current->x), this->mRECT.B, this->mRECT.T);


                pGraphics->FillCircle(mColor, percentToCoordinates(current->x), y, 4);
            }
            
            pGraphics->FillCircle(mColor2, percentToCoordinates(current->x), y, 2);

            
            IText text = IText(12, &COLOR_WHITE, "Futura");
            const char* str = (formatFreq(getFreq(i+1)));
            IRECT textRect = IRECT(percentToCoordinates(current->x)-10, this->mRECT.B, percentToCoordinates(current->x)+10, this->mRECT.B+20);
            pGraphics->DrawIText(&text, (char*)str, &textRect);
        }
        return true;
    };
    
    CrossoverHandle getHandle(double x, double epsilon){
        for (int i=0; i<4; i++) {
            CrossoverHandle current = handles[i];
            double xh = percentToCoordinates(current.x);
            
            if(x-epsilon < xh && x+epsilon > xh){
                return current;
            }
        }
        
        CrossoverHandle none;
        none.uid=0;
        return none;
    };
    
    void OnMouseUp(int x, int y, IMouseMod* pMouseMod){
        isDragging=false;
        CrossoverHandle none;
        none.uid=0;
        selected=none;
    };
    
    void OnMouseDown(int x, int y,  IMouseMod* pMouseMod){
        CrossoverHandle current = getHandle(x, 10);
        if (current.uid==0) {
            CrossoverHandle none;
            none.uid=0;
            selected=none;
            isDragging=false;
            SetDirty();
        }
        else{
            selected = current;
            isDragging = true;
            SetDirty();
        }
    };
    
    void OnMouseDrag(int x, int y, int dX, int dY, IMouseMod* pMouseMod){
        if(selected.uid==0 || isDragging==false) return;

        double leftBound;
        double rightBound;
        double xPercent=convertToPercent(x);
        
        if (selected.uid==1) {
            leftBound=0;
            rightBound=handles[1].x;
        }
        else if(selected.uid==4){
            leftBound=handles[2].x;
            rightBound=1;
        }
        else{
            leftBound=handles[selected.uid-2].x;
            rightBound=handles[selected.uid].x;
        }

        if(xPercent<rightBound-.02 && xPercent>leftBound+.02){
            handles[selected.uid-1].x=xPercent;
        }
  
        SetDirty();
    };
    
    double getFreq(int band){
        double mF = maxFreq/minFreq;
        double xDist = percentToCoordinates(handles[band-1].x)-mRECT.L;
        return minFreq * std::pow(mF, (double)xDist / (double)(mRECT.W()-1));
    };
    
    const char* formatFreq(double freq){
        std::stringstream ss;
        ss << (int)freq;
        std::string val = ss.str();
        std::string out;
        if(freq>=1000 && freq < 10000){
            out = val.substr(0,1) + "." + val.substr(1,2) + "k";
        }
        else if(freq>=10000){
            out = val.substr(0,2) + "." + val.substr(2,1) + "k";
        }
        else{
            out = val;
        }
        
        return out.c_str();
    }
};

#endif