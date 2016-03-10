//
//  IRoundedPanelControl.h
//  MultibandDistortion
//
//  Created by Michael on 3/9/16.
//
//

#ifndef IRoundedPanelControl_h
#define IRoundedPanelControl_h
class IRoundedPanelControl : public IControl {
public:
    IRoundedPanelControl(IPlugBase *pPlug, IRECT pR, IColor *c1) : IControl(pPlug, pR),mColor(c1) {};
    
    bool Draw(IGraphics *pGraphics){
        IChannelBlend blend = IChannelBlend();
        blend.mMethod=blend.kBlendNone;
        pGraphics->FillRoundRect(mColor, &(this->mRECT), &blend, 10, true);
        SetDirty();

        return true;
    }
private:
    IColor* mColor;
};

#endif /* IRoundedPanelControl_h */
