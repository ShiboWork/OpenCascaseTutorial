#pragma once

#include "pch.h"
#include <format>
#include <string>
#include <mutex>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepPrimAPI_MakeSphere.hxx> // 新增头文件
#include <BRepLib_ToolTriangulatedShape.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <TopExp_Explorer.hxx>
#include <Poly_Triangulation.hxx>
#include <AIS_Shape.hxx>
#include <TopoDS.hxx>
#include "BaseScene.h"
#include "VisSceneComponents.h"
#include "TutorialWindow.h"
#include "CodeHintAction.h"

class BRepMesh001 : public BaseScene, public VisSceneComponents, public TutorialWindow {
public:
    BRepMesh001() {
        openTutorialWindow();
    }
     
    void displayScene(const Handle(V3d_View)& view, const Handle(AIS_InteractiveContext)& context) override {
        if (!bIsSceneInit) {
            sceneInit(view, context);
            bIsSceneInit = true;
        }

        if (bMeshDirty) {
            updateMesh(context);
            bMeshDirty = false; 
        }

        renderTutorialWindow(context);
    }

    void customInitTutorialWindow(const Handle(AIS_InteractiveContext)&) override {}

private:
    Handle(AIS_Shape)       aisShape;
    CodeHintManager         hintMgr;  
    TopoDS_Shape            meshShape;
    IMeshTools_Parameters   meshParams;
    std::mutex              meshMutex;

    float linearDeflection = 1.0f;
    float angularDeflectionDeg = 30.0f;
    float minSize = 0.0f;
    bool isProcessing = false;       // 标记是否正在处理


    bool bMeshDirty = false; // 参数变化刷新标志位

    void sceneInit(const Handle(V3d_View)&, const Handle(AIS_InteractiveContext)& context) override {
        meshShape =  BRepPrimAPI_MakeSphere(50.0).Shape();
        updateMesh(context);
    }

    void updateMesh(const Handle(AIS_InteractiveContext)& context) {

        std::lock_guard<std::mutex> lock(meshMutex); // 自动加锁

        // 开始处理
        isProcessing = true;
        clearTriangulation(meshShape);

        meshParams.Deflection = linearDeflection;
        meshParams.Angle = angularDeflectionDeg * M_PI / 180.0;
        meshParams.InParallel = true;

        BRepMesh_IncrementalMesh mesher;
        mesher.SetShape(meshShape);
        mesher.ChangeParameters() = meshParams;
        mesher.Perform();

        // 添加网格生成验证
        if (!mesher.IsDone()) {
            std::cerr << "Mesh generation failed!" << std::endl;
            return;
        }

        aisShape = new AIS_Shape(meshShape);
        // 强制设置显示模式为网格
        //aisShape->SetDisplayMode(AIS_Shaded);  // 或 AIS_WireFrame
        aisShape->Attributes()->SetFaceBoundaryDraw(true);
        context->RemoveAll(true);
        context->Display(aisShape, true);
        context->UpdateCurrentViewer();  // 强制刷新视图

        drawTriangulations(meshShape, context);
    }

    void clearTriangulation(const TopoDS_Shape& shape) {
        for (TopExp_Explorer exp(shape, TopAbs_FACE); exp.More(); exp.Next()) {
            TopoDS_Face face = TopoDS::Face(exp.Current());
            BRepTools::Clean(face); // 移除 Poly_Triangulation
        }
    }

    void renderTutorialContent(const Handle(AIS_InteractiveContext)& context) override {
        ImGui::Text("BRepMesh_IncrementalMesh 教学示例");
        ImGui::Separator();


        float oldLinearDeflection = linearDeflection;
        if (ImGui::SliderFloat("Linear Deflection", &linearDeflection, 0.1f, 0.7f, "%.2f")) {
            if (oldLinearDeflection != linearDeflection) {
                bMeshDirty = true;
            }
        }

        float oldAngularDeflectionDeg = angularDeflectionDeg;
        if (ImGui::SliderFloat("Angular Deflection (deg)", &angularDeflectionDeg, 5.0f, 11.0f, "%.1f")) {
            if (oldAngularDeflectionDeg != angularDeflectionDeg) {
                bMeshDirty = true;
            }
        }

        if (ImGui::Button("Clear Triangulation")) {
            BRepTools::Clean(meshShape);
            context->RemoveAll(true);
            hintMgr.AddHint("BRepTools::Clean", "BRepTools::Clean(meshShape);");
        }

        if (ImGui::Button("Reset Default")) {
            linearDeflection = 1.0f;
            angularDeflectionDeg = 30.0f;
            minSize = 0.0f;
            updateMesh(context);
        }

        if (aisShape) {
            ImGui::Separator();
            ImGui::Text("Triangulation Info:");

            int totalTriangles = 0;
            for (TopExp_Explorer exp(meshShape, TopAbs_FACE); exp.More(); exp.Next()) {
                TopLoc_Location loc;
                Handle(Poly_Triangulation) tri = BRep_Tool::Triangulation(TopoDS::Face(exp.Current()), loc);
                if (!tri.IsNull()) {
                    totalTriangles += tri->NbTriangles();
                }
            }
            ImGui::Text("Total Triangles: %d", totalTriangles);
        }
    }

    void drawTriangulations(const TopoDS_Shape& shape, const Handle(AIS_InteractiveContext)& context) {
        for (TopExp_Explorer faceExp(shape, TopAbs_FACE); faceExp.More(); faceExp.Next()) {
            TopoDS_Face face = TopoDS::Face(faceExp.Current());
            TopLoc_Location loc;
            Handle(Poly_Triangulation) tri = BRep_Tool::Triangulation(face, loc);

            if (tri.IsNull()) continue;

            const Poly_Array1OfTriangle& triangles = tri->Triangles();

            for (int i = triangles.Lower(); i <= triangles.Upper(); ++i) {
                int n1, n2, n3;
                triangles(i).Get(n1, n2, n3);

                // 使用 tri->Node(index) 获取三角形顶点坐标
                gp_Pnt p1 = tri->Node(n1).Transformed(loc.Transformation());
                gp_Pnt p2 = tri->Node(n2).Transformed(loc.Transformation());
                gp_Pnt p3 = tri->Node(n3).Transformed(loc.Transformation());

                // 创建一个三角形边线展示
                BRepBuilderAPI_MakePolygon triPoly;
                triPoly.Add(p1);
                triPoly.Add(p2);
                triPoly.Add(p3);
                triPoly.Add(p1);
                triPoly.Close();

                Handle(AIS_Shape) aisTri = new AIS_Shape(triPoly.Shape());
                aisTri->SetDisplayMode(AIS_WireFrame);
                aisTri->SetColor(Quantity_NOC_RED);
                context->Display(aisTri, false);
            }
        }

        context->UpdateCurrentViewer();
    }

};
