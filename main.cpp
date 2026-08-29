
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>

#include <windows.h>

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
		{GLFW_KEY_COMMA, {u8"б", u8"Б"}}, {GLFW_KEY_PERIOD, {u8"ю", u8"Ю"}}, {GLFW_KEY_SLASH, {",", "."}}
}; //reference table for keys

//declarations
string getUtf8Char(const string& str, size_t index); 
size_t getUtf8Len(const string& str);
vector<string> lesson = {};

static map<int, bool> key_state;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	bool shift = (mods & GLFW_MOD_SHIFT) != 0;
	if (action == GLFW_PRESS) {
		if(key == GLFW_KEY_BACKSPACE){
			if(!input.empty()){
				while(!input.empty() && (input.back() & 0xC0) == 0x80) input.pop_back();
				input.pop_back();
				if(curchar > 0){
					curchar--;
				}
				if(curchar == 0 && curline > 0){
					curline--; curchar = getUtf8Len(lesson[curline])-1;
				}
			}
			return;
		}
		if(key == GLFW_KEY_SPACE){
			string target = getUtf8Char(lesson[curline], curchar);
			if(target == " "){
				input += " ";
				curchar++;
			}
			return;
		}
		if (key_map.count(key)) {
			const auto& binding = key_map[key];
			key_state[key] = true;
			string typed = shift ? binding.upper : binding.lower;
			if(curline >= lesson.size()) return;
			string target = getUtf8Char(lesson[curline], curchar);
			if(typed == target){
				input += typed;
				curchar++;
				//change line upon completion
				if(curchar >= getUtf8Len(lesson[curline])-1){
					if(curline + 1 < lesson.size()){
						curline++;
						curchar = 0;
						input.clear();
					}
				}
			}
		}
	}
	else if (action == GLFW_RELEASE) {
		if (key_map.count(key)) {
			key_state[key] = false;
		}
	}
}

string getUtf8Char(const string& str, size_t index){
	size_t pos = 0;
	for(size_t i = 0; i < index; i++){
		 if ((str[pos] & 0x80) == 0)
            pos += 1;
        else if ((str[pos] & 0xE0) == 0xC0)
            pos += 2;
        else if ((str[pos] & 0xF0) == 0xE0)
            pos += 3;
        else if ((str[pos] & 0xF8) == 0xF0)
            pos += 4;
	}
	if ((str[pos] & 0x80) == 0)
        return str.substr(pos, 1);
    else if ((str[pos] & 0xE0) == 0xC0)
        return str.substr(pos, 2);
    else if ((str[pos] & 0xF0) == 0xE0)
        return str.substr(pos, 3);
    else if ((str[pos] & 0xF8) == 0xF0)
        return str.substr(pos, 4);
	return "";
}

size_t getUtf8Len(const string& str){
	size_t count = 0;
	for (size_t i = 0; i < str.size();)
    {
        if ((str[i] & 0x80) == 0)
            i += 1;
        else if ((str[i] & 0xE0) == 0xC0)
            i += 2;
        else if ((str[i] & 0xF0) == 0xE0)
            i += 3;
        else if ((str[i] & 0xF8) == 0xF0)
            i += 4;

        count++;
    }
	return count;
}

void RenderLine(const string& line, size_t cursor){
	size_t len = getUtf8Len(line);
	for(size_t i = 0; i < len; i++){
		string chr = getUtf8Char(line, i);
		if(i == cursor){
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
			ImGui::TextUnformatted(chr.c_str());
			ImGui::PopStyleColor();
		}
		else ImGui::TextUnformatted(chr.c_str());
		if(i+1 < len) ImGui::SameLine(0.0f, 0.0f);
	}
}

void ParseTemplate(const string& filename){
	ifstream f;
	f.open(filename);
	if(f.is_open()){
		cout << "File loaded successfully" << endl;
		string line = "";
		char c;
		int flag = 0;
		while(f.get(c)){
			line += c;
			if(c == '\r') flag = 1;
			if(c == '.' || c == '?' || c == '!' || (c == '\n')){
				if(flag == 1) flag = 0;
				lesson.push_back(line);
				line.clear();
			}
		}
		if(!line.empty()) lesson.push_back(line);
		cout << "loaded " << lesson.size() << " sentences." << endl;
		/*for(const string& sentence : lesson)*/ cout << lesson[0] << endl;
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
	//console encoding issue fix
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);

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
	if(!font) cerr << "Font failed\n";


	//import text template
	LoadTemplate(); 

	
	//console debug messages
	cout << filesystem::current_path() << endl;
	cout << "line 1: " << lesson[0] << endl;
	
	
	//loop for rendering window/viewport
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		NewFrame();


		//display text
		ImGui::Begin("cyrTypewriter Testing Window");
		ImGui::PushFont(font);
		ImGui::Separator();
		ImGui::Text("%s", input.c_str());
		ImGui::Separator();
		if(!lesson.empty()){
			//ImGui::Text("%s", lesson[curline].c_str());
			RenderLine(lesson[curline], curchar);
		}

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


		ImGui::PopFont();
		ImGui::End();
		ImGui::Render();
		
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}