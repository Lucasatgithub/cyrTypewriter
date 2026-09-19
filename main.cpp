
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <chrono>

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
fs::path dir = "C:\\Users\\iamra\\Desktop\\cyrTypewriter\\templates";
std::error_code ec;
//fs::current_path(dir, ec) noexcept;


//==========TYPEWRITER ENGINE & TEXT PROCESSING==========
struct typingSession
{
	vector<string> lesson = {};
	string input = "";
	size_t curline = 0;
	size_t curchar = 0;
};

struct keyBinding
{
	string lower;
	string upper;
};

static map<int, keyBinding> key_map = {
{GLFW_KEY_GRAVE_ACCENT, {u8"ё", u8"Ё"}}, {GLFW_KEY_1, {"1", "!"}}, {GLFW_KEY_2, {"2", "\""}}, {GLFW_KEY_3, {"3", "#"}}, {GLFW_KEY_4, {"4", ";"}}, {GLFW_KEY_5, {"5", "%"}}, {GLFW_KEY_6, {"6", ":"}}, 
	{GLFW_KEY_7, {"7", "?"}}, {GLFW_KEY_8, {"8", "*"}}, {GLFW_KEY_9, {"9", "("}}, {GLFW_KEY_0, {"0", ")"}}, {GLFW_KEY_MINUS, {"-", "_"}}, {GLFW_KEY_EQUAL, {"=", "+"}}, 
	{GLFW_KEY_Q, {u8"й", u8"Й"}}, {GLFW_KEY_W, {u8"ц", u8"Ц"}}, {GLFW_KEY_E, {u8"у", u8"У"}}, {GLFW_KEY_R, {u8"к", u8"К"}}, {GLFW_KEY_T, {u8"е", u8"Е"}}, {GLFW_KEY_Y, {u8"н", u8"Н"}}, 
		{GLFW_KEY_U, {u8"г", u8"Г"}}, {GLFW_KEY_I, {u8"ш", u8"Ш"}}, {GLFW_KEY_O, {u8"щ", u8"Щ"}}, {GLFW_KEY_P, {u8"з", u8"З"}}, {GLFW_KEY_LEFT_BRACKET, {u8"х", u8"Х"}}, {GLFW_KEY_RIGHT_BRACKET, {u8"ъ", u8"Ъ"}},
	{GLFW_KEY_A, {u8"ф", u8"Ф"}}, {GLFW_KEY_S, {u8"ы", u8"Ы"}}, {GLFW_KEY_D, {u8"в", u8"В"}}, {GLFW_KEY_F, {u8"а", u8"А"}}, {GLFW_KEY_G, {u8"п", u8"П"}}, {GLFW_KEY_H, {u8"р", u8"Р"}}, 
		{GLFW_KEY_J, {u8"о", u8"О"}}, {GLFW_KEY_K, {u8"л", u8"Л"}}, {GLFW_KEY_L, {u8"д", u8"Д"}}, {GLFW_KEY_SEMICOLON, {u8"ж", u8"Ж"}}, {GLFW_KEY_APOSTROPHE, {u8"э", u8"Э"}},
	{GLFW_KEY_Z, {u8"я", u8"Я"}}, {GLFW_KEY_X, {u8"ч", u8"Ч"}}, {GLFW_KEY_C, {u8"с", u8"С"}}, {GLFW_KEY_V, {u8"м", u8"М"}}, {GLFW_KEY_B, {u8"и", u8"И"}}, {GLFW_KEY_N, {u8"т", u8"Т"}}, {GLFW_KEY_M, {u8"ь", u8"Ь"}}, 
		{GLFW_KEY_COMMA, {u8"б", u8"Б"}}, {GLFW_KEY_PERIOD, {u8"ю", u8"Ю"}}, {GLFW_KEY_SLASH, {",", "."}}
}; //reference table for keys

string lowercase(string& targ){
	for(const auto& [key, binding] : key_map){
		if(targ == binding.upper || targ == binding.lower) return binding.lower;
	}
	return targ;
};

string alph = u8"абвгдеёжзийкмнопрстуфхцчшщъыьэюя";

map<string, int> charid ={
	{u8"а", 0}, {u8"б", 1}, {u8"в", 2}, {u8"г", 3}, {u8"д", 4}, {u8"е", 5}, 
	{u8"ё", 6},	{u8"ж", 7}, {u8"з", 8}, {u8"и", 9}, {u8"й", 10}, {u8"к", 11}, 
	{u8"л", 12}, {u8"м", 13}, {u8"н", 14}, {u8"о", 15}, {u8"п", 16}, {u8"р", 17},
	{u8"с", 18}, {u8"т", 19}, {u8"у", 20}, {u8"ф", 21}, {u8"х", 22}, {u8"ц", 23},
	{u8"ч", 24}, {u8"ш", 25}, {u8"щ", 26}, {u8"ъ", 27}, {u8"ы", 28}, {u8"ь", 29}, 
	{u8"э", 30}, {u8"ю", 31}, {u8"я", 32}, {" ", 33}, {",", 34}, {".", 35}
};

chrono::steady_clock::time_point now, prevtime = chrono::steady_clock::now();
long long int elapsed;

vector<vector<long long>> time_by_char(36, vector<long long>(0, 0)); 
vector<int> mistakes(36, 0);
vector<int> charfreq(36, 0);
typingSession session;

//declarations
string getUtf8Char(const string& str, size_t index); 
size_t getUtf8Len(const string& str);

static map<int, bool> key_state;

bool timerStarted = false;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	bool shift = (mods & GLFW_MOD_SHIFT) != 0;
	if (action == GLFW_PRESS) {
		if(key == GLFW_KEY_ENTER){
			if(!timerStarted){
				prevtime = chrono::steady_clock::now();
				timerStarted = true;
				return;
			}
		}
		if(key == GLFW_KEY_BACKSPACE){
			if(!session.input.empty()){
				while(!session.input.empty() && (session.input.back() & 0xC0) == 0x80) session.input.pop_back();
				session.input.pop_back();
				if(session.curchar > 0){
					session.curchar--;
				}
				if(session.curchar == 0 && session.curline > 0){
					session.curline--; session.curchar = getUtf8Len(session.lesson[session.curline])-1;
				}
			}
			return;
		}
		if(key == GLFW_KEY_SPACE){
			string target = getUtf8Char(session.lesson[session.curline], session.curchar);
			if(target == " "){
				now = chrono::steady_clock::now();
				elapsed = chrono::duration_cast<chrono::milliseconds>(now - prevtime).count();
				time_by_char[33].push_back(elapsed);
				prevtime = now;
				session.input += " ";
				session.curchar++;
			}
			return;
		}
		if (key_map.count(key)) {
			const auto& binding = key_map[key];
			key_state[key] = true;
			string typed = shift ? binding.upper : binding.lower;
			string typed_id = binding.lower;
			if(session.curline >= session.lesson.size()) return;
			string target = getUtf8Char(session.lesson[session.curline], session.curchar);
			string lctarg = lowercase(target);
			if(typed == target){
				session.input += typed;
				session.curchar++;
				if(timerStarted){
					now = chrono::steady_clock::now();
					elapsed = chrono::duration_cast<chrono::milliseconds>(now - prevtime).count();
					time_by_char[charid[typed_id]].push_back(elapsed);
					prevtime = now;
					//change line upon completion
					if(session.curchar >= getUtf8Len(session.lesson[session.curline])){
						if(session.curline + 1 < session.lesson.size()){
							session.curline++;
							session.curchar = 0;
							session.input.clear();
						}
						else{
							cout << "End of text" << endl;
						}
					}
				}
			}
			else{
				mistakes[charid[lctarg]]++;
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
//====================


//==========MAIN FRAMEWORK==========
enum class AppState
{
	MainMenu,
	Practice
};

AppState currentState = AppState::MainMenu;

void LoadLesson(const string& filename);
void CenterText(const char* text);

void RenderMenu(){
	ImGui::Begin("cyrTypewriter");
	//ImGui::Text("Home");
	CenterText("Home");
	ImGui::Separator();

	float buttonWidth = 900.0f;
	ImGui::SetCursorPosX((ImGui::GetWindowWidth() - buttonWidth) * 0.5f);

	if(ImGui::Button("Warmup", ImVec2(buttonWidth, 0))){
		LoadLesson("templates/testmsg.txt");
		currentState = AppState::Practice;
	}

	if(ImGui::Button("Lyrics: Sourced directly from iconic songs", ImVec2(buttonWidth, 0))){
		LoadLesson("templates/spokoynaya-noch.txt");
		currentState = AppState::Practice;
	}
	
	if(ImGui::Button("Passages: Selected excerpts from short novels", ImVec2(buttonWidth, 0))){
		LoadLesson("templates/shinel.txt");
		currentState = AppState::Practice;
	}

	if(ImGui::Button("Classics: The best of Russian literature", ImVec2(buttonWidth, 0))){
		//LoadLesson("placeholder_novel_file");
	}

	if(ImGui::Button("Import custom text", ImVec2(buttonWidth, 0))){
		//In Progress
	}

	ImGui::End();
}

void RenderTypewriter(){
	ImGui::Begin("cyrTypewriter");
	ImGui::Separator();
	ImGui::Text("%s", session.input.c_str());
	ImGui::Separator();
	if(!session.lesson.empty()){
		RenderLine(session.lesson[session.curline], session.curchar);
	}
	//render keyboard keys
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
	const char* rows[] = { "`1234567890-=", "qwertyuiop[]", "asdfghjkl;'", "zxcvbnm,./" };
	float sx = 50, sy = 250;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; rows[i][j] != '\0'; j++) {
			string label = key_map[std::toupper(rows[i][j])].lower;
			ImGui::SetCursorPos(ImVec2(sx, sy));
			if (key_state[std::toupper(rows[i][j])]) {
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.45f, 0.45f, 0.45f, 1.0f));
			}
			ImGui::Button(label.c_str(), ImVec2(40, 40));
			if (key_state[std::toupper(rows[i][j])]) {
				ImGui::PopStyleColor();
			}
			sx += 50;
		}
		sy += 50;
		sx = 100 + (25 * i);
		ImGui::NewLine();
	} 
	ImGui::PopStyleColor(2);
	ImGui::Separator();
	
	if(ImGui::Button("View stats")){
		for(int i = 0; i < 33; i++){ 
			cout << "<" << getUtf8Char(alph, i) << ">" << "(letter #" << i+1 << ")" << endl;
			cout << "times missed: " << mistakes[i] << endl;
			float accuracy;
			if(charfreq[i] != 0) accuracy = static_cast<float>(charfreq[i]-mistakes[i]) / charfreq[i];
			else accuracy = 0.0f;
			cout << "accuracy: " << accuracy*100 << "%" << endl;
			cout << "typing speed (in milliseconds)" << endl;
			for(int j = 0; j < time_by_char[i].size(); j++){
				cout << time_by_char[i][j] << endl;
			}
		}
	}
	if(ImGui::Button("Exit")){
		currentState = AppState::MainMenu;
	}
	ImGui::End();	
}
//====================


//==========FILE PROCESSING==========
void ParseTemplate(const string& filename){
	ifstream f;
	f.open(filename);
	if(f.is_open()){
		cout << "File loaded successfully" << endl;
		string text((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
		size_t charIdx = 0;
		while(charIdx < getUtf8Len(text)){
			string chr = getUtf8Char(text, charIdx);
			if(charid.count(chr) && charid[chr] < 33){
				charfreq[charid[chr]]++;
			}
			charIdx++;
		}
		string line = "";
		for(size_t i = 0; i < getUtf8Len(text); i++){
			string c = getUtf8Char(text, i);
			line += c;
			if(c == "." || c == "?" || c == "!" || (c == "\n")){
				session.lesson.push_back(line);
				line.clear();
			}
		}
		if(!line.empty()) session.lesson.push_back(line);
		cout << "loaded " << session.lesson.size() << " sentences." << endl;
		for(const string& sentence : session.lesson) cout << session.lesson[0] << endl;
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

void LoadLesson(const string& filename){
	session.lesson.clear();
	charfreq.assign(36, 0);
	mistakes.assign(36, 0);
	time_by_char.assign(36, vector<long long>(0));
	ParseTemplate(filename);
	FormatTemplate(session.lesson);
	session.curline = 0;
	session.curchar = 0;
	session.input.clear();
}
//====================


//==========VISUAL DETAILS==========
void SetupStyle(){
	ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.98f, 0.961f, 0.902f, 1.0f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.90f, 0.90f, 0.90f, 0.0f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.90f, 0.90f, 0.90f, 0.3f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.90f, 0.90f, 0.90f, 0.6f);
	style.Colors[ImGuiCol_Text] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.98f, 0.961f, 0.902f, 0.3f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.98f, 0.961f, 0.902f, 0.3f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.98f, 0.961f, 0.902f, 0.6f);
}

void CenterText(const char* text){
	float textWidth = ImGui::CalcTextSize(text).x;
	ImGui::SetCursorPosX((ImGui::GetWindowWidth() - textWidth)*0.5f);
	ImGui::Text("%s", text);
}
//====================


int main() {
	cout << "A: entered main" << endl;

	//console encoding issue fix
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);
	cout << "B: console configured" << endl;

	if (!glfwInit()) {
		cerr << "(C) Failed to initialize GLFW\n";
		return -1;
	}
	cout << "D: GLFW initialized" << endl;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	cout << "E: GLFW hints set" << endl;
	
	GLFWwindow* window = glfwCreateWindow(1280, 720, "cyrTypewriter", nullptr, nullptr);
	if (!window) {
		cerr << "(F) Failed to create GLFW window\n";
		glfwTerminate();
		return -1;
	}
	cout << "G: GLFW window created" << endl;

	//glClearColor(0.1f, 0.1f, 0.0f, 1.0f);
	glfwMakeContextCurrent(window);
	cout << "H: OpenGL context current" << endl;

	glfwSetKeyCallback(window, key_callback);
	glfwSwapInterval(true);
	cout << "I: GLFW configured" << endl;

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cerr << "(J) Failed to initialize GLAD\n";
		return -1;
	}
	printf("OpenGL ver: %s\n", glGetString(GL_VERSION));
	cout << "K: GLAD initialized" << endl;

	IMGUI_CHECKVERSION();
	cout << "L: ImGui version checked" << endl;
	CreateContext();
	cout << "M: ImGui context created" << endl;
	ImGuiIO& io = GetIO();
	cout << "N: ImGui IO obtained" << endl;
	SetupStyle();
	cout << "O: SetupStyle finished" << endl;
	ImFont* font = io.Fonts->AddFontFromFileTTF("C:/Users/iamra/Desktop/cyrTypewriter/OpenSans-Light.ttf", 15.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
	cout << "P: Font loading finished" << endl;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	cout << "Q: Config flags set" << endl;

	const char* glsl_version = "#version 150";
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	cout << "R: ImGui GLFW backend initialized" << endl;

	ImGui_ImplOpenGL3_Init(glsl_version);
	cout << "S: ImGui OpenGL backend initialized" << endl;

	//load font
	if(!font) cerr << "Font failed\n";
	cout << "T: Entering main loop" << endl;
	
	//console debug messages
	cout << filesystem::current_path() << endl;
	//cout << "line 1: " << session.lesson[0] << endl;
	
	//loop for rendering window/viewport
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		NewFrame();

		//display text
		//ImGui::Begin("cyrTypewriter Testing Window");
		ImGui::PushFont(font);
		if(currentState == AppState::MainMenu){
			RenderMenu();
		}
		else{
			RenderTypewriter();
		}
		ImGui::PopFont();
		
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