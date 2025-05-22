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
    // ���캯���д򿪽�ѧ����
    MDCurveInterpolation002() { openTutorialWindow(); }

    void displayScene(const Handle(V3d_View)& view, const Handle(AIS_InteractiveContext)& context) override {

        if (context.IsNull()) {
            return;
        }

        // ��ʼ����������
        if (!bIsSceneInit) {
            sceneInit(view, context);
            bIsSceneInit = true;
        }
        // ��Ⱦ��ѧ��������
        renderTutorialWindow(context);

    }

    void sceneInit(const Handle(V3d_View)& view, const Handle(AIS_InteractiveContext)& context) override {

        // ����㼯
        TColgp_Array1OfPnt demoPoints(1, 4);

        // ��������
        demoPoints(1) = gp_Pnt(3, 5, 0);
        demoPoints(2) = gp_Pnt(4, 6, 0);
        demoPoints(3) = gp_Pnt(1, 7, 0);
        demoPoints(4) = gp_Pnt(7, 3, 0);

        /*
        �� B ���������У�**��Ͷ�����DegMin������߶�����DegMax��**ָ�������ߵĽ����������ߵĶ���ʽ��������
        �� GeomAPI_PointsToBSpline �У��������������ڶ������ɵ� B �������ߵĶ�����Χ��

        ���ͣ�
        DegMin�������� B �������ߵ���Ͷ��������� B ������һ�����ƣ���ʾ���ɵ����ߵĶ���ʽ�������ܵ��ڴ�ֵ��
        DegMax�������� B �������ߵ���߶�������ָ�����������ߵ���߶�����
                ͨ���ڸ����ĵ㼯���ݲ������£����ɵ����߿��ܻ��Ǹö������ߵ��ڸö�����
        */
        // ������ͺ���߶���
        Standard_Integer DegMin_demo = 2;
        Standard_Integer DegMax_demo = 3;

        // ���������ԣ�C2 �����ԣ�
        GeomAbs_Shape Continuity_demo = GeomAbs_C2;

        // �����ݲ���Ը�����Ҫ������
        Standard_Real Tol_demo = 1e-6;

        // ʹ�� GeomAPI_PointsToBSpline ���Ƶ㼯������ B ��������
        GeomAPI_PointsToBSpline Approx(demoPoints, DegMin_demo, DegMax_demo, Continuity_demo, Tol_demo);

        // ��ȡ���ɵ� B ��������
        Handle(Geom_BSplineCurve) bsplineCurve = Approx.Curve();

        // ���� Edge�����ߵ����˱�ʾ��
        BRepBuilderAPI_MakeEdge makeEdge(bsplineCurve);

        // ��ȡ������״
        TopoDS_Shape shape = makeEdge.Shape();

        // ���� AIS_Shape
        Handle(AIS_Shape) aisShape = new AIS_Shape(shape);
        aisShape->SetColor(COLOR_RED);

        // �� AIS_Shape ��ӵ���������
        context->Display(aisShape, Standard_True);
    }

    // ��ѧ�ര��
    // �Զ����ʼ������ѧ����
    void customInitTutorialWindow(const Handle(AIS_InteractiveContext)& context) override {
        // ��ʼ����ʾ����
        pointCoordinates = std::vector<std::vector<float>>(4, std::vector<float>(3));
        previousPointCoordinates = std::vector<std::vector<float>>(4, std::vector<float>(3));  // ���ڴ洢ǰһ�ε�����

        // Ĭ������ĸ���ĳ�ʼ����
        points = TColgp_Array1OfPnt(1, 4);
        points(1) = gp_Pnt(3, 5, 0);
        points(2) = gp_Pnt(4, 6, 0);
        points(3) = gp_Pnt(1, 7, 0);
        points(4) = gp_Pnt(7, 3, 0);

        // ʹ�� GeomAPI_PointsToBSpline ���Ƶ㼯������ B ��������
        GeomAPI_PointsToBSpline Approx(points, DegMin, DegMax, Continuity, Tol);

        // ��ȡ���ɵ� B ��������
        Handle(Geom_BSplineCurve) bsplineCurve = Approx.Curve();

        // ���� Edge�����ߵ����˱�ʾ��
        BRepBuilderAPI_MakeEdge makeEdge(bsplineCurve);

        // ��ȡ������״
        TopoDS_Shape shape = makeEdge.Shape();

        // ���� AIS_Shape
        tutorialAisShape = new AIS_Shape(shape);

        tutorialAisShape->SetColor(Quantity_NOC_RED);  // ������ɫΪ��ɫ

        context->Display(tutorialAisShape, Standard_True);
    }

    // ��ѧ������Ⱦ����
    void renderTutorialContent(const Handle(AIS_InteractiveContext)& context) override {
        // �����޸ļ���־λ
        bool pointsChanged = false;

        // ����ĳ�ʼ����
        for (int i = 0; i < 4; ++i) {
            pointCoordinates[i][0] = static_cast<float>(points[i + 1].X());
            pointCoordinates[i][1] = static_cast<float>(points[i + 1].Y());
            pointCoordinates[i][2] = static_cast<float>(points[i + 1].Z());
        }

        // ʹ�� ImGui::InputFloat3 �����ĸ��������
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "### Modify control point");
        ImGui::InputFloat3("Point 1", pointCoordinates[0].data());
        ImGui::InputFloat3("Point 2", pointCoordinates[1].data());
        ImGui::InputFloat3("Point 3", pointCoordinates[2].data());
        ImGui::InputFloat3("Point 4", pointCoordinates[3].data());

        // ����Ƿ�������仯
        for (int i = 0; i < 4; ++i) {
            if (!arePointsEqual(pointCoordinates[i], previousPointCoordinates[i])) {
                pointsChanged = true;
                break;
            }
        }

        // ������귢���仯�����¿��Ƶ�� TColgp_Array1OfPnt
        if (pointsChanged) {
            for (int i = 0; i < 4; ++i) {
                // ���¿��Ƶ�
                points(i + 1) = gp_Pnt(pointCoordinates[i][0], pointCoordinates[i][1], pointCoordinates[i][2]);
                // ���� previousPointCoordinates
                previousPointCoordinates[i] = pointCoordinates[i];
            }

            // ʹ�� GeomAPI_PointsToBSpline �������� B ��������
            GeomAPI_PointsToBSpline Approx(points, DegMin, DegMax, Continuity, Tol);
            Handle(Geom_BSplineCurve) bsplineCurve = Approx.Curve();

            // �����µ� Edge
            BRepBuilderAPI_MakeEdge makeEdge(bsplineCurve);
            TopoDS_Shape shape = makeEdge.Shape();

            // ���� AIS_Shape
            tutorialAisShape->SetShape(shape);

            // ˢ����ʾ
            context->Redisplay(tutorialAisShape, Standard_True);
        }
    }

private:
    // �Ƚ�������ά�����Ƿ����
    static bool arePointsEqual(const std::vector<float>& point1, const std::vector<float>& point2, double tolerance = 1e-6) {
        return std::abs(point1[0] - point2[0]) < tolerance &&
            std::abs(point1[1] - point2[1]) < tolerance &&
            std::abs(point1[2] - point2[2]) < tolerance;
    }

private:
    // ��ѧ���ڳ�Ա����
    TColgp_Array1OfPnt points;
    // ������ͺ���߶���
    Standard_Integer DegMin = 2;
    Standard_Integer DegMax = 3;
    // ���������ԣ�C2 �����ԣ�
    GeomAbs_Shape Continuity = GeomAbs_C2;
    // �����ݲ���Ը�����Ҫ������
    Standard_Real Tol = Precision::Confusion();

    std::vector<std::vector<float>> pointCoordinates;
    std::vector<std::vector<float>> previousPointCoordinates;

    Handle(AIS_Shape) tutorialAisShape; // ��ʾ����
};