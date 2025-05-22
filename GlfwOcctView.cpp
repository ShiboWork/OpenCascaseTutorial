// MIT License
// 
// Copyright(c) 2023 Shing Liu
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "GlfwOcctView.h"

#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <AIS_Shape.hxx>
#include <Aspect_Handle.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <TopAbs_ShapeEnum.hxx>

#include <iostream>

#include <GLFW/glfw3.h>

#include "scene/SceneSelector.h"

#define SHOW_DEMO   false   // 显示Imguidemo窗口
#define SHOW_LOG    flase   // 显示Log窗口

namespace
{
    //! Convert GLFW mouse button into Aspect_VKeyMouse.
    static Aspect_VKeyMouse mouseButtonFromGlfw(int theButton)
    {
        switch (theButton)
        {
        case GLFW_MOUSE_BUTTON_LEFT:   return Aspect_VKeyMouse_LeftButton;
        case GLFW_MOUSE_BUTTON_RIGHT:  return Aspect_VKeyMouse_RightButton;
        case GLFW_MOUSE_BUTTON_MIDDLE: return Aspect_VKeyMouse_MiddleButton;
        }
        return Aspect_VKeyMouse_NONE;
    }

    //! Convert GLFW key modifiers into Aspect_VKeyFlags.
    static Aspect_VKeyFlags keyFlagsFromGlfw(int theFlags)
    {
        Aspect_VKeyFlags aFlags = Aspect_VKeyFlags_NONE;
        if ((theFlags & GLFW_MOD_SHIFT) != 0)
        {
            aFlags |= Aspect_VKeyFlags_SHIFT;
        }
        if ((theFlags & GLFW_MOD_CONTROL) != 0)
        {
            aFlags |= Aspect_VKeyFlags_CTRL;
        }
        if ((theFlags & GLFW_MOD_ALT) != 0)
        {
            aFlags |= Aspect_VKeyFlags_ALT;
        }
        if ((theFlags & GLFW_MOD_SUPER) != 0)
        {
            aFlags |= Aspect_VKeyFlags_META;
        }
        return aFlags;
    }
}

// ================================================================
// Function : GlfwOcctView
// Purpose  :
// ================================================================
GlfwOcctView::GlfwOcctView()
{
}

// ================================================================
// Function : ~GlfwOcctView
// Purpose  :
// ================================================================
GlfwOcctView::~GlfwOcctView()
{
}

// ================================================================
// Function : toView
// Purpose  :
// ================================================================
GlfwOcctView* GlfwOcctView::toView(GLFWwindow* theWin)
{
    return static_cast<GlfwOcctView*>(glfwGetWindowUserPointer(theWin));
}

// ================================================================
// Function : errorCallback
// Purpose  :
// ================================================================
void GlfwOcctView::errorCallback(int theError, const char* theDescription)
{
    Message::DefaultMessenger()->Send(TCollection_AsciiString("Error") + theError + ": " + theDescription, Message_Fail);
}

// ================================================================
// Function : run
// Purpose  :
// ================================================================
void GlfwOcctView::run()
{
    initWindow(1920, 1080, "OCCT IMGUI Tutorial");
    initViewer();
    initDemoScene();
    if (myView.IsNull())
    {
        return;
    }

    myView->MustBeResized();
    myOcctWindow->Map();
    initGui();
    mainloop();
    cleanup();
}

// ================================================================
// Function : initWindow
// Purpose  :
// ================================================================
void GlfwOcctView::initWindow(int theWidth, int theHeight, const char* theTitle)
{
    glfwSetErrorCallback(GlfwOcctView::errorCallback);
    glfwInit();
    const bool toAskCoreProfile = true;
    if (toAskCoreProfile)
    {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#if defined (__APPLE__)
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        //glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, true);
        //glfwWindowHint(GLFW_DECORATED, GL_FALSE);
    }
    myOcctWindow = new GlfwOcctWindow(theWidth, theHeight, theTitle);
    glfwSetWindowUserPointer(myOcctWindow->getGlfwWindow(), this);

    // window callback
    glfwSetWindowSizeCallback(myOcctWindow->getGlfwWindow(), GlfwOcctView::onResizeCallback);
    glfwSetFramebufferSizeCallback(myOcctWindow->getGlfwWindow(), GlfwOcctView::onFBResizeCallback);

    // mouse callback
    glfwSetScrollCallback(myOcctWindow->getGlfwWindow(), GlfwOcctView::onMouseScrollCallback);
    glfwSetMouseButtonCallback(myOcctWindow->getGlfwWindow(), GlfwOcctView::onMouseButtonCallback);
    glfwSetCursorPosCallback(myOcctWindow->getGlfwWindow(), GlfwOcctView::onMouseMoveCallback);
}

// ================================================================
// Function : initViewer
// Purpose  :
// ================================================================
void GlfwOcctView::initViewer()
{
    if (myOcctWindow.IsNull()
        || myOcctWindow->getGlfwWindow() == nullptr)
    {
        return;
    }

    Handle(OpenGl_GraphicDriver) aGraphicDriver
        = new OpenGl_GraphicDriver(myOcctWindow->GetDisplay(), Standard_False);
    aGraphicDriver->SetBuffersNoSwap(Standard_True);

    Handle(V3d_Viewer) aViewer = new V3d_Viewer(aGraphicDriver);
    aViewer->SetDefaultLights();
    aViewer->SetLightOn();
    aViewer->SetDefaultTypeOfView(V3d_PERSPECTIVE);
    aViewer->ActivateGrid(Aspect_GT_Rectangular, Aspect_GDM_Lines);
    myView = aViewer->CreateView();
    myView->SetImmediateUpdate(Standard_False);
    myView->SetWindow(myOcctWindow, myOcctWindow->NativeGlContext());
    myView->ChangeRenderingParams().ToShowStats = Standard_True;

    myContext = new AIS_InteractiveContext(aViewer);
}

void GlfwOcctView::initGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& aIO = ImGui::GetIO();
    aIO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    aIO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    aIO.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // 设置字体大小，使用默认字体
    ImGui::GetIO().Fonts->AddFontFromFileTTF("misc/fonts/Montserrat/static/Montserrat-Regular.ttf", 18.0f);

    ImGui_ImplGlfw_InitForOpenGL(myOcctWindow->getGlfwWindow(), Standard_True);
    ImGui_ImplOpenGL3_Init("#version 330");

    m_log = OCCImguiTutorial::Log();

    // Setup Dear ImGui style.
    //ImGui::StyleColorsClassic();
    //ImGui::StyleColorsDark();
}

void GlfwOcctView::renderGui()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ShowAppDockSpace();

    // Demo窗口
#if SHOW_DEMO
    ImGui::ShowDemoWindow();
#endif // SHOW_DEMO

    //// 渲染日志窗口
#if SHOW_LOG
    renderLogWindow();
#endif

    // 场景切换
    sceneSelector.renderGui();
    currentScene = sceneSelector.getCurrentScene();
    autoClearPreviousScene();
    if (currentScene) {
        currentScene->setLog(&m_log);
        currentScene->displayScene(myView, myContext);
        myContext->UpdateCurrentViewer();
        previousScene = currentScene;
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(myOcctWindow->getGlfwWindow());
}

// ================================================================
// Function : initDemoScene
// Purpose  :
// ================================================================
void GlfwOcctView::initDemoScene()
{
    if (myContext.IsNull())
    {
        return;
    }

    myView->TriedronDisplay(Aspect_TOTP_LEFT_LOWER, Quantity_NOC_GOLD, 0.08, V3d_WIREFRAME);

    gp_Ax2 anAxis;
    anAxis.SetLocation(gp_Pnt(0.0, 0.0, 0.0));
    //Handle(AIS_Shape) aBox = new AIS_Shape(BRepPrimAPI_MakeBox(anAxis, 50, 50, 50).Shape());
    //myContext->Display(aBox, AIS_Shaded, 0, false);
    anAxis.SetLocation(gp_Pnt(25.0, 125.0, 0.0));
    Handle(AIS_Shape) aCone = new AIS_Shape(BRepPrimAPI_MakeCone(anAxis, 25, 0, 50).Shape());
    myContext->Display(aCone, AIS_Shaded, 0, false);

    TCollection_AsciiString aGlInfo;
    {
        TColStd_IndexedDataMapOfStringString aRendInfo;
        myView->DiagnosticInformation(aRendInfo, Graphic3d_DiagnosticInfo_Basic);
        for (TColStd_IndexedDataMapOfStringString::Iterator aValueIter(aRendInfo); aValueIter.More(); aValueIter.Next())
        {
            if (!aGlInfo.IsEmpty()) { aGlInfo += "\n"; }
            aGlInfo += TCollection_AsciiString("  ") + aValueIter.Key() + ": " + aValueIter.Value();
        }
    }
    Message::DefaultMessenger()->Send(TCollection_AsciiString("OpenGL info:\n") + aGlInfo, Message_Info);
}

// Render Log window
// Demonstrate creating a simple log window with basic filtering.
void GlfwOcctView::renderLogWindow()
{

    // For the demo: add a debug button _BEFORE_ the normal log window contents
    // We take advantage of a rarely used feature: multiple calls to Begin()/End() are appending to the _same_ window.
    // Most of the contents of the window will be added by the log.Draw() call.
    //ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
    //ImGui::Begin("Log Window");
    //ImGui::End();

    // Actually call in the regular Log helper (which will Begin() into the same window as we just did)
    m_log.Draw("Log");
}

// ================================================================
// Function : mainloop
// Purpose  :
// ================================================================
void GlfwOcctView::mainloop()
{
    while (!glfwWindowShouldClose(myOcctWindow->getGlfwWindow()))
    {
        // glfwPollEvents() for continuous rendering (immediate return if there are no new events)
        // and glfwWaitEvents() for rendering on demand (something actually happened in the viewer)
        //glfwPollEvents();
        glfwWaitEvents();
        if (!myView.IsNull() && !myContext.IsNull())
        {
            FlushViewEvents(myContext, myView, Standard_True);

            renderGui();
        }
    }
}

// ================================================================
// Function : cleanup
// Purpose  :
// ================================================================
void GlfwOcctView::cleanup()
{
    // Cleanup IMGUI.
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (!myView.IsNull())
    {
        myView->Remove();
    }
    if (!myOcctWindow.IsNull())
    {
        myOcctWindow->Close();
    }

    glfwTerminate();
}

// ================================================================
// Function : onResize
// Purpose  :
// ================================================================
void GlfwOcctView::onResize(int theWidth, int theHeight)
{
    if (theWidth != 0
        && theHeight != 0
        && !myView.IsNull())
    {
        myView->Window()->DoResize();
        myView->MustBeResized();
        myView->Invalidate();
        myView->Redraw();
    }
}

// ================================================================
// Function : onMouseScroll
// Purpose  :
// ================================================================
void GlfwOcctView::onMouseScroll(double theOffsetX, double theOffsetY)
{
    ImGuiIO& aIO = ImGui::GetIO();
    if (!myView.IsNull() && !aIO.WantCaptureMouse)
    {
        UpdateZoom(Aspect_ScrollDelta(myOcctWindow->CursorPosition(), int(theOffsetY * 8.0)));
    }
}

// ================================================================
// Function : onMouseButton
// Purpose  :
// ================================================================
void GlfwOcctView::onMouseButton(int theButton, int theAction, int theMods)
{
    ImGuiIO& aIO = ImGui::GetIO();
    if (myView.IsNull() || aIO.WantCaptureMouse)
    {
        return;
    }

    const Graphic3d_Vec2i aPos = myOcctWindow->CursorPosition();
    if (theAction == GLFW_PRESS)
    {
        PressMouseButton(aPos, mouseButtonFromGlfw(theButton), keyFlagsFromGlfw(theMods), false);
    }
    else
    {
        ReleaseMouseButton(aPos, mouseButtonFromGlfw(theButton), keyFlagsFromGlfw(theMods), false);
    }
}

// ================================================================
// Function : onMouseMove
// Purpose  :
// ================================================================
void GlfwOcctView::onMouseMove(int thePosX, int thePosY)
{
    if (myView.IsNull())
    {
        return;
    }

    ImGuiIO& aIO = ImGui::GetIO();
    if (aIO.WantCaptureMouse)
    {
        myView->Redraw();
    }
    else
    {
        const Graphic3d_Vec2i aNewPos(thePosX, thePosY);
        UpdateMousePosition(aNewPos, PressedMouseButtons(), LastMouseFlags(), Standard_False);
    }
}

// ================================================================
// Function : onMouseMove
// Purpose  :
// ================================================================
void GlfwOcctView::autoClearPreviousScene() {
    if (currentScene != previousScene) {
        myContext->EraseAll(false);
    }
}


// Demonstrate using DockSpace() to create an explicit docking node within an existing window.
// Note: You can use most Docking facilities without calling any API. You DO NOT need to call DockSpace() to use Docking!
// - Drag from window title bar or their tab to dock/undock. Hold SHIFT to disable docking.
// - Drag from window menu button (upper-left button) to undock an entire node (all windows).
// - When io.ConfigDockingWithShift == true, you instead need to hold SHIFT to enable docking.
// About dockspaces:
// - Use DockSpace() to create an explicit dock node _within_ an existing window.
// - Use DockSpaceOverViewport() to create an explicit dock node covering the screen or a specific viewport.
//   This is often used with ImGuiDockNodeFlags_PassthruCentralNode.
// - Important: Dockspaces need to be submitted _before_ any window they can host. Submit it early in your frame! (*)
// - Important: Dockspaces need to be kept alive if hidden, otherwise windows docked into it will be undocked.
//   e.g. if you have multiple tabs with a dockspace inside each tab: submit the non-visible dockspaces with ImGuiDockNodeFlags_KeepAliveOnly.
// (*) because of this constraint, the implicit \"Debug\" window can not be docked into an explicit DockSpace() node,
// because that window is submitted as part of the part of the NewFrame() call. An easy workaround is that you can create
// your own implicit "Debug##2" window after calling DockSpace() and leave it in the window stack for anyone to use.
void GlfwOcctView::ShowAppDockSpace()
{
    // READ THIS !!!
    // TL;DR; this demo is more complicated than what most users you would normally use.
    // If we remove all options we are showcasing, this demo would become:
    //     void ShowExampleAppDockSpace()
    //     {
    //         ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
    //     }
    // In most cases you should be able to just call DockSpaceOverViewport() and ignore all the code below!
    // In this specific demo, we are not using DockSpaceOverViewport() because:
    // - (1) we allow the host window to be floating/moveable instead of filling the viewport (when opt_fullscreen == false)
    // - (2) we allow the host window to have padding (when opt_padding == true)
    // - (3) we expose many flags and need a way to have them visible.
    // - (4) we have a local menu bar in the host window (vs. you could use BeginMainMenuBar() + DockSpaceOverViewport()
    //      in your code, but we don't here because we allow the window to be floating)

    static bool dockSpaceOpen = true;
    static bool opt_fullscreen = true;
    static bool opt_padding = false;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }
    else
    {
        dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
    // and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    if (!opt_padding)
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &dockSpaceOpen, window_flags);
    if (!opt_padding)
        ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // Submit the DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Options"))
        {
            // Disabling fullscreen would allow the window to be moved to the front of other windows,
            // which we can't undo at the moment without finer window depth/z control.
            ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
            ImGui::MenuItem("Padding", NULL, &opt_padding);
            ImGui::Separator();

            if (ImGui::MenuItem("Flag: NoSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoSplit; }
            if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
            if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode; }
            if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
            if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen)) { dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
            ImGui::Separator();

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    ImGui::End();
}
