#pragma once

#include "BaseScene.h"
#include "VisSceneComponents.h"
#include "TutorialWindow.h"

class MDPointAndVector001 : public BaseScene, public VisSceneComponents, public TutorialWindow {
public:
	MDPointAndVector001() { openTutorialWindow(); }

	void displayScene(const Handle(V3d_View)& view, const Handle(AIS_InteractiveContext)& context) override {
		//<创建起始点
		// 1. 使用默认构造函数创建一个零点
		gp_Pnt point1;
		// 2. 使用三维坐标创建一个点
		gp_Pnt point2(1.0, 2.0, 3.0);
		// 3. 使用另外一个点来构造新点
		gp_Pnt point3 = point2;

		//<创建Vector
		// 1. 使用默认构造函数创建一个零向量
		gp_Vec vec1;
		// 2. 使用两个点之间的差值创建向量
		gp_Pnt p1(2.0, 3.0, 4.0);
		gp_Pnt p2(1.0, 3.0, 5.0);
		gp_Vec vec2(p1, p2);  // vec2 = p2 - p1
		// 3. 使用三维坐标直接创建一个向量
		gp_Vec vec3(7.0, 8.0, 9.0);

		//<向量计算演示
		// 计算两个向量之间的加法
		gp_Vec vec4 = vec2 + vec3;  // vec4 = vec2 + vec3

		//<可视化
		visPoint(point2, context);  // 可视化点
		visVector(gp_Pnt(7.0, 8.0, 9.0), vec2, context, makeColor(0.4157, 0.5020, 0.7255));    // 可视化向量vec2
		visVector(point1, vec3, context, makeColor(0.5333, 0.6196, 0.4510));    // 可视化向量加法结果
		visVector(point1, vec4, context, makeColor(0.6627, 0.2902, 0.2902));    // 可视化向量叉积结果

		//<可视化教学窗口
		renderTutorialWindow(context);
	}

	void customInitTutorialWindow(const Handle(AIS_InteractiveContext)& context) override {
		// 初始化点
		x = 10.0f;
		y = 10.0f;
		z = 10.0f;
		visPnt = gp_Pnt(x, y, z);
		lastPnt = visPnt;

		// 设置小球的半径
		double radius = 5.0;  // 半径为5，可以根据需要修改

		// 使用BRepBuilderAPI_MakeVertex构建一个顶点
		BRepPrimAPI_MakeSphere  makeSphere(visPnt, radius);
		TopoDS_Shape shape = makeSphere.Shape(); // 创建一个TopoDS_Shape对象

		// 创建AIS_Shape对象
		aisShape = new AIS_Shape(shape);
		aisShape->SetColor(COLOR_RED);

		// 将显示模式设置为表面（Surface）模式
		aisShape->SetDisplayMode(AIS_Shaded);  // 显示为表面

		// 假设你已经在context中添加了该AIS_Shape
		context->Display(aisShape, Standard_True);
	}

	// 教学窗口内容
	void renderTutorialContent(const Handle(AIS_InteractiveContext)& context) override {
		// 使用ImGui创建滑动条来控制x、y、z坐标
		ImGui::InputFloat("X", &x, 0.0f, 0.0f, "%.3f");  // 控制X坐标，步长为10
		ImGui::InputFloat("Y", &y, 0.0f, 0.0f, "%.3f");  // 控制Y坐标，步长为10
		ImGui::InputFloat("Z", &z, 0.0f, 0.0f, "%.3f");  // 控制Z坐标，步长为10

		// 更新可视化点
		visPnt.SetCoord(x, y, z);

		if (!visPnt.IsEqual(lastPnt, Precision::Confusion())) {
			// 创建一个变换矩阵，平移到新的位置
			gp_Trsf transform;
			transform.SetTranslation(gp_Vec(lastPnt, visPnt)); // 计算偏移量

			// 将变换应用于AIS_Shape
			aisShape->SetLocalTransformation(transform);

			// 更新坐标与显示
			lastPnt = visPnt;
			context->Redisplay(aisShape, Standard_True);
		}

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "### gp_Pnt and gp_Vector Overview");
		ImGui::Text("gp_Pnt and gp_Vector are fundamental classes in the Open Cascade (OCC) geometry modeling library, used to represent points and vectors in geometric space.");

		ImGui::Spacing();
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "### gp_Pnt Class:");
		ImGui::Text("gp_Pnt is used to represent a point in three-dimensional space, defined by three coordinates (x, y, z). This point typically indicates a position in 3D space.");

		ImGui::BulletText("Constructor: Create a point with given coordinates:");
		ImGui::Text("gp_Pnt(double X, double Y, double Z);");
		ImGui::BulletText("Default Constructor: Create the origin (0, 0, 0):");
		ImGui::Text("gp_Pnt();");

		ImGui::BulletText("Member Functions:");
		ImGui::BulletText("X(), Y(), Z() - Get the x, y, and z coordinates of the point.");
		ImGui::BulletText("Distance(const gp_Pnt&) - Calculate the distance between the current point and another point.");
		ImGui::BulletText("Transformed(const gp_Trsf&) - Apply a transformation matrix to the point.");

		ImGui::Spacing();
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "### gp_Vector Class:");
		ImGui::Text("gp_Vector is used to represent a vector in three-dimensional space. A vector has both direction and magnitude, typically representing the relative displacement between two points or motion in space.");

		ImGui::BulletText("Constructor: Create a vector with given coordinates:");
		ImGui::Text("gp_Vector(double X, double Y, double Z);");
		ImGui::BulletText("Alternatively, create a vector using two points:");
		ImGui::Text("gp_Vector(const gp_Pnt& P1, const gp_Pnt& P2);");

		ImGui::BulletText("Member Functions:");
		ImGui::BulletText("Magnitude() - Get the magnitude (size) of the vector.");
		ImGui::BulletText("Normalize() - Normalize the vector to a unit vector.");
		ImGui::BulletText("Crossed(const gp_Vector&) - Calculate the cross product with another vector.");
		ImGui::BulletText("Dot(const gp_Vector&) - Calculate the dot product with another vector.");

		ImGui::Spacing();
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "### Relationship and Differences:");
		ImGui::Text("gp_Pnt represents a point with specific coordinates, while gp_Vector represents a vector, which has direction and magnitude, often used to express the relationship or changes between two points.");
		ImGui::Text("In Open Cascade, gp_Pnt and gp_Vector are commonly used together. For example, a gp_Vector can represent the direction and distance from one point to another, or it can be applied to a gp_Pnt to yield a new point.");

	}

	void sceneInit(const Handle(V3d_View)& view, const Handle(AIS_InteractiveContext)& context) override {
		// No need to Init scene
	};

private:
	gp_Pnt visPnt;
	gp_Pnt lastPnt;	// 用于避免重复刷新

	Handle(AIS_Shape) aisShape; // 显示对象

	// VisPnt的坐标
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
};