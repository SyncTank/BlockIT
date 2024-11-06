// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "List.h"
#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// This example can also compile and run with Emscripten! See 'Makefile.emscripten' for details.
#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

#pragma region Helpers

static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

// Data to be shared accross different functions of the demo.
struct ImGuiWindowData
{
    // Examples Apps (accessible from the "Examples" menu)
    bool ShowMainMenuBar = false;
    bool ShowAppAssetsBrowser = false;
    bool ShowAppConsole = false;
    bool ShowAppCustomRendering = false;
    bool ShowAppDocuments = false;
    bool ShowAppDockSpace = false;
    bool ShowAppLog = false;
    bool ShowAppLayout = false;
    bool ShowAppPropertyEditor = false;
    bool ShowAppSimpleOverlay = false;
    bool ShowAppAutoResize = false;
    bool ShowAppConstrainedResize = false;
    bool ShowAppFullscreen = false;
    bool ShowAppLongText = false;
    bool ShowAppWindowTitles = false;

    // Dear ImGui Tools (accessible from the "Tools" menu)
    bool ShowMetrics = false;
    bool ShowDebugLog = false;
    bool ShowIDStackTool = false;
    bool ShowStyleEditor = false;
    bool ShowAbout = false;

};

void convertWStringToCString(const std::wstring& wstr, char* cstr, size_t cstrSize) {
    size_t convertedChars = 0;
    errno_t err = wcstombs_s(&convertedChars, cstr, cstrSize, wstr.c_str(), _TRUNCATE);
    if (err != 0) {
        std::cerr << "Conversion error occurred." << std::endl;
    }
}

void setTable(std::unordered_map<std::wstring, std::vector<DWORD>>& totalProcessList)
{
    float spacing = ImGui::GetStyle().ItemInnerSpacing.x;

    static ImGuiTabBarFlags parentFlags = 
         ImGuiTableFlags_NoPadOuterX | ImGuiTableFlags_NoPadInnerX | ImGuiTableFlags_Borders;

    static ImGuiTabBarFlags tableflags = 
        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY;

    static ImGuiInputFlags route_options = ImGuiInputFlags_Repeat;
    static ImGuiInputFlags route_type = ImGuiInputFlags_RouteFocused;

    ImGuiInputFlags flags = route_type | route_options; // Merged flags
    if (route_type != ImGuiInputFlags_RouteGlobal)
        flags &= ~(ImGuiInputFlags_RouteOverFocused | ImGuiInputFlags_RouteOverActive | ImGuiInputFlags_RouteUnlessBgFocused);

    static float offset_x = 140.0f;
    
    HelpMarker("This is a List of Processes which you can designate which to kill, the 'save' button sets the present.");
    ImGui::SameLine();
    ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_S, flags | ImGuiInputFlags_Tooltip);
    ImGui::Button("Save");
    ImGui::SameLine();
    
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset_x);
    if (ImGui::ArrowButton("##left", ImGuiDir_Left)) {  }
    ImGui::SameLine(0.0f, spacing);
    if (ImGui::ArrowButton("##right", ImGuiDir_Right)) {  }

    ImGui::Spacing();

    static std::wstring item_selected_idx = L""; // Here we store our selected data as an index.
    static bool item_highlight = false;
    std::wstring  item_highlighted_idx = L""; // Here we store our highlighted data as an index.

    if (ImGui::BeginTable("Parent_Table", 2, parentFlags, ImVec2(ImGui::GetContentRegionAvail().x, 275)))
    {
        ImGui::TableSetupColumn("Active Processes");
        ImGui::TableSetupColumn("Murder List");
        ImGui::TableHeadersRow();
        ImGui::TableNextColumn();

        if (ImGui::BeginTable("table1", 1, tableflags, ImVec2(ImGui::GetContentRegionAvail().x, 275)))
        {
            for (const auto& [key, value] : totalProcessList)
            {
                const size_t bufferSize = 50;
                char cstr[bufferSize];
                convertWStringToCString(key, cstr, bufferSize);

                const bool is_selected = (item_selected_idx == key);

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                if (ImGui::Selectable(cstr, is_selected, ImGuiSelectableFlags_SpanAllColumns))
                    item_selected_idx = key;

                if (item_highlight && ImGui::IsItemHovered())
                    item_highlighted_idx = key;

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();

            }
            ImGui::EndTable();
        }

        ImGui::TableNextColumn();

        static std::wstring item_selected_idx2 = L""; // Here we store our selected data as an index.
        static bool item_highlight2 = false;
        std::wstring  item_highlighted_idx2 = L""; // Here we store our highlighted data as an index.

        if (ImGui::BeginTable("table2", 1, tableflags, ImVec2(ImGui::GetContentRegionAvail().x, 275)))
        {
            for (const auto& [key, value] : totalProcessList) // change totalProcessList to blacklist
            {
                const size_t bufferSize = 50;
                char cstr[bufferSize];
                convertWStringToCString(key, cstr, bufferSize);

                const bool is_selected2 = (item_selected_idx2 == key);

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                if (ImGui::Selectable(cstr, is_selected2, ImGuiSelectableFlags_SpanAllColumns))
                    item_selected_idx2 = key;

                if (item_highlight2 && ImGui::IsItemHovered())
                    item_highlighted_idx2 = key;

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected2)
                    ImGui::SetItemDefaultFocus();

            }
            ImGui::EndTable();
        }

        ImGui::EndTable();
    }
    //ImGui::SameLine();
}

void renderWindowContext()
{
    std::unordered_map<std::wstring, std::vector<DWORD>> totalProcessList;
    Core::ProcessList(totalProcessList);

    ImGui::SeparatorText("Block IT Now");
    ImGui::Spacing();
    {
        HelpMarker("This Countdown Timer Set in Minutes.");
        ImGui::SameLine();
        ImGui::PushItemWidth(75);

        static char str1[9] = "";
        ImGui::InputTextWithHint("Hour(s)", "H", str1, IM_ARRAYSIZE(str1), 0, 0, 0);

        ImGui::SameLine();

        static char str2[9] = "";
        ImGui::InputTextWithHint("Minute(s)", "Mins", str2, IM_ARRAYSIZE(str2));

        ImGui::Spacing();

        ImGui::Button("Block");
        ImGui::SameLine();
        ImGui::Button("Pause");
        ImGui::SameLine();
        ImGui::Button("Cancel");

        ImGui::PopItemWidth();
    }

    ImGui::SeparatorText("When to Block");
    ImGui::Spacing();

    {
        ImGui::PushItemWidth(150);

        ImGui::Text("Enter the time period within which to block these sites");
        static char str3[128] = "";
        ImGui::InputTextWithHint(" ", "Example", str3, IM_ARRAYSIZE(str3));
        ImGui::SameLine();
        ImGui::Button("All Day");

        ImGui::PopItemWidth();
        ImGui::Spacing();
        ImGui::PushItemWidth(75);

        static char str4[128] = "";
        ImGui::InputTextWithHint("Minutes in every", "Mins", str4, IM_ARRAYSIZE(str4));
        ImGui::SameLine();
        static int item_current_2 = 0;
        ImGui::Combo("Span", &item_current_2, "Hour\0Day\0Week\0Year\0\0");

        ImGui::PopItemWidth();
    }

    ImGui::Spacing();

    ImGui::SeparatorText("Processes");
    setTable(totalProcessList);
}

#pragma endregion

// Main code
#ifdef _WIN32
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
int main(int argc, char** argv)
#endif

{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context using Dear ImGui GLFW+OpenGL3 
    GLFWwindow* window = glfwCreateWindow(1280, 720, "BlockIT", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    // Use this to hide the context window - note you must dock it outside or edit the .ini file of imgui for it to work
    //glfwHideWindow(window); 
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    io.IniFilename = nullptr;
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 3.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        style.WindowPadding = ImVec2(10,8);
        style.ItemInnerSpacing = ImVec2(6,4);
        style.ScrollbarSize = 6;
        style.FrameRounding = 12;
        style.TabRounding = 8;
        style.CellPadding = ImVec2(2,2);
        style.WindowTitleAlign = ImVec2(.5,.5);
        style.WindowMenuButtonPosition = ImGuiDir_Right;
        style.SelectableTextAlign = ImVec2(.03f, 0.0f);
        style.SeparatorTextBorderSize = 1;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    // - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != nullptr);

    // Our state data
    bool show_demo_window = true;
    bool show_window = true;
    bool no_scroll = true;
    bool show_debug_frame = true;
    bool no_resize = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    static bool no_menu = true;
    static bool no_collapse = false;
    static bool unsaved_document = false;
    static bool no_close = false;

    ImGuiWindowFlags window_flags = 0;
    if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
    if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
    if (unsaved_document)   window_flags |= ImGuiWindowFlags_UnsavedDocument;
    if (no_scroll)          window_flags |= ImGuiWindowFlags_NoScrollbar;
    if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
    if (no_close)           show_window = NULL; // Don't pass our bool* to Begin

    static ImGuiWindowData gui_data;
    //if (gui_data.ShowMainMenuBar) { ShowAppMainMenuBar(); }
    //if (gui_data.ShowAppAutoResize) {}
    //if (gui_data.ShowAppLongText) {}
    //if (gui_data.ShowAbout) { ShowAboutWindow(); }



    // Main loop
#ifdef __EMSCRIPTEN__
    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = nullptr;
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (!glfwWindowShouldClose(window))
#endif
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        #pragma region UI Components

        //menu save
        //ShowWindowMenuBar(&gui_data);

        {
            ImGui::Begin("BlockIT", &show_window, window_flags);
            ImGui::SetWindowSize(ImVec2(500, 650), 0); // temp - cache this outside

            if (show_debug_frame)
            {
                ImGui::Spacing();
                ImGui::SeparatorText("Block Timers");
                static float f = 0.0f;
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
                ImGui::Spacing();
            }

            ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None | ImGuiTabBarFlags_TabListPopupButton;
            if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
            {
                if (ImGui::BeginTabItem("Set 1"))
                {
                    renderWindowContext();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Set 2"))
                {
                    renderWindowContext();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Set 3"))
                {
                    renderWindowContext();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Set 4"))
                {
                    renderWindowContext();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Set 5"))
                {
                    renderWindowContext();
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }

            ImGui::End();
        };



        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        #pragma endregion

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}