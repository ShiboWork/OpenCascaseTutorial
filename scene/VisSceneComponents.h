#pragma once

#include "pch.h"

#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <Geom2d_BSplineCurve.hxx>

#define COLOR_RED makeColor(0.6627, 0.2902, 0.2902)

// 使用 inline 函数代替宏来创建颜色
inline Quantity_Color makeColor(double R, double G, double B) {
    return Quantity_Color(R, G, B, Quantity_TOC_RGB);
}

class VisSceneComponents {
public:
    // 构造函数
    VisSceneComponents() = default;

    // 可视化一个点
    void visPoint(const gp_Pnt& point, const Handle(AIS_InteractiveContext)& context) {
        Handle(AIS_Shape) pointShape = new AIS_Shape(BRepBuilderAPI_MakeVertex(point).Shape());
        pointShape->SetColor(COLOR_RED);
        context->Display(pointShape, Standard_True);
    }

    // 可视化一个向量，默认颜色为红色
    void visVector(const gp_Pnt& startPoint, 
                    const gp_Vec& vector, 
                    const Handle(AIS_InteractiveContext)& context, 
                    const Quantity_Color& color = COLOR_RED) {
        gp_Pnt endPoint = startPoint.Translated(vector);
        Handle(AIS_Shape) edgeShape = new AIS_Shape(BRepBuilderAPI_MakeEdge(startPoint, endPoint).Shape());
        // 设置用户传入的颜色（默认为红色）
        edgeShape->SetColor(color);
        context->Display(edgeShape, Standard_True);
    }
};