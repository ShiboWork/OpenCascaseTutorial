#pragma once

#include "pch.h"
#include <GeomAPI_Interpolate.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <GeomAPI_PointsToBSpline.hxx>
#include <Vector>
#include "BaseScene.h"
#include "VisSceneComponents.h"
#include "TutorialWindow.h"


class MDCurveInterpolation002 : public BaseScene, public VisSceneComponents, public TutorialWindow {
public:
    // 构造函数中打开教学窗口
    MDCurveInterpolation002() { openTutorialWindow(); }

    void displayScene(const Handle(V3d_View)& view, const Handle(AIS_InteractiveContext)& context) override {

        if (context.IsNull()) {
            return;
        }

        // 初始化场景窗口
        if (!bIsSceneInit) {
            sceneInit(view, context);
            bIsSceneInit = true;
        }
        // 渲染教学交互窗口
        renderTutorialWindow(context);

    }

    void sceneInit(const Handle(V3d_View)& view, const Handle(AIS_InteractiveContext)& context) override {

        // 定义点集
        TColgp_Array1OfPnt demoPoints(1, 4);

        // 填充点数据
        demoPoints(1) = gp_Pnt(3, 5, 0);
        demoPoints(2) = gp_Pnt(4, 6, 0);
        demoPoints(3) = gp_Pnt(1, 7, 0);
        demoPoints(4) = gp_Pnt(7, 3, 0);

        /*
        在 B 样条曲线中，**最低度数（DegMin）和最高度数（DegMax）**指定了曲线的阶数（即曲线的多项式度数）。
        在 GeomAPI_PointsToBSpline 中，这两个参数用于定义生成的 B 样条曲线的度数范围。

        解释：
        DegMin：定义了 B 样条曲线的最低度数。这是 B 样条的一种限制，表示生成的曲线的多项式阶数不能低于此值。
        DegMax：定义了 B 样条曲线的最高度数。它指定了生成曲线的最高度数，
                通常在给定的点集和容差条件下，生成的曲线可能会是该度数或者低于该度数。
        */
        // 定义最低和最高度数
        Standard_Integer DegMin_demo = 2;
        Standard_Integer DegMax_demo = 3;

        // 设置连续性（C2 连续性）
        GeomAbs_Shape Continuity_demo = GeomAbs_C2;

        // 设置容差（可以根据需要调整）
        Standard_Real Tol_demo = 1e-6;

        // 使用 GeomAPI_PointsToBSpline 近似点集并生成 B 样条曲线
        GeomAPI_PointsToBSpline Approx(demoPoints, DegMin_demo, DegMax_demo, Continuity_demo, Tol_demo);

        // 获取生成的 B 样条曲线
        Handle(Geom_BSplineCurve) bsplineCurve = Approx.Curve();

        // 创建 Edge（曲线的拓扑表示）
        BRepBuilderAPI_MakeEdge makeEdge(bsplineCurve);

        // 获取拓扑形状
        TopoDS_Shape shape = makeEdge.Shape();

        // 创建 AIS_Shape
        Handle(AIS_Shape) aisShape = new AIS_Shape(shape);
        aisShape->SetColor(COLOR_RED);

        // 将 AIS_Shape 添加到上下文中
        context->Display(aisShape, Standard_True);
    }

    // 教学类窗口
    // 自定义初始化教中学窗口
    void customInitTutorialWindow(const Handle(AIS_InteractiveContext)& context) override {
        // 初始化显示坐标
        pointCoordinates = std::vector<std::vector<float>>(4, std::vector<float>(3));
        previousPointCoordinates = std::vector<std::vector<float>>(4, std::vector<float>(3));  // 用于存储前一次的坐标

        // 默认填充四个点的初始坐标
        points = TColgp_Array1OfPnt(1, 4);
        points(1) = gp_Pnt(3, 5, 0);
        points(2) = gp_Pnt(4, 6, 0);
        points(3) = gp_Pnt(1, 7, 0);
        points(4) = gp_Pnt(7, 3, 0);

        // 使用 GeomAPI_PointsToBSpline 近似点集并生成 B 样条曲线
        GeomAPI_PointsToBSpline Approx(points, DegMin, DegMax, Continuity, Tol);

        // 获取生成的 B 样条曲线
        Handle(Geom_BSplineCurve) bsplineCurve = Approx.Curve();

        // 创建 Edge（曲线的拓扑表示）
        BRepBuilderAPI_MakeEdge makeEdge(bsplineCurve);

        // 获取拓扑形状
        TopoDS_Shape shape = makeEdge.Shape();

        // 创建 AIS_Shape
        tutorialAisShape = new AIS_Shape(shape);

        tutorialAisShape->SetColor(Quantity_NOC_RED);  // 设置颜色为红色

        context->Display(tutorialAisShape, Standard_True);
    }

    // 教学窗口渲染内容
    void renderTutorialContent(const Handle(AIS_InteractiveContext)& context) override {
        // 重置修改检查标志位
        bool pointsChanged = false;

        // 填充点的初始坐标
        for (int i = 0; i < 4; ++i) {
            pointCoordinates[i][0] = static_cast<float>(points[i + 1].X());
            pointCoordinates[i][1] = static_cast<float>(points[i + 1].Y());
            pointCoordinates[i][2] = static_cast<float>(points[i + 1].Z());
        }

        // 使用 ImGui::InputFloat3 控制四个点的坐标
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "### Modify control point");
        ImGui::InputFloat3("Point 1", pointCoordinates[0].data());
        ImGui::InputFloat3("Point 2", pointCoordinates[1].data());
        ImGui::InputFloat3("Point 3", pointCoordinates[2].data());
        ImGui::InputFloat3("Point 4", pointCoordinates[3].data());

        // 检查是否有坐标变化
        for (int i = 0; i < 4; ++i) {
            if (!arePointsEqual(pointCoordinates[i], previousPointCoordinates[i])) {
                pointsChanged = true;
                break;
            }
        }

        // 如果坐标发生变化，更新控制点和 TColgp_Array1OfPnt
        if (pointsChanged) {
            for (int i = 0; i < 4; ++i) {
                // 更新控制点
                points(i + 1) = gp_Pnt(pointCoordinates[i][0], pointCoordinates[i][1], pointCoordinates[i][2]);
                // 更新 previousPointCoordinates
                previousPointCoordinates[i] = pointCoordinates[i];
            }

            // 使用 GeomAPI_PointsToBSpline 重新生成 B 样条曲线
            GeomAPI_PointsToBSpline Approx(points, DegMin, DegMax, Continuity, Tol);
            Handle(Geom_BSplineCurve) bsplineCurve = Approx.Curve();

            // 创建新的 Edge
            BRepBuilderAPI_MakeEdge makeEdge(bsplineCurve);
            TopoDS_Shape shape = makeEdge.Shape();

            // 更新 AIS_Shape
            tutorialAisShape->SetShape(shape);

            // 刷新显示
            context->Redisplay(tutorialAisShape, Standard_True);
        }
    }

private:
    // 比较两个三维坐标是否相等
    static bool arePointsEqual(const std::vector<float>& point1, const std::vector<float>& point2, double tolerance = 1e-6) {
        return std::abs(point1[0] - point2[0]) < tolerance &&
            std::abs(point1[1] - point2[1]) < tolerance &&
            std::abs(point1[2] - point2[2]) < tolerance;
    }

private:
    // 教学窗口成员变量
    TColgp_Array1OfPnt points;
    // 定义最低和最高度数
    Standard_Integer DegMin = 2;
    Standard_Integer DegMax = 3;
    // 设置连续性（C2 连续性）
    GeomAbs_Shape Continuity = GeomAbs_C2;
    // 设置容差（可以根据需要调整）
    Standard_Real Tol = Precision::Confusion();

    std::vector<std::vector<float>> pointCoordinates;
    std::vector<std::vector<float>> previousPointCoordinates;

    Handle(AIS_Shape) tutorialAisShape; // 显示对象
};