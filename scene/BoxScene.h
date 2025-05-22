#pragma once

#include "BaseScene.h"

class BoxScene : public BaseScene {
public:
    void displayScene(const Handle(V3d_View)& view, const Handle(AIS_InteractiveContext)& context) override {
        gp_Ax2 anAxis;
        anAxis.SetLocation(gp_Pnt(0.0, 0.0, 0.0));
        Handle(AIS_Shape) aBox = new AIS_Shape(BRepPrimAPI_MakeBox(anAxis, 50, 50, 50).Shape());
        context->Display(aBox, AIS_Shaded, 0, false);
    }

    void sceneInit(const Handle(V3d_View)& view, const Handle(AIS_InteractiveContext)& context) override {
        // No need to Init scene
    };
};
