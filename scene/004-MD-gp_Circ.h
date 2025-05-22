#pragma once

#include "pch.h"
#include <gp_Circ.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Plane.hxx>
#include <TopoDS_Edge.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <AIS_Shape.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <GCE2d_MakeCircle.hxx>
#include <GC_MakeCircle.hxx>
#include <Geom_Circle.hxx>

#include "BaseScene.h"
#include "VisSceneComponents.h"
#include "TutorialWindow.h"

class MDCircle004 : public BaseScene, public VisSceneComponents, public TutorialWindow {
public:
    MDCircle004() { openTutorialWindow(); }

    void displayScene(const Handle(V3d_View)& view, const Handle(AIS_InteractiveContext)& context) override {
        if (context.IsNull()) return;
        if (!bIsSceneInit) {
            sceneInit(view, context);
            bIsSceneInit = true;
        }
        renderTutorialWindow(context);
    }

    void sceneInit(const Handle(V3d_View)& view, const Handle(AIS_InteractiveContext)& context) override {
        DisplayCircle(context);
    }

    void customInitTutorialWindow(const Handle(AIS_InteractiveContext)& context) override {
        DisplayCircle(context);
    }

    void renderTutorialContent(const Handle(AIS_InteractiveContext)& context) override {
        const char* items[] = {
            "Construct from Center/Normal/Radius",
            "Construct from 3 Points",
            "Extract Circle Parameters",
            "Show Circle Normal Arrow",
            "Show Circle Plane"
        };

        ImGui::Combo("Circle Tutorial Mode", &tutorialModeIndex, items, IM_ARRAYSIZE(items));

        if (tutorialModeIndex == 0){
            if (ImGui::SliderFloat3("Center", center.data(), -100.f, 100.f)) {}
            if (ImGui::SliderFloat3("Normal", normal.data(), -1.f, 1.f)) {}
            if (ImGui::SliderFloat("Radius", &radius, 0.1f, 100.f)) {}
        }

        if (tutorialModeIndex == 1) {
            if (ImGui::SliderFloat3("P1", point1.data(), -100.f, 100.f)) {}
            if (ImGui::SliderFloat3("P2", point2.data(), -100.f, 100.f)) {}
            if (ImGui::SliderFloat3("P3", point3.data(), -100.f, 100.f)) {}

            if (ImGui::SliderFloat("Angle (deg)", &angleDeg, 0, 360)) {}
        }

        if (tutorialModeIndex == 2) {
            ImGui::Text("Center: (%.2f, %.2f, %.2f)", m_circ.Location().X(), m_circ.Location().Y(), m_circ.Location().Z());
            ImGui::Text("Normal: (%.2f, %.2f, %.2f)", m_circ.Axis().Direction().X(), m_circ.Axis().Direction().Y(), m_circ.Axis().Direction().Z());
            ImGui::Text("Radius: %.2f", m_circ.Radius());
        }


        if (ImGui::Button("Display")) {
            DisplayCircle(context);
        }
    }

private:
    int tutorialModeIndex = 0;

    gp_Circ m_circ;

    std::array<float, 3> center{ 0.f, 0.f, 0.f };
    std::array<float, 3> normal{ 0.f, 0.f, 1.f };
    float radius = 10.0f;

    std::array<float, 3> point1{ 0.f, 0.f, 0.f };
    std::array<float, 3> point2{ 10.f, 0.f, 0.f };
    std::array<float, 3> point3{ 0.f, 10.f, 0.f };

    float angleDeg = 45.0f;

    Handle(AIS_Shape) mainShape;
    std::vector<Handle(AIS_Shape)> auxShapes;

    void DisplayCircle(const Handle(AIS_InteractiveContext)& context) {
        ClearAuxShapes(context);
        if (!mainShape.IsNull()) {
            context->Remove(mainShape, Standard_False);
            mainShape.Nullify();
        }

        try {
            switch (tutorialModeIndex) {
                case 0: { // Center + Normal + Radius
                    gp_Pnt ctr(center[0], center[1], center[2]);
                    gp_Dir dir(normal[0], normal[1], normal[2]);
                    m_circ = gp_Circ(gp_Ax2(ctr, dir), radius);
                    break;
                }
                case 1: { // From 3 points
                    gp_Pnt p1(point1[0], point1[1], point1[2]);
                    gp_Pnt p2(point2[0], point2[1], point2[2]);
                    gp_Pnt p3(point3[0], point3[1], point3[2]);
                    // 替代方式：从三点构造圆
                    GC_MakeCircle mkCirc(p1, p2, p3);
                    if (!mkCirc.IsDone()) {
                        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Failed to create circle from 3 points.");
                        return;
                    }
                    m_circ = mkCirc.Value()->Circ();
                    break;
                }
                case 2: { // Extract Params
                    gp_Pnt ctr = m_circ.Location();
                    gp_Dir dir = m_circ.Axis().Direction();
                    double radius = m_circ.Radius();

                    center = { (float)ctr.X(), (float)ctr.Y(), (float)ctr.Z() };
                    normal = { (float)dir.X(), (float)dir.Y(), (float)dir.Z() };
                    this->radius = (float)radius;
                    break;
                }

                case 3: { // Show normal arrow
                    
                        gp_Pnt ctr = m_circ.Location();
                        gp_Dir dir = m_circ.Axis().Direction();

                        gp_Pnt end = ctr.Translated(dir.XYZ() * 15);
                        DisplayAuxLine(context, ctr, end, Quantity_NOC_RED, 2.0);
                    
                    break;
                }

                case 4: { // Show Circle Plane

                        gp_Pnt ctr = m_circ.Location();
                        gp_Dir dir = m_circ.Axis().Direction();

                        gp_Pln pln(ctr, dir);
                        Handle(Geom_Plane) gpln = new Geom_Plane(pln);
                        Handle(AIS_Shape) face = new AIS_Shape(BRepBuilderAPI_MakeFace(gpln->Pln(), -20, 20, -20, 20).Shape());
                        face->SetTransparency(0.7);
                        face->SetColor(Quantity_NOC_YELLOW);
                        context->Display(face, Standard_True);
                        auxShapes.push_back(face);

                    break;
                }
            }

            Handle(Geom_Circle) geomCirc = new Geom_Circle(m_circ);
            TopoDS_Edge edge = BRepBuilderAPI_MakeEdge(geomCirc);
            mainShape = new AIS_Shape(edge);
            mainShape->SetColor(Quantity_NOC_BLUE1);
            context->Display(mainShape, Standard_True);

        }
        catch (...) {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Error constructing or displaying circle.");
        }
    }

    void ClearAuxShapes(const Handle(AIS_InteractiveContext)& context) {
        for (auto& s : auxShapes) context->Remove(s, Standard_False);
        auxShapes.clear();
    }

    Handle(AIS_Shape) DisplayAuxLine(
        const Handle(AIS_InteractiveContext)& context,
        const gp_Pnt& a,
        const gp_Pnt& b,
        Quantity_NameOfColor color = Quantity_NOC_RED,
        float width = 2.0f) {

        TopoDS_Edge edge = BRepBuilderAPI_MakeEdge(a, b);
        Handle(AIS_Shape) ais = new AIS_Shape(edge);
        ais->SetColor(color);
        ais->SetWidth(width);
        context->Display(ais, Standard_False);
        auxShapes.push_back(ais);
        return ais;
    }
};
