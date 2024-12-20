/*
    -   Rudy | syncedtank@gmail.com
    -   release : v1.0.0
*/


#include "List.h" // includes the imgui.h
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h" // ??

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

// Main code
#ifdef _WIN32
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
int main(int argc, char** argv)
#endif
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

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

    // More options to hide window context
    //glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    //glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    // Create window with graphics context using Dear ImGui GLFW+OpenGL3 // ImVec2(500, 675); ImVec2(1280, 720);
    GLFWwindow* window = glfwCreateWindow(500, 675, "BlockIT", nullptr, nullptr);

    if (window == nullptr)
        return 1;

    glfwMakeContextCurrent(window);
    glfwSetWindowPos(window, 200, 200);
    // Use this to hide the context window - note you must dock it outside or edit the .ini file of imgui for it to work
    //glfwHideWindow(window); 

    glfwSwapInterval(6); // Enable vsync
    // Monitor refresh rate/interval = application FPS.
    // Below is a list of some intervals and the effects they will have.
    // This is tied to the evetpolling from I'm Gui so careful on modification

    //    0 = inf.FPS // free.
    //    1 = 360  FPS // ~2.7 ms/frame
    //    2 = 180  FPS // ~5.5 ms/frame
    //    4 = 90   FPS // ~11.12 ms/frame
    //    6 = 60   FPS // ~16.6 ms/frame

    #pragma region IO

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    //io.ConfigFlags |= ImGuiViewportFlags_NoDecoration;
    io.ConfigViewportsNoAutoMerge = true;
    io.ConfigViewportsNoTaskBarIcon = true;
    //io.ConfigViewportsNoDecoration = true;
    io.IniFilename = nullptr;

    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None | ImGuiTabBarFlags_TabListPopupButton;

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


    // Taskbar Icon code - Not working still

    //HICON hIcon = App::LoadIconFromResource();
    //if (hIcon)
    //{
    //    GLFWimage image_Icons[1];
    //    image_Icons[0].pixels = (unsigned char*)hIcon;
    //    image_Icons[0].width = 48;
    //    image_Icons[0].height = 48;
    //    glfwSetWindowIcon(window, 1, image_Icons);
    //}

    // working?

    HICON loaded_Image = App::LoadIconFromResource();
    ICONINFO iconInfo;
    GetIconInfo(loaded_Image, &iconInfo);

    BITMAP bmp;
    GetObject(iconInfo.hbmColor, sizeof(BITMAP), &bmp);

    int width = bmp.bmWidth;
    int height = bmp.bmHeight;
    int size = width * height * 4; // Assuming 32-bit color depth

    unsigned char* pixels = new unsigned char[size];
    BITMAPINFO bmi;
    ZeroMemory(&bmi, sizeof(BITMAPINFO));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height; // Negative to indicate top-down bitmap
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    GetDIBits(GetDC(NULL), iconInfo.hbmColor, 0, height, pixels, &bmi, DIB_RGB_COLORS);

    GLFWimage images[1];
    images[0].width = width;
    images[0].height = height;
    images[0].pixels = pixels;

    glfwSetWindowIcon(window, 1, images);

    delete[] pixels;


    #pragma endregion

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

    #pragma region windowFlags

    // Our flags for the Window Context
    bool show_demo_window = false;
    bool show_window = true; // Useless now, but kept var/functions in case for reference
    bool show_debug_frame = false;
    bool no_scroll = true;
    bool no_resize = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    bool no_menu = false;
    bool no_move = true;
    bool unsaved_document = false;
    bool no_title = true;
    static bool no_collapse = true;

    ImGuiWindowFlags window_flags = 0;
    if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
    if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
    if (unsaved_document)   window_flags |= ImGuiWindowFlags_UnsavedDocument;
    if (no_scroll)          window_flags |= ImGuiWindowFlags_NoScrollbar;
    if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
    if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
    if (no_title)           window_flags |= ImGuiWindowFlags_NoTitleBar;

    //const ImVec2 windowSize = ImVec2(500, 675);
    const ImGuiCond windowCondor = 0;
    static int setState = 0; // Defaults to 0 / first set
    static int pastState = 0; // used to check if loading is needed
    const int TARGET_FPS = 30;
    const double FRAME_TIME = 1.0 / TARGET_FPS;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;

    App::init(io); // setup function - preloads stuff

    // Clock for threads
    static float deltaClock = 0;

    #pragma endregion

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
        
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);

        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(5);
            continue;
        }
        
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        if (!show_window)
        {
            startTime = std::chrono::high_resolution_clock::now();
        }

        //UpdateWindowPosition(window, io); // need to make better

        #pragma region UI Components
                
        {
            //ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2((float)display_w, (float)display_h));
            ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);

            // Main body
            ImGui::Begin("BlockIT", 0, window_flags);

            if (ImGui::BeginMenuBar())
            {

                //if (!App::getIsRunning())
                //{
                //    if (ImGui::BeginMenu("Quit"))
                //    {
                //        return 0;
                //        ImGui::EndMenu();
                //    }
                //}

                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0)); // Remove background color
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(.26f, .59f, .98f, .8f));   // Remove hover color
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(.26f, .59f, .98f, 1.0f));   // Remove active color

                //if (ImGui::Button("Minimize"))
                //{
                //    glfwIconifyWindow(window);
                //}

                if (ImGui::Button("Save"))
                {
                    App::writeAppFileOut(setState);
                }
                if (ImGui::Button("Load"))
                {
                    App::loadBuffer(setState);
                }
                if (ImGui::Button("Clear"))
                {
                    App::clearBuffer();
                }

                ImGui::PopStyleColor(3);
                ImGui::EndMenuBar();
            }

            if (show_debug_frame)
            {
                ImGui::Spacing();
                ImGui::SeparatorText("Block Timers");
                static float f = 0.0f;
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
                ImGui::Spacing();

                if (deltaClock > 1000)
                {
                    deltaClock = 0;
                }

                deltaClock += io.DeltaTime * 100;
                std::string temp = "Delta Clock Cycle: " + std::to_string(deltaClock);
                ImGui::Text(+temp.c_str());
            }

            // Settings for different Sets
            if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
            {
                if (ImGui::BeginTabItem("Set 1"))
                {
                    setState = 0;
                    if (pastState != setState)
                    {
                        pastState = setState;
                        App::loadBuffer(setState);
                    }
                    App::renderWindowContext();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Set 2"))
                {
                    setState = 1;
                    if (pastState != setState)
                    {
                        pastState = setState;
                        App::loadBuffer(setState);
                    }
                    App::renderWindowContext();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Set 3"))
                {
                    setState = 2;
                    if (pastState != setState)
                    {
                        pastState = setState;
                        App::loadBuffer(setState);
                    }
                    App::renderWindowContext();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Set 4"))
                {
                    setState = 3;
                    if (pastState != setState)
                    {
                        pastState = setState;
                        App::loadBuffer(setState);
                    }
                    App::renderWindowContext();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Set 5"))
                {
                    setState = 4;
                    if (pastState != setState)
                    {
                        pastState = setState;
                        App::loadBuffer(setState);
                    }
                    App::renderWindowContext();
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
            ImGui::End();

            // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
            if (show_demo_window)
            {
                ImGui::ShowDemoWindow(&show_demo_window);
            }
                
        }
       
        #pragma endregion

        if (!show_window)
        {
            // Calculate frame duration
            endTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = endTime - startTime;

            // Sleep to maintain the target frame rate
            if (io.Framerate > TARGET_FPS) {
                std::this_thread::sleep_for(std::chrono::duration<double>(FRAME_TIME - elapsed.count()));
            }
        }

        // Rendering
        ImGui::Render();
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