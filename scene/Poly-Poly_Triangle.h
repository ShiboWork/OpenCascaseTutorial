#pragma once

#include "pch.h"
#include <Poly_Triangulation.hxx>
#include <AIS_Triangulation.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include "BaseScene.h"
#include "VisSceneComponents.h"
#include "TutorialWindow.h"

class PolyTriangle : public BaseScene, public VisSceneComponents, public TutorialWindow {
public:
    PolyTriangle() {
        openTutorialWindow();
        initAllNodes();
    }

    void displayScene(const Handle(V3d_View)& view, const Handle(AIS_InteractiveContext)& context) override {
        if (!bIsSceneInit) {
            sceneInit(view, context);
            bIsSceneInit = true;
        }
        renderTutorialWindow(context);
    }

    void customInitTutorialWindow(const Handle(AIS_InteractiveContext)&) override {}

private:
    std::vector<TColgp_Array1OfPnt> allNodes;
    int currentNodeSetIndex = 0;

    Handle(Poly_Triangulation) triangulation;
    Handle(AIS_Triangulation) aisTriangulation;
    std::vector<Handle(AIS_Shape)> vertexMarkers;

    int triIndices[3] = { 1, 2, 3 };
    bool needsUpdate = true;

    void initAllNodes() {
        allNodes.clear();

        // 节点集 1: 正三角形
        TColgp_Array1OfPnt nodes1(1, 3);
        nodes1.SetValue(1, gp_Pnt(0, 0, 0));
        nodes1.SetValue(2, gp_Pnt(100, 0, 0));
        nodes1.SetValue(3, gp_Pnt(50, 100, 0));
        allNodes.push_back(nodes1);

        // 节点集 2: 矩形
        TColgp_Array1OfPnt nodes2(1, 4);
        nodes2.SetValue(1, gp_Pnt(0, 0, 0));
        nodes2.SetValue(2, gp_Pnt(100, 0, 0));
        nodes2.SetValue(3, gp_Pnt(100, 100, 0));
        nodes2.SetValue(4, gp_Pnt(0, 100, 0));
        allNodes.push_back(nodes2);

        // 节点集 3: 10 个点（圆形分布）
        TColgp_Array1OfPnt nodes3(1, 10);
        for (int i = 0; i < 10; ++i) {
            double angle = 2.0 * M_PI * i / 10.0;
            nodes3.SetValue(i + 1, gp_Pnt(100 * cos(angle), 100 * sin(angle), 0));
        }
        allNodes.push_back(nodes3);
    }

    void sceneInit(const Handle(V3d_View)&, const Handle(AIS_InteractiveContext)& context) override {
        updateVisualization(context);
    }

    void renderTutorialContent(const Handle(AIS_InteractiveContext)& context) override {
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Poly_Triangulation Multi-point Construct Example");

        // 点集选择 UI（非 CollapsingHeader）
        ImGui::SeparatorText("Point Set Selection");
        const char* labels[] = { "Equilateral Triangle (3 pts)", "Rectangle (4 pts)", "Circle (10 pts)" };
        if (ImGui::Combo("Current Point Set", &currentNodeSetIndex, labels, IM_ARRAYSIZE(labels))) {
            needsUpdate = true;

            // 重置三角形索引在合法范围内
            int lower = allNodes[currentNodeSetIndex].Lower();
            int upper = allNodes[currentNodeSetIndex].Upper();
            for (int i = 0; i < 3; ++i) {
                triIndices[i] = std::clamp(lower + i, lower, upper);
            }
        }

        if (triIndices[0] == triIndices[1] ||
            triIndices[0] == triIndices[2] ||
            triIndices[1] == triIndices[2]) {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Three vertices must be unique!");
        }

        // 三角形顶点选择 UI（默认展开）
        if (ImGui::CollapsingHeader("Triangle Index Selection", ImGuiTreeNodeFlags_DefaultOpen)) {
            int lower = allNodes[currentNodeSetIndex].Lower();
            int upper = allNodes[currentNodeSetIndex].Upper();
            for (int i = 0; i < 3; ++i) {
                std::string label = "Vertex Index " + std::to_string(i + 1);
                if (ImGui::SliderInt(label.c_str(), &triIndices[i], lower, upper)) {
                    needsUpdate = true; // 用户拖动滑块后设置更新标志
                }
            }
        }

        if (needsUpdate) {
            updateVisualization(context);
            needsUpdate = false;
        }
    }

    void updateVisualization(const Handle(AIS_InteractiveContext)& context) {
        // 清理之前的显示
        for (auto& v : vertexMarkers)
            context->Remove(v, false);
        vertexMarkers.clear();

        if (!triangulation.IsNull())
            context->Remove(aisTriangulation, false);

        // 当前节点集
        const auto& nodes = allNodes[currentNodeSetIndex];
        int n = nodes.Length();

        triangulation = new Poly_Triangulation(n, 1, false);
        for (int i = nodes.Lower(); i <= nodes.Upper(); ++i)
            triangulation->SetNode(i, nodes(i));

        triangulation->SetTriangle(1, Poly_Triangle(triIndices[0], triIndices[1], triIndices[2]));

        for (int i = nodes.Lower(); i <= nodes.Upper(); ++i) {
            TopoDS_Vertex v = BRepBuilderAPI_MakeVertex(nodes(i));
            auto ais = new AIS_Shape(v);
            ais->SetColor(Quantity_NOC_RED1);
            context->Display(ais, false);
            vertexMarkers.push_back(ais);
        }

        aisTriangulation = new AIS_Triangulation(triangulation);
        aisTriangulation->SetTransparency(0.4f);
        context->Display(aisTriangulation, true);
    }
};
