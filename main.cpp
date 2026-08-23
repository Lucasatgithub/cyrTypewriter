
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#ifdef _MSC_VER
#pragma comment(linker, "/SUBSYSTEM:CONSOLE")
#endif

using namespace std;
using namespace ImGui;

namespace fs = std::filesystem;
fs::path dir = "C:\\Users\\iamra_q81h3ev\\Desktop\\cyrTypewriter\\templates";
std::error_code ec;
//fs::current_path(dir, ec) noexcept;

string input = "";
size_t curline = 0;
size_t curchar = 0;

struct keyBinding
{
	string lower;
	string upper;
};

static map<int, keyBinding> key_map = {
	{GLFW_KEY_Q, {u8"й", u8"Й"}}, {GLFW_KEY_W, {u8"ц", u8"Ц"}}, {GLFW_KEY_E, {u8"у", u8"У"}}, {GLFW_KEY_R, {u8"к", u8"К"}}, {GLFW_KEY_T, {u8"е", u8"Е"}}, {GLFW_KEY_Y, {u8"н", u8"Н"}}, 
		{GLFW_KEY_U, {u8"г", u8"Г"}}, {GLFW_KEY_I, {u8"ш", u8"Ш"}}, {GLFW_KEY_O, {u8"щ", u8"Щ"}}, {GLFW_KEY_P, {u8"з", u8"З"}}, {GLFW_KEY_LEFT_BRACKET, {u8"х", u8"Х"}}, {GLFW_KEY_RIGHT_BRACKET, {u8"ъ", u8"Ъ"}},
	{GLFW_KEY_A, {u8"ф", u8"Ф"}}, {GLFW_KEY_S, {u8"ы", u8"Ы"}}, {GLFW_KEY_D, {u8"в", u8"В"}}, {GLFW_KEY_F, {u8"а", u8"А"}}, {GLFW_KEY_G, {u8"п", u8"П"}}, {GLFW_KEY_H, {u8"р", u8"Р"}}, 
		{GLFW_KEY_J, {u8"о", u8"О"}}, {GLFW_KEY_K, {u8"л", u8"Л"}}, {GLFW_KEY_L, {u8"д", u8"Д"}}, {GLFW_KEY_SEMICOLON, {u8"ж", u8"Ж"}}, {GLFW_KEY_APOSTROPHE, {u8"э", u8"Э"}},
	{GLFW_KEY_Z, {u8"я", u8"Я"}}, {GLFW_KEY_X, {u8"ч", u8"Ч"}}, {GLFW_KEY_C, {u8"с", u8"С"}}, {GLFW_KEY_V, {u8"м", u8"М"}}, {GLFW_KEY_B, {u8"и", u8"И"}}, {GLFW_KEY_N, {u8"т", u8"Т"}}, {GLFW_KEY_M, {u8"ь", u8"Ь"}}, 
		{GLFW_KEY_COMMA, {u8"б", u8"Б"}}, {GLFW_KEY_PERIOD, {u8"ю", u8"Ю"}}
}; //reference table for keys


static map<int, bool> key_state;
static char inbuf[1000] = u8"";

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	/*if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		input.clear();
		memset(inbuf, 0, sizeof(inbuf));
		return;
	}*/
	bool shift = (mods & GLFW_MOD_SHIFT) != 0;
	if (action == GLFW_PRESS) {
		if(key == GLFW_KEY_BACKSPACE){
			if(!input.empty()){
				while(!input.empty() && (input.back() & 0xC0) == 0x80) input.pop_back();
				input.pop_back();
			}
			return;
		}
		if (key_map.count(key)) {
			const auto& binding = key_map[key];
			key_state[key] = true;
			input += shift ? binding.upper : binding.lower;
		}
	}
	else if (action == GLFW_RELEASE) {
		if (key_map.count(key)) {
			key_state[key] = false;
		}
	}
}

vector<string> lesson = {};

void ParseTemplate(const string& filename){
	ifstream f;
	f.open(filename);
	if(f.is_open()){
		string line = "";
		char c;
		while(f.get(c)){
			line += c;
			if(c == '.' || c == '?' || c == '!'){
				lesson.push_back(line);
				line.clear();
			}
		}
		if(!line.empty()) lesson.push_back(line);
	}
	f.close();
}

void FormatTemplate(vector<string>& text){
	for(int i = 0; i < text.size(); i++){
		if(!text[i].empty() && text[i][0] == ' '){
			text[i].erase(0);
		}
	}
}

void LoadTemplate(){
	ParseTemplate("spokoynaya-noch.txt");
	FormatTemplate(lesson);
}

int main() {
	if (!glfwInit()) {
		cerr << "Failed to initialize GLFW\n";
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(1280, 720, "cyrTypewriter", nullptr, nullptr);
	if (!window) {
		cerr << "Failed to create GLFW window\n";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSwapInterval(true);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cerr << "Failed to initialize GLAD\n";
		return -1;
	}
	printf("OpenGL ver: %s\n", glGetString(GL_VERSION));

	IMGUI_CHECKVERSION();
	CreateContext();
	ImGuiIO& io = GetIO();
	StyleColorsDark();
	ImFont* font = io.Fonts->AddFontFromFileTTF("C:/Users/iamra_q81h3ev/Desktop/cyrTypewriter/OpenSans-Light.ttf", 15.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
	
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	const char* glsl_version = "#version 150";
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	//load font
	//if(!font) cerr << "Font failed\n";


	//import text template
	LoadTemplate(); 

	
	//console debug messages
	cout << filesystem::current_path() << endl;
	cout << lesson[0] << endl;
	
	
	//loop for rendering window/viewport
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		NewFrame();
		//ImGui::PushFont(font);


		//display text
		ImGui::Begin("cyrTypewriter Testing Window");
		ImGui::Separator();
		ImGui::Text("%s", input.c_str());
		ImGui::Separator();
		ImGui::Text("%s", lesson[curline]);


		//render keyboard keys
		const char* rows[] = { "qwertyuiop[]", "asdfghjkl;'", "zxcvbnm,." };
		float sx = 50, sy = 250;
		float mtpr = 1;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; rows[i][j] != '\0'; j++) {
				string label = key_map[std::toupper(rows[i][j])].lower;
				ImGui::SetCursorPos(ImVec2(sx, sy));
				if (key_state[std::toupper(rows[i][j])]) {
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.6f, 1.0f, 1.0f));
				}
				ImGui::Button(label.c_str(), ImVec2(40, 40));
				if (key_state[std::toupper(rows[i][j])]) {
					ImGui::PopStyleColor();
				}
				sx += 50;
			}
			sy += 50;
			sx = 50 + (25 * mtpr);
			mtpr++;
			ImGui::NewLine();
		} 

		
		//main
		if(curchar == input.back()){
			if(curchar++ >= lesson[curline].size()) curline++;
			else curchar++;
		}


		ImGui::End();
		ImGui::Render();
		//ImGui::PopFont();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		//ImGui::PopFont();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}